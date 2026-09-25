// menus.cpp
//
// Menus: what each click does on every screen (title, slot picker, pause,
// options and their six settings screens, keybindings, the block library),
// sliders, and the reset-to-default rows. The screens are drawn by
// hud.cpp from the layouts in game_internal.h. Moved out of game.cpp in
// M0.11, unchanged. Game layer (5).

#include "game_internal.h"

void ResetKeybindingsToDefault() {
    for (int i = 0; i < ACT_COUNT; i++) g_keyBindings[i] = g_defaultBindings[i];
}

// Look/Graphics/Display/Audio preference VALUES (g_sensitivityMultX/Y,
// g_invertX/Y, g_showFPS, g_masterVolume) live in persist.h for the same
// reason as the keybinding data above -- Section VII needs them.
static const float SENS_MIN = 0.25f, SENS_MAX = 3.0f;

void ResetLookSettings() { g_sensitivityMultX = 1.0f; g_sensitivityMultY = 1.0f; g_invertX = false; g_invertY = false; }
void ResetGraphicsSettings() {
    g_loadRadius = 3;
    g_fineDetail = 2;
    g_shadows = true; g_postEdges = false; g_postSSAO = false; g_bloom = true;
    g_vsync = true; g_frameLimit = 60;
    g_lastPlayerChunkX = INT32_MIN; g_lastPlayerChunkZ = INT32_MIN; // force a rescan at the new radius
}
void ResetDisplaySettings() { g_showFPS = false; g_showProfiler = false; if (g_fullscreen) { g_fullscreen = false; ApplyFullscreen(false); } }
void ResetAudioSettings() { g_masterVolume = 1.0f; g_musicVolume = 1.0f; g_worldVolume = 1.0f; ApplyAudioVolumes(); }
void ResetAccessibilitySettings() {
    g_fov = 45.0f;
    g_toggleMovement = false;
    g_highContrastUI = false;
    g_monoAudio = false;
    g_musicIntensity = 1.0f;
    memset(g_moveToggleLatch, 0, sizeof(g_moveToggleLatch));
}

int g_draggingSlider = SLIDER_NONE;

SliderRange GetSliderRange(int id) {
    switch (id) {
    case SLIDER_SENS_X: case SLIDER_SENS_Y: return { SENS_MIN, SENS_MAX };
    case SLIDER_RENDER_DIST: return { 1.0f, 8.0f };
    case SLIDER_FINE_DETAIL: return { 0.0f, 3.0f };
    case SLIDER_FRAME_LIMIT: return { 30.0f, 200.0f };
    case SLIDER_MASTER_VOLUME: case SLIDER_MUSIC_VOLUME: case SLIDER_WORLD_VOLUME: return { 0.0f, 1.0f };
    case SLIDER_FOV: return { 45.0f, 100.0f };
    case SLIDER_MUSIC_INTENSITY: return { 0.0f, 1.0f };
    default: return { 0.0f, 1.0f };
    }
}
// Only meaningful while the slider's own submenu is the active screen
// -- the only time it can be dragged or needs drawing.
UIRect GetSliderRowRect(int id) {
    switch (id) {
    case SLIDER_SENS_X: return SubmenuRowRect(LOOK_LAYOUT, LROW_SENS_X);
    case SLIDER_SENS_Y: return SubmenuRowRect(LOOK_LAYOUT, LROW_SENS_Y);
    case SLIDER_RENDER_DIST: return SubmenuRowRect(GRAPHICS_LAYOUT, GROW_RENDER_DIST);
    case SLIDER_FINE_DETAIL: return SubmenuRowRect(GRAPHICS_LAYOUT, GROW_FINE_DETAIL);
    case SLIDER_FRAME_LIMIT: return SubmenuRowRect(GRAPHICS_LAYOUT, GROW_FRAME_LIMIT);
    case SLIDER_MASTER_VOLUME: return SubmenuRowRect(AUDIO_LAYOUT, AROW_MASTER_VOLUME);
    case SLIDER_MUSIC_VOLUME: return SubmenuRowRect(AUDIO_LAYOUT, AROW_MUSIC_VOLUME);
    case SLIDER_WORLD_VOLUME: return SubmenuRowRect(AUDIO_LAYOUT, AROW_WORLD_VOLUME);
    case SLIDER_FOV: return SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_FOV);
    case SLIDER_MUSIC_INTENSITY: return SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_MUSIC_INTENSITY);
    default: return { 0, 0, 0, 0 };
    }
}
float GetSliderValue(int id) {
    switch (id) {
    case SLIDER_SENS_X: return g_sensitivityMultX;
    case SLIDER_SENS_Y: return g_sensitivityMultY;
    case SLIDER_RENDER_DIST: return (float)g_loadRadius;
    case SLIDER_FINE_DETAIL: return (float)g_fineDetail;
    case SLIDER_FRAME_LIMIT: return (float)g_frameLimit;
    case SLIDER_MASTER_VOLUME: return g_masterVolume;
    case SLIDER_MUSIC_VOLUME: return g_musicVolume;
    case SLIDER_WORLD_VOLUME: return g_worldVolume;
    case SLIDER_FOV: return g_fov;
    case SLIDER_MUSIC_INTENSITY: return g_musicIntensity;
    default: return 0.0f;
    }
}
void SetSliderValue(int id, float v) {
    switch (id) {
    case SLIDER_SENS_X: g_sensitivityMultX = v; break;
    case SLIDER_SENS_Y: g_sensitivityMultY = v; break;
    case SLIDER_RENDER_DIST: {
        int newRadius = (int)(v + 0.5f);
        if (newRadius != g_loadRadius) {
            g_loadRadius = newRadius;
            g_lastPlayerChunkX = INT32_MIN; g_lastPlayerChunkZ = INT32_MIN;
        }
        break;
    }
    case SLIDER_FRAME_LIMIT: g_frameLimit = (int)(v / 10.0f + 0.5f) * 10; break; // steps of 10
    case SLIDER_FINE_DETAIL: g_fineDetail = (int)(v + 0.5f); break; // the renderer rebuilds what changes (23.6)
    case SLIDER_MASTER_VOLUME: g_masterVolume = v; ApplyAudioVolumes(); break;
    case SLIDER_MUSIC_VOLUME: g_musicVolume = v; ApplyAudioVolumes(); break;
    case SLIDER_WORLD_VOLUME: g_worldVolume = v; ApplyAudioVolumes(); break;
    case SLIDER_FOV: g_fov = v; break;
    case SLIDER_MUSIC_INTENSITY: g_musicIntensity = v; break;
    }
}
std::string GetSliderLabel(int id) {
    auto pct = [](float v) { return std::to_string((int)(v * 100.0f + 0.5f)); };
    char x[16];
    switch (id) {
    case SLIDER_SENS_X: snprintf(x, sizeof(x), "%.2f", g_sensitivityMultX); return StrF("slider.sens_x", { x });
    case SLIDER_SENS_Y: snprintf(x, sizeof(x), "%.2f", g_sensitivityMultY); return StrF("slider.sens_y", { x });
    case SLIDER_RENDER_DIST: return StrF("slider.render_distance", { std::to_string(g_loadRadius) });
    case SLIDER_FINE_DETAIL: return g_fineDetail == 0 ? Str("slider.fine_detail_off") : StrF("slider.fine_detail", { std::to_string(g_fineDetail) });
    case SLIDER_FRAME_LIMIT: return StrF("slider.frame_limit", { std::to_string(g_frameLimit) }); // vsync paces frames when on (main.cpp)
    case SLIDER_MASTER_VOLUME: return StrF("slider.master_volume", { pct(g_masterVolume) });
    case SLIDER_MUSIC_VOLUME: return StrF("slider.music_volume", { pct(g_musicVolume) });
    case SLIDER_WORLD_VOLUME: return StrF("slider.world_volume", { pct(g_worldVolume) });
    case SLIDER_FOV: return StrF("slider.fov", { std::to_string((int)(g_fov + 0.5f)) });
    case SLIDER_MUSIC_INTENSITY: return StrF("slider.music_intensity", { pct(g_musicIntensity) });
    default: return "";
    }
}
// The slider track sits in the lower half of its row, with the label
// above it. The hit rect is a bit taller than the visible track so it's
// not fiddly to grab.
UIRect GetSliderTrackRect(UIRect r) { return { r.x0 + 8, r.y0 + 34, r.x1 - 8, r.y0 + 42 }; }
UIRect GetSliderHitRect(UIRect r) { return { r.x0 + 8, r.y0 + 24, r.x1 - 8, r.y0 + 50 }; }

// Sets a slider's value directly from a mouse x position along its
// track -- shared by the initial click and every subsequent drag
// update while the button stays held.
void ApplySliderDrag(int mx) {
    if (g_draggingSlider == SLIDER_NONE) return;
    UIRect track = GetSliderTrackRect(GetSliderRowRect(g_draggingSlider));
    float t = (mx - track.x0) / (track.x1 - track.x0);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    SliderRange rng = GetSliderRange(g_draggingSlider);
    SetSliderValue(g_draggingSlider, rng.minV + t * (rng.maxV - rng.minV));
}
void BeginSliderDrag(int id, int mx) {
    g_draggingSlider = id;
    SetCapture(g_hwnd); // keep receiving WM_MOUSEMOVE if the drag leaves the client area
    ApplySliderDrag(mx);
}

void HandleMenuClick(int mx, int my) {
    if (PointInRect(mx, my, SubmenuRowRect(PAUSE_LAYOUT, PROW_RESUME))) { g_menuScreen = MenuScreen::None; CaptureMouseForPlay(); StartMusicPlayback(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(PAUSE_LAYOUT, PROW_OPTIONS))) { g_optionsReturnScreen = MenuScreen::Pause; g_menuScreen = MenuScreen::OptionsHub; return; }
    if (PointInRect(mx, my, SubmenuRowRect(PAUSE_LAYOUT, PROW_SAVE))) { DoSave(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(PAUSE_LAYOUT, PROW_LOAD))) {
        // Same slot list as the title screen's Load Game (the bound
        // quick-load input is what reloads the current slot directly).
        g_slotPickerMode = SlotPickerMode::Load;
        g_slotPickerReturnScreen = MenuScreen::Pause;
        g_confirmOverwriteSlot = -1;
        g_menuScreen = MenuScreen::SlotPicker;
        return;
    }
    if (PointInRect(mx, my, SubmenuRowRect(PAUSE_LAYOUT, PROW_QUIT_TO_TITLE))) {
        AutosaveNow(false);
        g_gameState = GameState::Title;
        g_menuScreen = MenuScreen::TitleMain;
        StopMusicPlayback(); // already stopped (Pause is only reachable with music already stopped), but explicit/idempotent
        return;
    }
    if (PointInRect(mx, my, SubmenuRowRect(PAUSE_LAYOUT, PROW_QUIT))) { AutosaveNow(false); PostQuitMessage(0); return; }
}

void HandleOptionsHubClick(int mx, int my) {
    if (PointInRect(mx, my, SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_LOOK))) { g_menuScreen = MenuScreen::LookSettings; return; }
    if (PointInRect(mx, my, SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_GRAPHICS))) { g_menuScreen = MenuScreen::Graphics; return; }
    if (PointInRect(mx, my, SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_DISPLAY))) { g_menuScreen = MenuScreen::Display; return; }
    if (PointInRect(mx, my, SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_AUDIO))) { g_menuScreen = MenuScreen::Audio; return; }
    if (PointInRect(mx, my, SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_ACCESSIBILITY))) { g_menuScreen = MenuScreen::Accessibility; return; }
    if (PointInRect(mx, my, SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_KEYBINDS))) { g_menuScreen = MenuScreen::Keybindings; return; }
    if (PointInRect(mx, my, SubmenuRowRect(OPTIONS_HUB_LAYOUT, OHROW_BACK))) { g_menuScreen = g_optionsReturnScreen; return; }
}

void HandleLookSettingsClick(int mx, int my) {
    if (PointInRect(mx, my, SubmenuRowRect(LOOK_LAYOUT, LROW_INVERT_X))) { g_invertX = !g_invertX; SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(LOOK_LAYOUT, LROW_INVERT_Y))) { g_invertY = !g_invertY; SaveSettings(); return; }
    if (PointInRect(mx, my, GetSliderHitRect(SubmenuRowRect(LOOK_LAYOUT, LROW_SENS_X)))) { BeginSliderDrag(SLIDER_SENS_X, mx); return; }
    if (PointInRect(mx, my, GetSliderHitRect(SubmenuRowRect(LOOK_LAYOUT, LROW_SENS_Y)))) { BeginSliderDrag(SLIDER_SENS_Y, mx); return; }
    if (PointInRect(mx, my, SubmenuRowRect(LOOK_LAYOUT, LROW_RESET))) { ResetLookSettings(); SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(LOOK_LAYOUT, LROW_BACK))) { g_menuScreen = MenuScreen::OptionsHub; return; }
}
void HandleGraphicsClick(int mx, int my) {
    if (PointInRect(mx, my, GetSliderHitRect(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_RENDER_DIST)))) { BeginSliderDrag(SLIDER_RENDER_DIST, mx); return; }
    if (PointInRect(mx, my, GetSliderHitRect(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_FINE_DETAIL)))) { BeginSliderDrag(SLIDER_FINE_DETAIL, mx); return; }
    if (PointInRect(mx, my, GetSliderHitRect(SubmenuRowRect(GRAPHICS_LAYOUT, GROW_FRAME_LIMIT)))) { BeginSliderDrag(SLIDER_FRAME_LIMIT, mx); return; }
    if (PointInRect(mx, my, SubmenuRowRect(GRAPHICS_LAYOUT, GROW_VSYNC))) { g_vsync = !g_vsync; SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(GRAPHICS_LAYOUT, GROW_SHADOWS))) { g_shadows = !g_shadows; SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(GRAPHICS_LAYOUT, GROW_OUTLINES))) { g_postEdges = !g_postEdges; SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(GRAPHICS_LAYOUT, GROW_SSAO))) { g_postSSAO = !g_postSSAO; SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(GRAPHICS_LAYOUT, GROW_BLOOM))) { g_bloom = !g_bloom; SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(GRAPHICS_LAYOUT, GROW_RESET))) { ResetGraphicsSettings(); SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(GRAPHICS_LAYOUT, GROW_BACK))) { g_menuScreen = MenuScreen::OptionsHub; return; }
}
void HandleDisplayClick(int mx, int my) {
    if (PointInRect(mx, my, SubmenuRowRect(DISPLAY_LAYOUT, DROW_SHOW_FPS))) { g_showFPS = !g_showFPS; SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(DISPLAY_LAYOUT, DROW_SHOW_PROFILER))) { g_showProfiler = !g_showProfiler; SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(DISPLAY_LAYOUT, DROW_FULLSCREEN))) { ToggleFullscreenSetting(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(DISPLAY_LAYOUT, DROW_RESET))) { ResetDisplaySettings(); SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(DISPLAY_LAYOUT, DROW_BACK))) { g_menuScreen = MenuScreen::OptionsHub; return; }
}
void HandleAudioClick(int mx, int my) {
    if (PointInRect(mx, my, GetSliderHitRect(SubmenuRowRect(AUDIO_LAYOUT, AROW_MASTER_VOLUME)))) { BeginSliderDrag(SLIDER_MASTER_VOLUME, mx); return; }
    if (PointInRect(mx, my, GetSliderHitRect(SubmenuRowRect(AUDIO_LAYOUT, AROW_MUSIC_VOLUME)))) { BeginSliderDrag(SLIDER_MUSIC_VOLUME, mx); return; }
    if (PointInRect(mx, my, GetSliderHitRect(SubmenuRowRect(AUDIO_LAYOUT, AROW_WORLD_VOLUME)))) { BeginSliderDrag(SLIDER_WORLD_VOLUME, mx); return; }
    if (PointInRect(mx, my, SubmenuRowRect(AUDIO_LAYOUT, AROW_RESET))) { ResetAudioSettings(); SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(AUDIO_LAYOUT, AROW_BACK))) { g_menuScreen = MenuScreen::OptionsHub; return; }
}
void HandleAccessibilityClick(int mx, int my) {
    if (PointInRect(mx, my, GetSliderHitRect(SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_FOV)))) { BeginSliderDrag(SLIDER_FOV, mx); return; }
    if (PointInRect(mx, my, SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_TOGGLE_MOVE))) {
        g_toggleMovement = !g_toggleMovement;
        memset(g_moveToggleLatch, 0, sizeof(g_moveToggleLatch)); // switching modes shouldn't leave a stale latch active
        SaveSettings();
        return;
    }
    if (PointInRect(mx, my, SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_HIGH_CONTRAST))) { g_highContrastUI = !g_highContrastUI; SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_MONO))) { g_monoAudio = !g_monoAudio; SaveSettings(); return; }
    if (PointInRect(mx, my, GetSliderHitRect(SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_MUSIC_INTENSITY)))) { BeginSliderDrag(SLIDER_MUSIC_INTENSITY, mx); return; }
    if (PointInRect(mx, my, SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_RESET))) { ResetAccessibilitySettings(); SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(ACCESSIBILITY_LAYOUT, ARROW_BACK))) { g_menuScreen = MenuScreen::OptionsHub; return; }
}
void HandleKeybindingsClick(int mx, int my) {
    for (int i = 0; i < ACT_COUNT; i++) {
        if (PointInRect(mx, my, SubmenuRowRect(KEYBIND_LAYOUT, i))) { g_rebindingAction = i; return; }
    }
    if (PointInRect(mx, my, SubmenuRowRect(KEYBIND_LAYOUT, ACT_COUNT))) { ResetKeybindingsToDefault(); SaveSettings(); return; }
    if (PointInRect(mx, my, SubmenuRowRect(KEYBIND_LAYOUT, ACT_COUNT + 1))) { g_menuScreen = MenuScreen::OptionsHub; return; }
}

void HandleTitleClick(int mx, int my) {
    if (PointInRect(mx, my, SubmenuRowRect(TITLE_LAYOUT, TROW_NEW_GAME))) {
        g_slotPickerMode = SlotPickerMode::New;
        g_slotPickerReturnScreen = MenuScreen::TitleMain;
        g_confirmOverwriteSlot = -1;
        g_menuScreen = MenuScreen::SlotPicker;
        return;
    }
    if (PointInRect(mx, my, SubmenuRowRect(TITLE_LAYOUT, TROW_LOAD_GAME))) {
        g_slotPickerMode = SlotPickerMode::Load;
        g_slotPickerReturnScreen = MenuScreen::TitleMain;
        g_confirmOverwriteSlot = -1;
        g_menuScreen = MenuScreen::SlotPicker;
        return;
    }
    if (PointInRect(mx, my, SubmenuRowRect(TITLE_LAYOUT, TROW_OPTIONS))) { g_optionsReturnScreen = MenuScreen::TitleMain; g_menuScreen = MenuScreen::OptionsHub; return; }
    if (PointInRect(mx, my, SubmenuRowRect(TITLE_LAYOUT, TROW_QUIT))) { PostQuitMessage(0); return; }
}

void HandleSlotPickerClick(int mx, int my) {
    for (int slot = 0; slot < MAX_SAVE_SLOTS; slot++) {
        if (!PointInRect(mx, my, SubmenuRowRect(SLOT_PICKER_LAYOUT, slot))) continue;

        if (g_slotPickerMode == SlotPickerMode::Load) {
            if (!SlotExists(slot)) { ShowToast(Str("toast.empty_slot"), 1.5f); return; }
            g_currentSlot = slot;
            if (!LoadGame(g_world, g_player, slot)) {
                ShowToast(Str("toast.load_corrupt"), 2.0f);
                return;
            }
            // May change g_dayTimeSeconds -- EnterGameplay's
            // StartMusicPlayback re-anchors to whatever it loaded.
            EnterGameplay();
            return;
        }

        // New Game: an empty slot starts immediately; an occupied one
        // needs a second click within a few seconds to confirm the
        // overwrite, rather than silently destroying an existing world.
        if (SlotExists(slot) && g_confirmOverwriteSlot != slot) {
            g_confirmOverwriteSlot = slot;
            g_confirmOverwriteTimer = 4.0f;
            return;
        }
        g_currentSlot = slot;
        ResetWorldForNewGame();
        SaveGame(g_world, g_player, slot); // write immediately so the slot is no longer "empty" from this point on
        EnterGameplay();
        return;
    }
    if (PointInRect(mx, my, SubmenuRowRect(SLOT_PICKER_LAYOUT, SLOTROW_BACK))) { g_menuScreen = g_slotPickerReturnScreen; return; }
}

// Centralizes what a just-pressed input does, whatever its source (a
// VK_* from WM_KEYDOWN, or a MOUSE_* sentinel from a mouse-button-down
// message) -- the single place Menu/Save/Load/Break/Place dispatch is
// gated, so every input source is guaranteed to agree on the rules
// instead of each caller re-deriving them.

// ---- Block library (library.h): every placeable block in a grid; click
// one to place it now, or drag it onto a hotbar slot to keep it there.
LibraryGesture g_libGesture;
int g_libScroll = 0; // first visible row
void OpenLibrary() {
    g_libGesture = LibraryGesture();
    g_menuScreen = MenuScreen::Library;
    ReleaseMouseForMenu();
    StopMusicPlayback();
    WorldSoundCue(SND_LIBRARY_OPEN);
}
// `quiet`: a pick already made its own sound (Drop).
void CloseLibrary(bool quiet) {
    if (!quiet) WorldSoundCue(SND_LIBRARY_CLOSE);
    g_libGesture = LibraryGesture();
    g_menuScreen = MenuScreen::None;
    CaptureMouseForPlay();
    StartMusicPlayback();
}
void LibraryMouseDown(int mx, int my) {
    LibraryLayout L = ComputeLibraryLayout(g_screenW, g_screenH, g_placeableList.count);
    int entry = LibraryCellAt(L, g_placeableList.count, g_libScroll, (float)mx, (float)my);
    if (entry >= 0) { LibraryPress(g_libGesture, entry, (float)mx, (float)my); return; }
    int slot = HotbarSlotAt(g_screenW, g_screenH, (float)mx, (float)my);
    if (slot >= 0) g_player.hotbarIndex = slot; // pick which slot a click fills
}
void LibraryMouseUp(int mx, int my) {
    LibraryResult r = LibraryRelease(g_libGesture, HotbarSlotAt(g_screenW, g_screenH, (float)mx, (float)my));
    if (r.outcome == LibraryOutcome::Select) {
        g_hotbar[g_player.hotbarIndex] = g_placeableList.ids[r.entry];
        SaveSettings();
        WorldSoundCue(SND_DROP, g_player.hotbarIndex, 0.0f);
        CloseLibrary(true);
    } else if (r.outcome == LibraryOutcome::Assign) {
        g_hotbar[r.slot] = g_placeableList.ids[r.entry];
        g_player.hotbarIndex = r.slot;
        SaveSettings();
        WorldSoundCue(SND_DROP, r.slot, 1.0f);
    }
}

// Dispatches a click to whichever submenu is currently open. Only
// called for the left button -- menus never respond to right/middle
// click, matching ordinary UI convention.
// A button press in progress (menus): buttons act on release, like real
// ones -- held down they show pressed in, and sliding off before letting
// go cancels. g_pressRect is the button under the press, found while the
// buttons draw (a press that lands on no drawn button acts on release as
// clicks always did).
bool g_pressActive = false;
int g_pressX = 0, g_pressY = 0;
UIRect g_pressRect = {};
bool g_pressRectValid = false;

// Sliders act on press (they're dragged), and so does the library.
bool PressActsImmediately(int mx, int my) {
    if (g_menuScreen == MenuScreen::Library) return true;
    static const struct { int id; MenuScreen screen; } sliders[] = {
        { SLIDER_SENS_X, MenuScreen::LookSettings }, { SLIDER_SENS_Y, MenuScreen::LookSettings },
        { SLIDER_RENDER_DIST, MenuScreen::Graphics }, { SLIDER_FINE_DETAIL, MenuScreen::Graphics }, { SLIDER_FRAME_LIMIT, MenuScreen::Graphics }, { SLIDER_MASTER_VOLUME, MenuScreen::Audio },
        { SLIDER_MUSIC_VOLUME, MenuScreen::Audio }, { SLIDER_WORLD_VOLUME, MenuScreen::Audio },
        { SLIDER_FOV, MenuScreen::Accessibility }, { SLIDER_MUSIC_INTENSITY, MenuScreen::Accessibility },
    };
    for (const auto& s : sliders)
        if (s.screen == g_menuScreen && PointInRect(mx, my, GetSliderHitRect(GetSliderRowRect(s.id)))) return true;
    return false;
}

void DispatchMenuClick(int mx, int my) {
    switch (g_menuScreen) {
    case MenuScreen::Pause: HandleMenuClick(mx, my); break;
    case MenuScreen::OptionsHub: HandleOptionsHubClick(mx, my); break;
    case MenuScreen::LookSettings: HandleLookSettingsClick(mx, my); break;
    case MenuScreen::Graphics: HandleGraphicsClick(mx, my); break;
    case MenuScreen::Display: HandleDisplayClick(mx, my); break;
    case MenuScreen::Audio: HandleAudioClick(mx, my); break;
    case MenuScreen::Accessibility: HandleAccessibilityClick(mx, my); break;
    case MenuScreen::Keybindings: HandleKeybindingsClick(mx, my); break;
    case MenuScreen::TitleMain: HandleTitleClick(mx, my); break;
    case MenuScreen::SlotPicker: HandleSlotPickerClick(mx, my); break;
    case MenuScreen::Library: LibraryMouseDown(mx, my); break;
    default: break;
    }
}
