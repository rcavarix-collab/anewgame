# M0 plan: separate the engine from Voxistics

**Goal.** walkgrid runs exactly as Voxistics did, minus the Voxistics-only features. It also gains a few foundations: the walkgrid name, a screenshot key, raw mouse input, smooth motion above 60 fps, camera-relative rendering, and the layer check. **No new gameplay and no new looks.** The faceted world is M1.

**Approved 2026-09-25.** Progress is ticked off below as steps land.

---

## Before anything changes: the baseline

- **Here (done 2026-09-25):**
  - native tests: 5,068 checks, 0 failed;
  - shader check: 15 of 15 variants clean;
  - Windows cross-compile with MinGW: builds.
- **You:** take screenshots of the current build (Windows' own Win+PrtScn is fine) at three spots and times: dawn, noon, night. Put them in `docs/baseline/`, or just keep them. They're what M0's result is compared against.

---

## Steps

Each step is one commit. After every step the game builds and runs, and the checks here pass. Every step lists the files it touches.

| Step | What | Files |
|---|---|---|
| **0.1 Screenshot key** | F2 saves the frame as a PNG in the save folder (only when pressed; local file). Done first, so the after-M0 comparison can use it | `render.cpp/.h` (backbuffer read), `game.cpp` (key), `persist.cpp/.h` (folder) |
| **0.2 Checks here** | `tools/check_mingw.sh` (the cross-compile, scripted) and `tools/check_layers.sh` (reports layer violations; it only starts failing once 0.4–0.8 have removed the known ones) | `tools/` (new files), `tests/run.sh` |
| **0.3 Docs** | Archive the Voxistics docs, write the new `CLAUDE.md` (**shown to you before commit**), start walkgrid's `DESIGN.md` from the engine sections that carry over | `CLAUDE.md`, `DESIGN.md` → `docs/voxistics/DESIGN.md`, new `DESIGN.md`, `docs/REVIEW_2026-09.md` and `docs/SOUND_PALETTE.md` → `docs/voxistics/` (SOUND_PALETTE stays referenced: the palette carries over) |
| **0.4 The game tick** | `main.cpp` calls one `GameTick(dt)`; the Line, pulse, flier and essence calls move behind it | `main.cpp`, `game.cpp/.h` |
| **0.5 Take The Line off** | Sky lead, ghost moon, star wobble, Line glow in the world shader, F7 marker, Line sounds, Line save data | `render.cpp`, `game.cpp`, `worldsound.cpp`, `worldfile.*`, `persist.cpp`, remove `theline.*` from the build and tests |
| **0.6 Take pulse off** | Pulse blocks, pipes, harvesters, stores, diffusers, store screen, pulse drawing, pulse colours, colour-vision setting, the tutorial (it taught pulse) | `blocks.h`, `render.cpp`, `game.cpp`, `world.cpp` (chunk-arrived hook becomes a generic callback), `soundscape.cpp`, `library.h`, `persist.cpp`, remove `pulse.*`, `pulse_colours.h` |
| **0.7 Take fliers and essence off** | Fliers and their glowing patches (the 8-spot loop leaves the world shader), mold, the attractor block, the essence map (M) | `render.cpp`, `blocks.h`, `game.cpp`, `persist.cpp`, `worldfile.*`, remove `fliers.*`, `essence.*`, `essencemap.*` |
| **0.8 Generic glow** | Glow kinds become engine-generic: steady, breathing, music-driven. The music-reactive block stays, since the music stays | `blocks.h`, `render.cpp`, `glowlight.*` |
| **0.9 Fresh save format** | walkgrid save v1: the same chunk records, plus a generic "game section"; Voxistics' v2–v9 loaders removed | `worldfile.*`, `persist.*`, tests |
| **0.10 Rename** | walkgrid: project and solution files, window title, `Documents\My Games\walkgrid\`, report headers | `Voxistics.sln/.vcxproj/.filters` → `walkgrid.*`, `main.cpp`, `persist.*`, `render.cpp`, `audio.cpp`, `profiler.cpp`, `textures.cpp`, `world.cpp`, `worldfile.cpp`, tools that name the project file |
| **0.11 Split game.cpp** | `input.cpp`, `menus.cpp`, `hud.cpp`, with `game.cpp` keeping the state machine and tick. Moves only, no behaviour change | `game.cpp/.h`, new files, project file |
| **0.12 Smooth motion** | Render between the last two ticks; the frame cap no longer applies while vsync is on | `main.cpp`, `world.h` (previous position) |
| **0.13 Camera-relative rendering** | The eye position is subtracted before the view transform. The picture is identical; it just holds its precision far from the start | `render.cpp`, world and sky shaders |
| **0.14 Raw mouse input** | `WM_INPUT` replaces cursor recentring for mouse look | `input.cpp`, `main.cpp` |
| **0.15 Back-face culling** | Opaque cube faces culled, *if* a native test shows every shape is wound consistently; otherwise left as is and noted | `render.cpp`, `tests/` |

The layer check starts passing, and joins `tests/run.sh` as a hard failure, once 0.4–0.8 are done.

**Removed from the tests:** The Line, pulse, fliers, colour vision, essence and legacy-load tests. They go with their systems. Everything else stays and must pass.

---

## How M0 is checked

**Here, after every step:** native tests, shader check, MSVC check, the MinGW cross-compile and (from 0.8) the layer check.

**You, at the end:**

| # | Check |
|---|---|
| C1 | It builds in Visual Studio and opens as "walkgrid"; settings and saves live in `Documents\My Games\walkgrid\` |
| C2 | Screenshots (F2) at the baseline spots and times match the baseline, apart from the removed features |
| C3 | Music, footsteps, place and remove sounds as before |
| C4 | Menus and settings all work; save, quit, load gets you back where you were |
| C5 | Mouse look feels at least as good (raw input) |
| C6 | Motion is smooth above 60 fps (vsync off, or a high-refresh display, if you have one) |
| C7 | An F3 or Ctrl+F3 report: no system slower than the baseline |

**What M0 can't show:** anything about the faceted world, blending or the new terrain. That's M1.
