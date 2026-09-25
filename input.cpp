// input.cpp
//
// Input: the window procedure (keys, mouse, focus, resizing), which
// actions are held, keybindings, the mouse capture for looking around, and
// borderless fullscreen. Moved out of game.cpp in M0.11, unchanged.
// Game layer (5).

#include "game_internal.h"

// Captures and hides the cursor, re-centering it, to enter FPS look mode.
// ---- Raw mouse input (M0.14) ----
// Mouse look reads the mouse itself (WM_INPUT), not the cursor: no Windows
// pointer acceleration, no precision lost to screen pixels, and nothing
// fighting high-polling mice or remote-desktop tools. Movement is summed
// as messages arrive and taken once a frame (TakeMouseLookDelta). If
// Windows refuses the registration, look falls back to the cursor, as
// Voxistics did. One small message per mouse movement; nothing per frame.
static bool g_rawMouse = false;
static long g_rawDX = 0, g_rawDY = 0;

void RegisterRawMouse(HWND hwnd) {
    RAWINPUTDEVICE rid = {};
    rid.usUsagePage = 0x01; // generic desktop controls
    rid.usUsage = 0x02;     // mouse
    rid.hwndTarget = hwnd;  // delivered while the window is in front (no RIDEV_INPUTSINK)
    g_rawMouse = RegisterRawInputDevices(&rid, 1, sizeof(rid)) != FALSE;
}

bool TakeMouseLookDelta(int& dx, int& dy) {
    if (!g_rawMouse) return false;
    dx = (int)g_rawDX; dy = (int)g_rawDY;
    g_rawDX = g_rawDY = 0;
    return true;
}

void CaptureMouseForPlay() {
    g_mouseCaptured = true;
    g_rawDX = g_rawDY = 0; // movement from before play resumed doesn't turn the view
    ShowCursor(FALSE);
    SetCapture(g_hwnd);
    RECT rc; GetClientRect(g_hwnd, &rc);
    POINT center = { (rc.right - rc.left) / 2, (rc.bottom - rc.top) / 2 };
    ClientToScreen(g_hwnd, &center);
    SetCursorPos(center.x, center.y);
}

// Releases the cursor so it can move freely over menu buttons.
void ReleaseMouseForMenu() {
    g_mouseCaptured = false;
    ShowCursor(TRUE);
    ReleaseCapture();
}

int g_rebindingAction = -1; // -1 = not capturing; else a GameAction index

bool g_mouseButtonDown[3] = {}; // 0=left,1=right,2=middle
int MouseButtonIndex(int code) {
    if (code == MOUSE_LEFT) return 0;
    if (code == MOUSE_RIGHT) return 1;
    if (code == MOUSE_MIDDLE) return 2;
    return -1;
}
bool IsInputDown(int code) {
    int mi = MouseButtonIndex(code);
    if (mi >= 0) return g_mouseButtonDown[mi];
    if (code >= 0 && code < 256) return g_keyDown[code];
    return false;
}
// In toggle-move mode (Accessibility, Section 11) the four movement
// actions report a latched state that a key PRESS flips, rather than
// whether the key is currently physically held -- the whole point being
// that a player no longer needs to hold it down for the entire duration
// of movement. Every other action (jump, break, place, menu, ...) is
// unaffected and keeps the ordinary held-state behavior.
bool IsMovementAction(GameAction a) {
    return a == ACT_FORWARD || a == ACT_BACK || a == ACT_LEFT || a == ACT_RIGHT;
}
bool IsActionDown(GameAction a) {
    if (g_toggleMovement && IsMovementAction(a)) return g_moveToggleLatch[a];
    return IsInputDown(g_keyBindings[a]);
}

GameAction OppositeMove(GameAction a) {
    switch (a) {
    case ACT_FORWARD: return ACT_BACK;
    case ACT_BACK: return ACT_FORWARD;
    case ACT_LEFT: return ACT_RIGHT;
    default: return ACT_LEFT;
    }
}
// A fresh press of a movement binding flips its latch, whether the
// binding is a key or a mouse button. Latching a direction releases its
// opposite: with forward and back both latched they cancel out, and the
// next press un-latches the one the player didn't mean, which reads as
// inverted controls. Perpendicular latches still combine, for diagonals.
void ToggleMoveLatches(int code) {
    if (!g_toggleMovement || g_menuScreen != MenuScreen::None) return;
    for (GameAction a : { ACT_FORWARD, ACT_BACK, ACT_LEFT, ACT_RIGHT }) {
        if (code != g_keyBindings[a]) continue;
        g_moveToggleLatch[a] = !g_moveToggleLatch[a];
        if (g_moveToggleLatch[a]) g_moveToggleLatch[OppositeMove(a)] = false;
    }
}

// Human-readable name for a bound input code, for the Keybindings rows.
std::string GetInputDisplayName(int code) {
    if (code == MOUSE_LEFT) return Str("input.mouse_left");
    if (code == MOUSE_RIGHT) return Str("input.mouse_right");
    if (code == MOUSE_MIDDLE) return Str("input.mouse_middle");
    UINT scan = MapVirtualKeyW((UINT)code, MAPVK_VK_TO_VSC);
    LONG fakeLParam = (LONG)(scan << 16);
    wchar_t buf[64] = {};
    int len = GetKeyNameTextW(fakeLParam, buf, 64);
    if (len <= 0) return Str("input.unknown");
    // The keyboard's own name for the key, in the player's Windows
    // language, as UTF-8 (the atlas holds Latin-1 and the table's letters;
    // one it lacks shows as '?').
    std::string s = WideToUtf8(std::wstring(buf, (size_t)len));
    return s;
}

// Wrap Save/Load so every call site (F5/F9-equivalent bound inputs and
// the pause-menu buttons) gets the same on-screen confirmation instead
// of failing or succeeding silently.
// Borderless fullscreen: the window loses its frame and covers its
// monitor (no exclusive mode -- alt-tab and other monitors behave
// normally); WM_SIZE then resizes the backbuffer to match.
WINDOWPLACEMENT g_windowedPlacement = {};
bool g_isFullscreen = false;
void ApplyFullscreen(bool on) {
    if (on == g_isFullscreen || !g_hwnd) return;
    if (on) {
        g_windowedPlacement.length = sizeof(g_windowedPlacement);
        GetWindowPlacement(g_hwnd, &g_windowedPlacement);
        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);
        GetMonitorInfoW(MonitorFromWindow(g_hwnd, MONITOR_DEFAULTTONEAREST), &mi);
        SetWindowLongW(g_hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowPos(g_hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                     mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
                     SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    } else {
        SetWindowLongW(g_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        SetWindowPlacement(g_hwnd, &g_windowedPlacement);
        SetWindowPos(g_hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
    g_isFullscreen = on;
}

void ToggleFullscreenSetting() {
    g_fullscreen = !g_fullscreen;
    ApplyFullscreen(g_fullscreen);
    SaveSettings();
}

bool IsSettingsSubmenu(MenuScreen s) {
    return s == MenuScreen::LookSettings || s == MenuScreen::Graphics || s == MenuScreen::Display
        || s == MenuScreen::Audio || s == MenuScreen::Accessibility || s == MenuScreen::Keybindings;
}
void FireBoundAction(int code) {
    if (code == g_keyBindings[ACT_MENU]) {
        if (g_gameState == GameState::Title) {
            // ESC only ever backs out one level while at the title
            // screen -- there's no "resume gameplay" state to return to,
            // and TitleMain itself is the top of this tree.
            if (g_menuScreen == MenuScreen::SlotPicker) g_menuScreen = MenuScreen::TitleMain;
            else if (g_menuScreen == MenuScreen::OptionsHub) g_menuScreen = g_optionsReturnScreen;
            else if (IsSettingsSubmenu(g_menuScreen)) g_menuScreen = MenuScreen::OptionsHub;
            return;
        }
        if (g_menuScreen == MenuScreen::None) {
            g_menuScreen = MenuScreen::Pause;
            ReleaseMouseForMenu();
            StopMusicPlayback();
            FadeWorldSounds(0.3f); // pause is silence: time stopped
        } else if (g_menuScreen == MenuScreen::Pause) {
            g_menuScreen = MenuScreen::None;
            CaptureMouseForPlay();
            StartMusicPlayback();
        } else if (g_menuScreen == MenuScreen::Library) {
            CloseLibrary();
        } else if (g_menuScreen == MenuScreen::OptionsHub) {
            g_menuScreen = g_optionsReturnScreen;
        } else if (IsSettingsSubmenu(g_menuScreen)) {
            g_menuScreen = MenuScreen::OptionsHub;
        } else {
            g_menuScreen = MenuScreen::Pause;
        }
        return;
    }
    if (g_gameState == GameState::Title) return; // Save/Load/Break/Place all require an actual game running
    if (code == g_keyBindings[ACT_LIBRARY]) {
        if (g_menuScreen == MenuScreen::None) OpenLibrary();
        else if (g_menuScreen == MenuScreen::Library) CloseLibrary();
        return;
    }
    if (code == g_keyBindings[ACT_SAVE]) { DoSave(); return; }
    if (code == g_keyBindings[ACT_LOAD]) {
        DoLoad();
        // DoLoad may change g_dayTimeSeconds, so re-anchor the music -- but
        // only if play is live. Quick-loading from a menu leaves it paused
        // and silent; Resume restarts the music at the loaded time.
        if (g_menuScreen == MenuScreen::None) StartMusicPlayback();
        return;
    }
    if (g_menuScreen != MenuScreen::None) return; // Break/Place only fire during actual play
    if (!g_mouseCaptured) return;
    if (code == g_keyBindings[ACT_BREAK]) { PickAndAct(true); return; }
    if (code == g_keyBindings[ACT_PLACE]) { PickAndAct(false); return; }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        AutosaveNow(false); // the window's X button mid-game
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_GETMINMAXINFO: {
        // Below this client size the taller menus (Keybindings: 13 rows)
        // and the hotbar would run off the screen.
        RECT r = { 0, 0, MIN_CLIENT_W, MIN_CLIENT_H };
        AdjustWindowRect(&r, (DWORD)GetWindowLongW(hwnd, GWL_STYLE), FALSE);
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = r.right - r.left;
        mmi->ptMinTrackSize.y = r.bottom - r.top;
        return 0;
    }
    case WM_SIZE:
        // The backbuffer follows the client area (resizing, maximising,
        // fullscreen); a minimised window keeps its old size.
        if (wParam != SIZE_MINIMIZED) ResizeRenderTargets((int)LOWORD(lParam), (int)HIWORD(lParam));
        return 0;
    case WM_INPUT: {
        // Relative motion only (a pen or remote desktop can send absolute
        // positions: those are ignored here and the cursor fallback isn't
        // needed for them), and only while looking around.
        RAWINPUT ri;
        UINT size = sizeof(ri);
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &ri, &size, sizeof(RAWINPUTHEADER)) != (UINT)-1 &&
            ri.header.dwType == RIM_TYPEMOUSE && !(ri.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) && g_mouseCaptured) {
            g_rawDX += ri.data.mouse.lLastX;
            g_rawDY += ri.data.mouse.lLastY;
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam); // lets Windows release the input's buffer
    }
    case WM_MOUSEMOVE:
        g_mouseX = (int)(short)LOWORD(lParam);
        g_mouseY = (int)(short)HIWORD(lParam);
        ApplySliderDrag(g_mouseX); // no-op unless a slider is actively held
        if (g_menuScreen == MenuScreen::Library) {
            bool wasDragging = g_libGesture.dragging;
            LibraryMove(g_libGesture, (float)g_mouseX, (float)g_mouseY);
            if (g_libGesture.dragging && !wasDragging) WorldSoundCue(SND_PICK);
        }
        return 0;
    case WM_MOUSEWHEEL:
        if (g_menuScreen == MenuScreen::Library) { // scroll the grid's rows, if they overflow
            LibraryLayout L = ComputeLibraryLayout(g_screenW, g_screenH, g_placeableList.count);
            g_libScroll += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1 : 1;
            g_libScroll = std::max(0, std::min(g_libScroll, L.rows - L.visibleRows));
            return 0;
        }
        // Cycle the hotbar's slots.
        if (g_menuScreen == MenuScreen::None && g_gameState == GameState::InGame) {
            int step = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1 : 1;
            g_player.hotbarIndex = (g_player.hotbarIndex + step + HOTBAR_SLOTS) % HOTBAR_SLOTS;
            WorldSoundCue(SND_SLOT, g_player.hotbarIndex);
        }
        return 0;
    case WM_LBUTTONDOWN: {
        g_mouseButtonDown[0] = true;
        int mx = (int)(short)LOWORD(lParam), my = (int)(short)HIWORD(lParam);
        if (g_rebindingAction != -1) { g_keyBindings[g_rebindingAction] = MOUSE_LEFT; g_rebindingAction = -1; SaveSettings(); return 0; }
        if (g_menuScreen != MenuScreen::None) {
            if (PressActsImmediately(mx, my)) { DispatchMenuClick(mx, my); return 0; }
            g_pressActive = true; g_pressX = mx; g_pressY = my; g_pressRectValid = false; // acts on release
            return 0;
        }
        if (!g_mouseCaptured) { CaptureMouseForPlay(); return 0; }
        ToggleMoveLatches(MOUSE_LEFT);
        FireBoundAction(MOUSE_LEFT);
        return 0;
    }
    case WM_LBUTTONUP:
        g_mouseButtonDown[0] = false;
        if (g_pressActive) {
            g_pressActive = false;
            int mx = (int)(short)LOWORD(lParam), my = (int)(short)HIWORD(lParam);
            // Released on the button it pressed (or on something that isn't a
            // drawn button): act, at the press point. Slid off: cancelled.
            if (g_menuScreen != MenuScreen::None && (!g_pressRectValid || PointInRect(mx, my, g_pressRect)))
                DispatchMenuClick(g_pressX, g_pressY);
            g_pressRectValid = false;
            return 0;
        }
        if (g_menuScreen == MenuScreen::Library) LibraryMouseUp((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
        // Only release capture if a slider drag actually set it --
        // unconditionally releasing here would also kick the player out
        // of FPS mouse-look capture (CaptureMouseForPlay's SetCapture)
        // on every ordinary left-click-to-break-block during gameplay.
        if (g_draggingSlider != SLIDER_NONE) {
            g_draggingSlider = SLIDER_NONE;
            ReleaseCapture();
            SaveSettings(); // once per completed drag, not per pixel of motion
        }
        return 0;
    case WM_RBUTTONDOWN:
        g_mouseButtonDown[1] = true;
        if (g_rebindingAction != -1) { g_keyBindings[g_rebindingAction] = MOUSE_RIGHT; g_rebindingAction = -1; SaveSettings(); return 0; }
        ToggleMoveLatches(MOUSE_RIGHT);
        FireBoundAction(MOUSE_RIGHT);
        return 0;
    case WM_RBUTTONUP:
        g_mouseButtonDown[1] = false;
        return 0;
    case WM_MBUTTONDOWN:
        g_mouseButtonDown[2] = true;
        if (g_rebindingAction != -1) { g_keyBindings[g_rebindingAction] = MOUSE_MIDDLE; g_rebindingAction = -1; SaveSettings(); return 0; }
        ToggleMoveLatches(MOUSE_MIDDLE);
        FireBoundAction(MOUSE_MIDDLE);
        return 0;
    case WM_MBUTTONUP:
        g_mouseButtonDown[2] = false;
        return 0;
    case WM_KEYDOWN:
        if (wParam < 256) g_keyDown[wParam] = true;
        if (g_rebindingAction != -1) {
            // Escape cancels a rebind -- except on the Pause Menu row,
            // where Escape is that action's own natural key: treating it
            // as cancel there meant a Pause Menu moved off Escape could
            // never be put back without resetting every binding. Every
            // other key or mouse button commits as the new binding,
            // wherever it's pressed (including, e.g., on the Back row).
            if (wParam != VK_ESCAPE || g_rebindingAction == ACT_MENU) {
                g_keyBindings[g_rebindingAction] = (int)wParam;
                SaveSettings();
            }
            g_rebindingAction = -1;
            return 0;
        }
        // F3 toggles the profiler overlay (Part XVI), F11 fullscreen --
        // unless the player has bound that key to an action, in which case
        // the action wins and the toggle stays reachable from Display settings.
        if ((wParam == VK_F2 || wParam == VK_F3 || wParam == VK_F8 || wParam == VK_F11) && !(lParam & (1 << 30))) {
            bool bound = false;
            for (int a = 0; a < ACT_COUNT; a++) if (g_keyBindings[a] == (int)wParam) bound = true;
            if (!bound && wParam == VK_F3 && (GetKeyState(VK_CONTROL) & 0x8000)) { // Ctrl+F3: a 30 s performance report
                if (g_gameState == GameState::InGame && !ProfCapturing()) {
                    ProfStartCapture(30.0f, PerfReportHeader());
                    ShowToast(Str("toast.recording"), 3.0f);
                }
                return 0;
            }
            if (!bound && wParam == VK_F2) { g_screenshotRequested = true; return 0; } // taken at the end of this frame
            if (!bound && wParam == VK_F3) { g_showProfiler = !g_showProfiler; SaveSettings(); return 0; }
            if (!bound && wParam == VK_F11) { ToggleFullscreenSetting(); return 0; }
            if (!bound && wParam == VK_F8) {                                          // debug: next time of day
                if (g_gameState == GameState::InGame && g_menuScreen == MenuScreen::None) JumpToNextTimeOfDay();
                return 0;
            }
        }
        if (wParam >= '0' && wParam <= '9' && (g_menuScreen == MenuScreen::None || g_menuScreen == MenuScreen::Library)) {
            int slot = wParam == '0' ? 9 : (int)(wParam - '1'); // 1-9, then 0 for the tenth
            if (slot != g_player.hotbarIndex) WorldSoundCue(SND_SLOT, slot);
            g_player.hotbarIndex = slot;
            return 0;
        }
        // Toggle-to-move (Accessibility, Section 11): genuine presses only --
        // bit 30 of lParam marks Windows' own key-repeat, which would
        // otherwise flip the latch back and forth while the key is held.
        if (!(lParam & (1 << 30))) ToggleMoveLatches((int)wParam);
        FireBoundAction((int)wParam);
        return 0;
    case WM_KEYUP:
        if (wParam < 256) g_keyDown[wParam] = false;
        return 0;
    case WM_KILLFOCUS:
        // Losing focus while a key is held would otherwise leave it
        // stuck "down" forever -- this window won't get the matching
        // WM_KEYUP if focus moved elsewhere. And losing focus while the
        // mouse is captured would otherwise keep yanking the real
        // cursor back to center every frame even while alt-tabbed away,
        // since the look-code's recenter loop only checked
        // g_mouseCaptured, not whether this window was still focused.
        // Auto-pausing (like most FPS games do on focus loss) fixes
        // both at once: it releases the cursor immediately, and the
        // key-state reset below prevents any stuck movement.
        memset(g_keyDown, 0, sizeof(g_keyDown));
        memset(g_mouseButtonDown, 0, sizeof(g_mouseButtonDown));
        memset(g_moveToggleLatch, 0, sizeof(g_moveToggleLatch)); // don't resume walking on refocus from a stale toggle
        g_rebindingAction = -1;
        // Mouse capture isn't guaranteed to be released automatically
        // just because keyboard focus was -- release whatever a slider
        // drag or FPS-look capture left behind explicitly (harmless
        // no-op if nothing was actually captured).
        g_draggingSlider = SLIDER_NONE;
        ReleaseCapture();
        if (g_mouseCaptured) {
            g_menuScreen = MenuScreen::Pause;
            ReleaseMouseForMenu();
            StopMusicPlayback();
        }
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}
