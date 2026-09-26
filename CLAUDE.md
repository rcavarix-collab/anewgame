# walkgrid: working notes for Claude

walkgrid is a first-person game on a faceted world (angular ground over a cell grid; not cubes, not blobs), built on the Voxistics engine. The design of record is `DESIGN.md`; read the relevant part before changing a system. How we build is `docs/FOUNDATIONS.md`; what the prototype is, `docs/PROTOTYPE_OUTLINE.md`; every decision, `docs/DECISIONS.md`.

## How we work (FOUNDATIONS.md 1)
1. **Plan, approve, build, check.** No code without a plan the owner approved that names every file it touches and whether it's kept, changed or new. If a request can be read two ways, ask before writing.
2. **One system per step.** The game builds and runs after every step; one commit per step.
3. **Carry across; don't rewrite.** The Voxistics engine (rendering, lighting, textures, streaming, saves, audio, UI, profiler) is moved and unhooked, never rewritten from memory. Rewriting anything that works needs the owner's approval and a reason. *The previous attempt failed by rewriting working systems into weaker ones. Don't.*
4. **Say what was and wasn't verified.** Every report separates what was checked here (tests, compiles, still images, offline audio) from what only the owner can check on Windows (the running game: how it looks, sounds, feels, performs). Test counts are never proof the game works.
5. **Measure, don't assume.** Performance claims come from F3 or a Ctrl+F3 report on the owner's machine.
6. **Decisions are logged.** Don't reverse anything in `docs/DECISIONS.md` without the owner; a reversal is a new entry.

## Adding anything: docs/SOP.md (binding, D27)
Every addition passes the SOP gate. **Small change** (a bug fix or tweak inside one file, adding no cost): a line in the step's report and the usual checks. **Feature** (anything new, anything with a cost, or anything touching more than one system): the full feature card before code, measured after, and its line in `docs/COST_LEDGER.md`. Docs are part of done.

## Scope: docs/SCOPE_MOSCOW.xlsx
The owner keeps scope with MoSCoW (Must / Should / Could / Won't) against the horizon on the "How to use" sheet. New work needs a row. New ideas enter as Could or Won't, marked "Claude (proposed)" in Decided by, until the owner decides. **Only the owner sets Must.** Won't means not this horizon (parked), not rejected. Update Status as work lands (edit with openpyxl, keeping its formatting). The Voxistics sheet is archived in `reference/voxistics/`.

## Structure (FOUNDATIONS.md 2–3)
- **Layers:** 1 base, 2 platform, 3 world, 4 presentation, 5 game, 6 app (`main.cpp`). A file includes only its own layer or lower ones, so the engine never names the game. Every source file has a layer in `tools/layers.txt`, and `python3 tools/check_layers.py` enforces it.
- **Hooks:** the engine reaches the game only through the hooks listed in FOUNDATIONS.md 2.1. Adding one is a logged decision (D27).
- **Threads:** only the main thread writes the world or calls D3D11. The job threads (2 today; they may be sized from the processor count, D21) build terrain and meshes from copies and hand results back. Results carry the chunk's version, and stale ones are dropped. Music and effects synthesize on their own threads.

## Standing priorities
- **Target machines.** Reference: the owner's PC (GTX 1060 3GB, 16 GB RAM, about six years old). Floor: 8 GB RAM and a 2 GB graphics card, so every heavy effect has a setting. Graphics: Direct3D 11, feature level 11.0 and shader model 5.0 minimum (D20).
- **Hardware: harmless reads only (D21).** The game may ask the operating system for limits it must respect (processor count for the job threads, the video-memory budget). Nothing beyond those limits, and they're never recorded, stored, sent or shown, so no one can take details of the player's machine from the game.
- **Frame rate.** 60 fps is the floor, and higher is welcome, with no jitter (interpolated rendering). Budgets per frame are in FOUNDATIONS.md 4.
- **Draw only what can be seen; load ahead what may be seen soon.** Cost scales with what's on screen or what changed, never with world size. Budget per-tick work, rebuild only on change, measure with the F3 profiler.
- **Privacy.** No telemetry, analytics, crash reporting, update checks, online play or any network use, ever (D23). Debug aids (F2 screenshot, F3, Ctrl+F3) act only when pressed, and only write local files.
- **Fast boot.** Nothing slow happens at launch that could be cached, deferred or done once. Check the boot timeline (F3 / Ctrl+F3) after any start-up change.
- **Organisation and careful annotation.** One system per file; the project file is the source list. Every file opens with a header saying what it is, what it costs, its layer and its DESIGN.md section. Non-obvious lines say what they guard against.
- **Fake it convincingly, cheaply.** Visual effects are per-pixel tricks driven by small per-frame constants, not extra passes.
- **Nothing anyone owns.** Everything is original or genuinely free to use:
  - no brands, logos, trademarks, product or company names, real currencies or insignia;
  - no copyrighted art, music, melodies, text or characters;
  - nothing recreated from another game's look (not Minecraft's cubes, not Astroneer's blobs). Shared genre mechanics are fine; their specific expression is not.

  Fonts are the player's installed system fonts. When unsure, make it more original.
- **Minimal text, any language (D26).** Show, don't tell; keep words few and plain. Every player-facing word goes through the string table (from M1's text step on), never written into code, so the game can be translated.
- **Sound is synthesized, only (D25).** No recorded or sampled audio, ever: music and effects come from the synth (`synth_kit.h`, `music_synth.cpp`, `sfx_synth.cpp`).
- **Research from reliable sources only (D66).** Agencies, standards bodies, peer-reviewed or society publications, recognised references; never forums or social sites. Like a college paper: independent sources agreeing, testable figures, every source listed, anything not read at the source marked. Be a considerate visitor: few requests, no bulk fetching, and a site that refuses access is respected, not worked around. Research is for theory and explanation; never copy anyone's code, art or text: we write our own from understanding. Notes go in `docs/research/`.
- **No numbers in player-facing displays** where a band or feel will do. Keep debug UI minimal.

## Reference material: ideas only, kept separate, removed when spent
Everything old that isn't part of walkgrid lives in `reference/`: the seed files (`reference/seeds/`: the prototypes Prismative.cpp, drillder.cpp, LG2.cpp, cc_2_2_2.cpp and the texture generators newpatterns.cpp, bitmappattern.cpp, D54) and Voxistics' own design, review and scope (`reference/voxistics/`). It's never compiled and never included. Take ideas from it, not code. `reference/README.md` lists what is still worth taking from each item; when nothing is left (or the owner says so), the item is deleted, logged in `docs/DECISIONS.md`. **This rule is about `reference/` only.** It never applies to the Voxistics engine code that was carried into walkgrid; that is walkgrid's code now. Voxistics-specific pieces still inside the build are listed in `reference/README.md` too, so they can be removed deliberately.

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
