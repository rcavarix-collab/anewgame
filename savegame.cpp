// savegame.cpp
//
// Implementations for savegame.h: save slots, and saving and loading a
// game -- encoding with worldfile.h, writing crash-safely through
// gamefiles.h, and applying a decoded save to the live world. Moved from
// Voxistics' persist.cpp (M0.9). Game layer (5).

#ifndef NOMINMAX // also set project-wide (walkgrid.vcxproj)
#define NOMINMAX
#endif
#include <windows.h> // OutputDebugStringA
#include "savegame.h"
#include "worldfile.h"
#include "gamefiles.h"
#include "library.h"
#include <cstdio>
#include <cwchar> // swprintf, for building slotN.sav filenames (Section 7.2.4)
#include <vector>

// Recomputed on every save/load rather than cached once -- cheap, and
// means a save directory that only becomes available partway through a
// run (e.g. a transient permissions/antivirus hiccup clears up) is
// retried instead of being stuck with whatever the very first attempt
// happened to find.
std::filesystem::path GetSaveFilePath(int slot) {
    std::filesystem::path dir = GameSubdirectory(L"Saves"); // the multi-slot saves folder (Section 7.2.4)
    wchar_t name[32];
    swprintf(name, 32, L"slot%d.sav", slot + 1);
    return dir.empty() ? std::filesystem::path(name) : dir / name;
}

bool SlotExists(int slot) {
    std::error_code ec;
    return std::filesystem::exists(GetSaveFilePath(slot), ec);
}

bool SaveGame(World& w, Player& p, int slot) {
    std::vector<uint8_t> buf;
    std::vector<uint8_t> game; // walkgrid's own section: nothing in it yet
    EncodeSave(p, g_dayTimeSeconds, g_worldGen, w, g_evictedChunks, SnapshotScheduledUpdates(), game, buf);

    // Crash-safe write sequence (Section 7.3): .tmp first, then the old
    // save becomes .bak and the new one takes its place.
    return WriteFileSafely(GetSaveFilePath(slot), buf.data(), buf.size(), true);
}

bool LoadGame(World& w, Player& p, int slot) {
    std::vector<uint8_t> buf;
    if (!ReadWholeFile(GetSaveFilePath(slot), buf)) return false;

    // Decoded into a scratch SaveData; nothing live changes unless the
    // whole file is valid.
    SaveData d;
    DecodeResult res = DecodeSave(buf.data(), buf.size(), d);
    if (res != DecodeResult::Ok) {
        char msg[160];
        snprintf(msg, sizeof(msg), "LoadGame: %s, aborting load\n", DecodeResultText(res));
        OutputDebugStringA(msg);
        return false;
    }
    for (const std::string& name : d.unknownBlockNames) {
        char msg[256];
        snprintf(msg, sizeof(msg), "LoadGame: unknown block name '%s', mapping to air\n", name.c_str());
        OutputDebugStringA(msg);
    }

    Player loaded = d.player;
    // The placeable roster can shrink between builds, so a save made with
    // a now-nonexistent hotbar slot selected must not index past the end.
    if (loaded.hotbarIndex < 0 || loaded.hotbarIndex >= g_placeableList.count) loaded.hotbarIndex = 0;
    p = loaded;
    g_dayTimeSeconds = d.dayTime;
    g_worldGen = d.gen;

    // Every saved chunk goes to the modified-chunk store; the normal
    // streaming path then generates the columns around the player and
    // overlays these onto them (Section 2.4), so loading costs only the
    // decode, however large the world.
    w.ClearChunks();
    ResetColumnStreaming();   // before the store is filled: it empties it
    g_evictedChunks = std::move(d.chunks);
    ClearScheduledUpdates();
    RestoreScheduledUpdates(d.updates); // unknown kinds are dropped
    return true;
}
