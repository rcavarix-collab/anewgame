// gamefiles.h
//
// Where the game keeps its files, and how it writes them safely
// (DESIGN.md 7.2.1, 7.3): Documents\My Games\walkgrid in the Documents folder
// Windows shows the player, or Saved Games\walkgrid when that Documents is
// inside OneDrive (never a cloud-synced folder, D28, D45), and its
// subfolders (Saves, ShaderCache, Screenshots), each created on demand and
// resolved fresh every time. Platform layer (2): nothing here knows what a
// world, a setting or a game is. Only ever local files (CLAUDE.md, Privacy).

#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

// Which folder the game uses (D45), given Windows' Documents folder (the
// one Explorer shows) and the Saved Games folder: Documents\My Games\walkgrid,
// unless Documents is inside OneDrive (any part of its path named OneDrive,
// or starting "OneDrive - " as work accounts are), then Saved Games\walkgrid.
// Empty if neither is known. Pure: tested natively.
inline std::filesystem::path ChooseGameFolder(const std::filesystem::path& documents, const std::filesystem::path& savedGames) {
    bool cloud = false;
    for (const auto& part : documents) {
        std::wstring s = part.wstring();
        for (auto& c : s) c = (wchar_t)((c >= L'A' && c <= L'Z') ? c - L'A' + L'a' : c);
        if (s == L"onedrive" || s.rfind(L"onedrive - ", 0) == 0) cloud = true;
    }
    if (!documents.empty() && !cloud) return documents / L"My Games" / L"walkgrid";
    if (!savedGames.empty()) return savedGames / L"walkgrid";
    return {};
}
// The game's folder, created if need be. Empty if there's no safe place for
// it; callers then fall back to the working directory.
std::filesystem::path GameDataDirectory();
// A named folder inside it (created if need be), or empty.
std::filesystem::path GameSubdirectory(const wchar_t* name);
// The game's shipped files of one kind: <root>/assets/<kind>, searched for
// from the working directory, then the exe's folder and up to three above
// it (a Visual Studio build runs from x64\Release). Empty if not found.
std::filesystem::path FindAssetDirectory(const wchar_t* kind);
// Where compiled shaders are cached between runs (render.cpp; Part XVI).
std::filesystem::path ShaderCacheDirectory();
// A fresh file name for a screenshot (F2): Screenshots\shot_<date>_<time>.png.
// Never an existing file. Empty if there's nowhere safe to write.
std::filesystem::path NextScreenshotPath();
// Writes a text file (e.g. the performance report) into the game's folder,
// replacing any old one. Returns the full path written, or "" on failure.
std::string WriteTextToSaveFolder(const char* fileName, const std::string& text);
// Crash-safe replace: writes <path>.tmp, then (if keepBackup) moves the
// old file to <path>.bak, then renames the new one into place.
bool WriteFileSafely(const std::filesystem::path& path, const void* data, size_t size, bool keepBackup);
// The whole file, or false if it's missing, empty or unreadable.
bool ReadWholeFile(const std::filesystem::path& path, std::vector<uint8_t>& out);
