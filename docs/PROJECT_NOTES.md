# walkgrid: project notes

This file brings anyone up to speed: the owner, Claude, or another agent (the owner may use one for small tasks between Claude sessions). It covers what walkgrid is, how we work, what's built, how the code is laid out, how to add to it, and what's next. It's a maintained document: update it when something here changes, as part of the step that changed it. Last full update: 2026-09-26, after M1 and the first round of fixes from the owner's play-test (decisions up to D60).

**If you are an agent starting here:** read this file, then `CLAUDE.md` (the working rules; they bind every agent, not only Claude), then the part of `DESIGN.md` for the system you'll touch. Don't start writing code until the owner has approved a plan that names the files you'll change (rule 1 below).

---

## 1. What walkgrid is

A first-person game set on a **faceted world**: the ground is a surface of angular flat facets laid over a grid of one-block cells. It is not cubes (Minecraft) and not smooth blobs (Astroneer). You walk it, dig into it and build on it one cell at a time. The surface re-forms around each edit, and it's lit by a moving sun, moon and sky. Everything you hear is synthesized, and the music follows the time of day.

- **The owner** (Ryan) decides what the game is. Everything that shapes the game is a decision in `docs/DECISIONS.md`, and anything marked "provisional" is waiting for their confirmation.
- **The look is ours** (D60). Today the terrain and light resemble Valheim's low-texel realism most. We learn from it but don't converge on it. The faceted ground is our signature. Future look work pushes what's distinctive (facets, palette, sky and light), never another game's recognizable style. Nothing in the game may copy anyone's brand, art, music, text or characters (CLAUDE.md, "Nothing anyone owns").
- **Built on the Voxistics engine**, the owner's earlier project. It was carried over, not rewritten (rule 3): rendering, lighting, textures, streaming, saves, audio, menus, profiler. walkgrid replaced its world (cubes to facets) and its content.
- **Target machine:** the owner's PC (GTX 1060 3 GB, 16 GB RAM, Windows, Visual Studio). The floor is 8 GB RAM and a 2 GB graphics card, so every heavy effect has a setting. Direct3D 11, feature level 11.0. 60 fps is the minimum, higher is welcome, with no judder.
- **Hard rules** (never bent): no network use of any kind, ever (no telemetry, updates or online play, D23). Sound is synthesized only, never recorded or sampled (D25). Every player-facing word comes from the string table, so the game can be translated (D26). No reading the player's machine beyond harmless limits (processor count, video memory budget), and those are never stored or shown (D21). The game's files never go into a cloud-synced folder (D28, D45).

## 2. How we work (the short version of CLAUDE.md)

1. **Plan, approve, build, check.** No code without a plan the owner approved that names every file it touches. If a request can be read two ways, ask.
2. **One system per step,** one commit per step, and the game builds after every step.
3. **Carry across; don't rewrite** working systems. The previous attempt failed by rewriting good systems into weaker ones.
4. **Say what was and wasn't verified.** Nothing can be run on Windows from here. Tests, compiles and still pictures are checked here; how the game looks, sounds, feels and performs only the owner can check. Test counts are never proof the game works.
5. **Measure, don't assume:** performance numbers come from the owner's F3 overlay or Ctrl+F3 report.
6. **Log decisions** in `docs/DECISIONS.md`. A reversal is a new entry, never an edit.
7. **The SOP gate** (`docs/SOP.md`): a feature gets a card before code, a measurement after, and a line in `docs/COST_LEDGER.md`. Docs are part of done.
8. **Scope** lives in `docs/SCOPE_MOSCOW.xlsx`. Only the owner sets Must; new ideas enter as Could, marked "(proposed)".
9. **Independent work only when the owner says so** (D51). Otherwise stop at each plan and hand-over.

Every hand-over opens with "what you'll see", including what hasn't changed (SOP 5).

## 3. Where things are

| What | Where |
|---|---|
| Working rules | `CLAUDE.md` |
| How each system works, as built | `DESIGN.md` (Part XXIII is the faceted world) |
| Structure: layers, threads, budgets | `docs/FOUNDATIONS.md` |
| Every decision (D1 to D60 so far) | `docs/DECISIONS.md` |
| Costs, budgeted and measured | `docs/COST_LEDGER.md` |
| Milestone plans and their status | `docs/M0_PLAN.md`, `docs/M1_PLAN.md`, `docs/ROADMAP.md` |
| Expected problems, reviewed each milestone | `docs/FORECASTS.md` |
| Lessons | `docs/REFLECTIONS.md` |
| Unproven ideas and their tests | `docs/THEORIES.md` |
| Sound rules | `docs/SOUND_PALETTE.md` |
| Texture brief (how materials are drawn) | `assets/textures/TEXTURE_BRIEF.md` |
| All player-facing text | `assets/text/en.txt` (and `test.txt`, a pseudo-language) |
| Preview pictures | `docs/pictures/` |
| Old code kept for ideas only (never compiled) | `reference/` (see its README) |

## 4. What's built (as of 2026-09-26)

**M0 (done):** Voxistics carried over and stripped of its own content; renamed walkgrid; sound effects moved off the main thread; smooth motion between physics ticks; rendering relative to the camera (no precision loss far out); raw mouse; the save format (v1); the game's folder on disk.

**M1 (done; the owner's first play-test passed its performance check):**
- the facet mesher;
- twelve materials (meadow grass, dry turf, moss, dirt, loam, clay, sand, gravel, stone, slate, sandstone, snow);
- the walkgrid-hills terrain;
- terrain and meshes built on job threads;
- a world shader with textures projected from the world and blended by height where materials meet;
- sky light, so hollows go dark;
- walking and picking on the facets;
- detail levels by distance, with no cracks where they meet;
- the old block roster removed;
- the string table;
- chunks sealed off by ground aren't drawn, and ground ahead of you loads first;
- footstep level work.

The owner's first report: 199 fps, worst frame 3.3 ms of work, GPU world 1 ms, start-up 0.54 s.

**After the play-test (D45 to D60):**
- the game's folder back in the Documents Windows gives, or Saved Games if Documents is in OneDrive;
- footsteps audible (out of the -21 dB limit, with a FOOTSTEPS volume, and reading the right ground);
- shadows glide with the sun instead of jumping (continuous redraws, crossfaded) and have softer edges;
- a softer look (facets lit partly by the smooth slope);
- crisp textures near the player;
- fine detail everywhere by default;
- walkgrid-hills v2 (no stone "teeth" at mesa feet, no dirt specks on grass);
- fog that only hides the world's edge;
- the sky on its own clocks: an 8-day lunar month with phases, eclipses of both kinds with the world's shadow crossing the moon, stars drifting a turn a year, eclipses dimming the land;
- clouds on a jet stream: thin high streaks, plus occasional low clouds that cast moving shadows.

**Not yet seen running:** everything after the owner's first play-test (D45 on) has been checked here by tests, compiles and pictures only, until the owner's next build.

## 5. The code, by layer

A file includes only its own layer or lower ones (`tools/layers.txt`; enforced by `python3 tools/check_layers.py`), so the engine never names the game. Only the main thread writes the world or calls Direct3D. Job threads build terrain and meshes from copies, and each result carries a version so stale ones are dropped.

| Layer | Files | What they do |
|---|---|---|
| 1 base | `common.h`, `profiler.*`, `jobs.*` | Maths, compass, F3 profiler and reports, the job pool |
| 2 platform | `gamefiles.*`, `settings.*`, `strtable.*` | The game's folder and safe writes; settings.cfg; the string table and UTF-8 |
| 3 world | `blocks.h`, `world.*`, `worldfile.*`, `facetmesh.*`, `terrain.*`, `collide.*` | Materials registry; chunks, streaming, physics; save format; the facet mesher and chunk openings; the terrain generator (versioned); walking and picking on facets |
| 4 presentation | `groundmesh.*`, `render.*`, `sky.h`, `textures.cpp`, `vtex.*`, `blocktex.*`, `icons.*`, `glowlight.*`, `audio.*`, `music_synth.*`, `sfx_synth.*`, `synth_kit.h`, `musiclevel.h`, `soundscape.*` | Packing ground for the GPU and the visibility walk; Direct3D, every shader (as strings in render.cpp), shadows, clouds; sun, moon, stars, eclipses, jet stream; the font atlas (GDI+); texture files and building; icons; glow light; XAudio2; the music and sound-effect synths |
| 5 game | `game.*`, `game_internal.h`, `hud.cpp`, `menus.cpp`, `input.cpp`, `savegame.*`, `worldsound.*`, `library.h` | What walkgrid is: game state, HUD and menus, input, saving, which sounds play when, the materials menu |
| 6 app | `main.cpp` | Wires it all together: window, start-up, the frame loop |

**One frame, roughly:** input, then fixed 60 Hz ticks (clock, streaming, physics, sounds), then meshing (jobs), then render: sky, shadow map (a quarter of it), the visibility walk, the world, the post effects, bloom, UI, present.

**One chunk's life:** terrain column generated on a job thread (terrain.cpp), made resident on the main thread (world.cpp), cells copied, facets built and packed plus openings recorded on a job thread (groundmesh.cpp, facetmesh.cpp), then uploaded on the main thread (render.cpp).

## 6. Building and checking

- **Owner's build:** Visual Studio, x64 Release, C++17, SDL checks on (`walkgrid.sln`). New .cpp files go into `walkgrid.vcxproj` and `.filters`, and every file gets a line in `tools/layers.txt`.
- **Before anything reaches the owner, run all of these** (on Linux; they need `g++`, `mingw-w64`, `glslang-tools`, Python 3 with Pillow and openpyxl):
  - `bash tests/run.sh`: native tests (740 checks), which also run the MSVC, layer and string checks;
  - `python3 tools/check_shaders.py`: all 13 shader variants must compile, and the count must stay 13;
  - `sh tools/check_msvc.sh`: what Visual Studio's SDL checks reject, and Windows macro names;
  - `sh tools/check_mingw.sh`: the Windows cross-compile (it builds, it doesn't run);
  - `python3 tools/check_layers.py`;
  - `bash tools/sound_demo.sh analyze` for any sound change (one known flag, "works", predates M1).
- **Pictures without Windows:** `sh tools/facet_preview.sh OUTDIR hills SEED [SOFTEN]` renders the terrain on the CPU with the game's formulas. These are previews, not the game: no bloom, AO, outlines or clouds.
- **Sound without Windows:** `bash tools/sound_demo.sh demo OUTDIR` or `steps OUTDIR` writes WAV files.

## 7. Plug-in points: adding things without touching the engine

These are the places where content goes in as data or small, local code. Each needs the checks above and a line in the step's report.

- **A player-facing word:** add a key to `assets/text/en.txt` and use `Str("key")` or `StrF("key", {...})`. `check_strings.py` fails the build on words written into code. Debug-only text goes between `// D26: debug text` and `// D26: end`.
- **A translation:** copy `en.txt` to `<language>.txt` and translate the texts. The player chooses it with `language=<name>` in settings.cfg; any missing key shows English. The `font` key names a system font with the language's letters (monospace).
- **A material:** a row in `blocks.h` (`g_blocks`: name, solid, placeable, textures, lumpiness), a name in `en.txt` (`material.<name>`), its textures in a `.vtex` file (see TEXTURE_BRIEF.md), and its sound material in `soundscape.cpp`. Saves store names, so adding rows is safe. Renaming one needs a migration.
- **A texture:** a `.vtex` entry in `assets/textures/` (generators in `tools/*_textures.py`). The game reports problems in `assets/textures/_errors.txt`.
- **A sound:** a recipe in `sfx_synth.cpp`, with pitches from the chord's safe set (SOUND_PALETTE.md). `sound_demo.sh analyze` must stay clean.
- **A setting:** a global in `settings.*` (engine) or the game's own keys (game.cpp hooks), a slider or toggle row in `menus.cpp` and `hud.cpp`, and its words in `en.txt`.
- **A terrain change:** a new version in `terrain.cpp` (`HILLS_LATEST`), keeping every old version byte for byte. Saves regenerate unmodified ground, so changing old output in place breaks worlds. Pin it with a fingerprint test.
- **Save data from the game:** the game's own section of the save (`savegame.cpp`) holds tagged fields (today `DAY1` + the day count). Add a new tag, and old saves without it get a default. The engine's header format changes only with a version bump.
- **Sky and weather knobs:** the cloud layers' cover (`CIRRUS_COVER`, `CUMULUS_COVER` in render.cpp, and `fClouds` in the shaders), the jet stream (`JetStreamAngle`, sky.h), and the sun, moon and eclipse numbers (sky.h). A weather system will set these per frame. Nothing new is needed in the shaders to vary them.

## 8. Things that have bitten us (read before changing code)

- **Windows macros:** `small`, `near`, `far`, `interface` and a few others are macros under `<windows.h>`. MSVC fails on them as variable names; MinGW didn't. `check_msvc.sh` now catches them.
- **Shader sources are C++ string pieces.** A comment line between pieces that ends in `;` once cut a shader short for the checker. The checker now fails loudly, but keep comments free of trailing semicolons anyway.
- **System header names:** a file called `strings.h` shadows the system header of that name. Hence `strtable.h`.
- **The generator is frozen per version.** Changing `terrain.cpp`'s output for an existing version corrupts old worlds. Make a new version.
- **Faceted ground sits up to half a block off the cell grid.** Anything that asks "what's under my feet" must look down a cell or two (footsteps once read air 61% of the time).
- **The game's folder** is `Documents\My Games\walkgrid` in the Documents Windows gives, or `Saved Games\walkgrid` when that Documents is inside OneDrive. Never build the path by hand from the profile folder (that hid the files, D45).
- **Counters in the F3 overlay** that count per frame must be reset in `ProfBeginFrame`.
- **Words:** no product or company names in anything player-facing. That includes messages (it's "graphics card with feature level 11.0", not a brand).

## 9. What's next

**Waiting for the owner:**
- try the current build: footsteps, shadows, the softer look, crisp textures, fog, the moon and clouds (find an eclipse by holding Shift with ] to scrub days quickly);
- a Ctrl+F3 report at default settings and 1080p, since fine detail everywhere and continuous shadows both cost something new;
- confirm the provisional decisions (D53 onward).

**Heavy work for Claude sessions** (structure, engine, anything risky): weather as a system (see below), caves and a terrain v3 if wanted, the M2 plan with the owner, anything in shaders or threads, and anything touching the save format.

**Small tasks a helper agent could do** (each still needs the owner's OK, the checks, and a report that says what was and wasn't verified):
1. **Translations:** a second language file (the owner picks the language), checked with `check_strings.py`.
2. **Texture candidates:** new `.vtex` materials using ideas from `reference/seeds/newpatterns.cpp` (100 stone and soil patterns), rendered as contact sheets with `facet_preview.sh OUTDIR sheets` for the owner to judge. Don't wire them into the game without a decision.
3. **Weather design notes (research only, no code):** kinds of weather and how they'd be orchestrated, using only cheap tricks (the owner rules out anything volumetric). The knobs that exist are the cloud covers, the jet stream, fog distance, light colour and the sound palette's axes. Propose a small "weather state" that sets them, with transitions, and a way for new kinds to plug in as data.
4. **Documentation upkeep:** keep this file, `DESIGN.md` and `COST_LEDGER.md` true to the code, and flag anything that drifted.
5. **Test coverage:** add native tests for pure functions that have few (e.g. `sky.h` atmosphere colours at eclipse, `strtable.cpp` edge cases, `ChooseGameFolder` paths).
6. **Sound palette clips:** offline renders (`sound_demo.sh`) of existing sounds for the owner to review. Recipes change only with a decision.

**Open design goals:** our own look (D60); realistic lighting at no frame cost, without heavy post-processing (owner, 2026-09-26: prefer light baked into the ground when chunks are built over screen-space effects); sand's own footstep sound (waiting on the owner hearing it); terracing on gentle slopes (parked, D62: a smoothing pass exists but is off because it softens one-cell edits); weather.
