// savegame.h
//
// Saving and loading a game (DESIGN.md Part VII): five slots in the game
// folder's Saves\ (slotN.sav), each a walkgrid v1 save (worldfile.h).
// Game layer (5): it decides what a game's state is -- the world, the
// player, the day clock, and (later) the game's own section.

#pragma once

#include "world.h"
#include <filesystem>

static const int MAX_SAVE_SLOTS = 5;

std::filesystem::path GetSaveFilePath(int slot);
bool SlotExists(int slot);

bool SaveGame(World& w, Player& p, int slot);
bool LoadGame(World& w, Player& p, int slot);
