// terrain.h
//
// walkgrid-hills v1 (DESIGN.md 23.2): the test landscape. Rolling ground
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
void HillsColumn(uint64_t seed, int cx, int cz, TerrainColumn& out);
