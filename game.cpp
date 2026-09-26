// game.cpp
//
// The game layer's core: the game state (title or in a world), the
// per-tick hook, placing and breaking, new game / save / load / autosave,
// the game's settings keys, screenshots, the performance report and the
// debug time controls. Input is input.cpp, menus menus.cpp, and the UI pass
// hud.cpp (split out of this file in M0.11; the code moved, it didn't
// change). See game.h for what crosses into main.cpp's own loop.

#include "game_internal.h"
#include "collide.h" // picking on facets (DESIGN.md 23.5)

// =======================================================================
// Menu/game state
// =======================================================================

int g_currentSlot = 0; // which of the MAX_SAVE_SLOTS files Save/Load/QuickSave/QuickLoad act on this session
bool g_mouseCaptured = false;
bool g_keyDown[256] = {};
// Starts at the title screen (Section 12) rather than dropping straight
// into gameplay -- the game begins with no world loaded until New Game
// or Load Game picks a slot.
MenuScreen g_menuScreen = MenuScreen::TitleMain;
GameState g_gameState = GameState::Title;
// Where OptionsHub's BACK row returns to -- Pause if Options was opened
// mid-game, TitleMain if opened from the title screen, since the same
// hub and the same six settings submenus serve both contexts.
MenuScreen g_optionsReturnScreen = MenuScreen::TitleMain;
SlotPickerMode g_slotPickerMode = SlotPickerMode::New;
// Where the slot picker's BACK row returns to: TitleMain, or Pause when
// Load Game was opened mid-game.
MenuScreen g_slotPickerReturnScreen = MenuScreen::TitleMain;
// New Game on an already-occupied slot needs a confirmation rather than
// silently overwriting -- a second click within a few seconds confirms;
// otherwise the arm times out and a third click starts over.
int g_confirmOverwriteSlot = -1;
float g_confirmOverwriteTimer = 0.0f;
int g_mouseX = 0, g_mouseY = 0;
std::string g_toastMessage;
float g_toastTimer = 0.0f; // seconds remaining; drawn by RenderUIPass
float g_fpsTimer = 0.0f;
int g_fpsFrameCount = 0, g_fpsDisplay = 0; // updated once/sec, shown when Display Settings' FPS counter is on

void PickAndAct(bool breakBlock) {
    Vec3 f, r, u;
    GetCameraVectors(g_player, f, r, u);
    float dx = f.x, dy = f.y, dz = f.z;
    float ex = g_player.x, ey = g_player.y + g_player.eyeHeight, ez = g_player.z;

    int hx, hy, hz, px, py, pz;
    // The crosshair meets the facets as drawn, and maps back to their cell
    // (and the empty cell across the face, for placing): DESIGN.md 23.5.
    if (!FacetRaycast(g_world, { ex, ey, ez }, { dx, dy, dz }, 6.0f, hx, hy, hz, px, py, pz)) return;

    if (breakBlock) {
        // The world floor stays: nothing exists below it, so a hole there
        // would drop the player into an endless void.
        if (hy <= Y_MIN) return;
        BlockID taken = g_world.Get(hx, hy, hz);
        LiveEdit(g_world, hx, hy, hz, BLOCK_AIR);
        WorldSoundBreak(taken, hx, hy, hz);
    } else {
        // Refuse a placement that would overlap the player's own box --
        // it would only trap them (or, with physics' unstick rule, pop
        // them up on top of it).
        const Player& p = g_player;
        bool overlapsPlayer = px + 1 > p.x - PLAYER_HALFW && px < p.x + PLAYER_HALFW
                           && py + 1 > p.y && py < p.y + PlayerHeight(p)
                           && pz + 1 > p.z - PLAYER_HALFW && pz < p.z + PLAYER_HALFW;
        if (overlapsPlayer) { WorldSoundCue(SND_CANT); return; }
        BlockID toPlace = g_hotbar[g_player.hotbarIndex];
        // Materials have no facing (M1.9): the state byte stays 0.
        uint8_t state = 0;
        LiveEdit(g_world, px, py, pz, toPlace, state);
        WorldSoundPlace(toPlace, px, py, pz);
    }
}

// ---- Debug time control (Section 13) --------------------------------
// There is one clock -- g_dayTimeSeconds -- and the sky, sun, shadows,
// light and music all read it, so moving it moves everything. F8 jumps to
// the next time of day; holding ] or Page Up runs it forward (a whole day
// in 15 s), [ or Page Down backward. The music stops while scrubbing and
// re-anchors to the new time on release. A testing aid, like F3 and F7.
static const float DEBUG_SCRUB_RATE = DAY_LENGTH_SECONDS / 15.0f; // clock seconds per real second
static const float kTimePresets[] = { 60.0f, 600.0f, 1500.0f, 2400.0f, 2940.0f, 3300.0f };
bool g_timeScrubbing = false;

// D26: debug text (the F8 clock toast and the performance report are debug aids)
std::string DayTimeLabel(float t) {
    const char* phase = t < 300 ? "DAWN" : t < 1200 ? "MORNING" : t < 1800 ? "NOON" : t < 2700 ? "AFTERNOON" : t < 3000 ? "DUSK" : "NIGHT";
    char buf[48];
    snprintf(buf, sizeof(buf), "TIME %02d:%02d  %s", (int)t / 60, (int)t % 60, phase);
    return buf;
}
// D26: end

bool DebugKeyFree(int vk) {
    for (int a = 0; a < ACT_COUNT; a++) if (g_keyBindings[a] == vk) return false; // a bound action wins
    return true;
}

void JumpToNextTimeOfDay() {
    float next = kTimePresets[0];
    for (float p : kTimePresets) if (p > g_dayTimeSeconds + 1.0f) { next = p; break; }
    g_dayTimeSeconds = next;
    StartMusicPlayback(); // re-anchor to the new time
    ShowToast(DayTimeLabel(g_dayTimeSeconds), 2.0f);
}

void UpdateDebugTimeScrub(float frameSeconds) {
    bool playing = g_gameState == GameState::InGame && g_menuScreen == MenuScreen::None;
    auto held = [](int vk) { return vk >= 0 && vk < 256 && g_keyDown[vk] && DebugKeyFree(vk); };
    int dir = (held(VK_OEM_6) || held(VK_PRIOR) ? 1 : 0) - (held(VK_OEM_4) || held(VK_NEXT) ? 1 : 0);
    if (playing && dir != 0) {
        if (!g_timeScrubbing) { StopMusicPlayback(); g_timeScrubbing = true; }
        float t = fmodf(g_dayTimeSeconds + dir * DEBUG_SCRUB_RATE * frameSeconds, DAY_LENGTH_SECONDS);
        g_dayTimeSeconds = t < 0 ? t + DAY_LENGTH_SECONDS : t;
        ShowToast(DayTimeLabel(g_dayTimeSeconds), 1.5f);
    } else if (g_timeScrubbing) {
        g_timeScrubbing = false;
        if (playing) StartMusicPlayback();
    }
}

void DoSave() {
    bool ok = SaveGame(g_world, g_player, g_currentSlot);
    if (ok) WorldSoundCue(SND_SEALED);
    g_toastMessage = Str(ok ? "toast.saved" : "toast.save_failed");
    g_toastTimer = 2.0f;
}
// Autosave (Section 7.3): every AUTOSAVE_SECONDS of actual play (paused
// time doesn't count), plus on Quit to Title and on closing the window
// mid-game. A delta save (7.2) is small, so this doesn't hitch.
static const float AUTOSAVE_SECONDS = 300.0f;
float g_autosaveTimer = 0.0f;
void AutosaveNow(bool announce) {
    if (g_gameState != GameState::InGame) return;
    bool ok = SaveGame(g_world, g_player, g_currentSlot);
    if (announce || !ok) ShowToast(Str(ok ? "toast.autosaved" : "toast.autosave_failed"), ok ? 1.5f : 3.0f);
    g_autosaveTimer = 0.0f;
}
bool IsInGame() { return g_gameState == GameState::InGame; }

// The top of a performance report: what was measured, and on what.
// D26: debug text (the report is for the developer, in English)
std::string PerfReportHeader() {
    char b[512];
#ifdef _DEBUG
    const char* build = "Debug";
#else
    const char* build = "Release";
#endif
    snprintf(b, sizeof b,
             "walkgrid performance report\n"
             "build: %s\nwindow: %d x %d%s\n"
             "render distance: %d  shadows: %s  bloom: %s  SSAO: %s  edges: %s  music intensity: %.0f%%\n"
             "day time: %s  position: %.0f, %.0f, %.0f\n\n",
             build, g_screenW, g_screenH, g_fullscreen ? " (fullscreen)" : "",
             g_loadRadius, g_shadows ? "on" : "off", g_bloom ? "on" : "off", g_postSSAO ? "on" : "off", g_postEdges ? "on" : "off",
             g_musicIntensity * 100.0f, DayTimeLabel(g_dayTimeSeconds).c_str(), g_player.x, g_player.y, g_player.z);
    return b + ProfBootSummary(true) + "\n";
}
// D26: end

// ---- Screenshots (F2) ----
// Asked for by the key, taken once the frame is fully drawn (main.cpp calls
// TakeScreenshotIfRequested after the UI pass), so the shot is exactly what
// was on screen. The toast is drawn from the next frame on, so it never
// appears in the shot it announces.
bool g_screenshotRequested = false;
extern "C" bool SavePngBGRA(const wchar_t* path, const uint8_t* bgra, int w, int h); // textures.cpp (GDI+)

void TakeScreenshotIfRequested() {
    if (!g_screenshotRequested) return;
    g_screenshotRequested = false;
    static std::vector<uint8_t> pixels;
    int w = 0, h = 0;
    std::filesystem::path path = NextScreenshotPath();
    bool ok = !path.empty() && ReadBackbuffer(pixels, w, h) && SavePngBGRA(path.c_str(), pixels.data(), w, h);
    std::error_code ec;
    ok = ok && std::filesystem::exists(path, ec); // only claim what's really on disk
    // Where it went, in full, so it can be found (the folder's path shown,
    // as the performance report does).
    ShowToast(ok ? StrF("toast.screenshot", { WideToUtf8(path.wstring()) }) : Str("toast.screenshot_failed"), ok ? 5.0f : 2.0f);
}

// ---- The game's settings.cfg keys (settings.h hooks) ----
// Same keys and rules as Voxistics had: the render distance (the world
// layer's g_loadRadius, clamped to the slider's 1-8 -- an out-of-range
// hand edit would have EnsureChunksLoaded queue millions of columns) and
// the hotbar by block name (an unknown or no-longer-placeable name keeps
// that slot's default).
BlockID g_hotbar[HOTBAR_SLOTS] = { BLOCK_MEADOW_GRASS, BLOCK_DIRT, BLOCK_STONE, BLOCK_SAND, BLOCK_GRAVEL,
                                   BLOCK_CLAY, BLOCK_SANDSTONE, BLOCK_SLATE, BLOCK_LOAM, BLOCK_SNOW }; // = DefaultHotbar (library.h)

void WriteGameSettings(std::string& out) {
    out += "renderDistance=" + std::to_string(g_loadRadius) + "\n";
    out += "hotbar=";
    for (int i = 0; i < HOTBAR_SLOTS; i++) out += std::string(i ? "," : "") + g_blocks[g_hotbar[i]].name;
    out += "\n";
}

void ReadGameSettings(const SettingsMap& kv) {
    g_loadRadius = SettingsGetI(kv, "renderDistance", g_loadRadius);
    if (g_loadRadius < 1) g_loadRadius = 1;
    if (g_loadRadius > 8) g_loadRadius = 8;
    auto it = kv.find("hotbar");
    if (it == kv.end()) return;
    std::string list = it->second + ",";
    int i = 0;
    for (size_t start = 0, comma; i < HOTBAR_SLOTS && (comma = list.find(',', start)) != std::string::npos; start = comma + 1, i++) {
        std::string name = list.substr(start, comma - start);
        for (int id = 1; id < BLOCK_COUNT; id++)
            if (name == g_blocks[id].name && g_blocks[id].placeable) { g_hotbar[i] = (BlockID)id; break; }
    }
}

void RegisterGameSettings() { SetGameSettingsHooks(WriteGameSettings, ReadGameSettings); }

void GameTick(float) {
    // Nothing yet: walkgrid's own per-tick systems arrive from M1 on.
}

void PollPerfCapture() {
    std::string text;
    if (!ProfTakeCaptureReport(text)) return;
    std::string path = WriteTextToSaveFolder("perf_report.txt", text);
    // The full path, so the player knows exactly where to look.
    ShowToast(path.empty() ? Str("toast.report_failed") : StrF("toast.report", { path }), 12.0f);
}
void TickAutosave(float dt) {
    if (g_gameState != GameState::InGame || g_menuScreen != MenuScreen::None) return;
    g_autosaveTimer += dt;
    if (g_autosaveTimer >= AUTOSAVE_SECONDS) AutosaveNow(true);
}


void DoLoad() {
    bool ok = LoadGame(g_world, g_player, g_currentSlot);
    g_toastMessage = Str(ok ? "toast.loaded" : "toast.load_failed");
    g_toastTimer = 2.0f;
}

// Resets world/player/chunk-generation bookkeeping to a brand-new game
// -- the same bookkeeping reset LoadGame performs after a load (Section
// 7.4), just starting from nothing instead of loaded data. The normal
// per-tick EnsureChunksLoaded/ProcessColumnGeneration path (Section 2.4)
// then lazily generates terrain around the spawn point exactly as it
// always has, once ticking resumes.
void ResetWorldForNewGame() {
    g_world = World();
    g_player = Player();
    g_worldGen = DefaultNewWorldGen(); // TerrainHeight below reads it
    // Start standing on the surface (terrain height is a pure function
    // of x/z, so this needs no generated chunks), taking the highest of
    // the cells the player's footprint overlaps.
    int top = 0;
    for (float ox : { -PLAYER_HALFW, PLAYER_HALFW })
        for (float oz : { -PLAYER_HALFW, PLAYER_HALFW })
            top = std::max(top, TerrainHeight((int)floorf(g_player.x + ox), (int)floorf(g_player.z + oz)));
    g_player.y = (float)(top + 1);
    g_dayTimeSeconds = 0.0f; // dawn -- first light in a land they've never seen (Section 13)
    g_player.hotbarIndex = 0;
    ResetColumnStreaming();
    ClearScheduledUpdates();
}

// Shared tail end of both New Game and Load Game: leave the slot
// picker, mark a real game as running, and hand control to the player.
void EnterGameplay() {
    WorldSoundReset(); // a fresh session: discoveries start over
    g_gameState = GameState::InGame;
    g_autosaveTimer = 0.0f;
    g_menuScreen = MenuScreen::None;
    CaptureMouseForPlay();
    StartMusicPlayback();
}
