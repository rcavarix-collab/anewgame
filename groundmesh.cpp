// groundmesh.cpp -- see groundmesh.h (DESIGN.md 23.3). Layer 4.

#include "groundmesh.h"
#include <algorithm>
#include <cmath>
#include <cstring>

FacetMaterial g_groundFacet[256];
GroundMaterial g_groundLayers[256];

float GroundBump(BlockID id) {
    switch (id) { // the starting materials (D36); lumpiness in blocks
    case BLOCK_MEADOW_GRASS: return 0.07f;
    case BLOCK_DRY_TURF: case BLOCK_MOSS: return 0.06f;
    case BLOCK_DIRT: case BLOCK_LOAM: return 0.05f;
    case BLOCK_STONE: return 0.045f;
    case BLOCK_SLATE: return 0.04f;
    case BLOCK_GRAVEL: return 0.035f;
    case BLOCK_CLAY: return 0.03f;
    case BLOCK_SNOW: return 0.02f;
    case BLOCK_SANDSTONE: return 0.015f;
    case BLOCK_SAND: return 0.012f;
    case BLOCK_FOUNDATION: return 0.0f;
    default: return 0.03f; // the rest of the old roster, until M1.9
    }
}

void InitGroundMaterials(const uint16_t faceLayer[BLOCK_COUNT][FACE_COUNT][FACE_COUNT]) {
    for (int i = 0; i < 256; i++) { g_groundFacet[i] = FacetMaterial(); g_groundLayers[i] = GroundMaterial(); }
    for (int i = 0; i < BLOCK_COUNT; i++) {
        BlockID id = (BlockID)i;
        g_groundFacet[i].solid = BlockSolid(id);
        g_groundFacet[i].bump = GroundBump(id);
        g_groundLayers[i].top = faceLayer[i][FACE_POS_Z][FACE_POS_Y];
        g_groundLayers[i].side = faceLayer[i][FACE_POS_Z][FACE_POS_X];
        g_groundLayers[i].bottom = faceLayer[i][FACE_POS_Z][FACE_NEG_Y];
    }
    // Grassy ground shows its soil where it's steep (M1.2 pictures).
    for (BlockID g : { BLOCK_MEADOW_GRASS, BLOCK_DRY_TURF, BLOCK_MOSS }) {
        g_groundLayers[g].side = faceLayer[BLOCK_DIRT][FACE_POS_Z][FACE_POS_X];
        g_groundLayers[g].bottom = faceLayer[BLOCK_DIRT][FACE_POS_Z][FACE_NEG_Y];
    }
}

void CopyGroundCells(World& w, const ChunkCoord& cc, GroundCells& out) {
    out.cc = cc;
    const int N = GROUND_GRID;
    int ox = cc.x * CHUNK_SIZE - FACET_PAD, oy = cc.y * CHUNK_SIZE - FACET_PAD, oz = cc.z * CHUNK_SIZE - FACET_PAD;
    memset(out.cells, 0, sizeof(out.cells));
    // The 27 chunks around, each once: copy the part of each that the
    // margin overlaps.
    for (int dy = -1; dy <= 1; dy++)
        for (int dz = -1; dz <= 1; dz++)
            for (int dx = -1; dx <= 1; dx++) {
                ChunkCoord nc{ cc.x + dx, cc.y + dy, cc.z + dz };
                int bx = nc.x * CHUNK_SIZE, by = nc.y * CHUNK_SIZE, bz = nc.z * CHUNK_SIZE;
                int x0 = std::max(ox, bx), x1 = std::min(ox + N, bx + CHUNK_SIZE);
                int y0 = std::max(oy, by), y1 = std::min(oy + N, by + CHUNK_SIZE);
                int z0 = std::max(oz, bz), z1 = std::min(oz + N, bz + CHUNK_SIZE);
                if (x0 >= x1 || y0 >= y1 || z0 >= z1) continue;
                Chunk* c = (by < 0) ? nullptr : w.FindChunk(nc);
                for (int y = y0; y < y1; y++)
                    for (int z = z0; z < z1; z++) {
                        uint8_t* dst = out.cells + ((size_t)(y - oy) * N + (z - oz)) * N + (x0 - ox);
                        if (y < Y_MIN) { memset(dst, BLOCK_FOUNDATION, x1 - x0); continue; } // under the world: solid
                        if (!c) continue;
                        const uint8_t* src = c->blocks + Chunk::LocalIndex(x0 - bx, y - by, z - bz);
                        memcpy(dst, src, x1 - x0);
                    }
            }
}

void BuildGroundMesh(const GroundCells& in, int (*band)(Vec3, void*), void* user, GroundMesh& out) {
    const int N = GROUND_GRID;
    FacetGrid g;
    g.x0 = in.cc.x * CHUNK_SIZE - FACET_PAD; g.y0 = in.cc.y * CHUNK_SIZE - FACET_PAD; g.z0 = in.cc.z * CHUNK_SIZE - FACET_PAD;
    g.nx = g.ny = g.nz = N;
    g.cells = in.cells;
    g.mats = g_groundFacet;
    FacetBuildParams p;
    p.bx0 = in.cc.x * CHUNK_SIZE; p.by0 = in.cc.y * CHUNK_SIZE; p.bz0 = in.cc.z * CHUNK_SIZE;
    p.bx1 = p.bx0 + CHUNK_SIZE; p.by1 = p.by0 + CHUNK_SIZE; p.bz1 = p.bz0 + CHUNK_SIZE;
    p.band = band; p.user = user;
    FacetMesh m;
    FacetBuild(g, p, m);
    out.verts.resize(m.verts.size());
    float bx = (float)p.bx0, by = (float)p.by0, bz = (float)p.bz0;
    // In double: v is exact (a float minus a whole number near it), and
    // times 2048 stays exact, so a corner shared by two chunks packs to
    // values exactly 16 blocks apart in both -- no hairline cracks.
    auto q16 = [](float v) {
        double s = std::floor((double)v * GROUND_POS_SCALE + GROUND_POS_BIAS * GROUND_POS_SCALE + 0.5);
        return (uint16_t)std::max(0.0, std::min(65535.0, s));
    };
    auto q8n = [](float v) { return (uint8_t)std::max(0.0f, std::min(255.0f, v * 127.5f + 128.0f)); };
    for (size_t i = 0; i < m.verts.size(); i++) {
        const FacetVertex& s = m.verts[i];
        GroundVertex& d = out.verts[i];
        d.x = q16(s.pos.x - bx); d.y = q16(s.pos.y - by); d.z = q16(s.pos.z - bz);
        d.w01 = (uint16_t)(s.w[0] | (s.w[1] << 8));
        d.nx = q8n(s.normal.x); d.ny = q8n(s.normal.y); d.nz = q8n(s.normal.z);
        d.ao = s.ao;
        d.m0 = s.mat[0]; d.m1 = s.mat[1]; d.m2 = s.mat[2];
        d.sky = s.sky;
    }
    out.idx = std::move(m.idx);
    m.verts.clear();
    out.stats = std::move(m);
}
