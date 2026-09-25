// gamefiles.cpp
//
// Implementations for gamefiles.h: the game's folder under Documents and
// its subfolders, and the crash-safe file writes everything else uses.
// Moved from Voxistics' persist.cpp (M0.9) unchanged in behaviour.

#ifndef NOMINMAX // also set project-wide (walkgrid.vcxproj)
#define NOMINMAX
#endif
#include <windows.h>
#include <shlobj.h> // SHGetKnownFolderPath
#include "gamefiles.h"
#include <cstdio>
#include <cwchar>
#include <fstream>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "uuid.lib") // provides the FOLDERID_* GUID data (declared, not defined, in knownfolders.h)

// Shared by every folder below: checks
// exists()&&!is_directory() before create_directories() specifically to
// catch a plain file already occupying part of the intended path,
// rather than letting a failed directory creation surface as a
// mysterious save failure. Empty return means "use the fallback"
// (the current working directory) rather than this path.
static std::filesystem::path EnsureDirectoryBulletproof(std::filesystem::path dir, const char* what) {
    namespace fs = std::filesystem;
    if (dir.empty()) return fs::path();
    std::error_code ec;
    if (fs::exists(dir, ec) && !fs::is_directory(dir, ec)) {
        char msg[256];
        snprintf(msg, sizeof(msg), "%s: a file already occupies the intended directory path, falling back\n", what);
        OutputDebugStringA(msg);
        return fs::path();
    }
    fs::create_directories(dir, ec);
    if (ec || !fs::is_directory(dir, ec)) {
        char msg[256];
        snprintf(msg, sizeof(msg), "%s: could not create the directory, falling back\n", what);
        OutputDebugStringA(msg);
        return fs::path();
    }
    return dir;
}

// Resolves (creating if needed) Documents\My Games\walkgrid -- the
// conventional PC-game save location: visible and easy for players to
// find, back up, or copy between machines, unlike a hidden AppData
// folder. Falls back to the current working directory (this prototype's
// original behavior) if the known-folder lookup fails for any reason,
// or if something unexpected already occupies part of the intended
// path -- e.g. a plain file sitting where a folder needs to be. A save
// attempt should always have somewhere safe to go rather than failing
// forever because the "nice" location didn't pan out.
std::filesystem::path GameDataDirectory() {
    namespace fs = std::filesystem;
    PWSTR docsPath = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &docsPath);
    fs::path dir;
    if (SUCCEEDED(hr) && docsPath) {
        dir = fs::path(docsPath) / L"My Games" / L"walkgrid";
    }
    if (docsPath) CoTaskMemFree(docsPath);

    if (dir.empty()) {
        OutputDebugStringA("GameDataDirectory: could not resolve Documents, falling back to working directory\n");
        return fs::path();
    }
    return EnsureDirectoryBulletproof(dir, "GameDataDirectory");
}

std::filesystem::path GameSubdirectory(const wchar_t* name) {
    std::filesystem::path base = GameDataDirectory();
    if (base.empty()) return base;
    return EnsureDirectoryBulletproof(base / name, "GameSubdirectory");
}

std::filesystem::path ShaderCacheDirectory() {
    return GameSubdirectory(L"ShaderCache");
}

std::filesystem::path NextScreenshotPath() {
    std::filesystem::path dir = GameSubdirectory(L"Screenshots");
    if (dir.empty()) return dir;
    // GetLocalTime rather than the CRT clock call, which is rejected by the
    // owner's Visual Studio build (SDL checks, tools/check_msvc.sh).
    SYSTEMTIME t; GetLocalTime(&t);
    wchar_t stem[64];
    swprintf(stem, 64, L"shot_%04u-%02u-%02u_%02u-%02u-%02u", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
    // Two shots in the same second get _2, _3, ... rather than overwriting.
    std::error_code ec;
    std::filesystem::path path = dir / (std::wstring(stem) + L".png");
    for (int n = 2; std::filesystem::exists(path, ec) && n < 100; n++)
        path = dir / (std::wstring(stem) + L"_" + std::to_wstring(n) + L".png");
    return path;
}

std::string WriteTextToSaveFolder(const char* fileName, const std::string& text) {
    std::filesystem::path dir = GameDataDirectory();
    std::filesystem::path path = dir.empty() ? std::filesystem::path(fileName) : dir / fileName;
    std::ofstream f(path, std::ios::binary | std::ios::trunc); // wide paths, and no deprecated CRT calls (MSVC SDL checks)
    if (!f) return "";
    f.write(text.data(), (std::streamsize)text.size());
    f.close();
    return f ? path.string() : "";
}

// Crash-safe write (Section 7.3): the whole buffer goes to <path>.tmp
// first; only once that has fully succeeded does the old file (if any)
// become <path>.bak (when asked for) and the new one take its name. A
// crash or power cut at any point leaves the previous file intact.
bool WriteFileSafely(const std::filesystem::path& path, const void* data, size_t size, bool keepBackup) {
    namespace fs = std::filesystem;
    fs::path tmpPath = path; tmpPath += L".tmp";
    {
        std::ofstream out(tmpPath, std::ios::binary | std::ios::trunc);
        if (!out) return false;
        out.write((const char*)data, (std::streamsize)size);
        if (!out) return false;
    }
    std::error_code ec;
    if (keepBackup && fs::exists(path, ec)) {
        fs::path bakPath = path; bakPath += L".bak";
        fs::remove(bakPath, ec);
        fs::rename(path, bakPath, ec);
    }
    ec.clear();
    fs::rename(tmpPath, path, ec);
    return !ec;
}

bool ReadWholeFile(const std::filesystem::path& path, std::vector<uint8_t>& out) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) return false;
    std::streamsize size = in.tellg();
    if (size <= 0) return false;
    in.seekg(0);
    out.resize((size_t)size);
    in.read((char*)out.data(), size);
    return (bool)in;
}
