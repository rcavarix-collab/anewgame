// worldfile.h
//
// The world save format (DESIGN.md Part VII): encoding a world to bytes
// and decoding bytes back, with no file or OS access -- savegame.cpp does
// the disk side (crash-safe write, slot paths) and applies the result to
// live state. Kept free of <windows.h> so it compiles and is tested
// natively (tests/). World layer (3).
//
// walkgrid save format v1 (FOUNDATIONS.md 5): only chunks that differ from
// the generator are stored (everything else regenerates from the recorded
// generator), pending scheduled updates, and one opaque "game section" the
// game layer owns, so the engine format never names game systems.
// Voxistics' formats (v2-v9) are not read: walkgrid has no saves before v1.

#pragma once

#include "world.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

static const uint32_t SAVE_VERSION = 1;

using ChunkMap = std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash>;

struct SaveData {
    Player player;
    float dayTime = 0.0f;
    WorldGenParams gen;
    ChunkMap chunks; // every chunk that differs from the generator, all flagged modified
    std::vector<PendingUpdate> updates; // scheduled block updates still pending
    std::vector<uint8_t> game;          // the game layer's own section, opaque here

    uint32_t version = 0; // of the file that was read
    // Saved block names this build doesn't know (loaded as air).
    std::vector<std::string> unknownBlockNames;
};

enum class DecodeResult { Ok, Truncated, BadChecksum, BadMagic, UnsupportedVersion, UnknownGenerator, Corrupt };
const char* DecodeResultText(DecodeResult r);

// Every modified chunk from both the resident world and the eviction
// store. Unmodified chunks are skipped. `game` is written as-is and handed
// back by DecodeSave (up to 16 MB).
void EncodeSave(const Player& p, float dayTime, const WorldGenParams& gen,
                const World& w, const ChunkMap& evicted, const std::vector<PendingUpdate>& updates,
                const std::vector<uint8_t>& game, std::vector<uint8_t>& out);

DecodeResult DecodeSave(const uint8_t* data, size_t size, SaveData& out);
