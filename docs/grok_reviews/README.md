# Grok deliveries, appraised

Every claim Grok makes about walkgrid is checked against the code before anything is kept (owner: it hallucinates). Each note says what was true, what wasn't, and what we keep. Grok's own text isn't stored.

| Task | Grade | Keep |
|---|---|---|
| [R1 lighting](R1.md) | Fair | Longer sky-view reach (after PLAN_LOOK G), a colour-curve tuning pass, height-aware haze later |
| [R2 weather](R2.md) | Good skeleton, wrong details | The data-driven design, with corrected light, haze instead of the world-edge fade, new sound recipes, saving |
| [R3 terracing](R3.md) | Fair | Mark edited cells so the existing smoothing can skip them |
| [R4 our own look](R4.md) | Weak to fair | A mark language per material; per-texel mosaic edges |
| [R5 presets](R5.md) | Fair | A preset table, with the reference preset equal to the defaults and no SSAO |
| [R6 caves](R6.md) | Good | Worms plus coarse-lattice density, in a terrain v3 |
| [R7 M2 options](R7.md) | Fair | Four candidates for the owner; building needs W067 first |

**What Grok is good for, so far:** comparing a few approaches on fixed points, and laying out a design skeleton. **Not good for:** anything that depends on how our code actually works (it fills gaps with plausible inventions), and exact numbers.
