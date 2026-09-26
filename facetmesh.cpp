// facetmesh.cpp -- see facetmesh.h (DESIGN.md Part XXIII).
//
// Layer 3 (world). Cost: one pass over the solid cells of the box; each
// surface corner is worked out once (cached), each face's level once.
// Points on shared edges are always computed from the edge's two corners
// in a fixed order, which is what keeps neighbouring faces crack-free.

#include "facetmesh.h"
#include <algorithm>
#include <cstring>
#include <unordered_map>

namespace {

uint32_t Hash3(int x, int y, int z, uint32_t salt) {
    uint32_t h = (uint32_t)x * 0x8da6b343u ^ (uint32_t)y * 0xd8163841u ^ (uint32_t)z * 0xcb1ab31fu ^ salt * 0x9e3779b9u;
    h ^= h >> 16; h *= 0x7feb352du; h ^= h >> 15; h *= 0x846ca68bu; h ^= h >> 16;
    return h;
}
float Hash01(int x, int y, int z, uint32_t salt) { return (float)(Hash3(x, y, z, salt) >> 8) * (1.0f / 16777216.0f); }

// Smooth value noise, 0..1, from a world position alone.
float Noise3(Vec3 p) {
    float fx = floorf(p.x), fy = floorf(p.y), fz = floorf(p.z);
    int ix = (int)fx, iy = (int)fy, iz = (int)fz;
    float tx = p.x - fx, ty = p.y - fy, tz = p.z - fz;
    tx = tx * tx * (3 - 2 * tx); ty = ty * ty * (3 - 2 * ty); tz = tz * tz * (3 - 2 * tz);
    auto h = [&](int dx, int dy, int dz) { return Hash01(ix + dx, iy + dy, iz + dz, 71); };
    float a = h(0, 0, 0) + (h(1, 0, 0) - h(0, 0, 0)) * tx;
    float b = h(0, 1, 0) + (h(1, 1, 0) - h(0, 1, 0)) * tx;
    float c = h(0, 0, 1) + (h(1, 0, 1) - h(0, 0, 1)) * tx;
    float d = h(0, 1, 1) + (h(1, 1, 1) - h(0, 1, 1)) * tx;
    float e = a + (b - a) * ty, f = c + (d - c) * ty;
    return e + (f - e) * tz;
}

// Up to four materials with weights.
struct MatW {
    uint8_t id[4] = {};
    float w[4] = {};
    int n = 0;
    void Add(uint8_t m, float x) {
        for (int i = 0; i < n; i++) if (id[i] == m) { w[i] += x; return; }
        if (n < 4) { id[n] = m; w[n] = x; n++; return; }
        int lo = 0; // full: replace the weakest if this is stronger
        for (int i = 1; i < 4; i++) if (w[i] < w[lo]) lo = i;
        if (x > w[lo]) { id[lo] = m; w[lo] = x; }
    }
};
MatW LerpW(const MatW& a, const MatW& b, float t) {
    MatW r;
    for (int i = 0; i < a.n; i++) r.Add(a.id[i], a.w[i] * (1 - t));
    for (int i = 0; i < b.n; i++) r.Add(b.id[i], b.w[i] * t);
    return r;
}

// Everything a point of the surface carries.
struct Attr {
    Vec3 pos, n;
    float bump, ao, sky;
    MatW mw;
};
Attr LerpA(const Attr& a, const Attr& b, float t) {
    Attr r;
    r.pos = a.pos + (b.pos - a.pos) * t;
    r.n = a.n + (b.n - a.n) * t;
    r.bump = a.bump + (b.bump - a.bump) * t;
    r.ao = a.ao + (b.ao - a.ao) * t;
    r.sky = a.sky + (b.sky - a.sky) * t;
    r.mw = LerpW(a.mw, b.mw, t);
    return r;
}

struct I3 { int x, y, z; };
inline I3 operator+(I3 a, I3 b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
inline bool Less(I3 a, I3 b) { return a.x != b.x ? a.x < b.x : a.y != b.y ? a.y < b.y : a.z < b.z; }
const I3 kAxis[3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
// The two axes across each face normal, ordered so u x v = +normal.
const int kU[3] = { 1, 2, 0 }, kV[3] = { 2, 0, 1 };
inline int Comp(I3 p, int a) { return a == 0 ? p.x : a == 1 ? p.y : p.z; }

// Where the surface crosses around corner (cx, cy, cz), before smoothing
// and jitter: the mean of the solid/empty crossings, as an offset from the
// lattice point. False when the corner isn't on the surface.
bool BaseOffset(const FacetGrid& g, int cx, int cy, int cz, Vec3* off, Vec3* normal) {
    bool sol[2][2][2];
    int nSolid = 0;
    for (int dy = 0; dy < 2; dy++)
        for (int dz = 0; dz < 2; dz++)
            for (int dx = 0; dx < 2; dx++) {
                sol[dx][dy][dz] = g.Solid(cx - 1 + dx, cy - 1 + dy, cz - 1 + dz);
                nSolid += sol[dx][dy][dz];
            }
    if (nSolid == 0 || nSolid == 8) return false;
    // Midpoints of the 12 edges joining neighbouring cell centres (+-0.5)
    // where solid meets empty.
    float sx = 0, sy = 0, sz = 0; int cross = 0;
    for (int a = 0; a < 2; a++)
        for (int b = 0; b < 2; b++) {
            float oa = a - 0.5f, ob = b - 0.5f;
            if (sol[0][a][b] != sol[1][a][b]) { sy += oa; sz += ob; cross++; } // along x
            if (sol[a][0][b] != sol[a][1][b]) { sx += oa; sz += ob; cross++; } // along y
            if (sol[a][b][0] != sol[a][b][1]) { sx += oa; sy += ob; cross++; } // along z
        }
    *off = { sx / cross, sy / cross, sz / cross };
    if (normal) {
        Vec3 nrm = { 0, 0, 0 };
        for (int dy = 0; dy < 2; dy++)
            for (int dz = 0; dz < 2; dz++)
                for (int dx = 0; dx < 2; dx++) {
                    float k = sol[dx][dy][dz] ? -1.0f : 1.0f; // toward the empty cells
                    nrm = nrm + Vec3{ (dx - 0.5f) * k, (dy - 0.5f) * k, (dz - 0.5f) * k };
                }
        *normal = Normalize(nrm);
        if (Dot(*normal, *normal) < 0.5f) *normal = { 0, 1, 0 }; // symmetric (a checkerboard): any is as good
    }
    return true;
}

// The corner position (FacetCorner and the mesher's cache share it).
bool CornerPos(const FacetGrid& g, const FacetShape& s, int cx, int cy, int cz, Vec3* pos, Vec3* normal) {
    Vec3 off, n;
    if (!BaseOffset(g, cx, cy, cz, &off, &n)) return false;
    if (normal) *normal = n;
    if (!s.relax) { *pos = { (float)cx, (float)cy, (float)cz }; return true; }
    // One gentle smoothing pass toward the neighbouring surface corners:
    // an inner corner of a terrace otherwise leaves a steep, dark pocket
    // (the first pictures). One pass only -- more rounds the ground off.
    if (s.smooth > 0) {
        static const int nb[6][3] = { { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 } };
        Vec3 sum = { 0, 0, 0 }; int k = 0;
        for (auto& d : nb) {
            Vec3 o;
            if (!BaseOffset(g, cx + d[0], cy + d[1], cz + d[2], &o, nullptr)) continue;
            sum = sum + Vec3{ d[0] + o.x, d[1] + o.y, d[2] + o.z };
            k++;
        }
        if (k > 0) off = off + (sum * (1.0f / k) - off) * s.smooth;
    }
    // Gentle ground rolls (D62): where the 4 x 4 columns around an upward
    // corner span 2 cells or less, the corner eases toward their mean
    // ground height, so a slope of one-cell steps becomes a slope instead
    // of treads and risers. Only cells within 2 of the corner are read
    // (the grid's margin), so neighbouring boxes agree exactly.
    if (s.terrace > 0 && n.y > 0.5f) {
        float sum = 0; int k = 0, lo = 1 << 30, hi = -(1 << 30);
        for (int dz = -2; dz <= 1; dz++)
            for (int dx = -2; dx <= 1; dx++)
                for (int y = cy; y >= cy - 2; y--) { // the column's ground: a solid cell with open air above
                    if (!g.Solid(cx + dx, y, cz + dz)) continue;
                    if (!g.Solid(cx + dx, y + 1, cz + dz)) { sum += (float)(y + 1); k++; lo = std::min(lo, y + 1); hi = std::max(hi, y + 1); }
                    break;
                }
        if (k >= 12 && hi - lo <= 2) {
            float target = sum / k - (float)cy;
            off.y += (target - off.y) * s.terrace;
        }
    }
    // The seeded nudge that makes facets vary: mostly in or out along the
    // corner's normal. A sideways nudge shears a step's riser until its top
    // leans out over its bottom -- a dark, downward pocket (M1.2 pictures) --
    // so only a little of it. Kept inside the corner's own cube, so
    // neighbouring facets can never fold through each other.
    // On ground that faces up, straight up or down: a vertical nudge can't
    // tip a riser either way.
    Vec3 jd = n.y > 0.3f ? Vec3{ 0, 1, 0 } : n;
    off = off + jd * ((Hash01(cx, cy, cz, 4) * 2 - 1) * s.jitter);
    off.x += (Hash01(cx, cy, cz, 1) * 2 - 1) * s.jitterSide;
    off.y += (Hash01(cx, cy, cz, 2) * 2 - 1) * s.jitterSide;
    off.z += (Hash01(cx, cy, cz, 3) * 2 - 1) * s.jitterSide;
    off.x = std::max(-0.45f, std::min(0.45f, off.x));
    off.y = std::max(-0.45f, std::min(0.45f, off.y));
    off.z = std::max(-0.45f, std::min(0.45f, off.z));
    *pos = { cx + off.x, cy + off.y, cz + off.z };
    return true;
}

// The base split of a quad a, b, c, d (in winding order) facing `n`: of
// the two folds, one that turns neither triangle inside out (a twisted
// quad at a step's corner can), preferring the one that bulges outward.
// True: split along a-c; false: along b-d. Shared by the mesher and
// FacetBaseFace, so what's drawn and what's collided with agree.
bool FoldAC(Vec3 a, Vec3 b, Vec3 c, Vec3 d, Vec3 n) {
    auto outward = [&](Vec3 x, Vec3 y, Vec3 z) { return Dot(Cross(y - x, z - x), n) > 0; };
    bool okAC = outward(a, b, c) && outward(a, c, d);
    bool okBD = outward(a, b, d) && outward(b, c, d);
    Vec3 m1 = (a + c) * 0.5f, m2 = (b + d) * 0.5f;
    bool useAC = Dot(m1 - m2, n) >= 0;
    if (useAC && !okAC && okBD) useAC = false;
    else if (!useAC && !okBD && okAC) useAC = true;
    return useAC;
}

struct Builder {
    const FacetGrid& g;
    const FacetBuildParams& p;
    FacetMesh& out;
    // Caches over the grid: corners (lattice points) and face levels.
    int lx, ly, lz;
    std::vector<int8_t> cornerState;   // 0 unknown, 1 surface, 2 not
    std::vector<Attr> corner;
    std::vector<int8_t> faceLevel;     // -1 unknown
    struct Key {
        uint32_t px, py, pz;
        uint8_t m[3], w[3];
        bool operator==(const Key& o) const { return memcmp(this, &o, sizeof(Key)) == 0; }
    };
    struct KeyHash {
        size_t operator()(const Key& k) const {
            uint64_t h = 1469598103934665603ull;
            const uint8_t* b = (const uint8_t*)&k;
            for (size_t i = 0; i < sizeof(Key); i++) { h ^= b[i]; h *= 1099511628211ull; }
            return (size_t)h;
        }
    };
    std::unordered_map<Key, uint32_t, KeyHash> dedup;
    Vec3 faceN = { 0, 1, 0 };   // the direction of the face being cut

    Builder(const FacetGrid& g_, const FacetBuildParams& p_, FacetMesh& o) : g(g_), p(p_), out(o) {
        lx = g.nx + 1; ly = g.ny + 1; lz = g.nz + 1;
        cornerState.assign((size_t)lx * ly * lz, 0);
        corner.resize((size_t)lx * ly * lz);
        faceLevel.assign((size_t)g.nx * g.ny * g.nz * 6, -1);
    }

    // By value: a corner outside the cache (only if a caller pads too
    // little) is worked out afresh, and must not overwrite another.
    Attr Corner(I3 c) {
        int x = c.x - g.x0, y = c.y - g.y0, z = c.z - g.z0;
        bool inside = (unsigned)x < (unsigned)lx && (unsigned)y < (unsigned)ly && (unsigned)z < (unsigned)lz;
        size_t i = inside ? ((size_t)y * lz + z) * lx + x : 0;
        if (inside && cornerState[i] != 0) return corner[i];
        Attr a;
        bool on = CornerPos(g, p.shape, c.x, c.y, c.z, &a.pos, &a.n);
        if (!on) { a.pos = { (float)c.x, (float)c.y, (float)c.z }; a.n = { 0, 1, 0 }; }
        // Lumpiness and materials from the solid cells around; openness
        // from a 4x4x4 block (flat ground is half solid: fully open).
        int nSolid = 0; float bump = 0;
        for (int dy = 0; dy < 2; dy++)
            for (int dz = 0; dz < 2; dz++)
                for (int dx = 0; dx < 2; dx++) {
                    uint8_t m = g.At(c.x - 1 + dx, c.y - 1 + dy, c.z - 1 + dz);
                    if (!g.mats[m].solid) continue;
                    nSolid++; bump += g.mats[m].bump;
                    a.mw.Add(m, 1.0f);
                }
        a.bump = nSolid ? bump / nSolid * p.shape.bumpScale : 0.0f;
        for (int k = 0; k < a.mw.n; k++) a.mw.w[k] /= (float)std::max(1, nSolid);
        int around = 0;
        for (int dy = -2; dy < 2; dy++)
            for (int dz = -2; dz < 2; dz++)
                for (int dx = -2; dx < 2; dx++) around += g.Solid(c.x + dx, c.y + dy, c.z + dz);
        float f = around / 64.0f;
        a.ao = std::max(0.3f, std::min(1.0f, 1.0f - std::max(0.0f, f - 0.5f) * 2.2f));
        a.sky = p.sky ? std::max(0.0f, std::min(1.0f, p.sky(c.x, c.y, c.z, p.user))) : 1.0f;
        if (inside) { cornerState[i] = on ? 1 : 2; corner[i] = a; }
        return a;
    }

    // Does detail show on this face? Cached per (cell, direction).
    int FaceLevel(I3 cell, int axis, int sign) {
        int x = cell.x - g.x0, y = cell.y - g.y0, z = cell.z - g.z0;
        bool inside = (unsigned)x < (unsigned)g.nx && (unsigned)y < (unsigned)g.ny && (unsigned)z < (unsigned)g.nz;
        size_t i = inside ? (((size_t)y * g.nz + z) * g.nx + x) * 6 + axis * 2 + (sign > 0) : 0;
        if (inside && faceLevel[i] >= 0) return faceLevel[i];
        int level = ComputeFaceLevel(cell, axis, sign);
        if (inside) faceLevel[i] = (int8_t)level;
        return level;
    }
    int ComputeFaceLevel(I3 cell, int axis, int sign, int bandOverride = -1) {
        I3 base = sign > 0 ? cell + kAxis[axis] : cell;
        int u = kU[axis], v = kV[axis];
        Vec3 centre = { base.x + 0.5f * (kAxis[u].x + kAxis[v].x), base.y + 0.5f * (kAxis[u].y + kAxis[v].y),
                        base.z + 0.5f * (kAxis[u].z + kAxis[v].z) };
        int band = bandOverride >= 0 ? bandOverride : p.band ? std::max(0, std::min(2, p.band(centre, p.user))) : 0;
        if (band == 0 || !p.selective) return band;
        // Detail is cut only where it adds shape: lumpy materials, finest
        // for the lumpiest. Creases and borders stay whole -- a cut crease
        // turns into a curved patch (blobby, not faceted), and borders are
        // blended per pixel, not by geometry (M1.2 pictures).
        uint8_t own = g.At(cell.x, cell.y, cell.z);
        float bump = g.mats[own].bump * p.shape.bumpScale;
        if (bump >= p.shape.bumpFine) return band;
        if (bump >= p.shape.bumpSome) return std::min(band, 1);
        return 0;
    }

    // How many pieces the lattice edge from `from` along `axis` is cut
    // into: as many as the finest face that uses it needs.
    // On the build box's boundary (the edge's lattice line lies in one of
    // its faces): with stitchBox, cut as if at the finest band.
    bool OnBoxBoundary(I3 from, int axis) const {
        for (int b = 0; b < 3; b++) {
            if (b == axis) continue;
            int c = Comp(from, b);
            int lo = b == 0 ? p.bx0 : b == 1 ? p.by0 : p.bz0, hi = b == 0 ? p.bx1 : b == 1 ? p.by1 : p.bz1;
            if (c == lo || c == hi) return true;
        }
        return false;
    }
    int EdgeSegments(I3 from, int axis) {
        int f = kU[axis], gg = kV[axis];
        int level = 0;
        const bool boundary = p.stitchBox && OnBoxBoundary(from, axis);
        // The four cells around the edge, indexed by their offsets across it.
        auto cellAt = [&](int df, int dg) {
            I3 c = from;
            I3 o = { 0, 0, 0 };
            if (f == 0) o.x = df; else if (f == 1) o.y = df; else o.z = df;
            if (gg == 0) o.x = dg; else if (gg == 1) o.y = dg; else o.z = dg;
            return c + o;
        };
        for (int k = -1; k <= 0; k++) {
            // Across f (faces with normal f), then across g.
            for (int which = 0; which < 2; which++) {
                I3 c0 = which == 0 ? cellAt(-1, k) : cellAt(k, -1);
                I3 c1 = which == 0 ? cellAt(0, k) : cellAt(k, 0);
                int nAxis = which == 0 ? f : gg;
                bool s0 = g.Solid(c0.x, c0.y, c0.z), s1 = g.Solid(c1.x, c1.y, c1.z);
                if (s0 == s1) continue;
                if (boundary) level = std::max(level, s0 ? ComputeFaceLevel(c0, nAxis, +1, 2) : ComputeFaceLevel(c1, nAxis, -1, 2));
                else level = std::max(level, s0 ? FaceLevel(c0, nAxis, +1) : FaceLevel(c1, nAxis, -1));
            }
        }
        return 1 << level;
    }

    // A point on the lattice edge lo..hi, k of m along: computed from the
    // edge alone, in the edge's own order, so both faces get the same bits.
    Attr EdgePoint(I3 a, I3 b, int k, int m) {
        if (Less(b, a)) { std::swap(a, b); k = m - k; }
        if (k == 0) return Corner(a);
        if (k == m) return Corner(b);
        float t = (float)k / (float)m;
        Attr ca = Corner(a), cb = Corner(b);
        Attr r = LerpA(ca, cb, t);
        Displace(r, (1.0f - fabsf(2.0f * t - 1.0f)) * Calm(ca.n, cb.n));
        return r;
    }
    // Lumps only where the ground is calm: they fade out where the corners'
    // normals disagree (a step's corner, a crease), which keeps those edges
    // sharp and stops a lump pushed along a steep normal folding a small
    // triangle over. From the corners alone, so shared edges agree.
    static float Calm(Vec3 a, Vec3 b) {
        float d = Dot(a, b);
        float t = (d - 0.72f) / 0.2f;
        return t < 0 ? 0.0f : t > 1 ? 1.0f : t;
    }
    void Displace(Attr& r, float taper) {
        r.n = Normalize(r.n);
        if (Dot(r.n, r.n) < 0.5f) r.n = { 0, 1, 0 };
        if (r.bump <= 0 || taper <= 0) return;
        float s = p.shape.bumpFreq;
        float d = r.bump * (2.0f * Noise3({ r.pos.x * s, r.pos.y * s, r.pos.z * s }) - 1.0f) * taper;
        r.pos = r.pos + r.n * d;
    }

    uint32_t Emit(const Attr& a, const uint8_t mats[3], int nm, const float wsum[3]) {
        FacetVertex v;
        v.pos = a.pos; v.normal = a.n;
        float w[3] = { 0, 0, 0 }, tot = 0;
        for (int i = 0; i < nm; i++) {
            for (int k = 0; k < a.mw.n; k++) if (a.mw.id[k] == mats[i]) w[i] = a.mw.w[k];
            tot += w[i];
        }
        (void)wsum;
        if (tot <= 0) { w[0] = 1; tot = 1; }
        int w0 = (int)(w[0] / tot * 255.0f + 0.5f), w1 = (int)(w[1] / tot * 255.0f + 0.5f);
        w0 = std::min(255, w0); w1 = std::min(255 - w0, w1);
        v.mat[0] = mats[0]; v.mat[1] = nm > 1 ? mats[1] : mats[0]; v.mat[2] = nm > 2 ? mats[2] : mats[0];
        v.w[0] = (uint8_t)w0; v.w[1] = (uint8_t)w1; v.w[2] = (uint8_t)(255 - w0 - w1);
        v.ao = (uint8_t)(a.ao * 255.0f + 0.5f);
        v.sky = (uint8_t)(a.sky * 255.0f + 0.5f);
        Key key;
        memset(&key, 0, sizeof(key));
        memcpy(&key.px, &v.pos.x, 4); memcpy(&key.py, &v.pos.y, 4); memcpy(&key.pz, &v.pos.z, 4);
        memcpy(key.m, v.mat, 3); memcpy(key.w, v.w, 3);
        auto it = dedup.find(key);
        if (it != dedup.end()) return it->second;
        uint32_t id = (uint32_t)out.verts.size();
        out.verts.push_back(v);
        dedup.emplace(key, id);
        return id;
    }

    // A face's triangles are collected first, so a face that would fold can
    // be cut again with its interior left flat (below).
    struct Pending { Attr a, b, c; };
    std::vector<Pending> pending;
    float interiorScale = 1.0f;
    void Tri(const Attr& a, const Attr& b, const Attr& c) { pending.push_back({ a, b, c }); }
    bool Folds(const Pending& t) const { return Dot(Cross(t.b.pos - t.a.pos, t.c.pos - t.a.pos), faceN) <= 0; }

    // One triangle: its three strongest materials, shared by its vertices.
    void EmitTri(const Attr& a, const Attr& b, const Attr& c) {
        MatW sum;
        for (const Attr* x : { &a, &b, &c })
            for (int k = 0; k < x->mw.n; k++) sum.Add(x->mw.id[k], x->mw.w[k]);
        uint8_t mats[3]; float ws[3]; int nm = 0;
        bool used[4] = {};
        for (; nm < 3 && nm < sum.n; nm++) {
            int best = -1;
            for (int k = 0; k < sum.n; k++)
                if (!used[k] && (best < 0 || sum.w[k] > sum.w[best] || (sum.w[k] == sum.w[best] && sum.id[k] < sum.id[best]))) best = k;
            used[best] = true; mats[nm] = sum.id[best]; ws[nm] = sum.w[best];
        }
        if (nm == 0) { mats[0] = 0; ws[0] = 1; nm = 1; }
        out.idx.push_back(Emit(a, mats, nm, ws));
        out.idx.push_back(Emit(b, mats, nm, ws));
        out.idx.push_back(Emit(c, mats, nm, ws));
    }

    void Face(I3 cell, int axis, int sign) {
        I3 base = sign > 0 ? cell + kAxis[axis] : cell;
        int u = kU[axis], v = kV[axis];
        // Corners in order, so (B - A) x (D - A) points out of the ground.
        I3 A = base, B, C = base + kAxis[u] + kAxis[v], D;
        if (sign > 0) { B = base + kAxis[u]; D = base + kAxis[v]; }
        else          { B = base + kAxis[v]; D = base + kAxis[u]; }
        int level = FaceLevel(cell, axis, sign);
        faceN = FaceNormal(axis, sign);
        out.baseQuads++;
        out.quadsAtLevel[level]++;
        pending.clear();
        interiorScale = 1.0f;
        CutFace(A, B, C, D, level, axis, sign);
        if (level > 0) {
            // Lumps pushed along a strongly tilted normal can fold a small
            // cell over (seen with jitter). Cut it again with the interior
            // flat; the edges are shared, so they stay exactly as they were.
            bool folds = false;
            for (auto& t : pending) folds |= Folds(t);
            if (folds) { pending.clear(); interiorScale = 0.0f; CutFace(A, B, C, D, level, axis, sign); }
        }
        for (auto& t : pending) { if (Folds(t)) out.folded++; EmitTri(t.a, t.b, t.c); }
    }
    void CutFace(I3 A, I3 B, I3 C, I3 D, int level, int axis, int sign) {
        (void)axis; (void)sign;
        int n = 1 << level;
        // The base split: of the two folds, one that turns neither triangle
        // inside out (a twisted quad at a step's corner can), preferring
        // the one that bulges outward.
        Attr ca = Corner(A), cb = Corner(B), cc = Corner(C), cd = Corner(D);
        bool useAC = FoldAC(ca.pos, cb.pos, cc.pos, cd.pos, faceN);
        diagLo = useAC ? A : B; diagHi = useAC ? C : D;
        if (Less(diagHi, diagLo)) std::swap(diagLo, diagHi);
        diagN = n;
        // Each base triangle is cut linearly (so it can't fold, and stays
        // flat but for the lumps), its edges as finely as their faces need.
        if (useAC) { CutTri(A, B, C, n); CutTri(A, C, D, n); }
        else       { CutTri(A, B, D, n); CutTri(B, C, D, n); }
    }
    // The current face's diagonal: points on it are the face's own, shared
    // by its two triangles, computed in the diagonal's fixed order.
    I3 diagLo = { 0, 0, 0 }, diagHi = { 0, 0, 0 };
    int diagN = 1;
    bool IsDiag(I3 p, I3 q) const {
        return (p.x == diagLo.x && p.y == diagLo.y && p.z == diagLo.z && q.x == diagHi.x && q.y == diagHi.y && q.z == diagHi.z) ||
               (q.x == diagLo.x && q.y == diagLo.y && q.z == diagLo.z && p.x == diagHi.x && p.y == diagHi.y && p.z == diagHi.z);
    }
    int Segs(I3 p, I3 q) { return IsDiag(p, q) ? diagN : EdgeSeg(p, q); }
    // k of m along the edge p..q (either a quad edge or the diagonal).
    Attr EdgeAt(I3 p, I3 q, int k, int m) {
        if (!IsDiag(p, q)) return EdgePoint(p, q, k, m);
        if (Less(q, p)) { std::swap(p, q); k = m - k; }
        if (k == 0) return Corner(p);
        if (k == m) return Corner(q);
        float t = (float)k / (float)m;
        Attr cp = Corner(p), cq = Corner(q);
        Attr r = LerpA(cp, cq, t);
        Displace(r, (1.0f - fabsf(2.0f * t - 1.0f)) * interiorScale * Calm(cp.n, cq.n));
        return r;
    }
    // Inside base triangle X, Y, Z at barycentric weights (1 - s - t, s, t).
    Attr Inside(I3 X, I3 Y, I3 Z, float s, float t) {
        Attr ax = Corner(X), ay = Corner(Y), az = Corner(Z);
        Attr r = LerpA(ax, ay, s / std::max(1e-6f, s + (1 - s - t)));
        r = LerpA(r, az, t);
        // Zero at the corners, and equal to the edges' own taper along them.
        float l = std::max(1 - s - t, std::max(s, t));
        float calm = std::min(Calm(ax.n, ay.n), std::min(Calm(ay.n, az.n), Calm(ax.n, az.n)));
        Displace(r, std::min(1.0f, 2.0f * (1.0f - l)) * interiorScale * calm);
        return r;
    }
    void CutTri(I3 X, I3 Y, I3 Z, int n) {
        int mXY = Segs(X, Y), mYZ = Segs(Y, Z), mXZ = Segs(X, Z);
        auto P = [&](int i, int j) -> Attr {   // Y weight i/n, Z weight j/n
            if (j == 0) return EdgeAt(X, Y, i * (mXY / n), mXY);
            if (i == 0) return EdgeAt(X, Z, j * (mXZ / n), mXZ);
            if (i + j == n) return EdgeAt(Y, Z, j * (mYZ / n), mYZ);
            return Inside(X, Y, Z, (float)i / n, (float)j / n);
        };
        std::vector<Attr> ring;
        for (int j = 0; j < n; j++)
            for (int i = 0; i + j < n; i++) {
                Attr v0 = P(i, j), v1 = P(i + 1, j), v2 = P(i, j + 1);
                bool onXY = j == 0 && mXY > n, onYZ = i + j == n - 1 && mYZ > n, onXZ = i == 0 && mXZ > n;
                if (!onXY && !onYZ && !onXZ) Tri(v0, v1, v2);
                else {
                    // Its side lies on an edge cut finer: a fan from its
                    // centre through every point around it (no T-junctions).
                    ring.clear();
                    ring.push_back(v0);
                    if (onXY) { int r = mXY / n; for (int k = i * r + 1; k < i * r + r; k++) ring.push_back(EdgeAt(X, Y, k, mXY)); }
                    ring.push_back(v1);
                    if (onYZ) { int r = mYZ / n; for (int k = j * r + 1; k < j * r + r; k++) ring.push_back(EdgeAt(Y, Z, k, mYZ)); }
                    ring.push_back(v2);
                    if (onXZ) { int r = mXZ / n; for (int k = (j + 1) * r - 1; k > j * r; k--) ring.push_back(EdgeAt(X, Z, k, mXZ)); }
                    Attr c = Inside(X, Y, Z, (i + 1.0f / 3) / n, (j + 1.0f / 3) / n);
                    for (size_t k = 0; k < ring.size(); k++) Tri(c, ring[k], ring[(k + 1) % ring.size()]);
                }
                if (i + j < n - 1) Tri(P(i + 1, j), P(i + 1, j + 1), P(i, j + 1)); // the downward cell beside it
            }
    }
    static Vec3 FaceNormal(int axis, int sign) {
        Vec3 n = { 0, 0, 0 };
        if (axis == 0) n.x = (float)sign; else if (axis == 1) n.y = (float)sign; else n.z = (float)sign;
        return n;
    }
    int EdgeSeg(I3 a, I3 b) {
        I3 lo = Less(a, b) ? a : b, hi = Less(a, b) ? b : a;
        int axis = hi.x != lo.x ? 0 : hi.y != lo.y ? 1 : 2;
        return EdgeSegments(lo, axis);
    }
    void Run() {
        for (int y = p.by0; y < p.by1; y++)
            for (int z = p.bz0; z < p.bz1; z++)
                for (int x = p.bx0; x < p.bx1; x++) {
                    if (!g.Solid(x, y, z)) continue;
                    for (int axis = 0; axis < 3; axis++)
                        for (int sign = -1; sign <= 1; sign += 2) {
                            I3 nb = { x, y, z };
                            if (axis == 0) nb.x += sign; else if (axis == 1) nb.y += sign; else nb.z += sign;
                            if (g.Solid(nb.x, nb.y, nb.z)) continue;
                            Face({ x, y, z }, axis, sign);
                        }
                }
    }
};

} // namespace

bool FacetCorner(const FacetGrid& g, const FacetShape& s, int cx, int cy, int cz, Vec3* pos) {
    return CornerPos(g, s, cx, cy, cz, pos, nullptr);
}

void FacetBaseFace(const FacetGrid& g, const FacetShape& s, int x, int y, int z, int axis, int sign, Vec3 tri[2][3]) {
    I3 cell = { x, y, z };
    I3 base = sign > 0 ? cell + kAxis[axis] : cell;
    int u = kU[axis], v = kV[axis];
    I3 A = base, B, C = base + kAxis[u] + kAxis[v], D;
    if (sign > 0) { B = base + kAxis[u]; D = base + kAxis[v]; }
    else          { B = base + kAxis[v]; D = base + kAxis[u]; }
    Vec3 p[4];
    I3 q[4] = { A, B, C, D };
    for (int k = 0; k < 4; k++)
        if (!CornerPos(g, s, q[k].x, q[k].y, q[k].z, &p[k], nullptr)) p[k] = { (float)q[k].x, (float)q[k].y, (float)q[k].z };
    Vec3 n = { 0, 0, 0 };
    if (axis == 0) n.x = (float)sign; else if (axis == 1) n.y = (float)sign; else n.z = (float)sign;
    if (FoldAC(p[0], p[1], p[2], p[3], n)) {
        tri[0][0] = p[0]; tri[0][1] = p[1]; tri[0][2] = p[2];
        tri[1][0] = p[0]; tri[1][1] = p[2]; tri[1][2] = p[3];
    } else {
        tri[0][0] = p[0]; tri[0][1] = p[1]; tri[0][2] = p[3];
        tri[1][0] = p[1]; tri[1][1] = p[2]; tri[1][2] = p[3];
    }
}

void FacetBuild(const FacetGrid& g, const FacetBuildParams& p, FacetMesh& out) {
    out = FacetMesh();
    Builder b(g, p, out);
    b.Run();
}

uint16_t FacetOpenings(const FacetGrid& g, int x0, int y0, int z0, int size) {
    const int n = size, n3 = n * n * n;
    auto open = [&](int x, int y, int z) {
        return !g.mats[g.cells[((size_t)(y0 + y) * g.nz + (z0 + z)) * g.nx + (x0 + x)]].solid;
    };
    // Scratch reused per thread: a fill label per cell (0 unvisited).
    thread_local std::vector<uint8_t> seen;
    thread_local std::vector<int> stack;
    seen.assign((size_t)n3, 0);
    uint16_t out = 0;
    for (int start = 0; start < n3; start++) {
        if (seen[start]) continue;
        int sx = start % n, sz = (start / n) % n, sy = start / (n * n);
        if (!open(sx, sy, sz)) { seen[start] = 1; continue; }
        // One connected pocket of open cells: which faces it touches.
        int faces = 0;
        stack.clear(); stack.push_back(start); seen[start] = 1;
        while (!stack.empty()) {
            int i = stack.back(); stack.pop_back();
            int x = i % n, z = (i / n) % n, y = i / (n * n);
            if (x == 0) faces |= 1;
            if (x == n - 1) faces |= 2;
            if (y == 0) faces |= 4;
            if (y == n - 1) faces |= 8;
            if (z == 0) faces |= 16;
            if (z == n - 1) faces |= 32;
            const int nb[6][3] = { { x - 1, y, z }, { x + 1, y, z }, { x, y - 1, z }, { x, y + 1, z }, { x, y, z - 1 }, { x, y, z + 1 } };
            for (const auto& q : nb) {
                if (q[0] < 0 || q[1] < 0 || q[2] < 0 || q[0] >= n || q[1] >= n || q[2] >= n) continue;
                int j = (q[1] * n + q[2]) * n + q[0];
                if (seen[j]) continue;
                seen[j] = 1;
                if (open(q[0], q[1], q[2])) stack.push_back(j);
            }
        }
        for (int a = 0; a < 6; a++)
            for (int b = a + 1; b < 6; b++)
                if ((faces >> a & 1) && (faces >> b & 1)) out |= (uint16_t)(1u << FacetPairBit(a, b));
        if (out == FACET_ALL_OPEN) break;
    }
    return out;
}
