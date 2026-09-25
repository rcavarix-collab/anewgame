# walkgrid theories

Ideas we believe but haven't proven. Each says why we believe it, how we'll test it, and what we'll do if it's wrong. A theory is settled only by a test, never by agreement. When it's settled, the result is recorded here and any decision it leads to goes in `DECISIONS.md`.

**Status:** Open (untested), Testing, Holds, Fails, Partly.

---

### T1. Faceted ground over a cell grid reads as its own look, not cubes and not blobs
- **Why we believe it.** Sharp, flat-shaded facets with a seeded jitter sit between Minecraft's cubes and Astroneer's smooth surfaces.
- **Test.** Still images from the M1 preview tool of rolling ground, a cliff and a dug pit, judged by the owner before the engine changes.
- **If wrong.** Tune the jitter and facet size; failing that, reopen the grid choice (B: hex columns; C: 12-sided cells).
- **Status:** Testing. Pictures made (M1.2, `docs/pictures/m1_2/`), including the same spot as cubes. Claude's read: it reads as its own look, strongest under a low sun; at noon facets are faint. Awaiting the owner.

### T2. Subdividing only where detail shows cuts near triangles 3 to 5 times against blanket subdivision, with no visible loss
- **Why we believe it.** Most ground is flat and single-material, and only borders, bumps and silhouettes show fine detail.
- **Test.** The preview tool counts triangles both ways on the same terrain (including dug-out ground) and renders both for comparison.
- **If wrong.** Shrink the near band, or subdivide by screen size instead of distance.
- **Status:** Fails as stated (M1.2). Near ground is almost all lumpy material (grass, dirt, gravel, stone), and lumps are the only reason to cut, so "only where it shows" saved 2% (402,000 against 412,000 triangles), not 3–5×. It saves on smooth materials only (sand, sandstone, snow). The budget holds anyway: 308,000 triangles within 96 blocks against 1.5 million. Consequence: the fine-detail distance setting (1.8) is the real lever for lesser machines.

### T3. Height-based blending looks natural where grass meets dirt, not muddy
- **Why we believe it.** Each material's height map decides which one shows through, so borders come out ragged and crisp. This is common practice in terrain rendering.
- **Test.** Preview images of every pairing of the starting materials, judged by the owner; then the same spot in the game.
- **If wrong.** Add a transition mask texture, or hand-tune per pair.
- **Status:** Testing. Preview pictures show ragged borders (grass tufts over gravel, sand into grass); borders read soft at a distance. Awaiting the owner, then the game (1.6).

### T4. Rendering between ticks removes stutter above 60 fps with no felt input delay
- **Why we believe it.** Mouse look is applied every frame; only the body is blended, and it's at most one tick (16.7 ms) behind.
- **Test.** The owner plays with vsync off, and on a high-refresh display if one is available (M0 check C6).
- **If wrong.** Extrapolate instead of interpolating, or raise the tick rate.
- **Status:** Testing (built in M0.12; awaiting the Windows check).

### T5. Two fixed job threads are enough for terrain and meshing
- **Why we believe it.** Voxistics meshed 6 chunks a frame on the main thread without hitching. Two threads more than double that, and take it off the main thread.
- **Test.** Walk and sprint into new ground at render distance 8, and watch the "waiting" counters and the worst frame in F3.
- **If wrong.** Lower the fine-detail distance first; then consider a third thread (still a fixed number).
- **Status:** Open.

### T6. The harmony-locked sound palette stays clear with many sounds at once
- **Why we believe it.** Every pitch comes from the current chord and every onset lands on the beat, so more sounds add up to more music.
- **Test.** Offline renders with the voice cap at its limit (`tools/sound_demo.sh`), and then busy moments in play.
- **If wrong.** Lower the voice cap, add ducking, or give each sound tier its own frequency band.
- **Status:** Open (matters most when action arrives).

### T7. The existing square-tiling textures work unchanged on faceted ground
- **Why we believe it.** Textures are projected from the world (top and two sides, blended by slope), not mapped per triangle, so triangle shape doesn't matter. Every generated texture already wraps seamlessly on its tile.
- **Where it could fail.** On slopes between floor and wall, a plain blend of the projections smears; textures with a strong direction (planks, bricks, strata, bark) stay aligned to the world, not the facet.
- **Test.** M1 step 1.2 and 1.3 pictures: slopes, cliffs and borders with the real textures, including one directional texture on purpose.
- **If wrong.** A sharp, height-guided blend between projections (shader only); keep directional textures for built pieces placed on axis-aligned faces later.
- **Status:** Holds on the pictures (M1.2). Sandstone's strata stay level along a wandering cliff, and the square textures show no stretching on slopes. One rule was needed: a slope shows its material's top texture up to about 50° and its side texture only when steeper; otherwise every one-cell rise in a meadow showed a stripe of dirt.
