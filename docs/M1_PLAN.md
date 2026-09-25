# M1 plan: the first functional test

**Goal.** Walk a faceted, textured, lit world that looks like walkgrid, not Voxistics. Hear your footsteps with the music. Place and remove ground. All at 60 fps or better on your machine. Scope and checklist: `PROTOTYPE_OUTLINE.md` sections 3–5 (T1–T13).

**Status: waiting on your approval.** Nothing below is built. Every step follows the SOP: a feature card, measured after, docs updated, all checks run. Every hand-over says what you'll see.

---

## What you'll see, step by step

| After step | In the game | Here, for you to judge |
|---|---|---|
| 1.1 | No stutter from sound. Footsteps you can hear. | — |
| 1.2 | Nothing new | **Pictures of faceted, blended ground**, to judge the look before the engine changes |
| 1.3 | Nothing new | Pictures of the starting materials you picked, meeting each other |
| 1.4 | Different ground shape: rolling hills with regions. **Still cubes.** | — |
| 1.5 | **The ground is faceted, not cubes.** One texture per facet, sharp edges. | — |
| 1.6 | **Materials blend naturally at borders.** Hollows and overhangs go dark. | — |
| 1.7 | Walking and placing work properly on the facets. | — |
| 1.8 | Fine detail up close, coarser far away, with no cracks. | Triangle counts |
| 1.9 | The old blocks and cube props are gone. The hotbar shows the new materials. | — |
| 1.10 | Menus look the same, but every word now comes from a text file | A test language, to show nothing is left in code |
| 1.11 | No visible change; distant and hidden ground costs less | Chunks-drawn counts |
| 1.12 | Everything together: the checklist T1–T13 | — |

---

## Steps

### 1.1 Sound effects on their own thread; footsteps you can hear
- **Why first.** Your baseline report shows the world sound system on the main thread spiking to 14.9 ms. It's behind every one of your worst frames (forecast F11).
- **Change.** The effects palette renders on its own thread, the way music already does, from copies of what the main thread sets (listener, gait, cues). There's a voice cap with priority tiers, and footsteps get the level you pick from the clips.
- **Files.** Changed: `audio.cpp`, `audio.h`, `sfx_synth.cpp` (level only). New: none.
- **Cost.** Main thread: the WORLD SOUND row falls to the census only (target ≤ 0.5 ms, with no spikes). One more thread, which waits when idle.
- **Checks here.** Native tests; `sound_demo analyze`; a stress render at the voice cap; footstep peak level measured against the music.
- **Your check.** Footsteps are audible on grass, sand and stone. A Ctrl+F3 report shows no WORLD SOUND spikes.

### 1.2 The preview tool: pictures of faceted ground (proves T1, T2, T3)
- **Change.** A native tool builds faceted ground from the grid and draws still images on the CPU: rolling ground, a cliff, a dug pit, and material borders. It uses the same lighting formulas as the game's shader: sun, sky light, height-based blending. It also counts triangles, blanket subdivision against "only where it shows".
- **Files.** New: `facetmesh.cpp/.h` (the mesher itself, layer 4; the game uses the same code from 1.5), `tools/facet_preview.cpp`, `tools/facet_preview.sh`. Its tests go in `tests/tests.cpp`.
- **Cost.** None in the game yet.
- **Checks here.** Mesher tests: closed surfaces, consistent winding, no holes, stable output.
- **Also tests T7:** the existing square-tiling textures, projected from the world, on slopes and cliffs, including one directional texture on purpose.
- **Your check.** The look. **This is the checkpoint for the new direction:** we tune facet size, jitter and blending on these pictures until you're happy, before the engine changes.

### 1.3 The starting materials and the texture direction
- **Change.** You pick about 8–12 starting materials from contact sheets. The candidates are the 93 existing textures (natural set first), shown blended against each other under dawn and noon light. `TEXTURE_BRIEF.md` is rewritten for faceted ground and smooth filtering. Anything new or tweaked comes from the generators (`tools/*_textures.py`), so it can be adjusted by a parameter.
- **Files.** Changed: `assets/textures/TEXTURE_BRIEF.md`, the generators as needed. New: a contact-sheet mode in `tools/facet_preview`. Textures not picked go to `assets/textures/parked/`.
- **Your check.** The picks, on the sheets.

### 1.4 The test landscape
- **Change.** A seeded, versioned generator (`walkgrid-hills v1`): rolling ground with regions of your starting materials, a few cliffs, and no caves. It runs on the job threads.
- **Files.** New: `terrain.cpp/.h` (layer 3), `jobs.cpp/.h` (layer 1: a small fixed pool, sized from the processor count per D21, capped). Changed: `world.cpp` (the Voxistics generators are removed, and generation is queued to jobs, applied on the main thread with version stamps).
- **Cost.** TERRAIN on the main thread falls to applying results only.
- **Checks here.** Generator determinism (same seed, same ground, forever); streaming tests.
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
- **Your check.** Screenshots: faceted, not cubes.

### 1.6 Blending and sky light
- **Change.** Height-based blending across up to 3 materials per triangle, with the single-material path for interiors, and sky light in the vertex. Both follow what you approved on the pictures in 1.2 and 1.3.
- **Files.** Changed: `render.cpp` (world shader), `facetmesh.cpp`.
- **Cost.** GPU WORLD rises; it's budgeted at ≤ 5 ms on your machine, from a 0.6 ms baseline.
- **Your check.** Natural borders (T3), dark hollows (T5), and your pictures from 1.2 matching the game.

### 1.7 Collision and picking on facets
- **Change.** The player's capsule collides with the coarse facets, and the crosshair ray hits real triangles and maps back to a cell. Walking, sprinting, crouching and the slide all carry over.
- **Files.** New: `collide.cpp/.h` (layer 3). Changed: `world.cpp` (physics calls it), `game.cpp` (placing and removing use it).
- **Checks here.** Scripted walks over varied ground, with no snagging (F3); picking at many angles.
- **Your check.** Feel (T6); placing and removing land where you aim (T8).

### 1.8 Fine detail near you
- **Change.** Detail bands: facets split only where it shows (T2), stitched at the band edges, with hysteresis so walking doesn't cause rebuild storms (F4). Shadows use the coarse mesh.
- **Files.** Changed: `facetmesh.cpp`, `world.cpp` (band bookkeeping), `settings.cpp` (a fine-detail distance setting). The Graphics menu gets one row.
- **Cost.** Triangles against the 1.5 million budget, measured.
- **Your check.** Close-up detail, no cracks, a Ctrl+F3 report.

### 1.9 Out with the old
- **Change.** Work through `reference/README.md`'s list:
  - the old block roster is replaced by your materials;
  - cube props and shapes leave the build;
  - falling ground and grass die-back are switched off (D12);
  - the soundscape is retuned for the new materials;
  - the old generators are gone;
  - hotbar icons become faceted lumps of each material.
- **Files.** Changed: `blocks.h` (becomes the materials registry), `blocktex.cpp`, `icons.cpp`, `soundscape.cpp`, `world.cpp`. Removed from the build: `shapes.*`.
- **Your check.** Nothing from Voxistics' roster left in the library or hotbar.

### 1.10 Text for any language (D26)
- **Change.** Every player-facing word moves to a string table (`assets/text/en.txt`). The font atlas is built from the characters the table uses. Layouts measure text instead of assuming English widths. A check script flags any player-facing literal left in code.
- **Files.** New: `strings.cpp/.h` (layer 2), `assets/text/en.txt`, `tools/check_strings.py`. Changed: `hud.cpp`, `menus.cpp`, `textures.cpp` (atlas), `render.h` (atlas layout), `main.cpp` (the message box text).
- **Why here and not first.** It changes nothing you can see, and you wanted the new look early. Placing it after the world work means the menus it converts are final.
- **Your check.** A test language shows no English left, and no boxes for missing characters.

### 1.11 Draw only what's seen; load ahead (D16)
- **Change.** Hidden-chunk skipping (open-side connectivity, recorded while meshing), and loading ordered by where you look and where you're heading.
- **Files.** Changed: `facetmesh.cpp`, `render.cpp`, `world.cpp`.
- **Your check.** A Ctrl+F3 report with chunks drawn well under chunks resident.

### 1.12 The first functional test
The whole checklist, T1–T13, on your machine. Then the M1 reflection, the forecast review and the ledger.

---

## Questions for you

1. **The footstep level:** clip 1 (today), 2 (+10 dB, a bit brighter) or 3 (+16 dB)?
2. **The hotbar:** the outline kept a ten-slot hotbar for choosing which material to place. Keep it, or did you mean you'd rather not have one? If not, what should choosing a material look like?
3. **The order:** is the sound fix first, then pictures, then the engine, what you want?
