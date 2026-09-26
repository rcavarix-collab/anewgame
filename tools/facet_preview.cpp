// tools/facet_preview.cpp
//
// Still pictures of the faceted ground, drawn on the CPU (M1.2): the look
// checkpoint before the engine changes. It builds a small test world,
// meshes it with facetmesh.cpp (the code the game uses from M1.5), and
// shades every pixel with the game's formulas: the sky and light colours
// from sky.h, the world shader's sun, ambient, fog and tone curve
// (render.cpp), plus what M1.6 adds -- world-projected textures, height-
// based blending between up to three materials, and sky light. A sun
// shadow map is drawn the same way the game's is.
//
// These are previews, not the game: no bloom, SSAO, outlines or clouds.
//
//   tools/facet_preview.sh OUTDIR          every view, PNG, plus stats.txt
//   tools/facet_preview.sh OUTDIR meadow   one view by name
//
// Not compiled into the game (tools/ only). Deterministic.

#include "../facetmesh.h"
#include "../sky.h"
#include "../vtex.h"
#include "../terrain.h"
#include "../blocks.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

// ---------------------------------------------------------------------
// Small maths
// ---------------------------------------------------------------------
static inline float Sat(float x) { return x < 0 ? 0 : x > 1 ? 1 : x; }
static inline float Lerpf(float a, float b, float t) { return a + (b - a) * t; }
static inline Vec3 Mul(Vec3 a, Vec3 b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }
static inline Vec3 LerpV(Vec3 a, Vec3 b, float t) { return a + (b - a) * t; }
static inline float Len(Vec3 a) { return sqrtf(Dot(a, a)); }
static inline float Frac(float x) { return x - floorf(x); }
// How far each facet's lighting normal leans to the smooth one (render.cpp:
// 0.55, D46); 0 draws the look before.
static float g_soften = 0.55f;

// ---------------------------------------------------------------------
// Textures: the authored .vtex art, as linear colour and height with mips
// ---------------------------------------------------------------------
struct Tex {
    int size = 0;
    std::vector<std::vector<Vec3>> col;   // per mip, linear RGB
    std::vector<std::vector<float>> h;    // per mip, 0..1
};
static float SrgbToLinear(float c) { return c <= 0.04045f ? c / 12.92f : powf((c + 0.055f) / 1.055f, 2.4f); }
static Tex MakeTex(const VtexTexture& t) {
    Tex r;
    r.size = t.size;
    std::vector<Vec3> c(t.size * t.size);
    std::vector<float> h(t.size * t.size, 0.5f);
    for (int i = 0; i < t.size * t.size; i++) {
        uint32_t p = t.rgb[i];
        c[i] = { SrgbToLinear(((p >> 16) & 255) / 255.0f), SrgbToLinear(((p >> 8) & 255) / 255.0f), SrgbToLinear((p & 255) / 255.0f) };
        if (!t.height.empty()) h[i] = t.height[i];
        else h[i] = 0.3f * c[i].x + 0.6f * c[i].y + 0.1f * c[i].z; // no height map: brightness stands in
    }
    int s = t.size;
    r.col.push_back(c); r.h.push_back(h);
    while (s > 1) {
        int n = s / 2;
        std::vector<Vec3> c2(n * n); std::vector<float> h2(n * n);
        const auto& pc = r.col.back(); const auto& ph = r.h.back();
        for (int y = 0; y < n; y++)
            for (int x = 0; x < n; x++) {
                int a = (2 * y) * s + 2 * x;
                c2[y * n + x] = (pc[a] + pc[a + 1] + pc[a + s] + pc[a + s + 1]) * 0.25f;
                h2[y * n + x] = (ph[a] + ph[a + 1] + ph[a + s] + ph[a + s + 1]) * 0.25f;
            }
        r.col.push_back(c2); r.h.push_back(h2);
        s = n;
    }
    return r;
}
// Bilinear, wrapping, at mip `m`; u, v in blocks (one tile per block).
static void SampleMip(const Tex& t, int m, float u, float v, Vec3& c, float& h) {
    int s = std::max(1, t.size >> m);
    float x = u * s - 0.5f, y = v * s - 0.5f;
    float fx = floorf(x), fy = floorf(y);
    float tx = x - fx, ty = y - fy;
    int x0 = ((int)fx % s + s) % s, y0 = ((int)fy % s + s) % s;
    int x1 = (x0 + 1) % s, y1 = (y0 + 1) % s;
    const auto& C = t.col[m]; const auto& H = t.h[m];
    Vec3 a = LerpV(C[y0 * s + x0], C[y0 * s + x1], tx), b = LerpV(C[y1 * s + x0], C[y1 * s + x1], tx);
    c = LerpV(a, b, ty);
    float ha = Lerpf(H[y0 * s + x0], H[y0 * s + x1], tx), hb = Lerpf(H[y1 * s + x0], H[y1 * s + x1], tx);
    h = Lerpf(ha, hb, ty);
}
// Trilinear: `lod` = log2 of texels per pixel.
static void Sample(const Tex& t, float lod, float u, float v, Vec3& c, float& h) {
    int maxM = (int)t.col.size() - 1;
    lod = std::max(0.0f, std::min((float)maxM, lod));
    int m0 = (int)lod; float f = lod - m0;
    SampleMip(t, m0, u, v, c, h);
    if (f > 0.01f && m0 < maxM) {
        Vec3 c1; float h1;
        SampleMip(t, m0 + 1, u, v, c1, h1);
        c = LerpV(c, c1, f); h = Lerpf(h, h1, f);
    }
}

// ---------------------------------------------------------------------
// Materials of the preview world. Ids are the preview's own (the game's
// registry changes in M1.9); textures are the existing art (T7).
// ---------------------------------------------------------------------
enum { M_AIR, M_GRASS, M_DIRT, M_STONE, M_SAND, M_SANDSTONE, M_GRAVEL, M_MOSS_STONE, M_SNOW, M_COUNT };
struct MatDef { const char* name; const char* top; const char* side; float bump; };
static const MatDef kMats[M_COUNT] = {
    { "air", "", "", 0 },
    { "meadow grass", "meadow_grass", "dirt", 0.07f },
    { "dirt", "dirt", "dirt", 0.05f },
    { "stone", "stone", "stone", 0.045f },
    { "sand", "sand", "sand", 0.012f },
    { "sandstone", "sandstone_top", "sandstone_layered", 0.015f },
    { "gravel", "gravel", "gravel", 0.035f },
    { "moss stone", "moss_stone", "moss_stone", 0.04f },
    { "snow", "snow", "snow", 0.02f },
};
static std::map<std::string, Tex> g_tex;
static const Tex* g_top[256];
static const Tex* g_side[256];
static std::string g_matName[256];
static bool g_cubeLook = false;  // the old look, for comparison: corners on the lattice

static bool LoadTextures(const std::string& dir) {
    VtexSet set;
    for (const char* f : { "natural.vtex", "batch_sept.vtex" }) {
        std::ifstream in(dir + "/" + f);
        if (!in) { fprintf(stderr, "can't read %s/%s\n", dir.c_str(), f); return false; }
        std::stringstream ss; ss << in.rdbuf();
        ParseVtex(ss.str(), f, set);
    }
    for (auto& t : set.textures) g_tex[t.name] = MakeTex(t);
    return true;
}
static FacetMaterial g_fm[256];
// Material `id` of the world being drawn: its textures and lumpiness.
static bool SetMaterial(int id, const std::string& name, const std::string& top, const std::string& side, float bump) {
    auto a = g_tex.find(top), b = g_tex.find(side);
    if (a == g_tex.end() || b == g_tex.end()) { fprintf(stderr, "missing texture for %s\n", name.c_str()); return false; }
    g_top[id] = &a->second; g_side[id] = &b->second; g_matName[id] = name;
    g_fm[id] = { true, bump };
    return true;
}

// ---------------------------------------------------------------------
// The preview world: rolling meadow, dirt and gravel patches, stone
// outcrops, a sandy hollow, a sandstone cliff with an overhang, and a pit
// dug into the meadow.
// ---------------------------------------------------------------------
static int WX = 176, WY = 64, WZ = 176;
static std::vector<uint8_t> g_cells;
static uint8_t& Cell(int x, int y, int z) { return g_cells[((size_t)y * WZ + z) * WX + x]; }
static float H2(int x, int z, uint32_t s) {
    uint32_t h = (uint32_t)x * 0x8da6b343u ^ (uint32_t)z * 0xcb1ab31fu ^ s * 0x9e3779b9u;
    h ^= h >> 16; h *= 0x7feb352du; h ^= h >> 15; h *= 0x846ca68bu; h ^= h >> 16;
    return (h >> 8) * (1.0f / 16777216.0f);
}
static float VN(float x, float z, uint32_t s) {
    int ix = (int)floorf(x), iz = (int)floorf(z);
    float tx = x - ix, tz = z - iz;
    tx = tx * tx * (3 - 2 * tx); tz = tz * tz * (3 - 2 * tz);
    float a = Lerpf(H2(ix, iz, s), H2(ix + 1, iz, s), tx), b = Lerpf(H2(ix, iz + 1, s), H2(ix + 1, iz + 1, s), tx);
    return Lerpf(a, b, tz);
}
static float Fbm(float x, float z, uint32_t s) {
    // No octave finer than ~11 blocks: rounding a fine one to whole cells
    // leaves lone one-cell pits all over (seen in the first pictures).
    return 0.62f * VN(x / 24, z / 24, s) + 0.38f * VN(x / 11, z / 11, s + 1);
}
static void BuildWorld() {
    g_cells.assign((size_t)WX * WY * WZ, M_AIR);
    for (int z = 2; z < WZ - 2; z++)
        for (int x = 2; x < WX - 2; x++) {
            float h = 22 + 9 * (Fbm((float)x, (float)z, 11) - 0.5f);
            // The cliff: a sandstone plateau to the east, its edge wandering.
            float edge = 120 + 6 * (VN(z / 9.0f, 3.3f, 5) - 0.5f) * 2;
            bool plateau = x > edge;
            if (plateau) h += 11 + 2 * (VN(x / 7.0f, z / 7.0f, 6) - 0.5f);
            int top = (int)floorf(h);
            // A sandy hollow to the south-west.
            float dh = sqrtf((x - 50.0f) * (x - 50.0f) + (z - 125.0f) * (z - 125.0f));
            if (dh < 22) top -= (int)((22 - dh) / 4.5f);
            bool sandy = dh < 18 + 3 * VN(x / 5.0f, z / 5.0f, 8);
            float rock = VN(x / 13.0f, z / 13.0f, 9);
            bool outcrop = !plateau && !sandy && rock > 0.72f;
            if (outcrop) top += (int)((rock - 0.72f) * 18);
            float dirtN = VN(x / 9.0f, z / 9.0f, 10), gravN = VN(x / 7.0f, z / 7.0f, 12);
            for (int y = 1; y <= top && y < WY - 2; y++) {
                int depth = top - y;
                uint8_t m;
                if (plateau) m = (y > top - 1 && x > edge + 2) ? M_GRASS : M_SANDSTONE;
                else if (sandy) m = depth < 3 ? M_SAND : M_STONE;
                else if (outcrop) m = depth < 1 && rock < 0.78f ? M_MOSS_STONE : M_STONE;
                else if (depth == 0) m = dirtN > 0.66f ? M_DIRT : gravN > 0.74f ? M_GRAVEL : M_GRASS;
                else m = depth < 4 ? M_DIRT : M_STONE;
                Cell(x, y, z) = m;
            }
        }
    // An overhang: a sandstone ledge jutting west from the cliff.
    for (int z = 60; z < 72; z++)
        for (int x = 108; x < 126; x++)
            for (int y = 30; y < 33; y++) {
                float reach = 112 + 3 * VN(z / 3.0f, y / 3.0f, 14);
                if (x > reach) Cell(x, y, z) = M_SANDSTONE;
            }
    // The pit: dug in steps into the meadow, as a player would.
    int px = 72, pz = 70;
    for (int y = 10; y < WY; y++)
        for (int z = pz - 7; z <= pz + 7; z++)
            for (int x = px - 7; x <= px + 7; x++) {
                int r = std::max(std::abs(x - px), std::abs(z - pz));
                int floorY = 13 + r; // one step per ring
                if (y > floorY) Cell(x, y, z) = M_AIR;
            }
    // Snow on the plateau's highest crowns.
    for (int z = 2; z < WZ - 2; z++)
        for (int x = 2; x < WX - 2; x++)
            for (int y = WY - 3; y > 0; y--)
                if (Cell(x, y, z)) { if (y >= 35 && Cell(x, y, z) == M_GRASS) Cell(x, y, z) = M_SNOW; break; }
}

// Sky light at a corner: how much of the sky it sees, from rays marched
// through the cells (the game's own method comes with M1.6).
static float SkyAt(int cx, int cy, int cz, void*) {
    static const float dirs[9][3] = { { 0, 1, 0 }, { 0.6f, 0.8f, 0 }, { -0.6f, 0.8f, 0 }, { 0, 0.8f, 0.6f }, { 0, 0.8f, -0.6f },
                                      { 0.42f, 0.8f, 0.42f }, { -0.42f, 0.8f, 0.42f }, { 0.42f, 0.8f, -0.42f }, { -0.42f, 0.8f, -0.42f } };
    float open = 0;
    for (auto& d : dirs) {
        float x = cx + d[0] * 0.8f, y = cy + 0.5f, z = cz + d[2] * 0.8f;
        bool blocked = false;
        for (int s = 0; s < 40; s++) {
            x += d[0] * 0.7f; y += d[1] * 0.7f; z += d[2] * 0.7f;
            int ix = (int)floorf(x), iy = (int)floorf(y), iz = (int)floorf(z);
            if (ix < 0 || iz < 0 || ix >= WX || iz >= WZ || iy >= WY) break;
            if (iy >= 0 && Cell(ix, iy, iz)) { blocked = true; break; }
        }
        open += blocked ? 0 : (d[1] > 0.9f ? 1.5f : 1.0f);
    }
    return open / 9.5f;
}

// ---------------------------------------------------------------------
// Meshing the world, with the detail bands centred on the camera
// ---------------------------------------------------------------------
struct BandCtx { Vec3 eye; float nearR, midR; };
static int BandOf(Vec3 p, void* u) {
    BandCtx* b = (BandCtx*)u;
    float dx = p.x - b->eye.x, dz = p.z - b->eye.z, dy = p.y - b->eye.y;
    float d = sqrtf(dx * dx + dy * dy + dz * dz);
    return d < b->nearR ? 2 : d < b->midR ? 1 : 0;
}
static FacetMesh BuildMesh(Vec3 eye, bool selective, bool detail, const FacetShape& shape) {
    FacetGrid g; g.nx = WX; g.ny = WY; g.nz = WZ; g.cells = g_cells.data(); g.mats = g_fm;
    FacetBuildParams p;
    p.bx0 = 0; p.by0 = 0; p.bz0 = 0; p.bx1 = WX; p.by1 = WY; p.bz1 = WZ;
    static BandCtx bc;
    bc = { eye, 32.0f, 64.0f };  // 2 chunks fine (0.25), 4 chunks medium (0.5): FOUNDATIONS 4.1
    p.band = detail ? BandOf : nullptr;
    p.sky = SkyAt;
    p.user = &bc;
    p.selective = selective;
    p.shape = shape;
    if (g_cubeLook) p.shape.relax = false;
    FacetMesh m;
    FacetBuild(g, p, m);
    return m;
}

// ---------------------------------------------------------------------
// Rasterising: a G-buffer (depth, triangle, barycentrics), then shading
// ---------------------------------------------------------------------
struct Camera { Vec3 eye, fwd, right, up; float tanHalf, aspect; };
static Camera MakeCamera(Vec3 eye, Vec3 target, float fovDeg, float aspect) {
    Camera c; c.eye = eye;
    c.fwd = Normalize(target - eye);
    c.right = Normalize(Cross(kUp, c.fwd));
    c.up = Cross(c.fwd, c.right);
    c.tanHalf = tanf(fovDeg * 0.5f * 3.14159265f / 180.0f);
    c.aspect = aspect;
    return c;
}
struct GPix { float z; int tri; float b1, b2; };

// Draws triangles given in view space (x right, y up, z forward) into a
// G-buffer, clipping at the near plane. Ortho: x, y already in pixels.
struct Raster {
    int W, H;
    std::vector<GPix> px;
    void Clear() { px.assign((size_t)W * H, { 1e30f, -1, 0, 0 }); }
    // Screen-space triangle with per-vertex (1/w) for perspective-correct
    // barycentrics; `bw` are the original barycentric coordinates of each
    // (clipped) vertex in the source triangle.
    void Tri(const float sx[3], const float sy[3], const float z[3], const float iw[3], const float bw[3][2], int id) {
        float minx = std::min({ sx[0], sx[1], sx[2] }), maxx = std::max({ sx[0], sx[1], sx[2] });
        float miny = std::min({ sy[0], sy[1], sy[2] }), maxy = std::max({ sy[0], sy[1], sy[2] });
        int x0 = std::max(0, (int)floorf(minx)), x1 = std::min(W - 1, (int)ceilf(maxx));
        int y0 = std::max(0, (int)floorf(miny)), y1 = std::min(H - 1, (int)ceilf(maxy));
        if (x0 > x1 || y0 > y1) return;
        float area = (sx[1] - sx[0]) * (sy[2] - sy[0]) - (sx[2] - sx[0]) * (sy[1] - sy[0]);
        if (fabsf(area) < 1e-12f) return;
        float ia = 1.0f / area;
        for (int y = y0; y <= y1; y++)
            for (int x = x0; x <= x1; x++) {
                float fx = x + 0.5f, fy = y + 0.5f;
                float l1 = ((fx - sx[0]) * (sy[2] - sy[0]) - (sx[2] - sx[0]) * (fy - sy[0])) * ia;
                float l2 = ((sx[1] - sx[0]) * (fy - sy[0]) - (fx - sx[0]) * (sy[1] - sy[0])) * ia;
                float l0 = 1 - l1 - l2;
                if (l0 < -1e-6f || l1 < -1e-6f || l2 < -1e-6f) continue;
                float zz = l0 * z[0] + l1 * z[1] + l2 * z[2];
                GPix& g = px[(size_t)y * W + x];
                if (zz >= g.z) continue;
                // Perspective-correct weights.
                float w0 = l0 * iw[0], w1 = l1 * iw[1], w2 = l2 * iw[2], ws = w0 + w1 + w2;
                w0 /= ws; w1 /= ws; w2 /= ws;
                g.z = zz; g.tri = id;
                g.b1 = w0 * bw[0][0] + w1 * bw[1][0] + w2 * bw[2][0];
                g.b2 = w0 * bw[0][1] + w1 * bw[1][1] + w2 * bw[2][1];
            }
    }
};

// ---------------------------------------------------------------------
// Rendering a view
// ---------------------------------------------------------------------
struct View {
    const char* name;
    const char* title;
    Vec3 eye, target;
    float dayTime;     // seconds into the day (sunrise 0, noon 1500, sunset 3000)
    int mode;          // 0 shaded, 1 shaded + triangle edges
    bool selective = true, detail = true, cubes = false;
};

struct Scene {
    FacetMesh mesh;
    std::vector<Vec3> flatN; // per triangle, out of the ground
};

static Vec3 SkyColorAt(const Atmosphere& a, Vec3 sun, Vec3 d) {
    float h = Sat(d.y);
    Vec3 col = LerpV(a.horizon, a.zenith, powf(h, 0.5f));
    col = col * (1.0f - 0.25f * Sat(-d.y * 4.0f));
    float mu = Dot(d, sun);
    float toward = Sat(mu * 0.5f + 0.5f);
    float band = a.twilightAmount * powf(Sat(1.0f - fabsf(d.y) * 2.0f), 2.5f) * (0.25f + 0.75f * toward * toward * toward);
    col = LerpV(col, a.twilight, Sat(band * 1.3f));
    float s = Sat(mu);
    col = col + a.sunColor * (0.10f * powf(s, 8.0f) + 0.25f * powf(s, 64.0f));
    return col;
}
static float FogAmount(float dist, float fogStart, float fogEnd) {
    float haze = 1.0f - expf(-dist / std::max(fogEnd * 1.2f, 1.0f));
    float f = Sat((dist - fogStart) / std::max(fogEnd - fogStart, 1.0f));
    return std::max(haze * 0.55f, f * f * (3.0f - 2.0f * f));
}
static float ToDisplay1(float x, float exposure) {
    x *= exposure;
    x = Sat((x * (2.51f * x + 0.03f)) / (x * (2.43f * x + 0.59f) + 0.14f));
    return powf(x, 1.0f / 2.2f);
}
static float VHash(float px, float py) {
    px = Frac(px * 0.1031f); py = Frac(py * 0.1030f);
    float d = px * (py + 33.33f) + py * (px + 33.33f);
    px += d; py += d;
    return Frac((px + py) * px);
}
static float VNoise2(float x, float y) {
    float cx = floorf(x), cy = floorf(y), fx = x - cx, fy = y - cy;
    fx = fx * fx * (3 - 2 * fx); fy = fy * fy * (3 - 2 * fy);
    return Lerpf(Lerpf(VHash(cx, cy), VHash(cx + 1, cy), fx), Lerpf(VHash(cx, cy + 1), VHash(cx + 1, cy + 1), fx), fy);
}
// render.cpp WorldVariation (4.16).
static Vec3 WorldVariation(Vec3 w) {
    float qx = w.x + w.y * 0.37f, qy = w.z + w.y * 0.21f;
    float val = 1.0f + 0.18f * (VNoise2(qx / 24, qy / 24) - 0.5f) + 0.08f * (VNoise2(qx / 9 + 17.3f, qy / 9 + 17.3f) - 0.5f);
    float warm = 0.10f * (VNoise2(qx / 31 + 5.1f, qy / 31 + 5.1f) - 0.5f);
    return { val * (1 + warm), val, val * (1 - warm) };
}

// A side projection's sample: the side texture, or the top one on gentle slopes.
static void SideSample(const Tex& top, const Tex& side, float topness, float lod, float u, float v, Vec3& c, float& h) {
    if (topness <= 0.001f) { Sample(side, lod, u, v, c, h); return; }
    if (topness >= 0.999f) { Sample(top, lod, u, v, c, h); return; }
    Vec3 c2; float h2;
    Sample(side, lod, u, v, c, h); Sample(top, lod, u, v, c2, h2);
    c = LerpV(c, c2, topness); h = Lerpf(h, h2, topness);
}

// The material at a pixel: world-projected (top/bottom and two sides,
// blended by the facing), then the up-to-three materials blended by
// their height maps (M1.6).
struct Surface { Vec3 albedo; Vec3 bumpN; };
static const float kBlendDepth = 0.18f;  // how deep the height-based border is
static const float kSurfaceDepth = 0.05f; // blocktex.h SURFACE_DEPTH: the relief's strength
static Surface Material(Vec3 p, Vec3 nSmooth, Vec3 nGeo, const uint8_t mat[3], const float w[3], float lod) {
    // Projection weights: sharpened so a slope takes mostly one projection.
    Vec3 an = { fabsf(nSmooth.x), fabsf(nSmooth.y), fabsf(nSmooth.z) };
    float px = powf(an.x, 4), py = powf(an.y, 4), pz = powf(an.z, 4), ps = px + py + pz;
    px /= ps; py /= ps; pz /= ps;
    // World noise nudges the weights so borders don't follow triangle edges.
    float wn = (VNoise2(p.x * 1.3f + p.y * 0.7f, p.z * 1.3f - p.y * 0.5f) - 0.5f) * 0.35f;
    // Which face of the material a slope shows: its top up to ~50 degrees,
    // its side only on steep ground -- so a one-cell rise in a meadow stays
    // grass, and a cliff shows its strata.
    float topness = Sat((nSmooth.y - 0.25f) / 0.17f); // the world shader's smoothstep(0.25, 0.42) (D50)
    topness = topness * topness * (3 - 2 * topness);
    Vec3 col[3]; float hgt[3]; Vec3 grad[3];
    for (int k = 0; k < 3; k++) {
        col[k] = { 0, 0, 0 }; hgt[k] = 0; grad[k] = { 0, 0, 0 };
        if (w[k] <= 0.001f) continue;
        const Tex* top = g_top[mat[k]];
        const Tex* side = g_side[mat[k]];
        Vec3 c; float h;
        const float e = 1.0f / 32.0f; // one texel: the relief's slope
        if (py > 0.001f) {
            Sample(*top, lod, p.x, p.z, c, h);
            Vec3 c1; float hx, hz;
            Sample(*top, lod, p.x + e, p.z, c1, hx); Sample(*top, lod, p.x, p.z + e, c1, hz);
            col[k] = col[k] + c * py; hgt[k] += h * py;
            grad[k] = grad[k] + Vec3{ (hx - h) / e, 0, (hz - h) / e } * (py * kSurfaceDepth);
        }
        if (px > 0.001f) { // v runs down the texture: world y up is -v
            SideSample(*top, *side, topness, lod, p.z, -p.y, c, h);
            Vec3 c1; float hu, hv;
            SideSample(*top, *side, topness, lod, p.z + e, -p.y, c1, hu); SideSample(*top, *side, topness, lod, p.z, -p.y + e, c1, hv);
            col[k] = col[k] + c * px; hgt[k] += h * px;
            grad[k] = grad[k] + Vec3{ 0, -(hv - h) / e, (hu - h) / e } * (px * kSurfaceDepth);
        }
        if (pz > 0.001f) {
            SideSample(*top, *side, topness, lod, p.x, -p.y, c, h);
            Vec3 c1; float hu, hv;
            SideSample(*top, *side, topness, lod, p.x + e, -p.y, c1, hu); SideSample(*top, *side, topness, lod, p.x, -p.y + e, c1, hv);
            col[k] = col[k] + c * pz; hgt[k] += h * pz;
            grad[k] = grad[k] + Vec3{ (hu - h) / e, -(hv - h) / e, 0 } * (pz * kSurfaceDepth);
        }
    }
    // Height-based blending: each material's weight lifted by its height;
    // the tallest shows through within a narrow band.
    float score[3], best = -1e9f;
    for (int k = 0; k < 3; k++) {
        score[k] = w[k] > 0.001f ? w[k] + (k == 0 ? wn : -wn * 0.5f) + hgt[k] * 0.6f : -1e9f;
        best = std::max(best, score[k]);
    }
    float bw[3], bs = 0;
    for (int k = 0; k < 3; k++) { bw[k] = std::max(0.0f, score[k] - (best - kBlendDepth)); bs += bw[k]; }
    Surface s; s.albedo = { 0, 0, 0 }; Vec3 g = { 0, 0, 0 };
    for (int k = 0; k < 3; k++) { s.albedo = s.albedo + col[k] * (bw[k] / bs); g = g + grad[k] * (bw[k] / bs); }
    // The relief tilts the facet's own normal (fades with distance, as in game).
    float fade = Sat(1.0f - lod * 0.5f);
    Vec3 gp = g - nGeo * Dot(g, nGeo);
    s.bumpN = Normalize(nGeo - gp * fade);
    return s;
}

static void WritePNG(const std::string& path, int W, int H, const std::vector<uint8_t>& rgb);

static void Render(const View& v, const std::string& outDir, FILE* stats, int W = 1280, int H = 720) {
    const int SS = 2; // 2x2 supersampling
    const int RW = W * SS, RH = H * SS;
    g_cubeLook = v.cubes;
    FacetShape shape;
    Scene sc;
    sc.mesh = BuildMesh(v.eye, v.selective, v.detail && !v.cubes, shape);
    const FacetMesh& m = sc.mesh;
    int T = m.triangles();
    sc.flatN.resize(T);
    for (int t = 0; t < T; t++) {
        Vec3 a = m.verts[m.idx[3 * t]].pos, b = m.verts[m.idx[3 * t + 1]].pos, c = m.verts[m.idx[3 * t + 2]].pos;
        sc.flatN[t] = Normalize(Cross(b - a, c - a));
    }
    SkyState sky = ComputeSky(v.dayTime);
    Atmosphere atm = ComputeAtmosphere(sky);
    Vec3 sun = sky.sunDir;
    Camera cam = MakeCamera(v.eye, v.target, 60.0f, (float)W / H);
    const float nearZ = 0.1f;

    // ---- the sun's shadow map (orthographic, like the game's) ----
    const int SM = 4096;
    Vec3 sF = { -sun.x, -sun.y, -sun.z };
    Vec3 sR = Normalize(Cross(fabsf(sun.y) > 0.99f ? Vec3{ 0, 0, 1 } : kUp, sF));
    Vec3 sU = Cross(sF, sR);
    Vec3 centre = { WX * 0.5f, 24.0f, WZ * 0.5f };
    const float ext = std::max(WX, WZ) * 0.75f;
    Raster shadow{ SM, SM, {} };
    shadow.Clear();
    auto toLight = [&](Vec3 p, float& x, float& y, float& z) {
        Vec3 d = p - centre;
        x = (Dot(d, sR) / ext * 0.5f + 0.5f) * SM;
        y = (0.5f - Dot(d, sU) / ext * 0.5f) * SM;
        z = Dot(d, sF);
    };
    bool sunUp = sky.sunLight > 0.0f;
    if (sunUp)
        for (int t = 0; t < T; t++) {
            float sx[3], sy[3], sz[3], iw[3] = { 1, 1, 1 }, bw[3][2] = { { 0, 0 }, { 1, 0 }, { 0, 1 } };
            for (int i = 0; i < 3; i++) toLight(m.verts[m.idx[3 * t + i]].pos, sx[i], sy[i], sz[i]);
            shadow.Tri(sx, sy, sz, iw, bw, t);
        }

    // ---- the camera's G-buffer ----
    Raster gb{ RW, RH, {} };
    gb.Clear();
    auto toView = [&](Vec3 p) { Vec3 d = p - cam.eye; return Vec3{ Dot(d, cam.right), Dot(d, cam.up), Dot(d, cam.fwd) }; };
    auto project = [&](Vec3 vv, float& sx, float& sy) {
        sx = (vv.x / (vv.z * cam.tanHalf * cam.aspect) * 0.5f + 0.5f) * RW;
        sy = (0.5f - vv.y / (vv.z * cam.tanHalf) * 0.5f) * RH;
    };
    for (int t = 0; t < T; t++) {
        Vec3 vv[3];
        for (int i = 0; i < 3; i++) vv[i] = toView(m.verts[m.idx[3 * t + i]].pos);
        if (vv[0].z < nearZ && vv[1].z < nearZ && vv[2].z < nearZ) continue;
        // Back faces aren't drawn (the game will cull them too, M1.5).
        if (Dot(sc.flatN[t], cam.eye - m.verts[m.idx[3 * t]].pos) < 0) continue;
        // Clip against the near plane (a polygon of up to 4 vertices).
        Vec3 poly[4]; float pb[4][2]; int n = 0;
        const float base[3][2] = { { 0, 0 }, { 1, 0 }, { 0, 1 } };
        for (int i = 0; i < 3; i++) {
            int j = (i + 1) % 3;
            bool in_i = vv[i].z >= nearZ, in_j = vv[j].z >= nearZ;
            if (in_i) { poly[n] = vv[i]; pb[n][0] = base[i][0]; pb[n][1] = base[i][1]; n++; }
            if (in_i != in_j) {
                float f = (nearZ - vv[i].z) / (vv[j].z - vv[i].z);
                poly[n] = LerpV(vv[i], vv[j], f);
                pb[n][0] = Lerpf(base[i][0], base[j][0], f); pb[n][1] = Lerpf(base[i][1], base[j][1], f);
                n++;
            }
        }
        for (int k = 1; k + 1 < n; k++) {
            int ids[3] = { 0, k, k + 1 };
            float sx[3], sy[3], z[3], iw[3], bw[3][2];
            for (int i = 0; i < 3; i++) {
                project(poly[ids[i]], sx[i], sy[i]);
                z[i] = poly[ids[i]].z; iw[i] = 1.0f / poly[ids[i]].z;
                bw[i][0] = pb[ids[i]][0]; bw[i][1] = pb[ids[i]][1];
            }
            gb.Tri(sx, sy, z, iw, bw, t);
        }
    }

    // ---- shading ----
    float fogEnd = 6 * 16.0f, fogStart = fogEnd * 0.5f; // render distance 6
    std::vector<Vec3> hdr((size_t)RW * RH);
    std::atomic<int> nextRow{ 0 };
    auto worker = [&]() {
        for (;;) {
            int y = nextRow++;
            if (y >= RH) break;
            for (int x = 0; x < RW; x++) {
                const GPix& g = gb.px[(size_t)y * RW + x];
                float ndx = ((x + 0.5f) / RW * 2 - 1) * cam.tanHalf * cam.aspect, ndy = (1 - (y + 0.5f) / RH * 2) * cam.tanHalf;
                Vec3 dir = Normalize(cam.fwd + cam.right * ndx + cam.up * ndy);
                if (g.tri < 0) {
                    Vec3 col = SkyColorAt(atm, sun, dir);
                    float sd = Dot(dir, sun);
                    if (sunUp && sd > 0.9995f) col = col + atm.sunColor * 6.0f; // the sun's disc
                    hdr[(size_t)y * RW + x] = col;
                    continue;
                }
                int t = g.tri;
                const FacetVertex& A = m.verts[m.idx[3 * t]];
                const FacetVertex& B = m.verts[m.idx[3 * t + 1]];
                const FacetVertex& C = m.verts[m.idx[3 * t + 2]];
                float b1 = g.b1, b2 = g.b2, b0 = 1 - b1 - b2;
                Vec3 p = A.pos * b0 + B.pos * b1 + C.pos * b2;
                Vec3 ns = Normalize(A.normal * b0 + B.normal * b1 + C.normal * b2);
                float w[3];
                for (int k = 0; k < 3; k++) w[k] = (A.w[k] * b0 + B.w[k] * b1 + C.w[k] * b2) / 255.0f;
                float ao = (A.ao * b0 + B.ao * b1 + C.ao * b2) / 255.0f;
                float skyL = (A.sky * b0 + B.sky * b1 + C.sky * b2) / 255.0f;
                Vec3 nGeo = sc.flatN[t];
                float dist = Len(p - cam.eye);
                // Texels per pixel: 32 per block over the pixel's footprint,
                // steeper at grazing angles (a cheap stand-in for anisotropy).
                float pixAngle = 2 * cam.tanHalf / RH;
                float foot = dist * pixAngle / std::max(0.3f, fabsf(Dot(dir, nGeo)));
                float lod = log2f(std::max(1e-4f, foot * 32.0f)) - 0.5f;
                // The softer look (D46): lighting normal partway to the smooth one.
                Vec3 nL = Normalize(LerpV(nGeo, ns, g_soften));
                Surface s = Material(p, ns, nL, A.mat, w, lod);
                Vec3 albedo = Mul(s.albedo, WorldVariation(p));
                Vec3 n = s.bumpN;
                // The world shader's lighting (render.cpp PSMain).
                float facing = g_soften > 0 ? Sat(Dot(nL, sun) * 4.0f) : Sat(Dot(nGeo, sun) * 8.0f);
                float sunLit = sqrtf(Sat(Dot(n, sun))) * facing;
                if (sunLit > 0) {
                    float lx, ly, lz;
                    toLight(p + nGeo * 0.2f, lx, ly, lz);
                    float lit = 0; int taps = 0;
                    for (int oy = -1; oy <= 1; oy++)
                        for (int ox = -1; ox <= 1; ox++) {
                            int ix = (int)lx + ox, iy = (int)ly + oy;
                            if (ix < 0 || iy < 0 || ix >= SM || iy >= SM) { lit += 1; taps++; continue; }
                            lit += shadow.px[(size_t)iy * SM + ix].z + 0.04f >= lz ? 1.0f : 0.0f;
                            taps++;
                        }
                    sunLit *= lit / taps;
                }
                // Sky light (M1.6): the sky's share of the ambient.
                float skyAmb = 0.25f + 0.75f * skyL;
                Vec3 ambient = LerpV(atm.ambientDown, atm.ambientUp, n.y * 0.5f + 0.5f) * (ao * skyAmb);
                Vec3 direct = atm.sunColor * (sunLit * (0.55f + 0.45f * ao)) + atm.moonColor * (Sat(Dot(n, sky.moonDir)) * ao);
                Vec3 col = Mul(albedo, ambient + direct);
                col = LerpV(col, SkyColorAt(atm, sun, dir), FogAmount(dist, fogStart, fogEnd));
                hdr[(size_t)y * RW + x] = col;
            }
        }
    };
    std::vector<std::thread> th;
    for (int i = 0; i < 4; i++) th.emplace_back(worker);
    for (auto& t : th) t.join();

    // ---- triangle edges (the detail view) ----
    if (v.mode == 1)
        for (int y = 1; y < RH; y++)
            for (int x = 1; x < RW; x++) {
                int a = gb.px[(size_t)y * RW + x].tri, b = gb.px[(size_t)y * RW + x - 1].tri, c = gb.px[(size_t)(y - 1) * RW + x].tri;
                if (a >= 0 && (a != b || a != c)) hdr[(size_t)y * RW + x] = hdr[(size_t)y * RW + x] * 0.35f + Vec3{ 0.9f, 0.8f, 0.2f } * 0.25f;
            }

    // ---- resolve: average the 2x2 samples after the tone curve ----
    std::vector<uint8_t> out((size_t)W * H * 3);
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++) {
            float r = 0, gg = 0, b = 0;
            for (int sy = 0; sy < SS; sy++)
                for (int sx = 0; sx < SS; sx++) {
                    Vec3 c = hdr[(size_t)(y * SS + sy) * RW + x * SS + sx];
                    r += ToDisplay1(c.x, atm.exposure); gg += ToDisplay1(c.y, atm.exposure); b += ToDisplay1(c.z, atm.exposure);
                }
            float k = 255.0f / (SS * SS);
            out[((size_t)y * W + x) * 3 + 0] = (uint8_t)std::min(255.0f, r * k + 0.5f);
            out[((size_t)y * W + x) * 3 + 1] = (uint8_t)std::min(255.0f, gg * k + 0.5f);
            out[((size_t)y * W + x) * 3 + 2] = (uint8_t)std::min(255.0f, b * k + 0.5f);
        }
    std::string path = outDir + "/" + v.name + ".png";
    WritePNG(path, W, H, out);
    int drawn = 0;
    {   // Triangles within the view's reach (the fog's end), as counted for the budget.
        for (int t = 0; t < T; t++) {
            Vec3 c = m.verts[m.idx[3 * t]].pos;
            if (Len(c - cam.eye) < fogEnd) drawn++;
        }
    }
    int flipped = 0;
    for (int t = 0; t < T; t++) {
        Vec3 ns = m.verts[m.idx[3 * t]].normal + m.verts[m.idx[3 * t + 1]].normal + m.verts[m.idx[3 * t + 2]].normal;
        if (Dot(sc.flatN[t], ns) < 0) flipped++;
    }
    printf("  facing away from the surface: %d; folded: %d\n", flipped, m.folded);
    fprintf(stats, "%s\t%s\ttriangles %d\twithin %.0f blocks %d\tbase quads %d\tcut 1x1 %d, 2x2 %d, 4x4 %d\tvertices %zu\n",
            v.name, v.title, T, fogEnd, drawn, m.baseQuads, m.quadsAtLevel[0], m.quadsAtLevel[1], m.quadsAtLevel[2], m.verts.size());
    printf("%s: %d triangles (%d base quads; %d/%d/%d at 1x1/2x2/4x4)\n", path.c_str(), T, m.baseQuads,
           m.quadsAtLevel[0], m.quadsAtLevel[1], m.quadsAtLevel[2]);
}

// ---------------------------------------------------------------------
// PNG: zlib "stored" blocks, so the tool needs no library.
// ---------------------------------------------------------------------
static uint32_t Crc(const uint8_t* d, size_t n, uint32_t c = 0xffffffffu) {
    static uint32_t tbl[256]; static bool init = false;
    if (!init) { for (uint32_t i = 0; i < 256; i++) { uint32_t k = i; for (int j = 0; j < 8; j++) k = k & 1 ? 0xedb88320u ^ (k >> 1) : k >> 1; tbl[i] = k; } init = true; }
    for (size_t i = 0; i < n; i++) c = tbl[(c ^ d[i]) & 255] ^ (c >> 8);
    return c;
}
static void WritePNG(const std::string& path, int W, int H, const std::vector<uint8_t>& rgb) {
    std::vector<uint8_t> raw;
    for (int y = 0; y < H; y++) { raw.push_back(0); raw.insert(raw.end(), rgb.begin() + (size_t)y * W * 3, rgb.begin() + (size_t)(y + 1) * W * 3); }
    std::vector<uint8_t> z = { 0x78, 0x01 };
    uint32_t a = 1, b = 0;
    for (uint8_t c : raw) { a = (a + c) % 65521; b = (b + a) % 65521; }
    for (size_t i = 0; i < raw.size(); i += 65535) {
        size_t n = std::min<size_t>(65535, raw.size() - i);
        z.push_back(i + n == raw.size() ? 1 : 0);
        z.push_back(n & 255); z.push_back(n >> 8); z.push_back(~n & 255); z.push_back((~n >> 8) & 255);
        z.insert(z.end(), raw.begin() + i, raw.begin() + i + n);
    }
    uint32_t ad = (b << 16) | a;
    z.push_back(ad >> 24); z.push_back(ad >> 16); z.push_back(ad >> 8); z.push_back(ad);
    FILE* f = fopen(path.c_str(), "wb");
    if (!f) { fprintf(stderr, "can't write %s\n", path.c_str()); return; }
    auto be = [&](uint32_t v) { uint8_t q[4] = { (uint8_t)(v >> 24), (uint8_t)(v >> 16), (uint8_t)(v >> 8), (uint8_t)v }; fwrite(q, 1, 4, f); };
    auto chunk = [&](const char* type, const std::vector<uint8_t>& d) {
        be((uint32_t)d.size());
        std::vector<uint8_t> td(type, type + 4); td.insert(td.end(), d.begin(), d.end());
        fwrite(td.data(), 1, td.size(), f);
        be(Crc(td.data(), td.size()) ^ 0xffffffffu);
    };
    const uint8_t sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    fwrite(sig, 1, 8, f);
    std::vector<uint8_t> ih = { (uint8_t)(W >> 24), (uint8_t)(W >> 16), (uint8_t)(W >> 8), (uint8_t)W,
                                (uint8_t)(H >> 24), (uint8_t)(H >> 16), (uint8_t)(H >> 8), (uint8_t)H, 8, 2, 0, 0, 0 };
    chunk("IHDR", ih); chunk("IDAT", z); chunk("IEND", {});
    fclose(f);
}

// ---------------------------------------------------------------------
// Contact sheets (M1.3): every candidate material alone on a small
// faceted mound with a cliff, then each pair of the picks meeting.
// ---------------------------------------------------------------------
struct Candidate { const char* name; const char* top; const char* side; float bump; };
static const Candidate kCandidates[] = {
    { "meadow grass", "meadow_grass", "dirt", 0.07f }, { "heather turf", "heather_turf", "loam", 0.07f },
    { "dry turf", "dry_turf", "dirt", 0.06f },           { "frost turf", "frost_turf", "dirt", 0.06f },
    { "moss", "moss", "dirt", 0.06f },                   { "dirt", "dirt", "dirt", 0.05f },
    { "loam", "loam", "loam", 0.05f },                   { "dark humus", "dark_humus", "dark_humus", 0.05f },
    { "clay", "clay", "clay", 0.03f },                   { "cracked earth", "cracked_earth", "cracked_earth", 0.02f },
    { "wet mud", "wet_mud", "wet_mud", 0.02f },          { "peat bog", "peat_bog", "peat_bog", 0.03f },
    { "sand", "sand", "sand", 0.012f },                  { "coastal sand", "coastal_sand", "coastal_sand", 0.012f },
    { "coarse sand", "coarse_sand", "coarse_sand", 0.015f }, { "silt", "silt", "silt", 0.01f },
    { "gravel", "gravel", "gravel", 0.035f },            { "river pebble", "river_pebble", "river_pebble", 0.035f },
    { "mossy gravel", "mossy_gravel", "mossy_gravel", 0.04f }, { "stone", "stone", "stone", 0.045f },
    { "granite", "granite", "granite", 0.045f },         { "slate", "slate", "slate", 0.04f },
    { "limestone", "limestone", "limestone", 0.04f },    { "chalk", "chalk", "chalk", 0.03f },
    { "basalt", "basalt", "basalt", 0.045f },            { "moss stone", "moss_stone", "moss_stone", 0.045f },
    { "sandstone", "sandstone_top", "sandstone_layered", 0.015f }, { "snow", "snow", "snow", 0.02f },
    { "salt flat", "salt_flat", "salt_flat", 0.01f },    { "volcanic ash", "volcanic_ash", "volcanic_ash", 0.02f },
    { "autumn leaf litter", "autumn_leaf_litter", "dirt", 0.05f }, { "clay bank", "clay_bank", "clay_bank", 0.03f },
};
static const int kCandidateCount = (int)(sizeof(kCandidates) / sizeof(kCandidates[0]));
// The provisional picks (D36), by candidate name.
static const char* kPicks[] = { "meadow grass", "dry turf", "moss", "dirt", "loam", "clay", "sand", "gravel",
                                "stone", "slate", "sandstone", "snow" };
static const int kPickCount = (int)(sizeof(kPicks) / sizeof(kPicks[0]));

static const int PLOT = 30;
// One plot: a mound with a two-cell cliff on its far side. `b` > 0 makes
// the plot's far half material b, meeting a along a wavy line.
static void Plot(int px, int pz, int a, int b) {
    for (int z = 0; z < PLOT; z++)
        for (int x = 0; x < PLOT; x++) {
            float dx = x - 13.0f, dz = z - 13.0f, r = sqrtf(dx * dx + dz * dz);
            float h = 8 + 5.0f * std::max(0.0f, 1 - r / 11.0f) + 1.2f * (VN(x / 4.0f + px, z / 4.0f + pz, 3) - 0.5f);
            if (x + z > 36) h += 3;   // the cliff
            int top = (int)floorf(h);
            bool farHalf = b > 0 && (x - z + 3.0f * sinf(z * 0.45f) + 2.0f * sinf(x * 0.3f)) > 0;
            for (int y = 1; y <= top; y++) Cell(px + x, y, pz + z) = (uint8_t)(farHalf ? b : a);
        }
}
static int Sheets(const std::string& out) {
    // Sheet 1: every candidate, one plot each.
    int cols = 8, rows = (kCandidateCount + cols - 1) / cols;
    WX = cols * PLOT; WZ = rows * PLOT; WY = 24;
    g_cells.assign((size_t)WX * WY * WZ, 0);
    for (int i = 0; i < kCandidateCount; i++) {
        if (!SetMaterial(i + 1, kCandidates[i].name, kCandidates[i].top, kCandidates[i].side, kCandidates[i].bump)) return 1;
        Plot((i % cols) * PLOT, (i / cols) * PLOT, i + 1, 0);
    }
    std::string statsPath = out + "/sheet_stats.txt";
    FILE* stats = fopen(statsPath.c_str(), "w");
    if (!stats) return 1;
    auto shoot = [&](const std::string& name, int px, int pz, float t) {
        View v = { nullptr, "sheet", { px + 1.0f, 17.0f, pz + 1.0f }, { px + 15.0f, 9.0f, pz + 15.0f }, t, 0 };
        static std::string keep; keep = name; v.name = keep.c_str();
        Render(v, out, stats, 480, 320);
    };
    for (int i = 0; i < kCandidateCount; i++) {
        std::string slug = kCandidates[i].name;
        for (auto& ch : slug) if (ch == ' ') ch = '_';
        shoot("mat_" + slug + "_late", (i % cols) * PLOT, (i / cols) * PLOT, 2650);
    }
    // Sheet 2: the picks meeting, each pair once.
    std::vector<int> pick;
    for (int k = 0; k < kPickCount; k++)
        for (int i = 0; i < kCandidateCount; i++)
            if (std::string(kCandidates[i].name) == kPicks[k]) pick.push_back(i + 1);
    std::vector<std::pair<int, int>> pairs;
    for (size_t a = 0; a < pick.size(); a++)
        for (size_t b = a + 1; b < pick.size(); b++) pairs.push_back({ pick[a], pick[b] });
    cols = 11; rows = ((int)pairs.size() + cols - 1) / cols;
    WX = cols * PLOT; WZ = rows * PLOT;
    g_cells.assign((size_t)WX * WY * WZ, 0);
    for (size_t i = 0; i < pairs.size(); i++) Plot((int)(i % cols) * PLOT, (int)(i / cols) * PLOT, pairs[i].first, pairs[i].second);
    for (size_t i = 0; i < pairs.size(); i++) {
        std::string slug = g_matName[pairs[i].first] + "__" + g_matName[pairs[i].second];
        for (auto& ch : slug) if (ch == ' ') ch = '_';
        shoot("pair_" + slug, (int)(i % cols) * PLOT, (int)(i / cols) * PLOT, 1100);
    }
    fclose(stats);
    return 0;
}

// ---------------------------------------------------------------------
// walkgrid-hills v1 (M1.4): the game's own terrain, drawn with the picks.
// ---------------------------------------------------------------------
static int Hills(const std::string& out, uint64_t seed) {
    struct M { int id; const char* name; const char* top; const char* side; float bump; };
    const M ms[] = {
        { BLOCK_MEADOW_GRASS, "meadow grass", "meadow_grass", "dirt", 0.07f }, { BLOCK_DRY_TURF, "dry turf", "dry_turf", "dirt", 0.06f },
        { BLOCK_MOSS, "moss", "moss", "dirt", 0.06f }, { BLOCK_DIRT, "dirt", "dirt", "dirt", 0.05f },
        { BLOCK_LOAM, "loam", "loam", "loam", 0.05f }, { BLOCK_CLAY, "clay", "clay", "clay", 0.03f },
        { BLOCK_SAND, "sand", "sand", "sand", 0.012f }, { BLOCK_GRAVEL, "gravel", "gravel", "gravel", 0.035f },
        { BLOCK_STONE, "stone", "stone", "stone", 0.045f }, { BLOCK_SLATE, "slate", "slate", "slate", 0.04f },
        { BLOCK_SANDSTONE, "sandstone", "sandstone_top", "sandstone_layered", 0.015f }, { BLOCK_SNOW, "snow", "snow", "snow", 0.02f },
        { BLOCK_FOUNDATION, "foundation", "stone", "stone", 0.0f },
    };
    for (auto& m : ms) if (!SetMaterial(m.id, m.name, m.top, m.side, m.bump)) return 1;
    // A 12 x 12-chunk region from the column at (ox, oz).
    const int CH = 12, ox = -6, oz = -6;
    WX = CH * 16; WZ = CH * 16; WY = 72;
    g_cells.assign((size_t)WX * WY * WZ, 0);
    for (int cz = 0; cz < CH; cz++)
        for (int cx = 0; cx < CH; cx++) {
            TerrainColumn t; HillsColumn(seed, ox + cx, oz + cz, t, HILLS_LATEST);
            for (int cy = 0; cy < t.chunks; cy++)
                for (int ly = 0; ly < 16; ly++) {
                    int y = cy * 16 + ly;
                    if (y >= WY) continue;
                    for (int lz = 0; lz < 16; lz++)
                        for (int lx = 0; lx < 16; lx++)
                            Cell(cx * 16 + lx, y, cz * 16 + lz) = t.cells[(size_t)cy * CHUNK_CELLS + (ly * 16 + lz) * 16 + lx];
                }
        }
    // Cameras: a few spots, looking across; heights from the terrain.
    auto groundAt = [&](int x, int z) { return (float)HillsHeight(seed, x + ox * 16, z + oz * 16); };
    FILE* stats = fopen((out + "/hills_stats.txt").c_str(), "w");
    if (!stats) return 1;
    struct Spot { const char* name; float x, z, tx, tz, up, t; };
    const Spot spots[] = {
        { "hills_a_morning", 40, 40, 110, 120, 7, 500 }, { "hills_b_noon", 150, 40, 80, 130, 7, 1500 },
        { "hills_c_evening", 96, 170, 90, 60, 7, 2750 }, { "hills_d_walk", 60, 100, 100, 110, 1.7f, 2300 },
    };
    for (auto& sp : spots) {
        View v = { sp.name, sp.name, { sp.x, groundAt((int)sp.x, (int)sp.z) + sp.up, sp.z },
                   { sp.tx, groundAt((int)sp.tx, (int)sp.tz) + 1, sp.tz }, sp.t, 0 };
        Render(v, out, stats);
    }
    fclose(stats);
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "usage: facet_preview OUTDIR [view]\n"); return 1; }
    std::string out = argv[1], only = argc > 2 ? argv[2] : "";
    std::string texDir = TEXDIR;
    if (!LoadTextures(texDir)) return 1;
    if (only == "sheets") return Sheets(out);
    if (argc > 4) g_soften = (float)atof(argv[4]); // hills SEED SOFTEN: 0 = the look before D46
    if (only == "hills") return Hills(out, argc > 3 ? strtoull(argv[3], nullptr, 10) : 1);
    for (int i = 1; i < M_COUNT; i++)
        if (!SetMaterial(i, kMats[i].name, kMats[i].top, kMats[i].side, kMats[i].bump)) return 1;
    BuildWorld();
    const float NOON = 1500, DAWN = 140, LATE = 2700;
    std::vector<View> views = {
        { "meadow", "Rolling meadow, noon", { 40, 34, 40 }, { 80, 22, 88 }, NOON, 0 },
        { "meadow_late", "The same, late afternoon", { 40, 34, 40 }, { 80, 22, 88 }, LATE, 0 },
        { "cliff_dawn", "Sandstone cliff and overhang, dawn", { 88, 30, 58 }, { 118, 30, 70 }, DAWN, 0 },
        { "pit", "A dug pit", { 60, 30, 58 }, { 72, 15, 70 }, 1100, 0 },
        { "borders", "Where materials meet, close up", { 36, 27, 104 }, { 48, 20, 118 }, 1100, 0 },
        { "detail_selective", "Triangles: cut only where it shows", { 40, 34, 40 }, { 80, 22, 88 }, NOON, 1 },
        { "detail_blanket", "Triangles: cut everywhere near", { 40, 34, 40 }, { 80, 22, 88 }, NOON, 1, false },
        { "cubes", "The old look, same spot, for comparison", { 40, 34, 40 }, { 80, 22, 88 }, NOON, 0, true, false, true },
    };
    // A camera of your own: OUTDIR custom ex ey ez tx ty tz daytime [edges]
    if (only == "custom" && argc >= 10) {
        View c = { "custom", "Custom view", { (float)atof(argv[3]), (float)atof(argv[4]), (float)atof(argv[5]) },
                   { (float)atof(argv[6]), (float)atof(argv[7]), (float)atof(argv[8]) }, (float)atof(argv[9]), argc > 10 ? atoi(argv[10]) : 0 };
        views = { c };
    }
    std::string statsPath = out + "/stats.txt";
    FILE* stats = fopen(statsPath.c_str(), only.empty() ? "w" : "a");
    if (!stats) { fprintf(stderr, "can't write %s\n", statsPath.c_str()); return 1; }
    for (auto& v : views)
        if (only.empty() || only == v.name) Render(v, out, stats);
    fclose(stats);
    return 0;
}
