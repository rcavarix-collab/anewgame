# Research: accessibility (sight, colour, flashing, motion comfort, hearing)

Proposed research step 3. Held up against DESIGN.md Part XI, which lists what's implemented. **Theory only (D66);** sources through search results **[s]**. W3C's own pages were blocked by this environment's network, so the WCAG figures below come through search summaries, to check at the source. Full citations: `BIBLIOGRAPHY.md` 129–137.

## 1. Who this is for
- **Yuan, Folmer & Harris (2011), *Universal Access in the Information Society*:** players with disabilities hit three kinds of barrier [s]:
  - not **receiving feedback** (seeing or hearing what happened);
  - not **deciding a response** (understanding it);
  - not **giving input** (the controls).
- **The Game Accessibility Guidelines** (a practitioner reference kept since 2012) sort advice by effort (basic, intermediate, advanced) [s, p].

## 2. Colour vision
- **How many:** red–green colour-vision deficiency affects **up to about 8% of men and 0.5% of women of northern European descent.** Pooled worldwide estimates are lower, about 4.5% of men. [s]
- **Simulation: Machado, Oliveira & Fernandes (2009), *IEEE Transactions on Visualization and Computer Graphics*:** a physiologically based model simulating normal vision, anomalous trichromacy (weakened) and dichromacy (missing) in one framework. [s]
  - **D68:** the paper's published conversion matrices are data. We'd **describe** its method and derive our own conversion from the standard cone responses, not copy its tables.
- **Against ours:**
  - Ground materials differ in **both colour and marks** (texture). That's the right defence: never colour alone.
  - The planned 64-pixel art (step 9) should be checked: render each material and each material border through a simulation of the three dichromat kinds offline, and confirm they stay distinct by pattern and lightness.
  - **Stale docs found:** DESIGN.md 11.2 describes a built "Colour vision" setting (`pulse_colours.h`) for the pulse colours. **Both the pulse and that file were removed in M0.6**, so the setting described doesn't exist in walkgrid. The paragraph should be marked historical, which is the owner's call as a doc of record.

## 3. Flashing
- **WCAG 2.3.1** [s; W3C, to read at the source]:
  - nothing may flash **more than three times in any one second**, unless the flashes stay below the general and red flash thresholds;
  - a **general flash** is a pair of opposing changes in relative luminance of 10% or more of the maximum, where the darker state is below 0.80;
  - **saturated red** flashing is treated more strictly;
  - small flashing areas are allowed: the combined area of simultaneous flashes must stay within about 25% of any 10° of the visual field at typical viewing distance.
- The broadcast standards behind this (ITU-R BT.1702 [k]) come from photosensitive epilepsy research.
- **Against ours:** DESIGN.md 11.3's hard rule (no uncontrolled flashing, ever) is stricter than the standard, which is good. **Watch:** lightning (weather, R2) and eclipse transitions must stay under 3 per second and below the thresholds; a lightning flash would count. Give the rule a test when weather lands: a native check that no scripted light effect exceeds 3 luminance swings per second.

## 4. Motion comfort
- **Measuring it: Kennedy, Lane, Berbaum & Lilienthal (1993),** the Simulator Sickness Questionnaire: 16 symptoms in three groups (nausea, oculomotor, disorientation). It's the standard measure. [s]
- **The theory** (sensory conflict, Reason & Brand 1975 [k]): sickness comes from the eyes reporting motion the balance organs don't feel.
- **The provocations:** first-person views; unexpected camera motion (bob, roll, shake); field of view; latency.
  - Evidence on field of view is **mixed**: narrowing it helps some people and not others. That supports a slider over a fixed choice, which DESIGN.md 11.1 already reasons. [s]
- **Against ours:**
  - We have an FOV slider (45–100°), no head bob, and interpolated smooth motion.
  - **The power slide rolls the camera about 12° and dips it** (`SLIDE_LEAN_ROLL` 0.21 rad). Roll is among the stronger provocations; camera roll the player didn't cause is especially so.
  - **Try:** a "camera lean" comfort setting (full, reduced, off), off by default for anyone who turns on reduced motion. The lean is feel, not information, so turning it off loses nothing.

## 5. Hearing
- **Jørgensen's studies** (SOUND_DESIGN.md 7): sound carries feedback and warnings. Players who can't hear it need a visual equivalent.
- **Against ours:**
  - Nothing essential is sound-only yet.
  - When sounds start informing (break-through or unsupported cues, SOUND_DESIGN.md 8.6), each needs a quiet visual twin (a crack line, a dust puff).
  - Captions for world sounds would be words, which D26 discourages; icons that show direction serve better and need no translation.

## 6. What to try

1. **An offline colour-vision check** of every material and border, in the texture tools: simulate the three dichromat kinds and compare lightness and pattern contrast.
2. **A camera-lean comfort setting.** Small.
3. **A native flash-rate test** when weather adds lightning.
4. **Visual twins** for any informative sound.
5. **DESIGN.md 11.2:** mark the colour-vision paragraph historical. Owner's call.

## Sources

All [s] unless marked; full citations in BIBLIOGRAPHY.md 129–137.
- Yuan, Folmer & Harris 2011
- Game Accessibility Guidelines [p]
- Colour-vision prevalence: review and survey sources
- Machado, Oliveira & Fernandes 2009
- WCAG 2.x, SC 2.3.1 (W3C) [s, to read at the source]
- ITU-R BT.1702 [k]
- Kennedy, Lane, Berbaum & Lilienthal 1993
- Reason & Brand 1975 [k]
