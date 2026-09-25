# Prototype outline (draft for approval)

The plan for the first prototype: what's decided, what gets built in what order, file by file, and the checklist the first functional test is judged by. **Nothing is built until you approve this.** The engine review behind it is `docs/ENGINE_REVIEW.md`.

---

## 1. Decisions

| # | Decision |
|---|---|
| D1 | The Voxistics engine carries across intact. Only the Voxistics game layer comes off (ENGINE_REVIEW.md section 3). |
| D2 | **Music stays as it is for now**: the procedural, clock-locked day music. Death metal is deferred; real instruments are hard to emulate convincingly. |
| D3 | Reactive sound effects that don't interfere with the music: the existing harmony-locked palette, plus a voice cap with priorities, and effects rendered off the main thread. |
| D4 | Textures and lighting carried over, plus **sky light**, so dug pits, overhangs and interiors go dark. |
| D5 | World: **option A**, a faceted surface over the existing grid. |
| D6 | **Smaller ground triangles without hurting performance**: fine detail near the player, coarser with distance (section 3). Scale is weighed throughout, measured in F3. |
| D7 | Natural material transitions: corner materials plus height-based blending (section 4). |
| D8 | **Smooth-filtered textures**, not pixel-crisp. |
| D9 | First and third person, the player's choice. Third person needs an animated player model: **milestone 2**, after the first test (section 6). |

---

## 2. Milestones

- **M0: separate engine from game.** Take the Voxistics layer off, with no other change. Also add screenshot capture and raw mouse input, and rewrite `CLAUDE.md` for the new game. *Done when:* it builds, the tests pass, and your frames from before and after match (minus the removed features).
- **M1: the first functional test.** A faceted, blended, textured and lit world to walk around in, with footsteps and placing and removing ground. Checklist in section 5.
- **M2: third person and the player model.** A camera toggle, a segmented animated character, and footsteps timed to the animation.
- **Later:** action systems (enemies, projectiles, particles) with fixed per-tick budgets, a real terrain generator with caves, and death metal music.

---

## 3. The faceted world (D5, D6)

**Storage stays the same.** The existing 16³ chunks, streaming, eviction and saves are kept. Each cell is still one material ID. Editing still works one cell at a time, as in Voxistics.

**The surface.** A new mesher builds a surface through the solid cells instead of cube faces:
- one vertex per surface cell, placed between its solid and empty neighbours and nudged by a seeded jitter so facets vary;
- flat-shaded facets with sharp edges: crystalline, not blobby, not blocky.

**Smaller triangles, where they're seen.** Each surface quad is subdivided, and its new points are displaced slightly by the material's height (grass lumpier than stone). The triangle count grows only where it's visible up close:

| Distance from player | Triangle size (blocks) | Relative triangle count |
|---|---|---|
| Within 2 chunks | about 0.25 | ×16 |
| 2–4 chunks | about 0.5 | ×4 |
| Beyond | about 1 | ×1 (the base facets) |

The cuts between detail levels are stitched, so no cracks show. When the player moves, only chunks crossing a band are rebuilt, at a fixed number per frame on a worker thread. **Collision and picking use the base (1-block) facets**, so fine detail costs nothing to the simulation.

**Other parts of the world code:**
- **Collision** is replaced: the player's capsule against the base triangles of nearby cells.
- **Picking** is replaced: a ray against the base triangles, which then tells us which cell was hit and which empty cell to place into.
- **Ambient occlusion** comes from how many of a vertex's neighbouring cells are solid (the same idea as Voxistics' corner AO).
- **Sky light**: each vertex stores how open to the sky it is, computed while meshing. Only cells near an edit are recomputed.
- **Terrain for the test:** a seeded rolling landscape with a few material regions (meadow, dirt, stone, sand), so blending and footsteps have something to show. Caves come later.

---

## 4. Material blending (D7, D8)

1. Each vertex carries the materials of its surrounding cells: at most three per triangle, with weights. Triangles inside a single material use the one-texture path.
2. **Height-based blending**: at a border, each material's height map decides which one shows through (grass tufts over dirt, pebbles standing proud of sand). The edge is ragged and natural, not a muddy crossfade.
3. World-space noise nudges the weights, so borders don't follow triangle edges.
4. Textures are projected from the world in two directions, blended by the surface angle, so slopes don't stretch.
5. Filtering is smooth (trilinear plus anisotropic), with normal maps faded out at distance as now.
6. Distant ground drops to its dominant material, which keeps the pixel cost down.

The vertex grows from 8 bytes to about 20: position, normal, three materials, two weights, AO and sky light.

---

## 5. First functional test (M1): the checklist

Each item says who can check it. **"Me"** means native tests, still images or offline audio in this environment. **"You"** means on your Windows machine. I can't run Direct3D here, so anything that must be seen in the running game is yours to confirm, and I'll say so every time rather than point to test counts.

| # | What you should find | Checked by |
|---|---|---|
| T1 | The game starts to the title screen. New game drops you on faceted, textured ground, not cubes. | You |
| T2 | Ground reads as angular facets. Triangles near you are visibly smaller than far away, with no cracks or popping holes between detail levels. | Me (still images), you (in game) |
| T3 | Grass, dirt, stone and sand meet in ragged, natural borders, not a triangle quilt. | Me (still images), you |
| T4 | Sun, shadows, sky, clouds, stars, moon and day/night all work as in Voxistics. | You |
| T5 | Dug pits and overhangs are darker inside. | You |
| T6 | You can walk, sprint, crouch and jump without sticking, sinking or jittering on slopes. | Me (collision tests), you (feel) |
| T7 | Footsteps sound on the beat, and sound different on grass, dirt, stone and sand. | Me (offline audio check), you (listen) |
| T8 | Placing adds ground and removing takes it away, exactly where the crosshair points, and the surface reshapes smoothly around the edit. | Me (picking tests), you |
| T9 | Placing and removing make their sounds and stay in key with the music. | Me (offline check), you |
| T10 | The music plays as in Voxistics. | You |
| T11 | Menus and settings all work (look, graphics, display, audio, keybindings, accessibility), including saves. | Me (save tests), you |
| T12 | **Performance on your machine:** steady 60 fps at the default render distance. No frame over 33 ms while walking into new ground or editing. The F3 GPU world row is under 10 ms. | You (F3 and a Ctrl+F3 report) |
| T13 | Smooth motion above 60 fps (interpolated between ticks). | You |

Numbers in T12 are my proposal; change them if you want different ones.

---

## 6. Files: kept, changed, new, removed

**Kept as they are:** `common.h`, `sky.h`, `profiler.*`, `vtex.*`, `synth_kit.h`, `music_synth.*`, `sfx_synth.*`, `musiclevel.h`, `textures.cpp`, `library.h`, the textures and their generators, `tests/` harness, `tools/` checks.

**Changed:**

| File | Change | Milestone |
|---|---|---|
| `main.cpp` | Game tick through one `GameTick`; interpolated camera; raw mouse input | M0, M1 |
| `render.cpp/.h` | Voxistics hooks removed; screenshot capture; new vertex layout and blended, world-projected world shader; sky light; back-face culling | M0, M1 |
| `world.cpp/.h` | Pulse hook removed; test terrain generator; new collision and picking | M0, M1 |
| `blocks.h` | New material roster (a dozen natural materials); shapes and Voxistics blocks out | M1 |
| `blocktex.cpp/.h` | Same texture array, laid out for world projection; smooth filtering | M1 |
| `glowlight.*` | Glow kinds made generic | M0 |
| `worldfile.*`, `persist.*` | Line and essence fields out; generic game section; new save version | M0 |
| `game.cpp/.h` | Map and store screens out; split into `input.cpp`, `menus.cpp`, `hud.cpp` | M0 |
| `audio.cpp` | Effects voice rendered on its own thread; voice cap | M1 |
| `worldsound.*`, `soundscape.*` | Line cues out; sound classes for the new materials | M0, M1 |
| `icons.*` | Hotbar icons render a faceted lump of each material | M1 |
| `Voxistics.vcxproj` | Source list follows the above; project renamed when the game has a name | M0 |
| `CLAUDE.md` | Rewritten for the new game | M0 |

**New:**

| File | What | Milestone |
|---|---|---|
| `facetmesh.cpp/.h` | Surface extraction, subdivision, detail bands, material weights, AO, sky light | M1 |
| `collide.cpp/.h` | Capsule against facets; ray picking | M1 |
| `terrain.cpp/.h` | Seeded test landscape (versioned, like Voxistics' generators) | M1 |
| `tools/facet_preview.cpp` | Renders still images of faceted, blended ground on the CPU, for checking the look here | M1 |
| `player_model.*` | Segmented animated character, third-person camera | M2 |

**Removed from the build** (the files stay in the repository history): `theline.*`, `pulse.*`, `pulse_colours.h`, `fliers.*`, `essence.*`, `essencemap.*`, `mesher.*` (replaced by `facetmesh`), `shapes.*` (cube props; parked).

---

## 7. Open questions

- **Q1.** Third person in M2, after the first test. Is that the right order, or should it be in the first test?
- **Q2.** The player model. A segmented figure (rigid parts, animated in code, faceted to match the world) is cheap and needs no art tools. A skinned model needs an art pipeline. Segmented for now?
- **Q3.** The T12 numbers: 60 fps, no frame over 33 ms, GPU world pass under 10 ms. Right?
- **Q4.** The game's name, for the project, window and files. It can stay "Voxistics" until you choose.
