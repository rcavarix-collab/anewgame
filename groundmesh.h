// groundmesh.h
//
// The world's ground mesh for drawing (DESIGN.md 23.3): the faceted
// surface (facetmesh.h) of one chunk, packed for the GPU. The main thread
// copies the chunk's cells with a margin (CopyGroundCells, a few
// microseconds); a job thread builds and packs the mesh from that copy
// (BuildGroundMesh); render.cpp uploads it. Nothing here touches D3D, so
// it's tested natively.
//
// Layer 4 (presentation). Cost: a 20^3 copy on the main thread per chunk
// rebuilt; the build itself on a job thread.

#pragma once

#include "world.h"
#include "facetmesh.h"
#include <cstdint>
#include <vector>

// One ground vertex, 16 bytes (the plan's ~20: PROTOTYPE_OUTLINE 4).
//  x, y, z  chunk-local position, (p + GROUND_POS_BIAS) * GROUND_POS_SCALE
//           (1/2048 block; facets reach up to half a block past the chunk)
//  w01      the first two material weights (w0 | w1 << 8); the third is
//           255 - w0 - w1
//  n        smooth normal, n * 127.5 + 127.5 (texture projection, blending)
//  ao       openness, 255 open .. 0 enclosed
//  m        the triangle's three materials (block IDs), strongest first
//  sky      how open to the sky, 255 .. 0 (M1.6)
struct GroundVertex {
    uint16_t x, y, z, w01;
    uint8_t nx, ny, nz, ao;
    uint8_t m0, m1, m2, sky;
};
static_assert(sizeof(GroundVertex) == 16, "ground vertex must stay 16 bytes");
static const float GROUND_POS_SCALE = 2048.0f;
static const float GROUND_POS_BIAS = 2.0f;

// What each material means to the surface and which texture layers it
// shows: top (ground up to ~50 degrees), side (steeper), bottom.
struct GroundMaterial {
    uint16_t top = 0, side = 0, bottom = 0;
};
extern FacetMaterial g_groundFacet[256];
extern GroundMaterial g_groundLayers[256];
// Built once textures are loaded (the layer table is blocktex.h's).
void InitGroundMaterials(const uint16_t faceLayer[BLOCK_COUNT][FACE_COUNT][FACE_COUNT]);
// How lumpy a material's fine detail is, blocks (DESIGN.md 23.1).
float GroundBump(BlockID id);

// A chunk's cells with FACET_PAD cells of margin on every side, and the
// column tops (World::Top) around it, for the sky light.
static const int GROUND_GRID = CHUNK_SIZE + 2 * FACET_PAD;
static const int GROUND_TOPS_PAD = 10;
static const int GROUND_TOPS = CHUNK_SIZE + 2 * GROUND_TOPS_PAD;
struct GroundCells {
    ChunkCoord cc{ 0, 0, 0 };
    uint8_t cells[GROUND_GRID * GROUND_GRID * GROUND_GRID];
    int16_t tops[GROUND_TOPS * GROUND_TOPS]; // index (z + 10) * 36 + (x + 10), chunk-local
};
// How open corner (cx, cy, cz) is to the sky, 0..1, from the column tops
// around it: in 8 directions, the steepest rise within 8 blocks sets a
// horizon, and that direction contributes cos^2 of its angle (the share
// of sky light a surface gets from above that horizon). Flat ground is
// fully open; a cliff face sees about half; a pit's floor or ground under
// an overhang, little. A heightmap can't see gaps under a roof: ground
// under an overhang counts as walled in, which is what it should look like.
float GroundSkyAt(const GroundCells& g, int cx, int cy, int cz);
// Main thread: copies from the world. Below the world's floor counts as
// solid (no faces pointing down out of the world); missing chunks are air.
void CopyGroundCells(World& w, const ChunkCoord& cc, GroundCells& out);

struct GroundMesh {
    std::vector<GroundVertex> verts;
    std::vector<uint32_t> idx;
    uint16_t openings = FACET_ALL_OPEN; // which of the chunk's sides see each other (facetmesh.h FacetOpenings)
    FacetMesh stats;   // counts only (its vectors are emptied)
};
// Detail bands (DESIGN.md 23.6): the level a chunk `d` chunks from the
// camera's (the largest of the three axis distances) should be built at,
// given the level it has now (-1: none) and the fine-detail setting. The
// finest cut within `fine` chunks, the middle one within twice that; a
// chunk only drops a level once it's a whole chunk past the edge, so
// walking along an edge doesn't rebuild chunks back and forth (F4).
int GroundWantLevel(int d, int current, int fine);
// The FINE DETAIL setting's top step: every chunk at the finest cut, at any
// distance (the default, D49).
static const int GROUND_FINE_ALL = 4;

// Job thread: the chunk's facets, packed, with fine detail cut to `level`
// (0 whole, 1 2x2, 2 4x4) where the materials call for it (DESIGN.md 23.6).
// Its boundary edges are always cut for the finest level, so neighbours
// built at other levels meet it exactly.
void BuildGroundMesh(const GroundCells& in, int level, GroundMesh& out);

// Hidden-chunk skipping (DESIGN.md 23.7, M1.11): the chunks that could be
// seen from the camera's chunk, walking outward chunk to chunk through
// the sides each chunk's open cells connect (its openings), never turning
// back toward the camera, and only through chunks in view. A chunk sealed
// off by ground -- under the hills, behind a cave wall -- is never reached.
// `openings(cc, user)` gives a chunk's openings, or -1 for no chunk there
// (air: open every way); `inView(cc, user)` is the frustum test. Walks
// chunk rows 0 .. chunkRows-1 within `radius` chunks across. Every chunk
// reached is appended to `out` once (resident or not). Cost: a few
// thousand steps at the default distance.
void GroundVisibleChunks(const ChunkCoord& cam, int radius, int chunkRows,
                         int (*openings)(const ChunkCoord&, void*), bool (*inView)(const ChunkCoord&, void*), void* user,
                         std::vector<ChunkCoord>& out);
