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
| [T2 proofread](T2.md) | Fair | Five text edits (waiting for the go-ahead) |
| [T1 translations](code_and_text.md) | Good mechanically | German, Mexican Spanish, French: all keys and slots right; a few word fixes listed |
| [P1 save reader](code_and_text.md) | Good | Works on a real save; goes to tools/ |
| [X2 text tests](code_and_text.md) | Good, one bug | Fix one C++ escape, then all pass |
| [E4–E9 theory](round2_E.md) | Good (E4 weak) | Near shadow cascade later; sky-lit ambient from spherical harmonics; per-octave cloud speeds and a moving threshold; turbidity-style look controls |
| [X1 sky tests](code_and_text.md) | Fair | Disc and moon-phase checks; two checks were wrong (one from Claude's own instruction) |

**What Grok is good for, so far:** comparing a few approaches on fixed points, laying out a design skeleton, translation, and small self-contained code written to an exact spec (the save reader worked first time). **Not good for:** anything that depends on how our code actually works (it fills gaps with plausible inventions), and exact numbers.
