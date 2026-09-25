// game_internal.h
//
// Private to the game layer's own files (game.cpp, input.cpp, menus.cpp,
// hud.cpp): the state and helpers they share. game.h is what the rest of
// the program sees; nothing outside these four files includes this.
// Split out of one game.cpp in M0.11 -- the code moved, it didn't change.
// Game layer (5).

#pragma once

#ifndef NOMINMAX // also set project-wide (walkgrid.vcxproj)
#define NOMINMAX
#endif
#include <windows.h>
#include "game.h"
#include "world.h"
#include "render.h"
#include "audio.h"
#include "worldsound.h"
#include "gamefiles.h"
#include "profiler.h"
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

// ---- Shared state (defined in game.cpp) ----
enum class GameState { Title, InGame };
enum class SlotPickerMode { New, Load };
extern int g_currentSlot; // which of the MAX_SAVE_SLOTS files Save/Load/QuickSave/QuickLoad act on this session
extern bool g_keyDown[256];
extern GameState g_gameState;
extern MenuScreen g_optionsReturnScreen;
extern SlotPickerMode g_slotPickerMode;
extern MenuScreen g_slotPickerReturnScreen;
extern int g_mouseX, g_mouseY;
extern std::string g_toastMessage;

// ---- Menu layouts (every screen's panel and rows) ----
// Generic submenu layout: every menu screen (Pause and each settings
// submenu) is a titled panel of stacked full-width rows plus, for a few
// rows, an inline slider. One shared layout system parameterized by row
// count/height means Keybindings' 12 compact rows and Look Settings'
// taller slider rows don't need duplicated panel/row-rect math.
struct UIRect { float x0, y0, x1, y1; };
struct SubmenuLayout { float panelW, rowH, rowGap, topMargin, bottomMargin; int rowCount; };

inline UIRect SubmenuPanelRect(const SubmenuLayout& L) {
    float h = L.topMargin + L.rowCount * (L.rowH + L.rowGap) - L.rowGap + L.bottomMargin;
    float px = (g_screenW - L.panelW) / 2.0f, py = (g_screenH - h) / 2.0f;
    return { px, py, px + L.panelW, py + h };
}
inline UIRect SubmenuRowRect(const SubmenuLayout& L, int rowIndex) {
    UIRect panel = SubmenuPanelRect(L);
    float bw = L.panelW - 60.0f;
    float bx = panel.x0 + 30.0f;
    float by = panel.y0 + L.topMargin + rowIndex * (L.rowH + L.rowGap);
    return { bx, by, bx + bw, by + L.rowH };
}
inline bool PointInRect(int px, int py, const UIRect& r) {
    return px >= r.x0 && px <= r.x1 && py >= r.y0 && py <= r.y1;
}

// Pause itself only handles resuming, save/load, and quitting -- every
// settings category now lives one level down in OptionsHub (Section
// 13), shared with the title screen's own Options button, rather than
// listing all six categories directly in both places.
static const SubmenuLayout PAUSE_LAYOUT    = { 320.0f, 40.0f, 12.0f, 70.0f, 20.0f, 6 };
enum PauseRow { PROW_RESUME = 0, PROW_OPTIONS = 1, PROW_SAVE = 2, PROW_LOAD = 3, PROW_QUIT_TO_TITLE = 4, PROW_QUIT = 5 };

// The options hub: one settings-category picker shared by Pause (mid-
// game) and the title screen (pre-game) alike, since every submenu
// underneath it is pure global-preference state with no dependency on
// a loaded world.
static const SubmenuLayout OPTIONS_HUB_LAYOUT = { 340.0f, 40.0f, 12.0f, 70.0f, 20.0f, 7 };
enum OptionsHubRow { OHROW_LOOK = 0, OHROW_GRAPHICS = 1, OHROW_DISPLAY = 2, OHROW_AUDIO = 3, OHROW_ACCESSIBILITY = 4, OHROW_KEYBINDS = 5, OHROW_BACK = 6 };

// The title screen (Section 12): shown at startup instead of dropping
// straight into gameplay, and again after "Quit to Title" from Pause.
static const SubmenuLayout TITLE_LAYOUT = { 320.0f, 44.0f, 14.0f, 90.0f, 20.0f, 4 };
enum TitleRow { TROW_NEW_GAME = 0, TROW_LOAD_GAME = 1, TROW_OPTIONS = 2, TROW_QUIT = 3 };

// One row per save slot plus BACK. Rows beyond MAX_SAVE_SLOTS-1 are
// BACK; see SLOTROW_BACK below rather than a fixed enum, since the slot
// count is a constant, not a fixed small set of named rows.
static const SubmenuLayout SLOT_PICKER_LAYOUT = { 420.0f, 48.0f, 10.0f, 90.0f, 20.0f, MAX_SAVE_SLOTS + 1 };
static const int SLOTROW_BACK = MAX_SAVE_SLOTS;

// Look Settings: separate X/Y sensitivity sliders and separate X/Y
// inversion, per the request -- a single combined sensitivity value
// didn't let the two axes be tuned independently.
static const SubmenuLayout LOOK_LAYOUT     = { 400.0f, 56.0f, 12.0f, 70.0f, 20.0f, 6 };
enum LookRow { LROW_INVERT_X = 0, LROW_SENS_X = 1, LROW_INVERT_Y = 2, LROW_SENS_Y = 3, LROW_RESET = 4, LROW_BACK = 5 };

// Graphics: one real setting -- render distance -- rather than stubbing
// out controls (fog distance, shadow quality, etc.) this prototype has
// no rendering path for yet.
// Row height 50 still fits the slider row (label, track, 50px hit area)
// while keeping six rows comfortably inside a 720p screen.
static const SubmenuLayout GRAPHICS_LAYOUT = { 400.0f, 50.0f, 10.0f, 70.0f, 20.0f, 10 };
enum GraphicsRow { GROW_RENDER_DIST = 0, GROW_FINE_DETAIL = 1, GROW_FRAME_LIMIT = 2, GROW_VSYNC = 3, GROW_SHADOWS = 4, GROW_OUTLINES = 5, GROW_SSAO = 6, GROW_BLOOM = 7, GROW_RESET = 8, GROW_BACK = 9 };

// Display: one real setting -- an FPS counter toggle. Resolution/
// fullscreen switching would need swap-chain resize and WM_SIZE
// handling this prototype doesn't have yet, so it isn't faked here.
static const SubmenuLayout DISPLAY_LAYOUT  = { 340.0f, 40.0f, 12.0f, 70.0f, 20.0f, 5 };
enum DisplayRow { DROW_SHOW_FPS = 0, DROW_SHOW_PROFILER = 1, DROW_FULLSCREEN = 2, DROW_RESET = 3, DROW_BACK = 4 };

// Audio: Master and Music sliders, backed by a real XAudio2 voice
// (Section 10) playing the procedural ambient track. Separate channels
// now even though Music is the only one with anything to play yet, so a
// future SFX channel is one more slider, not a remix of this one.
static const SubmenuLayout AUDIO_LAYOUT    = { 380.0f, 56.0f, 12.0f, 70.0f, 20.0f, 5 };
enum AudioRow { AROW_MASTER_VOLUME = 0, AROW_MUSIC_VOLUME = 1, AROW_WORLD_VOLUME = 2, AROW_RESET = 3, AROW_BACK = 4 };

// Accessibility: a real, working slice rather than every idea discussed
// -- a field-of-view slider (motion/vestibular comfort: neither wider
// nor narrower is universally more comfortable, so this is a slider a
// player tunes either direction, not a binary toggle), a toggle-to-move
// mode for WASD (motor accessibility: movement no longer requires
// holding a key down for the whole duration), and a high-contrast UI
// palette (low-vision legibility). Deliberately NOT here yet: a "reduce
// flashing" toggle, since nothing in this prototype flashes or strobes
// today -- the actual commitment (Section 11) is that no future effect
// introduces uncontrolled flashing/strobing at all, which a toggle
// controlling zero real effects wouldn't strengthen; a colorblind-safe
// palette, since nothing in the current UI conveys meaning through hue
// alone yet (nothing to remap); and a UI scale slider, which (unlike
// the above) is real future work, just architecturally bigger -- every
// hit-rect, not only the visuals, would need to move in lockstep.
static const SubmenuLayout ACCESSIBILITY_LAYOUT = { 400.0f, 56.0f, 12.0f, 70.0f, 20.0f, 7 };
enum AccessibilityRow { ARROW_FOV = 0, ARROW_TOGGLE_MOVE = 1, ARROW_HIGH_CONTRAST = 2, ARROW_MUSIC_INTENSITY = 3, ARROW_MONO = 4, ARROW_RESET = 5, ARROW_BACK = 6 };

// Keybindings: every action bindable to any keyboard key or the left/
// right/middle mouse button (GameAction/g_actionNames/g_keyBindings/
// MOUSE_LEFT etc. are declared in persist.h since Part VII's save/load
// code needs them too). Scope decisions worth being explicit about: no
// gamepad support exists in this prototype to bind to; mouse wheel and
// side (X1/X2) buttons aren't bindable inputs yet; the 9 hotbar-select
// keys stay fixed rather than adding 9 more rows; and rebinding does
// not warn about or prevent two actions sharing the same input.
inline const char* const g_actionLabels[ACT_COUNT] = { // on-screen text
    "MOVE FORWARD", "MOVE BACK", "MOVE LEFT", "MOVE RIGHT", "JUMP",
    "BREAK BLOCK", "PLACE BLOCK", "PAUSE MENU", "QUICK SAVE", "QUICK LOAD",
    "SPRINT", "CROUCH / SLIDE", "BLOCK LIBRARY"
};
// Rows sized so all of them (+reset +back) fit the minimum 680 px window.
static const SubmenuLayout KEYBIND_LAYOUT = { 480.0f, 26.0f, 5.0f, 88.0f, 18.0f, ACT_COUNT + 2 };

static const int g_defaultBindings[ACT_COUNT] = {
    'W', 'S', 'A', 'D', VK_SPACE, MOUSE_LEFT, MOUSE_RIGHT, VK_ESCAPE, VK_F5, VK_F9, VK_SHIFT, VK_CONTROL, 'E'
};

// ---- Sliders (menus.cpp) ----
// A handful of settings are sliders rather than toggles/buttons. One
// small generic slider system (value/range/row-rect all looked up by
// ID) instead of one-off X-sensitivity-shaped code repeated per slider.
enum SliderId { SLIDER_NONE = -1, SLIDER_SENS_X = 0, SLIDER_SENS_Y = 1, SLIDER_RENDER_DIST = 2, SLIDER_MASTER_VOLUME = 3, SLIDER_MUSIC_VOLUME = 4, SLIDER_FOV = 5, SLIDER_MUSIC_INTENSITY = 6, SLIDER_WORLD_VOLUME = 7, SLIDER_FRAME_LIMIT = 8, SLIDER_FINE_DETAIL = 9 };

struct SliderRange { float minV, maxV; };

// ---- Shared state defined beside its code ----
extern int g_rebindingAction;          // input.cpp
extern bool g_mouseButtonDown[3];      // input.cpp
extern int g_draggingSlider;           // menus.cpp
extern bool g_pressActive;             // menus.cpp
extern int g_pressX, g_pressY;         // menus.cpp
extern UIRect g_pressRect;             // menus.cpp
extern bool g_pressRectValid;          // menus.cpp
extern LibraryGesture g_libGesture;    // menus.cpp
extern int g_libScroll;                // menus.cpp
extern float g_autosaveTimer;          // game.cpp
extern bool g_screenshotRequested;     // game.cpp
extern bool g_timeScrubbing;           // game.cpp

// ---- Functions shared between the four files ----
void ResetLookSettings();  // menus.cpp
void ResetDisplaySettings();  // menus.cpp
void ResetAudioSettings();  // menus.cpp
UIRect GetSliderTrackRect(UIRect r);  // menus.cpp
UIRect GetSliderHitRect(UIRect r);  // menus.cpp

// hud.cpp
int UIBandForScale(float scale);
void UIGlyphRect(const UIFontBand& fb, int cell, float& u0, float& v0, float& u1, float& v1);
int UICharCell(char c);
void UIAddQuad(std::vector<UIVertex>& v, float x0, float y0, float x1, float y1,
                       float u0, float v0, float u1, float v1,
                       float r, float g, float b, float a);
void UIDrawRect(std::vector<UIVertex>& v, float x0, float y0, float x1, float y1,
                        float r, float g, float b, float a);
float UITextWidth(const std::string& text, float scale);
float UITextHeight(float scale);
void UIDrawText(std::vector<UIVertex>& v, const std::string& text, float x, float y,
                        float scale, float r, float g, float b, float a);
void UIDrawBatch(const UIVertex* verts, size_t count, ID3D11ShaderResourceView* srv);

// game.cpp
void PickAndAct(bool breakBlock);
std::string DayTimeLabel(float t);
bool DebugKeyFree(int vk);
void JumpToNextTimeOfDay();
void DoSave();
void AutosaveNow(bool announce);
std::string PerfReportHeader();
void WriteGameSettings(std::string& out);
void ReadGameSettings(const SettingsMap& kv);
void DoLoad();
void ResetWorldForNewGame();
void EnterGameplay();

// input.cpp
void CaptureMouseForPlay();
void ReleaseMouseForMenu();
int MouseButtonIndex(int code);
bool IsInputDown(int code);
bool IsMovementAction(GameAction a);
GameAction OppositeMove(GameAction a);
void ToggleMoveLatches(int code);
std::string GetInputDisplayName(int code);
void ToggleFullscreenSetting();
bool IsSettingsSubmenu(MenuScreen s);
void FireBoundAction(int code);

// menus.cpp
void ResetKeybindingsToDefault();
void ResetGraphicsSettings();
void ResetAccessibilitySettings();
SliderRange GetSliderRange(int id);
UIRect GetSliderRowRect(int id);
float GetSliderValue(int id);
void SetSliderValue(int id, float v);
std::string GetSliderLabel(int id);
void ApplySliderDrag(int mx);
void BeginSliderDrag(int id, int mx);
void HandleMenuClick(int mx, int my);
void HandleOptionsHubClick(int mx, int my);
void HandleLookSettingsClick(int mx, int my);
void HandleGraphicsClick(int mx, int my);
void HandleDisplayClick(int mx, int my);
void HandleAudioClick(int mx, int my);
void HandleAccessibilityClick(int mx, int my);
void HandleKeybindingsClick(int mx, int my);
void HandleTitleClick(int mx, int my);
void HandleSlotPickerClick(int mx, int my);
void OpenLibrary();
void CloseLibrary(bool quiet = false);
void LibraryMouseDown(int mx, int my);
void LibraryMouseUp(int mx, int my);
bool PressActsImmediately(int mx, int my);
void DispatchMenuClick(int mx, int my);
