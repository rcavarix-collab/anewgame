// icons.h
//
// Library and hotbar icons rendered at load time: a small faceted lump of
// each material (facetmesh.h, as the ground is built) drawn by a tiny
// software rasterizer from a three-quarter view into a transparent cell.
// One-off cost at startup, about a millisecond for the whole list.
// Pure C++, no D3D. Layer 4.

#pragma once

#include "blocktex.h"

// Replaces `set.icons` (one BLOCK_TEX_SIZE cell per BlockID in a strip,
// BGRA with alpha) with rendered icons, from `set`'s own layers.
void RenderBlockIcons(BlockTextureSet& set);
