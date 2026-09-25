// settings.h
//
// Input bindings and preferences (DESIGN.md 7.2.2): settings.cfg, a plain
// key=value text file in the game's folder, read once at start-up and
// rewritten whenever a setting changes. Platform layer (2): it holds the
// engine-wide preferences (look, graphics, display, audio, accessibility,
// keybindings). Settings that belong to the game (the hotbar, the render
// distance, which the world layer owns) are stored through a hook the game
// fills (SetGameSettingsHooks), so this layer never names them.

#pragma once

#include <string>
#include <unordered_map>

enum GameAction {
    ACT_FORWARD, ACT_BACK, ACT_LEFT, ACT_RIGHT, ACT_JUMP,
    ACT_BREAK, ACT_PLACE, ACT_MENU, ACT_SAVE, ACT_LOAD,
    ACT_SPRINT, ACT_CROUCH, ACT_LIBRARY, // appended: saved bindings are by name, so older settings files still load
    ACT_COUNT
};
// Stable identity for settings.cfg, same idea as block names in saves.
extern const char* g_actionNames[ACT_COUNT];
static const int MOUSE_LEFT = -1, MOUSE_RIGHT = -2, MOUSE_MIDDLE = -3; // share the bound-input-code space with VK_* (all positive)
extern int g_keyBindings[ACT_COUNT];

extern float g_sensitivityMultX, g_sensitivityMultY;
extern bool g_invertX, g_invertY;
extern bool g_showFPS;
extern bool g_fullscreen; // borderless fullscreen on the window's monitor (Display settings / F11)
// Graphics effects (Sections 4.8, 4.10), each independently toggleable.
extern bool g_shadows, g_postEdges, g_postSSAO, g_bloom;
// Fine ground detail (DESIGN.md 23.6): the finest cut reaches this many
// chunks from the camera, the middle one twice as far; 0 turns it off
// (lesser machines). Default 2.
extern int g_fineDetail;
extern float g_masterVolume;
extern float g_musicVolume;
extern float g_worldVolume; // the world sound palette (sfx_synth.h), under Master
extern float g_fov; // degrees, vertical
// Accessibility (Section 11): press-to-toggle instead of hold-to-move for WASD.
extern bool g_toggleMovement;
extern bool g_highContrastUI; // higher-luminance-contrast menu palette
extern bool g_monoAudio;      // world sounds centred: no stereo placement (Part XI)
extern bool g_vsync;          // present in step with the display (Graphics)
extern int g_frameLimit;      // frames per second cap, 30-200 (Graphics)
extern bool g_moveToggleLatch[ACT_COUNT]; // only ACT_FORWARD/BACK/LEFT/RIGHT indices are ever used
// Music Intensity (Accessibility, Section 11): 0 = ambient bed only, no
// arp/pulse layer at all; 1 = the full designed arc -- a ceiling, not a
// ceiling-breaker (see audio module docs / DESIGN.md Part XIV.3).
extern float g_musicIntensity;
bool SaveSettings();
void LoadSettings();
void ClampSettingsToValidRanges();

// The game's own settings.cfg keys. `write` appends "key=value\n" lines;
// `read` receives every key the file holds (unknown ones included) once
// the engine's own have been applied. Set before LoadSettings.
using SettingsMap = std::unordered_map<std::string, std::string>;
using SettingsWriteFn = void (*)(std::string& out);
using SettingsReadFn = void (*)(const SettingsMap& kv);
void SetGameSettingsHooks(SettingsWriteFn write, SettingsReadFn read);
// Reading helpers for the hook: a missing key keeps `def`.
float SettingsGetF(const SettingsMap& kv, const char* key, float def);
int SettingsGetI(const SettingsMap& kv, const char* key, int def);
