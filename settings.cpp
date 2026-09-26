// settings.cpp
//
// Implementations for settings.h: the preference globals and settings.cfg
// read/write. Moved from Voxistics' persist.cpp (M0.9); the file format
// and every key are unchanged, and the game's keys now go through the
// hooks. Platform layer (2).

#ifndef NOMINMAX // also set project-wide (walkgrid.vcxproj)
#define NOMINMAX
#endif
#include <windows.h> // VK_* codes for the default bindings
#include "settings.h"
#include "gamefiles.h"
#include "profiler.h" // g_showProfiler, kept in settings.cfg
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>

// =======================================================================
// Input bindings + gameplay/UI preferences
// =======================================================================

const char* g_actionNames[ACT_COUNT] = {
    "forward", "back", "left", "right", "jump", "break", "place", "menu", "save", "load", "sprint", "crouch", "library"
};
int g_keyBindings[ACT_COUNT] = {
    'W', 'S', 'A', 'D', VK_SPACE, MOUSE_LEFT, MOUSE_RIGHT, VK_ESCAPE, VK_F5, VK_F9, VK_SHIFT, VK_CONTROL, 'E'
};
float g_sensitivityMultX = 1.0f, g_sensitivityMultY = 1.0f;
bool g_invertX = false, g_invertY = false;
bool g_showFPS = false;
bool g_fullscreen = false;
bool g_shadows = true, g_postEdges = false, g_postSSAO = false, g_bloom = true;
int g_fineDetail = 4; // everywhere (D49; groundmesh.h GROUND_FINE_ALL)
float g_masterVolume = 1.0f;
float g_musicVolume = 1.0f;
float g_worldVolume = 1.0f;
float g_footstepVolume = 1.0f;
float g_fov = 45.0f;
std::string g_language = "en";
bool g_toggleMovement = false;
bool g_highContrastUI = false;
bool g_monoAudio = false;
bool g_vsync = true;
int g_frameLimit = 60;
bool g_moveToggleLatch[ACT_COUNT] = {};
float g_musicIntensity = 1.0f;

static SettingsWriteFn g_gameWrite = nullptr;
static SettingsReadFn g_gameRead = nullptr;
void SetGameSettingsHooks(SettingsWriteFn write, SettingsReadFn read) { g_gameWrite = write; g_gameRead = read; }
float SettingsGetF(const SettingsMap& kv, const char* key, float def) { auto it = kv.find(key); return it == kv.end() ? def : (float)atof(it->second.c_str()); }
int SettingsGetI(const SettingsMap& kv, const char* key, int def) { auto it = kv.find(key); return it == kv.end() ? def : atoi(it->second.c_str()); }

// =======================================================================
// Section 7.2.3 - Global settings file
// =======================================================================
//
// Gameplay/UI preferences (sensitivity, inversion, render distance, the
// FPS toggle, volumes, keybindings) live in their own small text file,
// separate from any world save, so they're available before any save is
// loaded (e.g. a title screen's Options) and carry over between saves
// rather than being tied to one. Plain "key=value" lines rather than the
// versioned binary format saves use: it's a handful of scalars a player
// might reasonably want to hand-edit or inspect, and forward/backward
// compatibility just falls out of "unknown keys are ignored, missing
// keys keep their compiled-in default" with no version field needed.
static std::filesystem::path GetSettingsFilePath() {
    std::filesystem::path dir = GameDataDirectory(); // same bulletproofed directory as the saves
    std::filesystem::path filename = L"settings.cfg";
    return dir.empty() ? filename : dir / filename;
}

bool SaveSettings() {
    std::ostringstream ss;
    ss << "sensitivityX=" << g_sensitivityMultX << "\n";
    ss << "sensitivityY=" << g_sensitivityMultY << "\n";
    ss << "invertX=" << (g_invertX ? 1 : 0) << "\n";
    ss << "invertY=" << (g_invertY ? 1 : 0) << "\n";
    ss << "showFPS=" << (g_showFPS ? 1 : 0) << "\n";
    ss << "showProfiler=" << (g_showProfiler ? 1 : 0) << "\n";
    ss << "fullscreen=" << (g_fullscreen ? 1 : 0) << "\n";
    ss << "sun_shadows=" << (g_shadows ? 1 : 0) << "\n";
    ss << "fine_detail_2=" << g_fineDetail << "\n"; // a new key: the old one held the old default (2), not a choice
    ss << "outlines=" << (g_postEdges ? 1 : 0) << "\n";
    ss << "ssao=" << (g_postSSAO ? 1 : 0) << "\n";
    ss << "bloom=" << (g_bloom ? 1 : 0) << "\n";
    ss << "masterVolume=" << g_masterVolume << "\n";
    ss << "musicVolume=" << g_musicVolume << "\n";
    ss << "worldVolume=" << g_worldVolume << "\n";
    ss << "footstepVolume=" << g_footstepVolume << "\n";
    ss << "fov=" << g_fov << "\n";
    ss << "toggleMovement=" << (g_toggleMovement ? 1 : 0) << "\n";
    ss << "highContrastUI=" << (g_highContrastUI ? 1 : 0) << "\n";
    ss << "monoAudio=" << (g_monoAudio ? 1 : 0) << "\n";
    ss << "vsync=" << (g_vsync ? 1 : 0) << "\n";
    ss << "frameLimit=" << g_frameLimit << "\n";
    ss << "musicIntensity=" << g_musicIntensity << "\n";
    ss << "language=" << g_language << "\n";
    for (int i = 0; i < ACT_COUNT; i++) {
        ss << "keybind." << g_actionNames[i] << "=" << g_keyBindings[i] << "\n"; // name-indexed, same reasoning as g_blockNames
    }

    std::string data = ss.str();
    if (g_gameWrite) g_gameWrite(data);
    return WriteFileSafely(GetSettingsFilePath(), data.data(), data.size(), false);
}

// settings.cfg is plain text a player may hand-edit, so values read from
// it are clamped to the same ranges the menu
// sliders allow (the game's own keys are clamped by its read hook).
static float ClampF(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
void ClampSettingsToValidRanges() {
    g_sensitivityMultX = ClampF(g_sensitivityMultX, 0.25f, 3.0f);
    g_sensitivityMultY = ClampF(g_sensitivityMultY, 0.25f, 3.0f);
    g_masterVolume = ClampF(g_masterVolume, 0.0f, 1.0f);
    g_musicVolume = ClampF(g_musicVolume, 0.0f, 1.0f);
    g_worldVolume = ClampF(g_worldVolume, 0.0f, 1.0f);
    g_footstepVolume = ClampF(g_footstepVolume, 0.0f, 2.0f);
    g_fov = ClampF(g_fov, 45.0f, 100.0f);
    g_musicIntensity = ClampF(g_musicIntensity, 0.0f, 1.0f);
}

// Missing file (first run) or missing/unrecognized individual keys
// (an older settings.cfg from before some setting existed) both just
// keep whatever the caller's compiled-in default already was -- loading
// settings can only ever refine current state, never fail outright.
void LoadSettings() {
    std::ifstream in(GetSettingsFilePath());
    if (!in) return;

    std::unordered_map<std::string, std::string> kv;
    std::string line;
    while (std::getline(in, line)) {
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        kv[line.substr(0, eq)] = line.substr(eq + 1);
    }

    auto getF = [&](const char* k, float def) { auto it = kv.find(k); return it == kv.end() ? def : (float)atof(it->second.c_str()); };
    auto getI = [&](const char* k, int def) { auto it = kv.find(k); return it == kv.end() ? def : atoi(it->second.c_str()); };
    auto getB = [&](const char* k, bool def) { auto it = kv.find(k); return it == kv.end() ? def : (atoi(it->second.c_str()) != 0); };

    g_sensitivityMultX = getF("sensitivityX", g_sensitivityMultX);
    g_sensitivityMultY = getF("sensitivityY", g_sensitivityMultY);
    g_invertX = getB("invertX", g_invertX);
    g_invertY = getB("invertY", g_invertY);
    g_showFPS = getB("showFPS", g_showFPS);
    g_showProfiler = getB("showProfiler", g_showProfiler);
    g_fullscreen = getB("fullscreen", g_fullscreen);
    // "sun_shadows", not the old "shadows": shadows now carry the lighting
    // (4.9) and default on, so an old file's default-off doesn't stick.
    g_shadows = getB("sun_shadows", g_shadows);
    g_fineDetail = (int)getF("fine_detail_2", (float)g_fineDetail);
    g_fineDetail = g_fineDetail < 0 ? 0 : g_fineDetail > 4 ? 4 : g_fineDetail;
    g_postEdges = getB("outlines", g_postEdges);
    g_postSSAO = getB("ssao", g_postSSAO);
    g_bloom = getB("bloom", g_bloom);
    g_masterVolume = getF("masterVolume", g_masterVolume);
    g_musicVolume = getF("musicVolume", g_musicVolume);
    g_worldVolume = getF("worldVolume", g_worldVolume);
    g_footstepVolume = getF("footstepVolume", g_footstepVolume);
    g_fov = getF("fov", g_fov);
    g_toggleMovement = getB("toggleMovement", g_toggleMovement);
    g_highContrastUI = getB("highContrastUI", g_highContrastUI);
    g_monoAudio = getB("monoAudio", g_monoAudio);
    g_vsync = getB("vsync", g_vsync);
    g_frameLimit = (int)getF("frameLimit", (float)g_frameLimit);
    if (g_frameLimit < 30) g_frameLimit = 30;
    if (g_frameLimit > 200) g_frameLimit = 200;
    g_musicIntensity = getF("musicIntensity", g_musicIntensity);
    {
        // A file name inside assets/text: letters, digits, '-' and '_' only,
        // so a hand-edited value can't reach outside that folder.
        auto it = kv.find("language");
        std::string lang = it == kv.end() ? g_language : it->second;
        while (!lang.empty() && (lang.back() == '\r' || lang.back() == ' ')) lang.pop_back();
        bool ok = !lang.empty() && lang.size() <= 16;
        for (char c : lang) ok = ok && (isalnum((unsigned char)c) || c == '-' || c == '_');
        g_language = ok ? lang : "en";
    }
    for (int i = 0; i < ACT_COUNT; i++) {
        std::string key = std::string("keybind.") + g_actionNames[i];
        g_keyBindings[i] = getI(key.c_str(), g_keyBindings[i]);
    }
    ClampSettingsToValidRanges();
    if (g_gameRead) g_gameRead(kv);
}
