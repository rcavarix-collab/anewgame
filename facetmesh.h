// facetmesh.h
//
// The faceted ground (DESIGN.md Part XXIII): the surface through the cell
// grid, neither cubes nor blobs.
//
//  - Corners. Every lattice corner with both solid and empty cells among
//    its eight neighbours carries one vertex, placed at the mean of the
//    solid/empty crossings around it, eased halfway toward its neighbours
//    (one pass) and nudged by a seeded jitter (FacetCorner). Flat ground stays level; a one-cell step becomes a
//    slope; nothing is rounded beyond one cell.
//  - Base facets. Every face between a solid and an empty cell becomes a
//    quad on its four corners, split into two triangles along the fold
//    that bulges outward. Collision and picking use these (the base mesh
//    never changes with distance).
//  - Detail. For drawing, a quad is cut into 2x2 or 4x4 cells where detail
//    shows -- near (the caller's band) and on a lumpy material; creases
//    stay sharp, borders are blended per pixel -- and its new points are
//    pushed out or in by the material's lumpiness. Every point on a shared edge is computed from
//    that edge alone (its two corners, in a fixed order), and an edge is
//    cut as finely as the finest face that uses it, so neighbouring quads
//    at different levels meet exactly: no cracks. Corners never move with
//    detail, so every level shares the base facets' outline.
//
// Pure C++, no OS or graphics dependency: the game, tests/ and
// tools/facet_preview share it. Layer 3 (world): collision (1.7) uses the
// same corners. Cost: linear in the surface faces of the box meshed, with
// a per-corner cache; nothing per frame. Deterministic: same cells, same
// mesh, wherever the box is cut.

#pragma once

#include "common.h"
#include <cstdint>
#include <vector>

// What a material means to the surface.
struct FacetMaterial {
    bool solid = false;   // part of the ground (the surface wraps it)
    float bump = 0.0f;    // how far detail points move in or out, in blocks (grass lumpier than stone)
};

// A box of cells copied from the world (the mesher never reads the world
// itself, so it can run on a job thread). Outside the box counts as air;
// callers pad the box by FACET_PAD cells around what they mesh.
static const int FACET_PAD = 2;
struct FacetGrid {
    int x0 = 0, y0 = 0, z0 = 0;       // world cell at index (0, 0, 0)
    int nx = 0, ny = 0, nz = 0;
    const uint8_t* cells = nullptr;   // nx * ny * nz material IDs, index (y * nz + z) * nx + x
    const FacetMaterial* mats = nullptr; // 256 entries
    uint8_t At(int wx, int wy, int wz) const {
        int x = wx - x0, y = wy - y0, z = wz - z0;
        if ((unsigned)x >= (unsigned)nx || (unsigned)y >= (unsigned)ny || (unsigned)z >= (unsigned)nz) return 0;
        return cells[((size_t)y * nz + z) * nx + x];
    }
    bool Solid(int wx, int wy, int wz) const { return mats[At(wx, wy, wz)].solid; }
};

// The shape's tuning (the preview pictures set these; D-log when changed).
struct FacetShape {
    bool relax = true;          // false: corners stay on the lattice -- exact cubes (the old look, for comparison)
    float smooth = 0.5f;        // one smoothing pass toward neighbouring corners, 0..1 (softens terrace corners)
    float jitter = 0.16f;       // how far a corner is nudged in or out along its normal, blocks (seeded per corner)
    float jitterSide = 0.04f;   // and sideways: little, or steps' risers lean out over themselves
    // Gentle slopes roll instead of terracing (D62): an upward corner eases
    // this far (0..1) toward the mean ground height of the 4 x 4 columns
    // around it, where they span 2 cells or less. Cliffs, digs and builds
    // keep their sharp steps. 0: the look before. Off (0) until the owner
    // decides how one-cell edits should behave (they'd soften too: the
    // mesher can't tell a built step from a natural one).
    float terrace = 0.0f;
    float bumpScale = 1.0f;     // scales every material's bump
    float bumpFreq = 1.6f;      // lumps per block
    float bumpFine = 0.04f;     // a material this lumpy is cut to its band's finest (4x4 near)
    float bumpSome = 0.015f;    // this lumpy: 2x2 at most; below it, never cut
};

// Corner (cx, cy, cz) is the lattice point shared by cells cx-1..cx,
// cy-1..cy, cz-1..cz. False when it isn't on the surface.
bool FacetCorner(const FacetGrid& g, const FacetShape& s, int cx, int cy, int cz, Vec3* pos);

// A vertex ready to draw. The three materials are the triangle's: its
// three vertices carry the same three, with their own weights.
struct FacetVertex {
    Vec3 pos;          // world position
    Vec3 normal;       // smooth, for texture projection and blending (facets are lit flat)
    uint8_t mat[3];    // materials, strongest first
    uint8_t w[3];      // their weights, summing to 255
    uint8_t ao;        // 255 open .. 0 enclosed (from the solid cells around)
    uint8_t sky;       // 255 open sky .. 0 none
};

struct FacetMesh {
    std::vector<FacetVertex> verts;
    std::vector<uint32_t> idx;     // triangles; b - a cross c - a points out of the ground
    int baseQuads = 0;             // surface faces meshed
    int quadsAtLevel[3] = {};      // how many were cut 1x1, 2x2, 4x4
    int folded = 0;                // triangles facing against their face's direction (should be none)
    int triangles() const { return (int)idx.size() / 3; }
};

struct FacetBuildParams {
    // The cells whose faces are meshed: solid cells in [b0, b1). A face
    // belongs to its solid cell, so boxes that tile the world mesh every
    // face exactly once.
    int bx0 = 0, by0 = 0, bz0 = 0, bx1 = 0, by1 = 0, bz1 = 0;
    // The finest cut allowed near a point: 0 (whole), 1 (2x2), 2 (4x4).
    // Must be a function of the point alone. Null: 0 everywhere.
    int (*band)(Vec3 p, void* user) = nullptr;
    // 0..1 how open corner (cx, cy, cz) is to the sky. Null: 1.
    float (*sky)(int cx, int cy, int cz, void* user) = nullptr;
    void* user = nullptr;
    // false: every face is cut to its band's level (blanket subdivision,
    // for comparison); true: only where it shows.
    bool selective = true;
    // Chunks built separately at different levels (DESIGN.md 23.6): every
    // edge on the box's boundary is cut as finely as its materials would be
    // at the finest band, whatever `band` says, so neighbouring boxes agree
    // on their shared edges without knowing each other's level -- no cracks,
    // and a chunk changing level never needs its neighbours rebuilt.
    bool stitchBox = false;
    FacetShape shape;
};

void FacetBuild(const FacetGrid& g, const FacetBuildParams& p, FacetMesh& out);

// The two base triangles of the face between solid cell (x, y, z) and its
// empty neighbour one step along `axis` (0 x, 1 y, 2 z) toward `sign`
// (+1 or -1), exactly as FacetBuild splits it before any detail: what
// collision and picking use (collide.h). Each is wound so (b - a) x (c - a)
// points out of the ground.
void FacetBaseFace(const FacetGrid& g, const FacetShape& s, int x, int y, int z, int axis, int sign, Vec3 tri[2][3]);

// Which sides of a cube of cells see each other through open (not solid)
// cells (DESIGN.md 23.7, M1.11): the faces are -X +X -Y +Y -Z +Z (0..5),
// and bit FacetPairBit(a, b) is set when an open cell on face a connects to
// one on face b by steps between open cells inside the cube. A solid cube
// is 0; an empty one, every bit. The cube is `size` cells from grid cell
// (x0, y0, z0). Cost: one flood fill of the cube, about 40 microseconds a
// chunk (tests, -O1), on the job thread that meshes it.
static inline int FacetPairBit(int a, int b) {
    if (a > b) { int t = a; a = b; b = t; }
    static const int base[6] = { 0, 5, 9, 12, 14, 15 };
    return base[a] + (b - a - 1);
}
static const uint16_t FACET_ALL_OPEN = 0x7FFF;
static inline bool FacetFacesSee(uint16_t openings, int a, int b) { return a == b || ((openings >> FacetPairBit(a, b)) & 1) != 0; }
uint16_t FacetOpenings(const FacetGrid& g, int x0, int y0, int z0, int size);
