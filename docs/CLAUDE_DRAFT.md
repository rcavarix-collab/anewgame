# walkgrid: working notes for Claude

walkgrid is a first-person game on a faceted world (angular ground over a cell grid; not cubes, not blobs), built on the Voxistics engine. The design of record is `DESIGN.md`; read the relevant part before changing a system. How we build is `docs/FOUNDATIONS.md`; what the prototype is, `docs/PROTOTYPE_OUTLINE.md`; every decision, `docs/DECISIONS.md`.

## How we work (FOUNDATIONS.md 1)
1. **Plan, approve, build, check.** No code without a plan the owner approved that names every file it touches and whether it's kept, changed or new. If a request can be read two ways, ask before writing.
2. **One system per step.** The game builds and runs after every step; one commit per step.
3. **Carry across; don't rewrite.** The Voxistics engine (rendering, lighting, textures, streaming, saves, audio, UI, profiler) is moved and unhooked, never rewritten from memory. Rewriting anything that works needs the owner's approval and a reason. *The previous attempt failed by rewriting working systems into weaker ones. Don't.*
4. **Say what was and wasn't verified.** Every report separates what was checked here (tests, compiles, still images, offline audio) from what only the owner can check on Windows (the running game: how it looks, sounds, feels, performs). Test counts are never proof the game works.
5. **Measure, don't assume.** Performance claims come from F3 or a Ctrl+F3 report on the owner's machine.
6. **Decisions are logged.** Don't reverse anything in `docs/DECISIONS.md` without the owner; a reversal is a new entry.

## Scope: docs/SCOPE_MOSCOW.xlsx
The owner keeps scope with MoSCoW (Must / Should / Could / Won't) against the horizon on the "How to use" sheet. New work needs a row. New ideas enter as Could or Won't, marked "Claude (proposed)" in Decided by, until the owner decides. **Only the owner sets Must.** Won't means not this horizon (parked), not rejected. Update Status as work lands (edit with openpyxl, keeping its formatting). The Voxistics sheet is archived in `docs/voxistics/`.

## Structure (FOUNDATIONS.md 2–3)
- **Layers:** 1 base, 2 platform, 3 world, 4 presentation, 5 game, 6 app (`main.cpp`). A file includes only its own layer or lower ones, so the engine never names the game. Every source file has a layer in `tools/layers.txt`, and `python3 tools/check_layers.py` enforces it.
- **Threads:** only the main thread writes the world or calls D3D11. The job threads (a fixed 2) build terrain and meshes from copies and hand results back. Results carry the chunk's version, and stale ones are dropped. Music and effects synthesize on their own threads.

## Standing priorities
- **Target machines.** Reference: the owner's PC (GTX 1060 3GB, 16 GB RAM, about six years old). Floor: 8 GB RAM and a 2 GB graphics card, so every heavy effect has a setting. The game itself never reads or reports the player's hardware.
- **Frame rate.** 60 fps is the floor, and higher is welcome, with no jitter (interpolated rendering). Budgets per frame are in FOUNDATIONS.md 4.
- **Draw only what can be seen; load ahead what may be seen soon.** Cost scales with what's on screen or what changed, never with world size. Budget per-tick work, rebuild only on change, measure with the F3 profiler.
- **Privacy.** No telemetry, analytics, crash reporting, update checks or any network use. Debug aids (F2 screenshot, F3, Ctrl+F3) act only when pressed, and only write local files.
- **Fast boot.** Nothing slow happens at launch that could be cached, deferred or done once. Check the boot timeline (F3 / Ctrl+F3) after any start-up change.
- **Organisation and careful annotation.** One system per file; the project file is the source list. Every file opens with a header saying what it is, what it costs, its layer and its DESIGN.md section. Non-obvious lines say what they guard against.
- **Fake it convincingly, cheaply.** Visual effects are per-pixel tricks driven by small per-frame constants, not extra passes.
- **Photosensitivity.** Nothing flashes faster than 3 times a second (`musiclevel.h` shows how).
- **Nothing anyone owns.** Everything is original or genuinely free to use:
  - no brands, logos, trademarks, product or company names, real currencies or insignia;
  - no copyrighted art, music, melodies, text or characters;
  - nothing recreated from another game's look (not Minecraft's cubes, not Astroneer's blobs). Shared genre mechanics are fine; their specific expression is not.

  Fonts are the player's installed system fonts. When unsure, make it more original.
- **Minimal text.** Show, don't tell. Keep words few and plain; English only for now.
- **No numbers in player-facing displays** where a band or feel will do. Keep debug UI minimal.

## The seed prototypes: ideas only
Prismative.cpp, drillder.cpp, LG2.cpp and cc_2_2_2.cpp are the owner's hand-tested prototypes: reference only, never compiled. Take ideas from them, not code. `docs/voxistics/DESIGN.md` Part VIII indexes what each offers (cc_2_2_2.cpp's 145 texture generators; LG2.cpp's cellular automata). **This rule is about those four files only. It never applies to the Voxistics engine, which is carried over as code.**

## Building and checking
- The owner builds with Visual Studio (x64, C++17, SDL checks on). Debug builds optimise the hot loops per file; judge performance in Release.
- Before anything reaches the owner, run all of these:
  - `bash tests/run.sh` (native tests);
  - `python3 tools/check_shaders.py` (every shader variant; needs `glslang-tools`);
  - `sh tools/check_msvc.sh` (what the SDL checks reject);
  - `sh tools/check_mingw.sh` (Windows cross-compile; needs `mingw-w64`; it builds, it doesn't run);
  - `python3 tools/check_layers.py`.
- New .cpp files go into the project file (and .filters), and every new file gets a line in `tools/layers.txt`.
- Sound: the world sound palette (`docs/SOUND_PALETTE.md`) is harmony-locked to the music. Any new sound goes through `sfx_synth.cpp` with pitches from the safe sets, and `bash tools/sound_demo.sh analyze` must stay clean.
- Art: `.vtex` files in `assets/textures` (spec: `assets/textures/TEXTURE_BRIEF.md`). Generators live in `tools/*_textures.py`.
