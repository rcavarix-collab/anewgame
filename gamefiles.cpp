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
#include "strtable.h" // WideToUtf8
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

// The game's folder (D45): Documents\My Games\walkgrid in the Documents
// folder Windows shows the player -- where they look, and where other games
// put theirs -- unless that Documents has been moved into OneDrive, then
// Saved Games\walkgrid (never a cloud-synced folder, D28). See
// ChooseGameFolder (gamefiles.h). A failed lookup falls back to the working
// directory, so a save always has somewhere to go.
static std::filesystem::path KnownFolder(REFKNOWNFOLDERID id) {
    PWSTR p = nullptr;
    std::filesystem::path out;
    if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &p)) && p) out = p;
    if (p) CoTaskMemFree(p);
    return out;
}
static std::filesystem::path ResolveGameDataDirectory() {
    return ChooseGameFolder(KnownFolder(FOLDERID_Documents), KnownFolder(FOLDERID_SavedGames));
}

// Where earlier builds kept the folder: %USERPROFILE%\Documents\My Games\walkgrid
// (M0.16 to M1.12, which Explorer doesn't show when Documents is redirected),
// and Saved Games\walkgrid. Once, if the chosen folder doesn't exist yet and
// one of those does, its contents move across -- a rename when they share a
// drive, else a copy and then removal -- so settings, saves and screenshots
// aren't left behind.
static void MoveOldGameFolderOnce(const std::filesystem::path& dir) {
    namespace fs = std::filesystem;
    static bool done = false;
    if (done) return;
    done = true;
    std::error_code ec;
    if (fs::exists(dir, ec)) return;
    fs::path profile = KnownFolder(FOLDERID_Profile), saved = KnownFolder(FOLDERID_SavedGames);
    std::vector<fs::path> olds;
    if (!profile.empty()) olds.push_back(profile / L"Documents" / L"My Games" / L"walkgrid");
    if (!saved.empty()) olds.push_back(saved / L"walkgrid");
    for (const fs::path& old : olds) {
        if (!fs::is_directory(old, ec) || fs::equivalent(old, dir, ec)) { ec.clear(); continue; }
        fs::create_directories(dir.parent_path(), ec);
        ec.clear();
        fs::rename(old, dir, ec);
        if (!ec) return;
        ec.clear();
        fs::copy(old, dir, fs::copy_options::recursive, ec);
        if (!ec) fs::remove_all(old, ec);
        return;
    }
}

std::filesystem::path GameDataDirectory() {
    std::filesystem::path dir = ResolveGameDataDirectory();
    if (dir.empty()) {
        OutputDebugStringA("GameDataDirectory: could not resolve the profile folder, falling back to working directory\n");
        return std::filesystem::path();
    }
    MoveOldGameFolderOnce(dir);
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
    {
        std::ofstream f(path, std::ios::binary | std::ios::trunc); // wide paths, and no deprecated CRT calls (MSVC SDL checks)
        if (!f) return "";
        f.write(text.data(), (std::streamsize)text.size());
        if (!f) return "";
    }
    // Only claim what's really on disk, by its full path (a relative
    // fallback name alone once read as a folder that didn't exist).
    std::error_code ec;
    if (!std::filesystem::exists(path, ec)) return "";
    std::filesystem::path full = std::filesystem::absolute(path, ec);
    std::wstring w = (ec ? path : full).wstring();
    return WideToUtf8(w); // UTF-8, as the toast draws it
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

std::filesystem::path FindAssetDirectory(const wchar_t* kind) {
    namespace fs = std::filesystem;
    std::error_code ec;
    std::vector<fs::path> roots = { fs::current_path(ec) };
    wchar_t exe[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, exe, MAX_PATH);
    if (n > 0 && n < MAX_PATH) {
        fs::path dir = fs::path(exe).parent_path();
        for (int i = 0; i < 4 && !dir.empty(); i++) { roots.push_back(dir); dir = dir.parent_path(); }
    }
    for (const fs::path& r : roots) {
        fs::path candidate = r / "assets" / kind;
        if (fs::is_directory(candidate, ec)) return candidate;
    }
    return {};
}
