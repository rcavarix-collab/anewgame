// main.cpp
//
// walkgrid - prototype. Entry point only: window creation,
// startup sequencing, and the fixed-timestep game loop. Everything the
// loop drives lives in its own module now (world.h/world.cpp,
// render.h/render.cpp, audio.h/audio.cpp, persist.h/persist.cpp,
// game.h/game.cpp) -- see DESIGN.md for the full design and Part XV for
// the build.

// MSVC's windows.h defines min/max function-like macros unless this is
// set first -- without it, any bare std::min/std::max call anywhere in
// this project would silently break at the token that happens to be
// followed by '('.
#ifndef NOMINMAX // also set project-wide (walkgrid.vcxproj)
#define NOMINMAX
#endif
#include <windows.h>
#include <d3d11.h>
#include <cmath>
#include <algorithm>

#include "common.h"
#include <mmsystem.h> // timeBeginPeriod
#pragma comment(lib, "winmm.lib")
#include "world.h"
#include "render.h"
#include "audio.h"
#include "worldsound.h"
#include "game.h"
#include "profiler.h"
#include "jobs.h"
#include "strtable.h"   // the string table (D26)
#include "gamefiles.h" // FindAssetDirectory
#include "settings.h"  // g_language

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    ProfBootMark("LAUNCH"); // Windows loading the exe and its DLLs, and static set-up
    // Wide (W-suffixed) throughout, deliberately -- mixing an ANSI-
    // registered window (RegisterClassA/CreateWindowA) with the wide
    // DefWindowProcW that the project's Unicode character-set setting
    // makes the unsuffixed DefWindowProc macro expand to is a known
    // Win32 mismatch that corrupts non-client text (the title bar):
    // it's what produced the garbled CJK-looking title before this.
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"walkgridWindowClass";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    // Per-monitor DPI aware, so Windows never bitmap-stretches (blurs) the
    // window on a scaled display. Looked up dynamically: the V2 context
    // needs Windows 10 1703+, with the Vista-era call as the fallback.
    {
        typedef BOOL (WINAPI *SetCtxFn)(HANDLE);
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        SetCtxFn setCtx = user32 ? (SetCtxFn)(void*)GetProcAddress(user32, "SetProcessDpiAwarenessContext") : nullptr;
        if (!setCtx || !setCtx((HANDLE)-4 /* DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 */)) SetProcessDPIAware();
    }

    RECT wr = { 0, 0, DEFAULT_WINDOW_W, DEFAULT_WINDOW_H };
    DWORD style = WS_OVERLAPPEDWINDOW; // resizable and maximisable; the backbuffer follows (WM_SIZE)
    AdjustWindowRect(&wr, style, FALSE);
    g_hwnd = CreateWindowW(L"walkgridWindowClass", L"walkgrid",
                            style, CW_USEDEFAULT, CW_USEDEFAULT,
                            wr.right - wr.left, wr.bottom - wr.top,
                            nullptr, nullptr, hInstance, nullptr);
    if (!g_hwnd) return -1;
    ShowWindow(g_hwnd, nCmdShow);
    RegisterRawMouse(g_hwnd); // mouse look reads the mouse itself (input.cpp)
    ProfBootMark("WINDOW");

    RegisterGameSettings(); // the game's own settings.cfg keys (hotbar, render distance)
    LoadSettings(); // before anything reads g_sensitivityMultX/g_loadRadius/g_masterVolume/etc.
    {
        // Every player-facing word (D26): English, then the chosen language
        // over it. Before the window's title and any message box.
        std::vector<std::string> textProblems;
        LoadStrings(FindAssetDirectory(L"text"), g_language, textProblems);
        for (const std::string& p : textProblems) OutputDebugStringA((p + "\n").c_str());
        SetWindowTextW(g_hwnd, Utf8ToWide(Str("window.title")).c_str());
    }
    ProfBootMark("SETTINGS");

    // XAudio2Create requires COM initialized on the calling thread.
    // Nothing else in this file has needed that so far (SHGetKnownFolderPath
    // manages its own COM state internally), so this is the first call
    // that actually needs it.
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool comInitialized = SUCCEEDED(comHr);

    if (!InitD3D(g_hwnd)) {
        // No Direct3D 11.0 device (D20), or the core shaders failed: say so
        // rather than vanish.
        MessageBoxW(g_hwnd, Utf8ToWide(Str("startup.no_graphics")).c_str(),
                    Utf8ToWide(Str("window.title")).c_str(), MB_OK | MB_ICONERROR);
        return -1;
    }
    if (g_fullscreen) ApplyFullscreen(true); // saved preference
    int textureProblems = 0;
    if (!InitTextures(textureProblems)) return -1;
    ProfBootMark("TEXTURES");
    // One toast (a second would replace the first).
    std::string startupProblems;
    if (textureProblems > 0)
        startupProblems = textureProblems == 1 ? Str("startup.texture_problem") : StrF("startup.texture_problems", { std::to_string(textureProblems) });
    if (!ShaderErrors().empty())
        startupProblems = startupProblems.empty() ? Str("startup.effects_failed") : StrF("startup.join", { startupProblems, Str("startup.effects_failed") });
    if (!startupProblems.empty()) ShowToast(startupProblems, 8.0f);
    InitAudio(); // a machine with no usable audio device still gets a silent but playable game (Section 10)
    BuildSkyMesh();
    ProfBootMark("AUDIO");
    JobsStart(); // terrain (and from M1.5 meshes) build here, sized from the processor count (D21)
    bool firstFrame = true;

    // 1 ms timer resolution while running, so the frame cap's Sleep is precise.
    timeBeginPeriod(1);
    LARGE_INTEGER freq, lastTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastTime);
    const float FIXED_DT = 1.0f / 60.0f;
    float accumulator = 0.0f;

    // Smooth motion (D10): the world moves in fixed 60 Hz ticks, but the
    // view is drawn between the last two, by how far the next tick has
    // come -- so above 60 fps the camera glides instead of repeating a
    // position some frames and jumping on others. Only the body is
    // blended; looking around is applied every frame (below), so it never
    // waits for a tick.
    struct Pose { float x, y, z, eyeHeight, roll, leanPitch; };
    auto poseOf = [](const Player& p) { return Pose{ p.x, p.y, p.z, p.eyeHeight, p.roll, p.leanPitch }; };
    Pose prevPose = poseOf(g_player);
    // Where the player is heading (M1.11): where they look, pulled toward
    // where they're moving once they move with purpose; ground ahead loads
    // and meshes first. Motion is eased over ~10 ticks so a jiggle doesn't
    // re-order the queues.
    float headX = 0.0f, headZ = 0.0f, moveVX = 0.0f, moveVZ = 0.0f;
    float headLastX = g_player.x, headLastZ = g_player.z;

    bool running = true;
    while (running) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { running = false; }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (!running) break;

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float dt = (float)(now.QuadPart - lastTime.QuadPart) / (float)freq.QuadPart;
        lastTime = now;
        // The profiler closes the previous frame with its true, unclamped
        // length -- a hitch is exactly what it is there to show.
        ProfEndFrame(dt);
        PollPerfCapture();
        ProfBeginFrame();
        if (dt > 0.25f) dt = 0.25f; // clamp huge stalls (e.g. window drag)
        accumulator += dt;

        TickAutosave(dt);
        if (g_toastTimer > 0.0f) {
            g_toastTimer -= dt;
            if (g_toastTimer < 0.0f) g_toastTimer = 0.0f;
        }
        if (g_confirmOverwriteSlot != -1) {
            g_confirmOverwriteTimer -= dt;
            if (g_confirmOverwriteTimer <= 0.0f) g_confirmOverwriteSlot = -1; // armed confirm expired; next click re-arms instead of overwriting
        }

        UpdateDebugTimeScrub(dt); // debug time control (F8, ] / [ or Page Up / Down)

        g_fpsFrameCount++;
        g_fpsTimer += dt;
        if (g_fpsTimer >= 1.0f) {
            g_fpsDisplay = g_fpsFrameCount;
            g_fpsFrameCount = 0;
            g_fpsTimer -= 1.0f;
        }

        // The foreground check is defense-in-depth alongside the
        // WM_KILLFOCUS handler above: without it, a focus change this
        // same frame that WM_KILLFOCUS hasn't been dispatched for yet
        // would still let this recenter the real cursor into the
        // window while some other application is what's actually
        // focused.
        if (g_mouseCaptured && GetForegroundWindow() == g_hwnd) {
            POINT cursor; GetCursorPos(&cursor);
            RECT rc; GetClientRect(g_hwnd, &rc);
            POINT center = { (rc.right - rc.left) / 2, (rc.bottom - rc.top) / 2 };
            ClientToScreen(g_hwnd, &center);
            // Raw mouse movement when available; the cursor's offset from
            // the centre otherwise. The cursor is recentred either way, so
            // the hidden pointer never reaches a screen edge.
            int dx, dy;
            if (!TakeMouseLookDelta(dx, dy)) { dx = cursor.x - center.x; dy = cursor.y - center.y; }
            float sensX = BASE_MOUSE_SENS * g_sensitivityMultX;
            float sensY = BASE_MOUSE_SENS * g_sensitivityMultY;
            g_player.yaw += (g_invertX ? -dx : dx) * sensX;
            g_player.pitch += (g_invertY ? dy : -dy) * sensY;
            if (g_player.pitch > 1.55f) g_player.pitch = 1.55f;
            if (g_player.pitch < -1.55f) g_player.pitch = -1.55f;
            SetCursorPos(center.x, center.y);
        }

        // Fixed-timestep simulation, decoupled from render/present rate
        // (Section 5.3's recommended accumulator approach). While the
        // pause menu is open the world is frozen and the accumulator is
        // dropped rather than left to build up, so resuming doesn't
        // trigger a burst of catch-up ticks for however long it was paused.
        if (g_menuScreen != MenuScreen::None) {
            accumulator = 0.0f;
        } else {
            // At most five ticks a frame: after a stall (a window drag, a
            // slow disk), running every missed tick at once makes the next
            // frame slow too, and that one the next -- a hitch that feeds
            // itself. Past five, the backlog is let go (the world runs a
            // touch behind real time for that moment instead).
            const int MAX_TICKS_PER_FRAME = 5;
            int ticks = 0;
            while (accumulator >= FIXED_DT && ticks++ < MAX_TICKS_PER_FRAME) {
                prevPose = poseOf(g_player); // where this tick starts from
                // Day clock (Section 13): advances only here, gated
                // identically to every other simulation system -- the
                // single authoritative source of "what time is it,"
                // which the music (Part XIV) reads directly rather than
                // tracking its own independent notion of time.
                g_dayTimeSeconds = fmodf(g_dayTimeSeconds + FIXED_DT, DAY_LENGTH_SECONDS);

                {
                    float stepX = g_player.x - headLastX, stepZ = g_player.z - headLastZ;
                    headLastX = g_player.x; headLastZ = g_player.z;
                    if (fabsf(stepX) + fabsf(stepZ) > 8.0f) stepX = stepZ = 0.0f; // a teleport or a load, not motion
                    moveVX += (stepX / FIXED_DT - moveVX) * 0.1f;
                    moveVZ += (stepZ / FIXED_DT - moveVZ) * 0.1f;
                    float speed = sqrtf(moveVX * moveVX + moveVZ * moveVZ);
                    float pull = speed > 0.5f ? std::min(1.0f, speed / 4.0f) * 1.5f / speed : 0.0f; // blocks/s -> weight
                    float hx = sinf(g_player.yaw) + moveVX * pull, hz = cosf(g_player.yaw) + moveVZ * pull; // yaw 0 = north (+z)
                    float len = sqrtf(hx * hx + hz * hz);
                    headX = len > 1e-3f ? hx / len : 0.0f; headZ = len > 1e-3f ? hz / len : 0.0f;
                }
                int pcx = FloorDiv16((int)floor(g_player.x));
                int pcz = FloorDiv16((int)floor(g_player.z));
                {
                    ProfScope prof(PROF_TERRAIN);
                    EnsureChunksLoaded(pcx, pcz, headX, headZ);
                    ProcessColumnGeneration(g_world);
                }
                {
                    ProfScope prof(PROF_EVICT);
                    ProcessColumnEviction(g_world);
                }

                MoveInput in;
                in.fwd = IsActionDown(ACT_FORWARD); in.back = IsActionDown(ACT_BACK);
                in.left = IsActionDown(ACT_LEFT); in.right = IsActionDown(ACT_RIGHT);
                in.jump = IsActionDown(ACT_JUMP);
                in.sprint = IsActionDown(ACT_SPRINT); in.crouch = IsActionDown(ACT_CROUCH);
                {
                    ProfScope prof(PROF_PHYSICS);
                    UpdatePlayerPhysics(g_world, g_player, FIXED_DT, in);
                }
                {
                    ProfScope prof(PROF_UPDATES);
                    ProcessScheduledUpdates(g_world);
                }
                GameTick(FIXED_DT);       // the game's own systems, after the world's (game.h)
                WorldSoundTick(FIXED_DT); // footfalls, landings, slides

                accumulator -= FIXED_DT;
            }
            if (accumulator >= FIXED_DT) accumulator = fmodf(accumulator, FIXED_DT);
        }

        // Once per frame, not per tick: after a stall the tick loop runs
        // many catch-up ticks in one frame, and each would otherwise
        // generate another music chunk on top of the stall.
        if (g_menuScreen == MenuScreen::None) {
            ProfScope prof(PROF_MUSIC);
            RefillMusicQueueIfNeeded();
        }
        if (IsInGame()) {
            // The soundscape census and the world sound palette's queue
            // (UI sounds still play in the library and map).
            ProfScope prof(PROF_SOUND);
            WorldSoundFrame(dt, g_menuScreen == MenuScreen::None);
        }
        {
            ProfScope prof(PROF_MESH);
            RebuildDirtyChunks(g_world, FloorDiv16((int)floorf(g_player.x)),
                               FloorDiv16((int)floorf(g_player.y + g_player.eyeHeight)), FloorDiv16((int)floorf(g_player.z)),
                               headX, headZ);
        }
        ProfSetCounter(PCOUNT_CHUNKS_RESIDENT, (int64_t)g_world.chunks.size());
        ProfSetCounter(PCOUNT_DIRTY_WAITING, (int64_t)g_world.dirtyChunks.size());
        ProfSetCounter(PCOUNT_COLUMNS_WAITING, (int64_t)g_pendingColumns.size());
        ProfSetCounter(PCOUNT_COLUMNS_GENERATING, (int64_t)ColumnsGenerating());
        ProfSetCounter(PCOUNT_MESHES_BUILDING, (int64_t)MeshesBuilding());
        ProfSetCounter(PCOUNT_UPDATES_WAITING, (int64_t)ScheduledUpdateCount());
        // The drawn pose: between the last two ticks (see Pose above). While
        // a menu is open the world is frozen, so the latest tick is shown
        // as is; a jump of more than a few blocks in one tick (a load, New
        // Game, being lifted out of the ground) is a teleport, not motion,
        // and snaps rather than sweeping across.
        Player seen = g_player;
        {
            Pose cur = poseOf(g_player);
            float dx = cur.x - prevPose.x, dy = cur.y - prevPose.y, dz = cur.z - prevPose.z;
            bool teleport = dx * dx + dy * dy + dz * dz > 4.0f * 4.0f;
            float a = (g_menuScreen != MenuScreen::None || teleport) ? 1.0f : accumulator / FIXED_DT;
            if (a > 1.0f) a = 1.0f;
            auto mix = [a](float p, float c) { return p + (c - p) * a; };
            seen.x = mix(prevPose.x, cur.x); seen.y = mix(prevPose.y, cur.y); seen.z = mix(prevPose.z, cur.z);
            seen.eyeHeight = mix(prevPose.eyeHeight, cur.eyeHeight);
            seen.roll = mix(prevPose.roll, cur.roll); seen.leanPitch = mix(prevPose.leanPitch, cur.leanPitch);
        }
        Vec3 f, r, u;
        GetCameraVectors(seen, f, r, u);
        Vec3 eye = { seen.x, seen.y + seen.eyeHeight, seen.z };
        Mat4 view = MatLookToLH(eye, f, u);
        // g_fov (Accessibility, Section 11) is stored in degrees since
        // that's the meaningful unit for a player-facing slider.
        float fovRadians = g_fov * (3.14159265359f / 180.0f);
        Mat4 proj = MatPerspectiveFovLH(fovRadians, (float)g_screenW / g_screenH, 0.1f, 500.0f);
        GpuFrameBegin();
        RenderScene(g_world, view, proj, eye, f, u, g_dayTimeSeconds);

        {
            ProfScope prof(PROF_UI);
            RenderUIPass();
        }
        GpuMarkUIDone();
        TakeScreenshotIfRequested(); // F2: the finished frame, UI included
        GpuFrameEnd();

        {
            ProfScope prof(PROF_PRESENT);
            g_swapChain->Present(g_vsync ? 1 : 0, 0);
            if (firstFrame) { firstFrame = false; ProfBootMark("FIRST FRAME"); }
            // The frame-rate cap (Graphics, 30-200): sleep off whatever is
            // left of this frame's share, then a short spin for precision.
            // The simulation runs on a fixed step, so the cap changes only
            // how often we draw -- never how fast the world moves.
            // Only with vsync off: with it on, the display paces the frames,
            // and a cap that isn't a whole divisor of its refresh rate (60 on
            // a 75 or 144 Hz screen) would make frames alternate between one
            // refresh and two -- judder (September review; D10).
            if (!g_vsync) {
                double target = 1.0 / (double)(g_frameLimit < 30 ? 30 : g_frameLimit);
                LARGE_INTEGER t;
                for (;;) {
                    QueryPerformanceCounter(&t);
                    double spent = (double)(t.QuadPart - now.QuadPart) / (double)freq.QuadPart;
                    double left = target - spent;
                    if (left <= 0.0) break;
                    if (left > 0.002) Sleep((DWORD)((left - 0.0015) * 1000.0));
                }
            }
        }
    }

    timeEndPeriod(1);
    JobsStop(); // before the world goes: nothing may still be building from it
    ShutdownAudio();
    if (comInitialized) CoUninitialize();
    return 0;
}
