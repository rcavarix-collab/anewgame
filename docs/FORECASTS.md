# walkgrid forecasts

Problems we expect before they happen: why we expect each one, the warning signs to watch for, when to act, and what we'll do. The point is to deal with them early, while they're cheap. Reviewed at every milestone. A forecast that comes true moves to `REFLECTIONS.md` with what we learned. One that stops being possible is marked "retired" with the reason.

**Likelihood / cost if ignored:** Low, Medium, High.

---

## Rendering and the world

### F1. The fine mesh's triangle count gets away from us
- **Likelihood:** High. **Cost if ignored:** High.
- **Why.** Small triangles multiply fast: a cliff or a dug-out hollow has far more surface than flat ground (FOUNDATIONS 4.1).
- **Warning signs.** Triangle count in F3 above 1.5 million; GPU world row above 5 ms; any chunk hitting its vertex cap.
- **Act.** From M1 step 1: the preview tool measures before the mesher is built.
- **Plan.** Subdivide only where it shows (T2); a hard per-chunk cap; shadows from the coarse mesh.

### F2. Cracks where detail levels meet
- **Likelihood:** High. **Cost if ignored:** Medium.
- **Why.** Every level-of-detail system has them, as edges of different resolution meet.
- **Warning signs.** Flickering pinholes along band edges, especially at grazing angles.
- **Act.** In the mesher's design, not after.
- **Plan.** Stitch band edges; a native test that walks every band border looking for gaps.

### F3. The player snags on facet edges
- **Likelihood:** Medium. **Cost if ignored:** High (feel).
- **Why.** A capsule sliding over triangles catches on the edges between them. It's a known problem in every triangle-collision game.
- **Warning signs.** Stops or hops when walking diagonally across slopes.
- **Act.** M1 step 4.
- **Plan.** Collide against the coarse facets only; smooth normals across shared edges; scripted walk tests over varied ground.

### F4. Rebuild storms when walking, on top of edits
- **Likelihood:** Medium. **Cost if ignored:** Medium.
- **Why.** Moving shifts every detail band, so many chunks want rebuilding at once.
- **Warning signs.** "Dirty waiting" climbing in F3 while walking; edits appearing late.
- **Act.** M1 step 3.
- **Plan.** Hysteresis on band edges; edits jump the queue; a cap on rebuilds per frame.

### F5. Material blending is expensive on the GPU
- **Likelihood:** Medium. **Cost if ignored:** Medium.
- **Why.** Up to 12 texture reads per pixel on border triangles.
- **Warning signs.** GPU world row rises with how many borders are on screen.
- **Act.** M1 step 5.
- **Plan.** Single-material path for interior triangles; dominant material only past a distance; the September review's two cheap cuts.

### F6. Chunk vertex data outgrows memory at long view distances
- **Likelihood:** Low. **Cost if ignored:** Medium.
- **Why.** The vertex grows from 8 to about 20 bytes, and there are more vertices near the player.
- **Warning signs.** Process or GPU memory near the budget at render distance 8.
- **Act.** When M1's mesher lands; the ledger records it.
- **Plan.** Coarse far chunks; free CPU copies once uploaded, except near the player (collision).

## Simulation and structure

### F7. The main thread fills up as systems arrive
- **Likelihood:** High (once action arrives). **Cost if ignored:** High.
- **Why.** Every system starts small and grows. This is the Minecraft trap.
- **Warning signs.** The main-thread CPU row creeping up milestone after milestone.
- **Act.** Every milestone (SOP section 4).
- **Plan.** The cost ledger; per-system caps; subtract before adding.

### F8. Game code creeps back into the engine
- **Likelihood:** Medium. **Cost if ignored:** High.
- **Why.** It's always faster in the moment to reach across.
- **Warning signs.** A layer-check failure, or a "temporary" exception.
- **Act.** Always; the layer check runs with every test run.
- **Plan.** Hooks, not includes; no exceptions to the layer rule without a logged decision.

### F9. Save format churn
- **Likelihood:** Medium. **Cost if ignored:** Medium.
- **Why.** Every feature wants to store something, and each change risks old saves.
- **Warning signs.** Several version bumps in a milestone.
- **Act.** When the first thing uses the game section.
- **Plan.** A tagged, length-prefixed record per system inside the game section, so a new record never breaks old saves and unknown records are skipped.

### F10. Precision far from the start
- **Likelihood:** Low. **Cost if ignored:** Low.
- **Why.** Positions are 32-bit floats.
- **Warning signs.** Jitter or physics oddities past about 100,000 blocks out.
- **Status.** Rendering is handled (M0.13). Physics and picking still use floats: revisit if the world is meant to be crossed that far.

## Sound

### F11. The effects voice starves during a hitch *(came true in the M0 baseline: WORLD SOUND worst 14.9 ms on the main thread; see REFLECTIONS)*
- **Likelihood:** Medium. **Cost if ignored:** Medium.
- **Why.** Today it's topped up once per frame from the main thread, with only about 40 ms queued.
- **Warning signs.** Clicks or gaps in effects during saves, loading or window drags.
- **Act.** M1 step 1.1: built. Awaiting the owner's Ctrl+F3 report to close it.
- **Plan.** Its own thread, like the music (done in M1.1: a mailbox from the main thread, woken by XAudio2 as each buffer ends).

### F12. The music wears thin
- **Likelihood:** High. **Cost if ignored:** Low now, Medium later.
- **Why.** It's the same hour every hour (September review).
- **Warning signs.** It stops being noticed, or starts being turned off.
- **Act.** After M1, or when the new score is decided.
- **Plan.** Seeded per-day variation, or the new score.

## Text

### F16. Translation gets harder the more text is written without it
- **Likelihood:** High. **Cost if ignored:** Medium, and growing.
- **Why.** Every menu or HUD string written into code has to be found and moved later. Some languages need characters the ASCII font atlas doesn't have, and some break layouts sized for English.
- **Warning signs.** Any new string literal shown to the player outside the string table.
- **Act.** M1 step 0, while the menus are small.
- **Plan.** A string table; a font atlas built from the characters the table uses; layouts that measure text instead of assuming its width; a check script that flags player-facing string literals in code.
- **Outcome (M1.10).** Done as planned (D42); `tools/check_strings.py` now fails the checks on any new player-facing literal. Remaining risk: scripts wider than the monospace cell (DESIGN 4.6.2).

## Process

### F15. Carried-over behaviour we didn't choose stays switched on
- **Likelihood:** High. **Cost if ignored:** Medium.
- **Why.** M0 carried Voxistics over whole, so everything it did still happens. Falling ground and grass die-back are live today, although parked for the first test (D12).
- **Warning signs.** Anything happening in play that isn't in the outline.
- **Act.** M1: the list in `reference/README.md` is worked through item by item.
- **Plan.** Every carried-over behaviour is either in the M1 scope or switched off, and the list is checked at the end of M1.

### F13. Scope creep inside a milestone
- **Likelihood:** High. **Cost if ignored:** High.
- **Why.** Good ideas arrive mid-work.
- **Warning signs.** Steps that didn't exist in the plan.
- **Act.** Always.
- **Plan.** New ideas go to the scope sheet as Could or Won't and wait (SOP section 1).

### F14. The docs drift from the code
- **Likelihood:** Medium. **Cost if ignored:** High.
- **Why.** Updating docs is easy to skip.
- **Warning signs.** A section describing something that isn't there.
- **Act.** Every step.
- **Plan.** Docs are part of done (SOP section 5); a check script flags code references to design sections that don't exist.

### F17. Small dark slivers at terrace corners
- **Likelihood:** High (seen in the M1.2 pictures). **Cost if ignored:** Low.
- **Why.** Where a terrace turns an inner corner, the corner rule makes a steep little pocket; about 0.08% of the surface faces away from its surroundings, and a few dozen detail triangles per scene fold slightly.
- **Warning signs.** Dark specks on smooth, terraced ground (sand), more when the sun is low.
- **Act.** If the owner notices them in game: a second, weaker smoothing pass only where neighbours disagree, or smoothing the terrain generator's steps.

### F18. Vertex memory with fine detail
- **Likelihood:** Medium. **Cost if ignored:** Medium (the 2 GB-card floor).
- **Why.** Detail vertices are per material triple, so borders duplicate them; the preview world's near detail is about 250,000 vertices around one viewpoint.
- **Warning signs.** GPU memory in a Ctrl+F3 report approaching 1 GB.
- **Act.** 1.8's fine-detail distance setting; pack the vertex to 20 bytes (1.5).

### F19. Pixel cost of blended borders
- **Likelihood:** Medium. **Cost if ignored:** Medium on the 2 GB floor machine.
- **Why.** A border pixel within 64 blocks reads up to 27 texels (three materials × three projections × colour, surface and height), where M0's cube face read 3.
- **Warning signs.** GPU WORLD in a Ctrl+F3 report above 3 ms at 1080p.
- **Act.** Pull the blend distance in (64 → 32), drop the height texture's third projection, or give the setting a quality level.

---

## Review at the end of M1 (2026-09-25, before the owner's run)

What M1 did to each forecast. "Awaiting" means only the owner's F3 / Ctrl+F3 report can settle it.

| # | Where it stands |
|---|---|
| F1 Triangle count | Budget holds on paper: 308,000 triangles within 96 blocks (preview), against 1.5 million. The fine-detail setting (0–3 chunks) is the lever. Awaiting TRIANGLES in F3. |
| F2 Cracks between levels | Designed out: boundary edges are always cut for the finest level (D40); 0 unmatched edges at every pairing, tested. Watch for holes in game. |
| F3 Snagging on facets | 16 scripted walks: stuck 0, sank 0, view jumps 0 (D39). The feel is the owner's (T6). |
| F4 Rebuild storms | A chunk of hysteresis on detail levels; only changed chunks rebuild, through the capped queue; meshes build on job threads. Awaiting MESHES BUILT / DIRTY WAITING while walking. |
| F5 Blending cost on the GPU | One path for single-material pixels and beyond 64 blocks; up to 27 texture reads on border triangles near. Awaiting GPU WORLD. |
| F6 / F18 Vertex memory | 16-byte vertices, 16-bit indices where they fit. Awaiting a report at the largest render distance. |
| F7 Main thread fills up | Terrain and meshes on job threads (M1.4–1.5), effects on their own thread (M1.1). New on the main thread: the visibility walk (about 0.3 ms at most, less with the frustum) and heading sorts. Awaiting RENDER and the worst frame. |
| F8 Game into the engine | Layer check: 0 violations throughout M1; no new hooks. |
| F9 Save format churn | Unchanged (v1). Renumbering the registry changed nothing saved (saves store names). |
| F10 Precision far out | Unchanged. |
| F11 Effects voice starves | Addressed in M1.1 (its own thread and a mailbox; 48-voice cap). Awaiting WORLD SOUND worst in a report: the M0 baseline's 14.9 ms should be gone. |
| F12 Music wears thin | Unchanged; not M1's subject. |
| F13 Scope creep | M1 kept to its twelve steps; each step lists the files it touched beyond the plan's list, and why. Two things were added unasked and flagged as provisional: the test language (1.10) and sand's own footstep (1.12). |
| F14 Docs drift | Docs were updated at every step, and the string check (1.10) now guards one more rule mechanically. The plan's file lists were often short: most steps touched two or three more files than listed. |
| F15 Carried-over behaviour | Falling ground and grass die-back switched off (D41, as D12 asked). Carried and still on: Voxistics' menus, music, sky, the flat test ground. |
| F16 Translation | Done (D42). |
| F17 Dark slivers at terrace corners | Unchanged since M1.2 (about 0.08% of the surface). Waiting on whether the owner sees them in game. |
| F19 Border pixel cost | Awaiting GPU WORLD with the camera over a patchwork of materials. |

**New forecast from M1:**

### F20. The visibility walk grows with how tall the world is
- **Likelihood:** Medium (once caves or tall builds arrive). **Cost if ignored:** Medium.
- **Why.** The walk visits every chunk in view up to one row above the highest ground, once per side it's entered by. On today's hills that's a few thousand steps; mountains, towers or deep caves multiply the rows.
- **Warning signs.** RENDER rising with render distance while triangles don't; a Ctrl+F3 report with the walk's share over 0.5 ms.
- **Act.** When a report shows it.
- **Plan.** Walk only when the camera changes chunk or turns past a threshold, and reuse the result between; or walk columns of chunks rather than single chunks above the ground.

