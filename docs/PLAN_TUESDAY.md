# walkgrid: the plan for Tuesday (agreed with the owner, 2026-09-26)

The next building session. Every step is one system and one commit (FOUNDATIONS 1); the game builds after each; the owner tests between batches. Details and causes: `docs/PLAN_LOOK.md`; research: `docs/research/`; Grok's work: `docs/grok_reviews/`.

**One finding shapes the plan.** Textures are already stored at 64×64 in the engine (`BLOCK_TEX_SIZE = 64`, blocktex.h); today's 32-pixel art is scaled up 2× when it loads, and the generators take `--size`. So 64-pixel textures (D65) are art work, not engine work.

## Batch 1: quick wins and Grok's deliveries (one test build)

| # | Step | Owner checks |
|---|---|---|
| 1 | Grok's save reader into `tools/dump_save.py`; its text tests (one escape fixed) and the good parts of its sky tests into `tests/tests.cpp` | nothing in game |
| 2 | Text: T2's five English edits, then the German, Spanish and French files with their word fixes and T2's changes carried through | menus in each language |
| 3 | Screenshots: read the picture back a frame later (the remaining ~26 ms goes to near zero) | Ctrl+F3 while pressing F2 |
| 3b | Durable saves (W070): flush the new save to disk before swapping it in, and load the `.bak` when the main save is missing or damaged; a test for each interruption point | nothing in game, unless a save is ever damaged |
| 4 | Debug views (PLAN_LOOK G): one developer key cycles baked darkening only, shadows only, materials only; costs nothing unless pressed; stays in the game. Likely finding: triangle wedges from per-corner darkening split along the bright diagonal (docs/research/VERTEX_DARKENING.md) | what the noon patches are |

## Also for approval: a notes pass (comments only)

From `docs/AUDIT_2026-09-26.md`:
- add the missing header fields in 24 headers;
- rewrite four stale headers;
- fix the comments naming removed files and features;
- add three one-line notes.

No behaviour changes, so it could ride in Batch 1 as its own step. After it, `tools/check_notes.py` joins the checks (`docs/CONVENTIONS.md` §3). The stale passages in DESIGN.md and CLAUDE.md are the owner's call.

## Batch 2: the sky (one test build)

Decide first: clouds in real time, or partly sped up with the fast day (`docs/research/CLOUDS.md`).

| # | Step | Owner checks |
|---|---|---|
| 5 | Low clouds that live (A + C): the pattern evolves as it drifts (flowed along the wind in two half-cycle-apart phases with per-pixel noise on the timing, distortion −f..+f around rest, Vlachos 2010; plus slow noise evolution, a different drift speed per noise octave and a slowly moving threshold so clouds form and dissolve; docs/research/SKY_AND_FLOW.md, docs/grok_reviews/round2_E.md), soft edges, lit rims, no hard outlines | a few minutes of sky |
| 6 | High streaks (B): shorter wisps, with gaps and heads moving along them, about half the low clouds' angular speed, turned from the low wind, no rays near the sun | facing the sun at dusk |

## Batch 3: the ground's look (one test build)

| # | Step | Owner checks |
|---|---|---|
| 7 | Clean material edges (D): a blue-noise threshold per texel makes the crisp border even, not speckled (docs/research/DITHERING.md); score each material by weight × height (max-blending with a small grace band, Schuster et al. 2020), not weight + height, so a material can't poke through far from its own ground; near the player decided per texel, a crisp mosaic (docs/research/TEXTURE_BLENDING.md) | the sand–stone edge |
| 8 | Less repetition (E): near the player, one randomly offset and turned copy per irregular texel-aligned region (no blending, so the pixel art stays crisp); further away, three offset copies with histogram-preserving blending as Burley 2019 makes practical (1-D tables built on load, luminance only, truncated Gaussian with soft clipping, weights raised to γ ≈ 4) for stochastic materials, shifts along the layers only for structured ones (sandstone, slate); plus large-scale colour patches (docs/research/TEXTURE_BLENDING.md) | a big flat area from a height |
| 9 | 64-pixel art (F + H): the generators redraw the 12 materials at 64, each in its own style of marks (Grok's A-series briefs), slate lighter | every material, close up |

## Batch 4, if time allows

Decided by what the debug views show: a longer baked sky view (16–24 blocks), and a pass on the day's colours (R1), shaped by Hošek & Wilkie's findings: the low sun's glow spreading sideways more than up, bright ground brightening the low sky, sunsets as a gradient (docs/research/SKY_AND_FLOW.md). Turbidity becomes the one haze knob weather will drive. Also a candidate: sky-lit ambient from a few spherical-harmonic numbers per frame, in place of the two-colour hemisphere (round2_E).

## Ideas on the table (thrown at the wall, not scheduled)

- **Footsteps on the stride or on the beat?** Ours play on the music's beat grid; published footstep synthesis drives each step from the foot's contact. A beat-locked step can't match the stride, which may be why they feel detached. Options: stride, beat, or stride quantised to the nearest subdivision within a small window. Owner's call (round2_E_part2).

- **Our own tone curve (D60):** ACES per channel is many engines' default and bleaches bright colours; a luminance-based curve of our own keeps grass green in strong sun (docs/research/TONE_MAPPING.md). A look decision: side-by-side renders for the owner.

- **From the research overview (docs/research/OVERVIEW.md):** far terrain as a coarse clipmap-style ring drawn from the height function (the biggest unplanned look upgrade); constrained surface nets as the principled answer to terracing (W055, read in full: `docs/research/SURFACE_NETS.md`), best as the terrain's continuous height clamped into each cube, seam-safe with no iterations, edited cells pinned; bent-normal ambient from the sky-view bake; Hosek–Wilkie as a colour reference for the sky and light.

- **East Asian text (W068):** the UI font atlas and text layout assume every character is one cell wide; Chinese, Japanese and Korean characters are two. A step gives wide characters double-width cells, in the atlas (`GenerateUIAtlas`) and in layout (`UIDrawText`, `UITextWidth`). Grok's T18–T21 translations wait on it. Scripts that need shaping (Devanagari, Thai, Arabic's right-to-left joining) need a shaping step beyond a glyph atlas: parked (W069). African languages (T26–T33): the Latin-script ones work now (the atlas takes any code point the font has); Yoruba's stacked tone marks need precomposed characters or combining-mark support; Amharic's Ethiopic script needs its font and may need wide cells like W068.

- **A walking test course** (E18, round2_E_part3): a small built area to walk after any change to terrain shape or movement, with:
  - steps and drops either side of `CLIMB` and `STICK`;
  - slopes either side of `WALK_UP`;
  - a crawlspace, a ledge and a narrow passage.

  Most useful before surface nets (W055).
- **The next terrain-generator version (group E, round2_E_part4):** changing terrain noise moves every hill in every save, so bundle these into one new generator version, with the old one kept for existing worlds:
  - the quintic fade;
  - each layer's lattice turned a little;
  - mild warping of the region and plateau edges;
  - caves (R6).
- **Tests from group E:**
  - a stale mesh result arriving after its chunk changed;
  - chunk openings never recorded as closed when cells connect;
  - facet seams identical whichever neighbour is built first.
- **F3: how long the oldest job has waited,** per channel (E13); only counts exist today.
- **Creative direction (docs/research/LANDSCAPE.md, GAME_DESIGN.md):** twelve landscape rules for terrain v2 and the far ring, including:
  - an offline skyline-roughness check in the facet preview (target D ≈ 1.3–1.5);
  - lookouts beside shelters;
  - regions with their own identity;
  - rare landmarks and rare vistas;
  - fog toward the sky's colour.

  Also a one-page outside-playtester protocol (think-aloud, no help given, no telemetry). Owner questions on the game's aesthetics, competence and relatedness are listed in GAME_DESIGN.md 5.
- **Music and world sound (docs/research/MUSIC.md):**
  - a roughness-by-register check in the analyzer, and an opened voicing for the mid pads (Dm9's E4–F4 semitone);
  - slow spectral drift in the voices;
  - reverb for world sounds that follows the soundscape census (caves sound like caves);
  - rare musical arrivals at sunrise and at landmarks;
  - motifs of place.

  Owner's calls: harmony by region, day tempo.
- **Sound design (docs/research/SOUND_DESIGN.md):**
  - the event grammar (material × interaction × pattern) for every world sound;
  - distance darkening;
  - grain counts per event;
  - informative dig and place cues;
  - keynotes and soundmarks by region.

  Owner's call: longer, harmony-locked rings for hard materials, which relaxes SOUND_PALETTE 1.6's 60 ms cap.
- Sky settings (cloud amount, cloud speed), now or with weather.
- A faint wind bed in the world sound that rises when the low clouds move faster.
- Start planning the building layer (W067) if it becomes M2, with Grok researching alongside.
