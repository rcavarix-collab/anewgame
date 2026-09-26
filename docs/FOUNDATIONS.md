# walkgrid: foundations before we write

How walkgrid is built so it stays fast, stays correct and doesn't box us in later. This settles the rules, the structure and the known risks before any code is written. The *what* is in `PROTOTYPE_OUTLINE.md`; this is the *how*. Items marked **Proposed** wait on your approval.

---

## 1. Working rules

These hold for every change, however small.

1. **Plan, approve, build, check.** No code without an approved plan that names each file it touches and says whether that file is kept, changed or new.
2. **One system per step.** A step changes one system. The game builds and runs after every step.
3. **Carry across; don't rewrite.** Engine code from Voxistics is moved and unhooked, never rewritten from memory. A rewrite of anything that already works needs its own approval, and a reason.
4. **Say what was and wasn't verified.** Every report separates what I checked here (tests, compiles, still images, offline audio) from what only you can check (the running game, what it looks like, how it sounds, how it feels). Test counts are never presented as proof the game works.
5. **Measure, don't assume.** Performance claims come from the F3 profiler or a Ctrl+F3 report on your machine, not from reasoning.
6. **Ask when unsure.** If a request can be read two ways, I ask before writing, not after.

---

## 2. Structure: layers and who may call whom

Voxistics' main problem for reuse was game content wired into engine files. walkgrid prevents that with layers. **A file may only include files from its own layer or the layers below it.**

| Layer | What lives there | Files (after M0/M1) |
|---|---|---|
| 1. Base | Maths, constants, hashing, the profiler, the small job pool | `common.h`, `profiler.*`, `jobs.*` (new) |
| 2. Platform | Window, message loop, raw input, timing, file locations | `main.cpp`, `input.*` (split from `game.cpp`), `persist.*` (paths, settings) |
| 3. World | Cell storage, streaming, terrain generation, edits, collision, picking, save format | `world.*`, `terrain.*`, `collide.*`, `worldfile.*` |
| 4. Presentation | Meshing, textures, rendering, sky, lighting, audio engine, sound palette, UI drawing | `facetmesh.*`, `blocktex.*`, `vtex.*`, `render.*`, `sky.h`, `glowlight.*`, `audio.*`, `music_synth.*`, `sfx_synth.*`, `synth_kit.h`, `soundscape.*` |
| 5. Game | What walkgrid *is*: materials, what sounds play when, menus and HUD, the tick's game logic | `materials.h` (was `blocks.h`), `game.*`, `menus.*`, `hud.*`, `worldsound.*` |

- **The engine never names the game.** Layers 1–4 reach game behaviour only through a few small hooks that the game fills in: the tick, the chunk-arrived callback, the save section and the effect constants. That's the lesson from ENGINE_REVIEW.md section 3.
- **Checked by a script.** A new `tools/check_layers.sh` fails if a lower-layer file includes a higher one. It runs with the tests, the same way `check_msvc.sh` does now.

---

### 2.1 Hooks: every place the engine reaches the game (D27)

The engine may only reach game behaviour through these. Each new one is a logged decision, and this list is kept exact.

| Hook | Declared in | Called by | What the game supplies |
|---|---|---|---|
| `GameTick(dt)` | `game.h` | `main.cpp`, once per 60 Hz tick | The game's own per-tick systems (none yet) |
| `SetGameSettingsHooks(write, read)` | `settings.h` | `game.cpp` (`RegisterGameSettings`, before `LoadSettings`) | The game's settings.cfg keys (hotbar, render distance) |
| The save's game section | `worldfile.h` (`EncodeSave`'s `game` bytes) | `savegame.cpp` | The game's own saved state (empty today) |

Not hooks: the game layer calling down into the engine (that's the normal direction), and `main.cpp` (the app layer) wiring everything together.

## 3. Threads

D3D11's drawing context isn't safe to share, and races are the worst kind of bug to find later. So the model is deliberately simple:

| Thread | Owns | Never does |
|---|---|---|
| **Main** | The window, input, the simulation tick, every write to the world, every D3D11 call, the UI | Mesh building, terrain generation, audio synthesis |
| **Jobs** (2 today; may be sized from the processor count, D21) | Terrain generation and facet meshing, each from its own **copy** of the input | Touch live world data or D3D11 |
| **Music** (exists) | Music synthesis | |
| **Effects** (new) | Sound-effect synthesis | |

- **The rule: jobs read copies and return results; only the main thread applies them.** The Voxistics mesher already builds from a padded copy of the chunk and its neighbours, so this fits with no redesign.
- **Uploads stay budgeted.** The main thread uploads finished meshes to the GPU a few per frame, as it does now.
- **Edits are version-stamped.** An edit bumps a chunk's version. A mesh or terrain result built from an older version is thrown away, never applied, so a finished job can never overwrite a newer edit.
- **The job count.** 2 threads today. Sizing it from the processor count is allowed (D21), capped, and never recorded or shown.

---

## 4. Performance budgets

**Two machines to design for.**
- **Reference machine (yours):** a GTX 1060 3GB, 16 GB of RAM, a good gaming PC of about six years ago. Budgets are measured on it.
- **Floor:** less than that, since not everyone has what you have. Design for 8 GB of RAM and a 2 GB graphics card. Every heavy effect gets a setting, and a preset can bring it down.

The game may ask the operating system for limits it must respect (processor count, video-memory budget), and nothing more (D21). The numbers above are for our planning.

Each budget has a row in F3, so a system over budget is visible the moment it happens.

| Budget (per frame, 1080p, reference machine) | Target |
|---|---|
| Frame rate | 60 fps never missed; aim for 120+ with vsync off or on a high-refresh display |
| Main-thread CPU work (everything but waiting for vsync) | ≤ 5 ms (the processor is the unknown; this leaves room) |
| GPU total / world pass | ≤ 8 ms / ≤ 5 ms (room for 120 fps, and for action later) |
| Mesh uploads per frame | ≤ 6, as now |
| Worst single frame while walking into new ground or editing | ≤ 16.7 ms (no dropped frame at 60) |
| Visible ground triangles | ≤ 1.5 million at the default settings |
| Memory: game process / GPU | ≤ 1.5 GB / ≤ 1 GB (fits the floor machine) |

### 4.1 Triangle budget

Rough estimate for the "small triangles near you" plan, using gently rolling ground (about 1.5 surface cells per column, 2 triangles per cell):

| Band | Chunks drawn (approx.) | Triangles per chunk | Total |
|---|---|---|---|
| Near, 0.25-block triangles, 2-chunk radius | ~50 | ~12,000 | ~600,000 |
| Middle, 0.5-block, out to 4 chunks | ~110 | ~3,000 | ~330,000 |
| Far, 1-block, out to 8 chunks | ~410 | ~750 | ~310,000 |
| **All** | | | **~1.2 million** |

A GTX 1060 can draw that comfortably. It still isn't the design, for three reasons:
- the floor machine can't;
- dug-out ground and cliffs multiply a chunk's count several times;
- every triangle spent on flat ground is budget taken from the action later.

So:
1. **Subdivide only where it shows.** A base facet is split only if it's near *and* it's bumpy, on a material border, or on an edge the player can see in silhouette. Flat, uniform facets stay whole at any distance.
2. **Shadows use the base mesh** at every distance.
3. **Limits per chunk.** A chunk's fine mesh has a hard vertex cap. Past it, the chunk drops to the next coarser level instead of spiking.
4. **A setting for fine-detail distance.** Graphics presets scale it down for lesser machines.
5. **Measure before committing.** The CPU preview tool (M1's first deliverable) counts real triangles, including on dug-out ground. The band sizes are set from those counts, not from this estimate.

### 4.3 Draw only what can be seen; load ahead what may be seen soon (D16)

**What exists (Voxistics):**
- chunks outside the view are skipped (frustum culling, per chunk);
- the sun's shadow map redraws only when stale, from chunks inside the light's view;
- ground loads one ring beyond the view distance, nearest first;
- everything slow happens once at start-up (textures, shaders from cache).

**What walkgrid adds:**

| What | Where | Milestone |
|---|---|---|
| **Back faces not drawn** (surfaces facing away from the camera) | Renderer | M0 (0.15), if safe |
| **Buried ground costs nothing:** the faceted mesh exists only where ground meets air, so a chunk that's solid or empty has no triangles | Mesher | M1 |
| **Hidden-chunk skipping:** while meshing, each chunk records which of its sides connect through open space. Drawing walks outward from the camera's chunk only through open connections, so sealed caves and ground behind solid rock are skipped. It's CPU-side, cheap, and has no GPU readback stalls | Mesher and renderer | M1 |
| **Ahead-of-time loading:** generation, meshing and the move up to finer detail are ordered by where you're looking and where you're heading (your position a second or two ahead), not just by distance. So what's about to come into view is ready before it does | Streaming | M1 |
| **Hills hiding what's behind them:** occlusion queries using the previous frame's results | Renderer | Later, only if F3 shows the world pass needs it |

### 4.2 Other performance risks noted now

- **Index size.** Voxistics uses 16-bit indices, which allow 65,536 vertices per chunk. Fine meshes on busy terrain can pass that. Fine chunks will use 32-bit indices; coarse ones stay at 16-bit.
- **Rebuild storms.** Walking crosses detail bands constantly. Band changes go through the same capped, nearest-first queue as edits, with hysteresis: a chunk has to move clearly past a band's edge before it's rebuilt, so walking along the edge doesn't thrash.
- **Pixel cost of blending.** Three materials, two projections each, colour plus surface maps is up to 12 texture reads per pixel on border triangles. Mitigations: the single-material path for interior triangles, dominant-material-only past a distance, and the two cheap cuts the September review proposed. The GPU world row will tell us.
- **Far from home.** Positions are 32-bit floats. At a million blocks from the start they lose centimetre precision and the view jitters. **Proposed:** render relative to the camera (subtract the eye position before the view transform). It's cheap to do now and painful to add later.

---

## 5. Data decisions to make now, because they're costly to change

| Topic | Proposed | Why now |
|---|---|---|
| Cell storage | Keep 16³ chunks, one byte of material and one byte of state per cell | Everything (saves, streaming, meshing) depends on it |
| Material IDs | 8-bit (254 usable). The first set is about a dozen | Enough for years; widening later is a format change, so decide consciously |
| Grid resolution | 1 block per cell. Fine detail is visual only | Collision, picking, saves and edits all stay at cell level |
| Mesh vertex | About 20 bytes: position, normal, three materials plus two weights, AO, sky light | Shader, mesher and upload all agree on it; changing it later touches all three |
| Save format | **A fresh walkgrid format, v1.** Voxistics' v2–v9 loaders don't come across; no walkgrid saves exist yet | Removes about 200 lines of legacy migration with nothing to migrate |
| Save and settings location | `Documents\My Games\walkgrid\` | Keeps Voxistics' saves untouched |
| Terrain generator | Versioned from day one (`walkgrid-hills v1`). Any change that alters output is a new version | Worlds regenerate unchanged ground, so this rule can't be retrofitted |
| Precision | Camera-relative rendering (4.2) | Cheap now, costly later |

---

## 6. Notes and annotation

- **Every file opens with a header**: what it is, what it costs per frame or per call, which layer it's in, and which `DESIGN.md` section describes it. This is the Voxistics convention, which worked.
- **Non-obvious lines say what they guard against**, not what they do.
- **The full standard for names and notes is `docs/CONVENTIONS.md`** (2026-09-26), written from what the code already does; the first audit against it is `docs/AUDIT_2026-09-26.md`.
- **A new `DESIGN.md` for walkgrid.** The Voxistics one moves to `reference/voxistics/DESIGN.md` as reference. walkgrid's starts with the engine sections that carry over (rewritten where they changed) plus the new ones: facets, blending, threads, layers. Code references to design sections (36 today) are updated as each file is touched. A check script flags any reference to a section that doesn't exist.
- **A decision log.** The decision table in `PROTOTYPE_OUTLINE.md` becomes `docs/DECISIONS.md`: one line per decision, with its date and reason, so a later session can't quietly reverse one. Last time's "never copy the whole thing" rule is the example of what this prevents.
- **A new `CLAUDE.md`** holds the working rules (section 1), the layer rule, the verification rule and the standing priorities that still apply. Those are: the modest target machine, privacy, lagless efficiency, fast boot, photosensitivity, nothing anyone owns, minimal text, and no numbers in player-facing displays. The Voxistics game description goes.

---

## 7. How each build is checked

**Here, before anything reaches you:**
- Native tests (`tests/run.sh`) pass. The tests are extended for each new system: collision, picking, the facet mesher (no cracks, no holes, stable output), save round-trips.
- **A Windows cross-compile with MinGW.** It's installable here; it isn't installed yet. It catches compile and link errors before you open Visual Studio. It isn't a substitute for your build.
- **The shader check** with `glslangValidator`. Also installable here, not installed yet.
- The MSVC check, the layer check and the offline sound check.
- Still images from the CPU preview for anything visual about the ground. These are labelled as previews, not the game.

**On your machine:**
- The section 5 checklist in `PROTOTYPE_OUTLINE.md` for that milestone.
- Screenshots from the new screenshot key at agreed spots and times of day, compared with the previous build's.
- A Ctrl+F3 report, compared against the budgets in section 4.

A milestone is done only when both halves are.

---

## 8. Risk register

| Risk | Effect if missed | Mitigation |
|---|---|---|
| Triangle count over budget (4.1) | Low frame rate on your machine | Subdivide only where it shows; measure with the preview tool before building bands |
| Cracks between detail levels | Visible holes and flicker | Stitch band edges; a test that checks every band border for gaps |
| Collision snagging on facet edges | The player sticks or jitters on slopes | Collide against smoothed base facets, not fine detail; scripted walk tests over varied ground |
| Picking the wrong cell on a sloped facet | Placing and removing feel wrong | Ray against real triangles, mapped back to the cell; tests at many angles |
| Worker results applied after a newer edit | Edits undone or flicker | Version stamps (section 3) |
| Blending looks muddy or noisy | The ground doesn't look natural | Height-based blending; judged on preview images first, then in game |
| Game content leaking back into the engine | The same reuse trap as Voxistics | Layer rule plus `check_layers.sh` |
| An effect or sound silently missing | A regression goes unnoticed | The feature checklist for each build; screenshots compared build to build |
| Precision far from the start | The view jitters after long travel | Camera-relative rendering |
| A decision reversed later without discussion | Last time's mistake again | `DECISIONS.md` and `CLAUDE.md` |

---

## 9. Status

Approved by the owner (2026-09-25). Scope is tracked in `docs/SCOPE_MOSCOW.xlsx` (fresh for walkgrid; the Voxistics sheet is in `reference/voxistics/`). Decisions are logged in `docs/DECISIONS.md`.
