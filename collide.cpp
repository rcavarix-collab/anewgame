// collide.cpp -- see collide.h (DESIGN.md 23.5). Layer 3.

#include "collide.h"
#include <algorithm>
#include <cmath>

namespace {

// What the surface wraps: every solid block (the same rule the ground
// mesh uses, groundmesh.cpp). Lumpiness doesn't matter here.
struct SolidTable {
    FacetMaterial m[256];
    SolidTable() { for (int i = 0; i < BLOCK_COUNT; i++) m[i].solid = BlockSolid((BlockID)i); }
};
const SolidTable& Solids() { static SolidTable t; return t; }

} // namespace

void GatherFacets(World& w, int x0, int y0, int z0, int x1, int y1, int z1, std::vector<FacetTri>& out) {
    out.clear();
    // The cells with the mesher's margin, so every corner comes out exactly
    // as it's drawn. Below the world's floor counts as solid, as it does
    // for the ground mesh (groundmesh.cpp CopyGroundCells).
    static std::vector<uint8_t> cells; // reused: no allocation per tick once warm
    FacetGrid g;
    g.x0 = x0 - FACET_PAD; g.y0 = y0 - FACET_PAD; g.z0 = z0 - FACET_PAD;
    g.nx = x1 - x0 + 2 * FACET_PAD; g.ny = y1 - y0 + 2 * FACET_PAD; g.nz = z1 - z0 + 2 * FACET_PAD;
    cells.resize((size_t)g.nx * g.ny * g.nz);
    for (int y = 0; y < g.ny; y++)
        for (int z = 0; z < g.nz; z++)
            for (int x = 0; x < g.nx; x++) {
                int wy = g.y0 + y;
                cells[((size_t)y * g.nz + z) * g.nx + x] = wy < Y_MIN ? (uint8_t)BLOCK_FOUNDATION : (uint8_t)w.Get(g.x0 + x, wy, g.z0 + z);
            }
    g.cells = cells.data();
    g.mats = Solids().m;
    FacetShape shape; // the game's shape (groundmesh builds with the same defaults)
    for (int y = y0; y < y1; y++)
        for (int z = z0; z < z1; z++)
            for (int x = x0; x < x1; x++) {
                if (!g.Solid(x, y, z)) continue;
                for (int axis = 0; axis < 3; axis++)
                    for (int sign = -1; sign <= 1; sign += 2) {
                        int nx = x + (axis == 0 ? sign : 0), ny = y + (axis == 1 ? sign : 0), nz = z + (axis == 2 ? sign : 0);
                        if (g.Solid(nx, ny, nz)) continue;
                        Vec3 tri[2][3];
                        FacetBaseFace(g, shape, x, y, z, axis, sign, tri);
                        for (int k = 0; k < 2; k++) out.push_back({ tri[k][0], tri[k][1], tri[k][2], x, y, z, axis, sign });
                    }
            }
}

bool GroundHeight(const std::vector<FacetTri>& tris, float x, float z, float yMin, float yMax, float minUp, float* y) {
    bool found = false;
    float best = -1e30f;
    for (const FacetTri& t : tris) {
        Vec3 n = Cross(t.b - t.a, t.c - t.a);
        if (n.y <= 1e-6f || n.y * n.y < minUp * minUp * Dot(n, n)) continue; // walls, ceilings and slopes too steep aren't ground
        // Barycentric coordinates in the xz-projection.
        float d = (t.b.x - t.a.x) * (t.c.z - t.a.z) - (t.c.x - t.a.x) * (t.b.z - t.a.z);
        if (fabsf(d) < 1e-9f) continue;
        float u = ((x - t.a.x) * (t.c.z - t.a.z) - (t.c.x - t.a.x) * (z - t.a.z)) / d;
        float v = ((t.b.x - t.a.x) * (z - t.a.z) - (x - t.a.x) * (t.b.z - t.a.z)) / d;
        const float e = -1e-5f; // on an edge counts: the line mustn't slip between two triangles
        if (u < e || v < e || u + v > 1 - e) continue;
        float h = t.a.y + u * (t.b.y - t.a.y) + v * (t.c.y - t.a.y);
        if (h < yMin || h > yMax || h <= best) continue;
        best = h; found = true;
    }
    if (found) *y = best;
    return found;
}

bool FacetRaycast(World& w, Vec3 o, Vec3 d, float maxDist,
                  int& hitX, int& hitY, int& hitZ, int& placeX, int& placeY, int& placeZ, float* dist) {
    // Every facet near the ray's path: the box around it, a cell wider
    // (facets reach half a block past their cells).
    Vec3 e = o + d * maxDist;
    int x0 = (int)floorf(std::min(o.x, e.x)) - 1, x1 = (int)floorf(std::max(o.x, e.x)) + 2;
    int y0 = (int)floorf(std::min(o.y, e.y)) - 1, y1 = (int)floorf(std::max(o.y, e.y)) + 2;
    int z0 = (int)floorf(std::min(o.z, e.z)) - 1, z1 = (int)floorf(std::max(o.z, e.z)) + 2;
    y0 = std::max(y0, Y_MIN); y1 = std::min(y1, Y_MAX + 1);
    if (y0 >= y1) return false;
    static std::vector<FacetTri> tris; // reused: no allocation per frame once warm
    GatherFacets(w, x0, y0, z0, x1, y1, z1, tris);
    float best = maxDist;
    const FacetTri* hit = nullptr;
    for (const FacetTri& t : tris) {
        // Moller-Trumbore, front faces only (the ray comes from the air).
        Vec3 e1 = t.b - t.a, e2 = t.c - t.a;
        Vec3 pv = Cross(d, e2);
        float det = Dot(e1, pv);
        // det = -d . n: a facet seen from the air (n against the ray) has
        // det > 0; back-facing or edge-on ones are skipped.
        if (det < 1e-9f) continue;
        float inv = 1.0f / det;
        Vec3 tv = o - t.a;
        float u = Dot(tv, pv) * inv;
        if (u < -1e-5f || u > 1 + 1e-5f) continue;
        Vec3 qv = Cross(tv, e1);
        float v = Dot(d, qv) * inv;
        if (v < -1e-5f || u + v > 1 + 1e-5f) continue;
        float s = Dot(e2, qv) * inv;
        if (s < 0 || s >= best) continue;
        best = s; hit = &t;
    }
    if (!hit) return false;
    hitX = hit->cx; hitY = hit->cy; hitZ = hit->cz;
    placeX = hit->cx + (hit->axis == 0 ? hit->sign : 0);
    placeY = hit->cy + (hit->axis == 1 ? hit->sign : 0);
    placeZ = hit->cz + (hit->axis == 2 ? hit->sign : 0);
    if (dist) *dist = best;
    return true;
}
