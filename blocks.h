// blocks.h
//
// The materials registry (DESIGN.md Part III; walkgrid M1.9): every
// material is exactly one row in g_blocks below -- its save identity,
// physical flags, textures and lumpiness. Adding a material means one enum
// entry plus one row; the ground mesh, collision, picking, the library,
// the save format and the texture loader all read this table rather than
// keeping their own lists. (The names still say "block": the cells are
// the grid the faceted ground is built through.) Layer 3.

#pragma once

#include <cstdint>

enum BlockID : uint8_t {
    BLOCK_AIR = 0,
    BLOCK_FOUNDATION,    // the world's floor (y = 0): never dug, never placed
    // The twelve starting materials (D36), top down roughly by softness.
    BLOCK_MEADOW_GRASS,
    BLOCK_DRY_TURF,
    BLOCK_MOSS,
    BLOCK_DIRT,
    BLOCK_LOAM,
    BLOCK_CLAY,
    BLOCK_SAND,
    BLOCK_GRAVEL,
    BLOCK_STONE,
    BLOCK_SLATE,
    BLOCK_SANDSTONE,
    BLOCK_SNOW,
    BLOCK_COUNT
};

// Materials that light up on their own (world shader; glowlight.h): the
// kinds are engine-generic, carried from Voxistics. No walkgrid material
// glows yet, so the glow grid stays empty and costs nothing.
enum BlockGlow : uint8_t {
    GLOW_NONE = 0,
    GLOW_DRIVEN,      // follows one per-frame level the game supplies (render.cpp glowDrive.x)
    GLOW_STEADY,      // a steady warm light source; what glows on it is the texture's glow map (4.13)
    GLOW_BREATHE,     // its texture's glow map breathes slowly (well under 1 Hz: flash-safe); casts no light
};
// Whether a glow kind lights the world around it (glowlight.h).
static inline bool GlowCastsLight(BlockGlow g) { return g == GLOW_DRIVEN || g == GLOW_STEADY; }

// Face order shared by the texture layer table and the ground mesh:
// +X, -X, +Y (top), -Y (bottom), +Z, -Z.
enum BlockFace : uint8_t {
    FACE_POS_X = 0, FACE_NEG_X, FACE_POS_Y, FACE_NEG_Y, FACE_POS_Z, FACE_NEG_Z, FACE_COUNT
};

// The per-cell state byte, stored beside the material in every chunk and
// in saves. Reserved: no walkgrid material uses it yet (Voxistics kept a
// facing there), so it can be claimed without a storage or format change.
static const uint8_t STATE_FACING_MASK = 0x07;

struct BlockDef {
    const char* name;     // identity on disk (Section 3.1) -- never renumbered, only renamed with a migration
    bool solid;           // part of the ground: the surface wraps it, collision and picking meet it
    bool foundational;    // never falls, always supports (Part V)
    bool placeable;       // appears in the library
    BlockGlow glow;
    // Texture names (assets/textures/TEXTURE_BRIEF.md): the top shows on
    // ground up to about 50 degrees, the side on steeper ground and
    // cliffs, the bottom on overhangs. A texture with no authored .vtex art
    // falls back to the procedural texture of the same name.
    const char* texTop;
    const char* texSide;
    const char* texBottom;
    float bump;           // how lumpy its fine detail is, in blocks (DESIGN.md 23.1)
};

// Columns: name, solid, foundational, placeable, glow, then textures top /
// side / bottom, then lumpiness.
inline const BlockDef g_blocks[BLOCK_COUNT] = {
    { "air",          false, false, false, GLOW_NONE, nullptr,         nullptr,             nullptr,         0.0f   },
    { "foundation",   true,  true,  false, GLOW_NONE, "foundation",    "foundation",        "foundation",    0.0f   },
    { "meadow_grass", true,  false, true,  GLOW_NONE, "meadow_grass",  "dirt",              "dirt",          0.07f  },
    { "dry_turf",     true,  false, true,  GLOW_NONE, "dry_turf",      "dirt",              "dirt",          0.06f  },
    { "moss",         true,  false, true,  GLOW_NONE, "moss",          "dirt",              "dirt",          0.06f  },
    { "dirt",         true,  false, true,  GLOW_NONE, "dirt",          "dirt",              "dirt",          0.05f  },
    { "loam",         true,  false, true,  GLOW_NONE, "loam",          "loam",              "loam",          0.05f  },
    { "clay",         true,  false, true,  GLOW_NONE, "clay",          "clay",              "clay",          0.03f  },
    { "sand",         true,  false, true,  GLOW_NONE, "sand",          "sand",              "sand",          0.012f },
    { "gravel",       true,  false, true,  GLOW_NONE, "gravel",        "gravel",            "gravel",        0.035f },
    { "stone",        true,  false, true,  GLOW_NONE, "stone",         "stone",             "stone",         0.045f },
    { "slate",        true,  false, true,  GLOW_NONE, "slate",         "slate",             "slate",         0.04f  },
    { "sandstone",    true,  false, true,  GLOW_NONE, "sandstone_top", "sandstone_layered", "sandstone_top", 0.015f },
    { "snow",         true,  false, true,  GLOW_NONE, "snow",          "snow",              "snow",          0.02f  },
};

static inline bool BlockSolid(BlockID id) { return g_blocks[id].solid; }
// Hides the light behind it (the glow grid).
static inline bool BlockOpaqueCube(BlockID id) { return g_blocks[id].solid; }

// The placeable materials in registry order: the library's contents.
struct PlaceableList {
    BlockID ids[BLOCK_COUNT];
    int count;
};
inline PlaceableList BuildPlaceableList() { // inline, not static: the inline variable below must see one function in every file
    PlaceableList l = {};
    for (int i = 0; i < BLOCK_COUNT; i++)
        if (g_blocks[i].placeable) l.ids[l.count++] = (BlockID)i;
    return l;
}
inline const PlaceableList g_placeableList = BuildPlaceableList();

// Texture name for one face of a material, resolved once per (material,
// face) into a texture-array layer at load time, never per vertex.
// (`facing` is unused since M1.9: kept so the layer table's shape holds.)
static inline const char* BlockFaceTextureName(BlockID id, BlockFace facing, BlockFace face) {
    (void)facing;
    const BlockDef& d = g_blocks[id];
    const char* name = face == FACE_POS_Y ? d.texTop : face == FACE_NEG_Y ? d.texBottom : d.texSide;
    return name ? name : d.name;
}
