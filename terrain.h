// terrain.h
//
// walkgrid-hills (DESIGN.md 23.2): the test landscape. Rolling ground
// in regions of the starting materials (D36) -- meadow, dry grassland and
// mossy hollows over dirt and loam, sandy lowlands edged with gravel and
// clay, stone and slate outcrops, sandstone plateaus with cliffs, snow on
// the highest ground. No caves.
//
// A pure function of (seed, coordinates): unmodified ground is never
// saved, only regenerated, so any change that alters the output is a new
// version (DESIGN.md 2.5), with this one kept for worlds made with it.
// No octave finer than ~11 blocks: rounding a fine one to whole cells
// leaves lone one-cell pits everywhere (M1.2 pictures).
//
// Layer 3 (world). Pure C++: runs on a job thread. Cost per column: a
// few noise samples per cell of an 18 x 18 height grid, then a fill.

#pragma once

#include "blocks.h"
#include <cstdint>
#include <vector>

// The surface height (the top solid cell's y) at a world column.
int HillsHeight(uint64_t seed, int wx, int wz);

// One column's cells, chunk by chunk from y = 0: `present[cy]` is true
// for chunks holding any ground; `cells` holds CHUNK_CELLS material IDs
// per chunk (index Chunk::LocalIndex), air = 0.
struct TerrainColumn {
    int chunks = 0;
    std::vector<bool> present;
    std::vector<uint8_t> cells;
};
// `version` (the world's, stored in its save; never changes for a world):
//  1  the first (M1.4), kept for worlds made with it.
//  2  (D50) a mesa's whole cliff ramp is sandstone, where v1 painted a row
//     of evenly spaced stone "teeth" along its foot (any cell dropping 3+
//     to a neighbour was rock, and a cliff crossing the grid at an angle
//     drops 3+ in a regular rhythm); and grass wraps down over the side of
//     a step, where v1 showed the soil under it in evenly spaced dark
//     triangles on 2-cell rises. Heights are the same in both.
static const int HILLS_LATEST = 2;
void HillsColumn(uint64_t seed, int cx, int cz, TerrainColumn& out, int version = 1);
