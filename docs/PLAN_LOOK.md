# walkgrid: look and sky plan (from the third play-test)

What this is: the owner's third play-test (2026-09-26, 1080p, render distance 3), digested into causes found in the code, and the plan for the next build. **Planning only.** Nothing here is built until the owner says go (expected Tuesday). Each item becomes a feature card (docs/SOP.md) when it's picked up. Scope rows W059–W067.

## What the owner saw, and why (checked against the code)

**Clouds barely move and never change.**
- The high streaks drift so slowly that one streak takes about five minutes to move its own width: they look frozen.
- They're stretched about 17 times longer than they are wide.
- The low clouds move 2.5 blocks a second, 220 blocks up, which barely registers.
- Both layers are one fixed pattern slid across the sky: nothing inside a cloud ever changes.

**Searchlight beams in the sky.** Looking along the wind, all the long parallel streaks meet at one point on the horizon. When that point is near the sun, they read as rays.

**Low clouds at sunset** look like flat, hard-edged blue puddles.

**Orange lines at the foot of the slate ridge.** These are the material blend, not shadows (an earlier guess, now withdrawn). Where two materials meet, each pixel shows the one whose texture stands "taller" there. The sand's ripple crests are tall, so they poke through the stone as thin orange lines. The same rule gives the ragged, noisy fringe along every material edge ("edge blending looks like garbage").

**Textures feel flat and repetitive.** Every material is 32 pixels per block and repeats exactly every block, so over a large area it's a perfect grid. The only variation on top is one slow drift in brightness and warmth across the world.

**Odd dark patches on the ground at noon.** Not yet proven. At noon the sun makes almost no shadows, so the likely cause is the darkening baked into each ground corner (openness and sky view). It's stored per corner, so it smears across whole triangles into triangle-shaped patches. Item G would confirm it.

**Slate is nearly black-blue** even in light: its texture is very dark and saturated.

**Distant ground toward a setting sun** still dissolves into orange. That's the sunset colour band on the horizon, which the fog keeps on purpose (D64).

**Performance** (the owner's report, 1080p, render distance 3):
- The graphics card spends about 2 ms on the world (4.1 ms at worst), inside the 5 ms budget.
- No hitches over 33 ms.
- An F2 screenshot now costs about 26 ms of work, down from 70–100 (W057). The rest is the wait for the graphics card to hand the picture back.

## The plan (items A–H, one step each)

| # | What | How, cheaply | Cost | Scope |
|---|---|---|---|---|
| A | Clouds that live | Drift about 10–20× faster. Shapes change over time: the pattern slowly transforms as well as sliding, and two pattern layers move at different speeds, so clouds form, stretch and break up as they flow downwind. | A few more calculations per sky pixel only | W059 |
| B | Believable high streaks | Shorter wisps (about 4:1, not 17:1), broken into patches, fainter toward the sun, so they don't form rays | Same as today | W060 |
| C | Soft, lit low clouds | Softer edges, brighter rims toward the sun, no hard outline | Sky pixels only | W061 |
| D | Clean material edges | A wider, smoother blend band driven by noise at several scales, with the heights only nudging it, so one material's pattern never shows as thin streaks in another | Same texture reads as today | W062 |
| E | Break up repetition | Each cell of ground reads the texture shifted and turned by a random amount, blended smoothly, so the grid disappears; plus large-scale patches per material (drier and wetter, darker and lighter, a second colour) | A couple more texture reads, near ground only | W063 |
| F | 64-pixel textures | The texture format gains 64 (it allows 16 or 32 today), and the generators redraw the materials at 64 | About 4× texture memory: a few MB | W064 |
| G | Find the noon patches | A debug key that cycles views: darkening only, shadows only, materials only | Debug only | W065 |
| H | Slate | Lighter and less saturated, in its generator | None | W066 |
| – | Screenshot read-back | Read the picture back one frame later, so F2 costs almost nothing | None | (small change) |

## Also on the list, from Grok's R1 (appraised in docs/grok_reviews/R1.md)

- A longer sky-view reach (16–24 blocks), after item G shows whether per-corner baking causes the noon patches.
- A tuning pass on the day's colours in `ComputeAtmosphere`.
- Height-aware haze, once we can see further.

## Open questions for the owner (asked after the Grok review)

1. **F, texture density.** Three choices:
   - 64 pixels per block: finer detail, repeats as often as now;
   - 64 pixels over 2 blocks: the same pixel size as now, repeating half as often;
   - 128 pixels over 2 blocks: both.
2. **A, cloud speed.** Low clouds crossing the whole sky in a few minutes, high streaks slower: about right?

## Parked: a building layer (owner's idea, W067)

Eventually the player will need to place **built blocks**: crisp, made pieces for buildings and structures, which can't be built the way the faceted earth is. They should sit on the ground in a satisfying way. Not this horizon; kept here so it isn't lost.

**History.** The engine carried over from Voxistics had a cube mesher and shaped pieces (beams, struts, bevelled blocks: DESIGN.md 4.15). All of it was removed in M1.9 (D41) when the world became faceted ground only. What's new is making built blocks and faceted ground meet well.

**Questions to answer then:**
1. **Where the two meet.** A cube resting on lumpy, jittered ground either floats at the corners or clips into it. Options:
   - the ground flattens itself under a built block;
   - the block gets a small skirt reaching down to the ground;
   - built blocks sit on a flattened footing, like a real foundation.
2. **One grid or two.** Built blocks could share the ground's cells (a cell holds earth or a built block), or live on their own finer grid (half or quarter blocks) for more freedom, at the cost of a second system.
3. **Drawing.** Built blocks need their own crisp, hard-edged mesh, not the eased facets. It can still live in the same chunks and come from the same background threads, so the cost stays per chunk.
4. **Walking and collision.** Boxes for built blocks alongside the facet floor; stepping from ground onto a built block has to feel smooth.
5. **Look.** Made by hand, and still clearly not another game's cubes (bevelled edges, as the old machine blocks had). This serves D60, our own look.
6. **Saves.** Built blocks slot into the per-cell block names; the save format already stores any block by name.
