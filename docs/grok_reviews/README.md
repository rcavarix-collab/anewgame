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
| [E14–E16 audio, saves, pacing](round2_E_part2.md) | Good | Durable saves (W070); footsteps on stride or beat (owner's call); modal dig and place sounds; event-population rain |
| [X1 sky tests](code_and_text.md) | Fair | Disc and moon-phase checks; two checks were wrong (one from Claude's own instruction) |
| [E1, E10–E13 surfaces, noise, far terrain, visibility, jobs](round2_E_part4.md) | Fair to good | The far-terrain ring from `TerrainHeight`; terrain-noise changes bundled into the next generator version; cellular noise for the art; job-wait timing in F3; tests for stale results and for openings never falsely closed. E1 wrongly assumed a height field (our mesher is already a surface net) |
| [E17, E18, E2, E3 text, walking, projection, repetition](round2_E_part3.md) | Fair to good | E18's walking test course; UAX #11 for W068; Wang tiles as a note. Two wrong citations in E2 and E3; E17's bundled fonts and shaping library go against our rules |

**Group E is complete** (E1–E18). The helpers' theory is usually right; their citations and their guesses about our code are the weak points. Where the facts were thin, they assumed a simpler walkgrid than the real one.

**What Grok is good for, so far:** comparing a few approaches on fixed points, laying out a design skeleton, translation, and small self-contained code written to an exact spec (the save reader worked first time). **Not good for:** anything that depends on how our code actually works (it fills gaps with plausible inventions), and exact numbers.
