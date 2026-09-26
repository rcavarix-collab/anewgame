# Round 2, E group, part 4: E1 surfaces, E10 noise, E11 far terrain, E12 visibility, E13 jobs

Delivered 2026-09-26, pasted into the chat (ChatGPT). This completes group E. Checked against the code. Their text isn't stored.

| Task | Grade | In one line |
|---|---|---|
| E13 jobs and threads | Good | Correct theory and sensible caution; our design already matches its advice; one wrong venue |
| E12 visibility | Good | Correct and well ordered; ours is already more precise than it assumed |
| E11 far terrain | Fair to good | A fair survey; the useful part is the coarse ring beyond the loaded world, which we'd already found |
| E10 noise | Fair to good | Right about the theory; misses that changing terrain noise changes every world |
| E1 surfaces from cells | Fair | Good sources, but its main assumption is wrong: our mesher isn't a height field, it's already a surface net |

**The pattern this round:** where the pack's facts were thin, the helper assumed the simpler system (a height field, a coarse visibility graph, a job queue without versions). Each time, walkgrid already does more. Its advice to "measure first" is right each time, and in most cases F3 already shows the counters.

## E13 jobs and threads: good

**Its questions answered from the code:**

| It asked | What the code does |
|---|---|
| One shared queue, or one per worker? | One shared queue with a lock and wake-ups, and a finished queue per channel (jobs.cpp). Threads: clamp(cores − 2, 1, 4), a harmless read (D21). |
| Can jobs submit dependent jobs? | No. Terrain and mesh jobs are independent; a mesh waits (on the main thread) until its neighbouring columns exist. |
| What do version stamps cover? | Per chunk (`meshVersion`, from one ever-growing counter). An edit within two cells of a chunk's side also bumps the neighbours' versions (`MarkChunkDirty` over the 2-cell margin the mesher copies). So the stamp covers every input the mesh reads, which is its main worry (the stamp must cover all inputs). |

**Sources:**
- Blumofe & Leiserson 1999 (JACM) and Gyrling (GDC 2015): right.
- Chase & Lev 2005 is from **SPAA 2005** (the ACM Symposium on Parallelism in Algorithms and Architectures), not the Symposium on Distributed Computing. The link is a copy on a third party's course page, not a publisher's or the authors' page.
- Unity's manual and cppreference.com: practitioner and reference pages, acceptable only as descriptions. The C++ standard itself is the primary source for the memory model.

**Keep:**
- **The conclusion:** with 1–4 threads and independent jobs, a shared queue is the right baseline. Work stealing and fibres wait until measurement shows idle workers with work waiting, or blocking between jobs. Neither is true today.
- **A profiler addition, when next touching F3:** the oldest waiting job's age, per channel. The counts exist (MESHES BUILDING, COLUMNS GENERATING, DIRTY WAITING); how long jobs wait doesn't.
- **Its test cases for stale results** (a result finishing after its chunk changed, results arriving out of order) are worth a native test.

## E12 visibility: good

**Ours is finer than it assumed.** Each chunk records which **pairs** of its six sides its open cells connect: 15 bits (`openings`, facetmesh.h `FacetOpenings`), not one flag per side. The walk from the camera's chunk:
- never turns back toward the camera;
- only enters chunks in view;
- is redone every frame (a few thousand steps);
- is refreshed on every remesh.

That's the "cave culling" refinement it proposes as step 2, already built (M1.11). It's also used only by the camera pass; the shadow pass sees what the sun sees.

**Its questions answered:**

| It asked | Answer |
|---|---|
| A coarse graph, or detailed openings? | Side-pair connectivity per chunk, as above |
| Reused across frames? | No, walked fresh each frame; its inputs update on remesh |
| Draw submission or GPU geometry, which costs more? | Not known: to measure (F3 has CHUNKS DRAWN, CHUNKS HIDDEN, TRIANGLES) |

**Sources:**
- Greene, Kass & Miller 1993; Zhang et al. 1997; Wonka et al. 2000; Bittner et al. 2004: right.
- The Luebke & Georges 1995 link doesn't point to that paper; the citation itself is right.
- Hey et al. 2001 and Li et al. 2023: not checked.

**Keep:**
- Its invariant: **an open connection must never be recorded as closed.** A false "closed" makes ground vanish; a false "open" only costs drawing. Worth a native test at chunk sides.
- **Occlusion queries or a depth pyramid:** parked. Our view distance is short (3 chunks by default) and open hills hide little. Revisit only if F3 shows many drawn chunks that are hidden, which is most likely in caves (R6).

## E11 far terrain: fair to good

**What it didn't know:**
- Our "detail bands" are **fine-detail cuts** (1×1, 2×2 or 4×4 per quad), not coarser base meshes, and the default draws every chunk at the finest cut (D49).
- Cracks are already solved: every chunk's side edges are cut at the finest level, so neighbours at any level meet exactly (facetmesh.h). That's the problem it spends most space on in geomipmapping.
- The loaded world is small: **3 chunks (48 blocks) by default**, with a slider from 1 to 8. The hard edge, hidden by fog, is the real limit, not the triangle count.

**Sources:** de Boer 2000, Losasso & Hoppe 2004, Lindstrom et al. 1996, Duchaineau et al. 1997: right. Schaufler's impostors paper: the year and venue need checking.

**Keep:**
- Its point that **far terrain must come from the same height rule as the near world**, or distant hills move as you approach. We have exactly that: `TerrainHeight` gives the height anywhere without loading chunks.
- This supports the research overview's lead: a **coarse ring of height-field terrain beyond the loaded chunks, clipmap-style**, drawn from `TerrainHeight`:
  - no collision, not editable;
  - fading into the fog;
  - built on the job threads only as the player crosses cell lines.

  That's the biggest possible look upgrade, and it would also address the distant white ground.
- A caution it rightly raises: the ring must hand over to the real chunks without a second visible edge. Plan it with the fog (D64).

## E10 noise: fair to good

**What ours is:**
- 2-D value noise, with the **cubic** fade (3t² − 2t³), both in the terrain generator (64-bit integer hash, doubles) and in the shaders (a small hash without sine).
- The terrain doesn't use a geometric octave sum: it adds separately named layers at chosen scales (72, 26 and 11 blocks, plus region, patch, rock and plateau fields).
- The CPU and GPU hashes differ, and needn't match: shader noise is only decoration.

**What it missed, and it matters: changing the terrain's noise changes every world.** A quintic fade or a gradient noise in the generator moves every hill in every existing save. Under our save rules (worldfile.h), that needs a **new generator version**, with the old one kept for old worlds. Shader noise has no such cost.

**Its paste has stray search-attribution labels** ("ITN Web", "OA Monitor Ireland", "Wiley Online Library") after several paragraphs. Those claims came through aggregator pages, not the sources. The citations themselves are right: Lagae et al. 2010; Perlin 1985, 2001 and 2002; Gustavson 2005; Worley 1996; Ebert et al.

**Keep:**
- **Shader side, free to try:** the quintic fade where lattice creases show in the macro colour patches (step 8).
- **Generator side, only with a new version:**
  - the quintic fade;
  - rotating each layer's lattice a little, so the layers' grids don't line up. That's our own idea, a known remedy for axis-aligned patterns;
  - mild domain warping of the region and plateau fields, so their edges wander.

  Bundle these with the next generator version rather than one at a time. The next such version is probably the caves (R6).
- **Cellular noise** for the texture generators (`tools/*_textures.py`): cracked clay, slate plates and stone joints for the 64-pixel art (step 9). It costs nothing in game, since it's baked into the art.

## E1 surfaces from cells: fair

**Its main assumption is wrong.** It took our mesher for a height field with no caves or overhangs, and called surface nets "less compelling" for us. In fact:
- The mesher wraps **every** face between a solid and an empty cell, so caves, overhangs and tunnels already work.
- It places one vertex per lattice corner where solid and empty cells meet, at the mean of the crossings, and builds a quad on each solid–empty face. **That is a surface net's layout,** with one easing pass for relaxation (docs/research/SURFACE_NETS.md, read in full).

The whole comparison table rests on the wrong premise.

**Its questions answered:**

| It asked | Answer |
|---|---|
| Overhangs, caves, several layers? | Yes, all |
| Chunk sides from global coordinates? | Yes: corners and jitter come from world coordinates and the seed, and a native test checks the seams |
| Deliberately faceted? | Yes, by design (D39): "neither cubes nor blobs" |

**Sources:** right, and useful for the bibliography:
- Lorensen & Cline 1987;
- Ju et al. 2002;
- Gibson 1998;
- Chan & Purisima 1998 (marching tetrahedra, new to us).

It gives Lorensen & Cline's DOI as 10.1145/37401.37422, where ours is 37402.37422. ACM lists the paper under both (the proceedings and the journal issue), as far as we know; still to check.

**Keep:**
- Its invariants list for the facet mesher: identical shared corners, reproducible jitter, seams that don't depend on build order, consistent winding. Most are tested; build-order independence is worth an explicit test.
- Its suggestion to compare split diagonals on the same corners fits step 4's debug views and the noon patches (docs/research/VERTEX_DARKENING.md): try the dark diagonal against the bulging fold.
