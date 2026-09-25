# M1 plan: the first functional test

**Goal.** Walk a faceted, textured, lit world that looks like walkgrid, not Voxistics. Hear your footsteps with the music. Place and remove ground. All at 60 fps or better on your machine. Scope and checklist: `PROTOTYPE_OUTLINE.md` sections 3–5 (T1–T13).

**Status: approved (D32).** Step 1.1 is built and awaiting your check; nothing after it is built. Every step follows the SOP: a feature card, measured after, docs updated, all checks run. Every hand-over says what you'll see.

---

## What you'll see, step by step

| After step | In the game | Here, for you to judge |
|---|---|---|
| 1.1 | No stutter from sound. | — |
| 1.2 | Nothing new | **Pictures of faceted, blended ground**, to judge the look before the engine changes |
| 1.3 | Nothing new | Pictures of the starting materials you picked, meeting each other |
| 1.4 | Different ground shape: rolling hills with regions. **Still cubes.** | — |
| 1.5 | **The ground is faceted, not cubes.** One texture per facet, sharp edges. | — |
| 1.6 | **Materials blend naturally at borders.** Hollows and overhangs go dark. | — |
| 1.7 | Walking and placing work properly on the facets. | — |
| 1.8 | Fine detail up close, coarser far away, with no cracks. | Triangle counts |
| 1.9 | The old blocks and cube props are gone. The library shows the new materials. | — |
| 1.10 | Menus look the same, but every word now comes from a text file | A test language, to show nothing is left in code |
| 1.11 | No visible change; distant and hidden ground costs less | Chunks-drawn counts |
| 1.12 | Everything together: the checklist T1–T13 | — |

---

## Steps

### 1.1 Sound effects on their own thread
- **Why first.** Your baseline report shows the world sound system on the main thread spiking to 14.9 ms. It's behind every one of your worst frames (forecast F11).
- **Change.** The effects palette renders on its own thread, the way music already does. The main thread only posts what happened (sounds to play, the gait, a fade) and the latest state (axes, scene, listener) into a small fixed mailbox; the thread applies them and renders. Footstep levels are left as they are (D29) until step 1.12 (D31).
- **Found while planning the card.** The voice cap with priority tiers already exists (48 voices; ambience yields, interaction steals the quietest low-priority voice). This step adds a test for it instead of building a new one.

**Feature card (SOP 1)**

| Question | Answer |
|---|---|
| What does the player get? | No stutter when sounds play: busy moments never cost a frame. |
| Scope row | W024 (Should). |
| Layer and files | Layer 4. Changed: `audio.cpp` (the worker, the mailbox), `audio.h` (comments only; same calls). Tests: `tests/tests.cpp` (voice cap under a flood). Docs: `DESIGN.md` 10.4, `COST_LEDGER.md`, scope sheet. No new files; `sfx_synth.cpp` unchanged. |
| What it costs | Main thread: posting a cue is a copy under a brief lock (microseconds); the WORLD SOUND row is left with the census. The effects thread: measured here, 0.6 ms median, 1.1 ms p95 to render 11.6 ms of sound with all 48 voices busy (about 5% of one core); nothing when silent (it sleeps until something is posted). Memory: the mailbox, 64 cues (a few KB), fixed. Grows with sounds playing, capped at 48 voices. |
| Which budget pays | Main thread: WORLD SOUND, 0.5 ms (target: census only, no spikes). The effects thread is off the frame, like MUSIC. |
| How it's switched off | Not a setting: it replaces the main-thread path. One marked place (`WorldWorker` in `audio.cpp`); taking it out means calling the old pump from the frame again. |
| How it's checked | Here: native tests (a flood holds at 48 voices and interaction still gets through); the stress render; `sound_demo analyze`; cross-compile. You: a Ctrl+F3 report with no WORLD SOUND spikes, and sounds still on time when placing and taking. |
| How it leaves | Revert `audio.cpp`; no saves or settings affected. |

- **Your check.** A Ctrl+F3 report shows no WORLD SOUND spikes; placing and taking still sound immediate.

### 1.2 The preview tool: pictures of faceted ground (proves T1, T2, T3)
- **Change.** A native tool builds faceted ground from the grid and draws still images on the CPU: rolling ground, a cliff, a dug pit, and material borders. It uses the same lighting formulas as the game's shader: sun, sky light, height-based blending. It also counts triangles, blanket subdivision against "only where it shows".
- **Files.** New: `facetmesh.cpp/.h` (the mesher itself, **layer 3**, not 4: collision uses it, D34; in the project file now, used by the game from 1.5), `tools/facet_preview.cpp`, `tools/facet_preview.sh`, `docs/pictures/m1_2/`. Changed: `tests/tests.cpp`, `tests/run.sh`, `tools/layers.txt`, the project files.
- **Also tests T7:** the existing square-tiling textures, projected from the world, on slopes and cliffs, including one directional texture on purpose.
- **Status: built (D33: you were away, so I judged the pictures and tuned; D35 records the look, provisionally).** Results: T1 testing (your call), T2 fails as stated (2% saved, budget holds), T3 testing, T7 holds.

**Feature card (SOP 1).** Written after the code, which breaks SOP rule "no card, no code"; noted in the report.

| Question | Answer |
|---|---|
| What does the player get? | Nothing yet in the game; you get pictures of the new ground to judge before the engine changes. |
| Scope row | W031 (Should); W012, W013 (Must) in progress. |
| Layer and files | Above. |
| What it costs | In the game: nothing until 1.5 (the mesher is compiled in but not called). The tool: about 7 s per picture here. |
| Which budget pays | None yet. 1.5 moves meshing to the job threads (MESH row). |
| How it's switched off | Not in the game yet. |
| How it's checked | Mesher tests (watertight at every mix of levels, outward winding, identical whole or in boxes, stable, no base folds); the pictures. |
| How it leaves | Delete the files; nothing else depends on them before 1.5. |

- **Your check.** The look, on the pictures in `docs/pictures/m1_2/` (and the review page).

### 1.3 The starting materials and the texture direction
- **Change.** You pick about 8–12 starting materials from contact sheets. The candidates are the 93 existing textures (natural set first), shown blended against each other under dawn and noon light. `TEXTURE_BRIEF.md` is rewritten for faceted ground and smooth filtering. Anything new or tweaked comes from the generators (`tools/*_textures.py`), so it can be adjusted by a parameter.
- **Files.** Changed: `assets/textures/TEXTURE_BRIEF.md`, the generators as needed. New: a contact-sheet mode in `tools/facet_preview`. Textures not picked go to `assets/textures/parked/`.
- **Status: built.** Twelve picks (D36, provisional): see `docs/pictures/m1_3/`. The brief is rewritten for faceted, filtered, world-projected ground. Parking the unpicked textures moves to 1.9 (the old roster still uses them). A contact-sheet mode was added to the preview (`facet_preview.sh OUTDIR sheets`), as planned.
- **Your check.** The picks, on the sheets.

### 1.4 The test landscape
- **Change.** A seeded, versioned generator (`walkgrid-hills v1`): rolling ground with regions of your starting materials, a few cliffs, and no caves. It runs on the job threads.
- **Files.** New: `terrain.cpp/.h` (layer 3), `jobs.cpp/.h` (layer 1: a small fixed pool, sized from the processor count per D21, capped). Changed: `world.cpp` (the Voxistics generators are removed, and generation is queued to jobs, applied on the main thread with version stamps).
- **Cost.** TERRAIN on the main thread falls to applying results only.
- **Checks here.** Generator determinism (same seed, same ground, forever); streaming tests.
- **Status: built.** Flat is kept as the test ground (D37). Terrain pictures: `docs/pictures/m1_4/`. Tests: pinned fingerprint, purity, materials present, streaming on real threads matches inline generation, stale columns dropped after a reset.
- **Your check.** Walking into new ground doesn't hitch. It still looks like cubes, and that's expected.

### 1.5 Faceted ground in the game
- **Change.** The chunk mesher is replaced by `facetmesh`, running on the job threads:
  - a new vertex format (position, normal, materials, AO, sky light);
  - 32-bit indices where needed;
  - consistent winding, so back faces are culled (from M0.15).

  The world shader reads the new vertex, projects textures from the world, and uses one material per facet at this step.
- **Files.** Changed: `render.cpp/.h`, `world.cpp` (mesh queue). Removed from the build: `mesher.*`.
- **Cost.** MESH on the main thread falls to uploads only. Triangles are counted in F3.
- **Checks here.** Shader check; mesher tests; winding test; cross-compile.
- **Status: built.** New file `groundmesh.cpp/.h` (D38); `mesher.*` stays until 1.9 (the icons use it). Tests: the cell copy, chunk seams closed exactly, winding agrees with the old cube mesher on screen. Not run here: Direct3D.
- **Your check.** Screenshots: faceted, not cubes.

### 1.6 Blending and sky light
- **Change.** Height-based blending across up to 3 materials per triangle, with the single-material path for interiors, and sky light in the vertex. Both follow what you approved on the pictures in 1.2 and 1.3.
- **Files.** Changed: `render.cpp` (world shader), `facetmesh.cpp`.
- **Cost.** GPU WORLD rises; it's budgeted at ≤ 5 ms on your machine, from a 0.6 ms baseline.
- **Status: built.** Also changed, beyond the plan's list: `blocktex.cpp/.h` (height layers), `world.h/.cpp` (column tops), `groundmesh.*` (sky light). Tests: height layers vary, tops follow edits, sky light at flat/pit/roof/cliff. The shader isn't run here.
- **Your check.** Natural borders (T3), dark hollows (T5), and your pictures from 1.2 matching the game.

### 1.7 Collision and picking on facets
- **Change.** The player's capsule collides with the coarse facets, and the crosshair ray hits real triangles and maps back to a cell. Walking, sprinting, crouching and the slide all carry over.
- **Files.** New: `collide.cpp/.h` (layer 3). Changed: `world.cpp` (physics calls it), `game.cpp` (placing and removing use it).
- **Checks here.** Scripted walks over varied ground, with no snagging (F3); picking at many angles.
- **Status: built.** Also changed: `facetmesh.*` (FacetBaseFace, the shared fold rule), `tests/tests.cpp` (the movement tests restated for facets: heights within the jitter, a one-cell step walked up, a two-cell wall stops). D39 records the one rule that changed.
- **Your check.** Feel (T6); placing and removing land where you aim (T8).

### 1.8 Fine detail near you
- **Change.** Detail bands: facets split only where it shows (T2), stitched at the band edges, with hysteresis so walking doesn't cause rebuild storms (F4). Shadows use the coarse mesh.
- **Files.** Changed: `facetmesh.cpp`, `world.cpp` (band bookkeeping), `settings.cpp` (a fine-detail distance setting). The Graphics menu gets one row.
- **Cost.** Triangles against the 1.5 million budget, measured.
- **Status: built.** Also changed: `facetmesh.*` (stitchBox), `groundmesh.*` (levels), `world.h` (a chunk's built level), `game_internal.h`, `menus.cpp`, `hud.cpp` (the Graphics row). Shadows on the coarse mesh deferred (D40).
- **Your check.** Close-up detail, no cracks, a Ctrl+F3 report.

### 1.9 Out with the old
- **Change.** Work through `reference/README.md`'s list:
  - the old block roster is replaced by your materials;
  - cube props and shapes leave the build;
  - falling ground and grass die-back are switched off (D12);
  - the soundscape is retuned for the new materials;
  - the old generators are gone;
  - materials are chosen from the library menu (D30); its icons become faceted lumps of each material;
  - the hotbar stays as a placeholder, possibly for other GUI later (D30).
- **Files.** Changed: `blocks.h` (becomes the materials registry), `blocktex.cpp`, `icons.cpp`, `soundscape.cpp`, `world.cpp`. Removed from the build: `shapes.*`.
- **Status: built (D41).** Also changed beyond the list: `game.cpp` (placement rules and the default hotbar), `worldsound.cpp` (the machine sound), `library.h`, `groundmesh.cpp` (lumpiness from the registry), the loaded `.vtex` files (split; the rest in `assets/textures/parked/`), `tests/tests.cpp` (tests of removed features retired: the cube mesher, shapes, props, grass cover, glowing blocks; the rest restated). Icons: `docs/pictures/m1_9/icons.png`.
- **Your check.** Nothing from Voxistics' roster left in the library.

### 1.10 Text for any language (D26)
- **Change.** Every player-facing word moves to a string table (`assets/text/en.txt`). The font atlas is built from the characters the table uses. Layouts measure text instead of assuming English widths. A check script flags any player-facing literal left in code.
- **Files.** New: `strings.cpp/.h` (layer 2), `assets/text/en.txt`, `tools/check_strings.py`. Changed: `hud.cpp`, `menus.cpp`, `textures.cpp` (atlas), `render.h` (atlas layout), `main.cpp` (the message box text).
- **Why here and not first.** It changes nothing you can see, and you wanted the new look early. Placing it after the world work means the menus it converts are final.
- **Your check.** A test language shows no English left, and no boxes for missing characters.
- **Status: built (D42).** The module is `strtable.cpp/.h` (a file named `strings.h` shadows the system header of that name when the tests build). Also changed beyond the list: `game.cpp`, `input.cpp` (toasts; key names as UTF-8), `game_internal.h` (action names become keys), `render.cpp` (the atlas's glyph list; the texture-problem toast's words move to `main.cpp`), `gamefiles.cpp/.h` (`FindAssetDirectory`, shared by textures and text), `settings.cpp/.h` (`language=`), `savegame.cpp` (debug lines marked), `tests/run.sh` and `tests/tests.cpp`. The test language is `assets/text/test.txt` (`language=test` in settings.cfg). Cost: the atlas grows from 6 to 12 rows for English (about 1.3 MB more video memory) and bakes twice the glyphs at start-up (see TEXTURES in the boot timeline).

### 1.11 Draw only what's seen; load ahead (D16)
- **Change.** Hidden-chunk skipping (open-side connectivity, recorded while meshing), and loading ordered by where you look and where you're heading.
- **Files.** Changed: `facetmesh.cpp`, `render.cpp`, `world.cpp`.
- **Your check.** A Ctrl+F3 report with chunks drawn well under chunks resident.
- **Status: built (D43).** The walk is `GroundVisibleChunks` in `groundmesh.cpp` (layer 4, platform-free so it's tested natively), rather than in `render.cpp`; openings are `FacetOpenings` in `facetmesh.cpp`, stored on each chunk (`world.h`). Also changed: `main.cpp` (the heading), `profiler.cpp/.h` (CHUNKS HIDDEN), `tests/tests.cpp`. Measured here: 0 of 4,500 eye rays met ground the walk skipped; the walk about 0.3 ms with everything in view at render distance 12, openings about 40 µs a chunk on the job threads (tests, -O1). On your machine: CHUNKS HIDDEN in F3, and the walk's cost inside RENDER.

### 1.12 The first functional test
The footsteps first (D31): the palette's −21 dB ceiling (DESIGN 10.4) holds them to about +5 dB of the raise you picked, so I'll make clips of footsteps allowed past it, on the new ground, for you to choose from. Then the whole checklist, T1–T13, on your machine. Then the M1 reflection, the forecast review and the ledger.

- **Status: my part done (D44); yours waits for you.** Footstep clips: `tools/sound_demo.sh steps OUTDIR` writes grass, stone and sand walks over the midday music at three footstep ceilings (−21 today, −16, −11). The loudest step alone: −28 / −23 / −17 dBFS on grass, −27 / −23 / −18 on stone, −25 / −19 / −17 on sand. The game stays at −21 until you pick. Sand now has its own sound (a hiss where soils crunch), so T7's four grounds differ. Reflection, forecast review and ledger are written. Files changed: `sfx_synth.cpp/.h` (the footsteps' own ceiling; sand), `soundscape.cpp` (sand's material), `tools/sound_demo.cpp/.sh` (the steps mode), `tests/tests.cpp`, docs.

**The checklist: what I checked here, and what's yours.** Nothing below has been seen running: I can't run Direct3D here.

| # | Checked here | Result here | Yours |
|---|---|---|---|
| T1 | It builds (MinGW cross-compile, 0 warnings; MSVC-rejected calls: none) | Builds; never run | Start it; new game on faceted ground |
| T2 | Pictures (`docs/pictures/m1_2`); seams at every pairing of detail levels | 0 unmatched edges at every pairing | Look near and far; any cracks or holes |
| T3 | Pictures of every material pairing (`docs/pictures/m1_3/pairs.jpg`) | Ragged borders in the pictures | The same in game |
| T4 | Every shader variant compiles (13) | Compiles; the sky is Voxistics' own, unchanged | Sun, shadows, sky, night |
| T5 | Sky-light tests | Flat 1.00, pit floor 0.19, under a roof 0.08, cliff face 0.64 | Pits and overhangs darker |
| T6 | 16 scripted walks over hills (455 blocks) | Sank 0, lifted 0, view jumps 0, stuck 0 | The feel |
| T7 | Offline renders; steps on the beat (tests) | Grass, soil, stone and sand differ (clips) | Listen; pick the ceiling |
| T8 | 120 picking rays | 119 hits, 0 wrong (a ray meeting nothing within the 8-block reach is a miss, not an error) | Place and remove |
| T9 | `sound_demo.sh analyze` | Clean except "works", which was already bright before M1 | Listen |
| T10 | Music code unchanged since M0 | — | Listen |
| T11 | Save round trip; every word in the string table | Pass | Every menu and setting |
| T12 | — | — | F3 and a Ctrl+F3 report |
| T13 | — | — | Smoothness above 60 fps |

---

## Questions for you

1. **The footstep level:** answered, clip 3 (D29). Whether they may pass the ceiling waits for step 1.12 (D31).
2. **The hotbar:** answered. Materials come from the library menu, and the hotbar stays as a placeholder (D30).
3. **The order:** answered, yes (D32).
