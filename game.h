// game.h
//
// Menu/UI state machine, input dispatch, the window procedure, and the
// second (orthographic UI) render pass. Everything here sits on top of
// world.h/render.h/audio.h/persist.h -- it's the layer that actually
// reacts to a click or a keypress and decides what the simulation or
// the renderer should do about it. main.cpp's message loop only reaches
// into this file for the handful of things below that it drives itself
// (registering WndProc, the per-tick input-to-action mapping, and
// kicking off the UI pass); everything else (menu layout, slider
// dragging, save-slot bookkeeping, ...) stays internal to game.cpp,
// same as it was file-static inside the original monolith.

#pragma once

#include "world.h"
#include "settings.h" // GameAction, preferences
#include "savegame.h"
#include "library.h"  // HOTBAR_SLOTS
#include <string>

// The two top-level state machines main.cpp's loop itself branches on
// (freezing the simulation accumulator while a menu is open, gating the
// mouse-look recenter). GameState (Title/InGame) stays entirely inside
// game.cpp -- nothing outside it needs to ask which one is active.
enum class MenuScreen { None, Pause, LookSettings, Graphics, Display, Audio, Keybindings, Accessibility, TitleMain, SlotPicker, OptionsHub, Library };
extern MenuScreen g_menuScreen;

extern bool g_mouseCaptured;

// Per-frame timers/counters main.cpp's loop ticks down or accumulates
// directly, same as it always did as file-static state in one function.
extern float g_toastTimer;
extern int g_confirmOverwriteSlot;
extern float g_confirmOverwriteTimer;
extern int g_fpsFrameCount, g_fpsDisplay;
extern float g_fpsTimer;

// Mouse-look scaling; main.cpp's loop applies it directly to raw cursor
// delta each frame rather than going through a function call.
static const float BASE_MOUSE_SENS = 0.0025f;

// The hotbar's ten blocks (library.h), chosen from the block library and
// saved by block name in settings.cfg so registry changes can't scramble them.
extern BlockID g_hotbar[HOTBAR_SLOTS];
// Hands settings.cpp the game's own keys (hotbar, render distance): call
// once, before LoadSettings.
void RegisterGameSettings();
// The game's own per-tick systems (FOUNDATIONS.md 2): main.cpp's fixed
// 60 Hz loop calls this once per tick, after the engine's world work
// (streaming, player physics, scheduled updates) and before the world
// sounds read the tick's results. The engine never names what runs here.
void GameTick(float dt);
// Whether an action is currently "held" per its bound input, honoring
// toggle-to-move for the four movement actions (Accessibility, Section
// 11) -- main.cpp's fixed-timestep tick calls this once per movement
// action every step.
bool IsActionDown(GameAction a);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
// Raw mouse input for looking around (input.cpp): register once the window
// exists; each frame, take the movement summed since the last take. False
// if raw input isn't available (look then falls back to the cursor).
void RegisterRawMouse(HWND hwnd);
bool TakeMouseLookDelta(int& dx, int& dy);

// Second (orthographic, depth-off) render pass: crosshair, hotbar, all
// menus, toasts, FPS counter. Assumes the world pass already ran this
// frame.
void RenderUIPass();

// Borderless fullscreen on the window's current monitor, or back to the
// window as it was. Doesn't touch the saved preference.
void ApplyFullscreen(bool on);

// Autosaves every few minutes of actual play; called once per frame.
void TickAutosave(float dt);
// True while a world is loaded (in play or in its menus), not at the title.
bool IsInGame();
// Once per frame: saves a finished performance capture (Ctrl+F3).
void PollPerfCapture();
// Screenshots (F2): saves the finished frame as a PNG in the save folder's
// Screenshots folder if F2 was pressed since the last call. Once per frame,
// after the UI pass and before Present. Only acts when asked; local file only.
void TakeScreenshotIfRequested();

// A transient centred message (save/load confirmations, startup problems).
void ShowToast(const std::string& message, float seconds);
// Debug time control (Section 13): holding ] / Page Up or [ / Page Down
// scrubs the day clock; call once per frame with the frame's real time.
void UpdateDebugTimeScrub(float frameSeconds);
