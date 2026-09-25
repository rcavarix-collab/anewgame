// collide.h
//
// Collision and picking on the faceted ground (DESIGN.md 23.5). The
// player and the crosshair meet the same base facets the ground is drawn
// with (facetmesh.h FacetBaseFace): never the fine detail, which is
// visual only (a few centimetres of lumps), so detail costs the
// simulation nothing.
//
//   GatherFacets   the base triangles of every surface face in a box
//   GroundHeight   where a vertical line meets the ground (walking)
//   FacetRaycast   the nearest facet along a ray, mapped back to a cell
//
// Layer 3 (world). Cost: GatherFacets reads the box's cells from the
// world (one lookup each) and works out each surface face's four corners;
// physics gathers about a 7 x 6 x 7 box once a tick, picking the ray's
// box once a frame. The rest is a few hundred triangle tests.

#pragma once

#include "world.h"
#include "facetmesh.h"
#include <vector>

struct FacetTri {
    Vec3 a, b, c;          // wound so (b - a) x (c - a) points out of the ground
    int cx, cy, cz;        // the solid cell whose face this is
    int axis, sign;        // the face: the empty cell is one step along axis toward sign
};

// The base facets of every surface face owned by a solid cell in
// [x0, x1) x [y0, y1) x [z0, z1). Replaces `out`'s contents.
void GatherFacets(World& w, int x0, int y0, int z0, int x1, int y1, int z1, std::vector<FacetTri>& out);

// Where the vertical line at (x, z) meets a facet facing up at least
// `minUp` (the y of its unit normal: 0.55 is about 57 degrees of slope):
// the highest such point in [yMin, yMax]. False when there's none.
bool GroundHeight(const std::vector<FacetTri>& tris, float x, float z, float yMin, float yMax, float minUp, float* y);

// Picking: the nearest facet the ray (unit direction d) meets within
// maxDist, the solid cell it belongs to, and the empty cell across its face
// (where a placed block goes). `dist` (optional): how far along the ray.
bool FacetRaycast(World& w, Vec3 o, Vec3 d, float maxDist,
                  int& hitX, int& hitY, int& hitZ, int& placeX, int& placeY, int& placeZ, float* dist = nullptr);
