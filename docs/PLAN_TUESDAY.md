# walkgrid: the plan for Tuesday (agreed with the owner, 2026-09-26)

The next building session. Every step is one system and one commit (FOUNDATIONS 1); the game builds after each; the owner tests between batches. Details and causes: `docs/PLAN_LOOK.md`; research: `docs/research/`; Grok's work: `docs/grok_reviews/`.

**One finding shapes the plan.** Textures are already stored at 64×64 in the engine (`BLOCK_TEX_SIZE = 64`, blocktex.h); today's 32-pixel art is scaled up 2× when it loads, and the generators take `--size`. So 64-pixel textures (D65) are art work, not engine work.

## Batch 1: quick wins and Grok's deliveries (one test build)

| # | Step | Owner checks |
|---|---|---|
| 1 | Grok's save reader into `tools/dump_save.py`; its text tests (one escape fixed) and the good parts of its sky tests into `tests/tests.cpp` | nothing in game |
| 2 | Text: T2's five English edits, then the German, Spanish and French files with their word fixes and T2's changes carried through | menus in each language |
| 3 | Screenshots: read the picture back a frame later (the remaining ~26 ms goes to near zero) | Ctrl+F3 while pressing F2 |
| 4 | Debug views (PLAN_LOOK G): one developer key cycles baked darkening only, shadows only, materials only; costs nothing unless pressed; stays in the game | what the noon patches are |

## Batch 2: the sky (one test build)

Decide first: clouds in real time, or partly sped up with the fast day (`docs/research/CLOUDS.md`).

| # | Step | Owner checks |
|---|---|---|
| 5 | Low clouds that live (A + C): the pattern evolves as it drifts (advected along the wind in two crossfaded phases, the flow-map idea, plus slow noise evolution; docs/research/OVERVIEW.md), soft edges, lit rims, no hard outlines | a few minutes of sky |
| 6 | High streaks (B): shorter wisps, with gaps and heads moving along them, about half the low clouds' angular speed, turned from the low wind, no rays near the sun | facing the sun at dusk |

## Batch 3: the ground's look (one test build)

| # | Step | Owner checks |
|---|---|---|
| 7 | Clean material edges (D): score each material by weight × height (max-blending with a small grace band, Schuster et al. 2020), not weight + height, so a material can't poke through far from its own ground; near the player decided per texel, a crisp mosaic (docs/research/TEXTURE_BLENDING.md) | the sand–stone edge |
| 8 | Less repetition (E): near the player, one randomly offset and turned copy per irregular texel-aligned region (no blending, so the pixel art stays crisp); further away, three offset copies with histogram-preserving blending (Heitz & Neyret 2018; read Burley 2019 first) for stochastic materials, shifts along the layers only for structured ones (sandstone, slate); plus large-scale colour patches (docs/research/TEXTURE_BLENDING.md) | a big flat area from a height |
| 9 | 64-pixel art (F + H): the generators redraw the 12 materials at 64, each in its own style of marks (Grok's A-series briefs), slate lighter | every material, close up |

## Batch 4, if time allows

Decided by what the debug views show: a longer baked sky view (16–24 blocks), and a pass on the day's colours (R1).

## Ideas on the table (thrown at the wall, not scheduled)

- **From the research overview (docs/research/OVERVIEW.md):** far terrain as a coarse clipmap-style ring drawn from the height function (the biggest unplanned look upgrade); constrained surface nets as the principled answer to terracing (W055, read in full: `docs/research/SURFACE_NETS.md`), best as the terrain's continuous height clamped into each cube, seam-safe with no iterations, edited cells pinned; bent-normal ambient from the sky-view bake; Hosek–Wilkie as a colour reference for the sky and light.

- **East Asian text (W068):** the UI font atlas and text layout assume every character is one cell wide; Chinese, Japanese and Korean characters are two. A step gives wide characters double-width cells, in the atlas (`GenerateUIAtlas`) and in layout (`UIDrawText`, `UITextWidth`). Grok's T18–T21 translations wait on it. Scripts that need shaping (Devanagari, Thai, Arabic's right-to-left joining) need a shaping step beyond a glyph atlas: parked (W069). African languages (T26–T33): the Latin-script ones work now (the atlas takes any code point the font has); Yoruba's stacked tone marks need precomposed characters or combining-mark support; Amharic's Ethiopic script needs its font and may need wide cells like W068.

- Sky settings (cloud amount, cloud speed), now or with weather.
- A faint wind bed in the world sound that rises when the low clouds move faster.
- Start planning the building layer (W067) if it becomes M2, with Grok researching alongside.
