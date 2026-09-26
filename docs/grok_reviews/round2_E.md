# Round 2, E group (theory behind our systems): appraisal

Delivered 2026-09-26 by the helpers (Grok, then ChatGPT once Grok's free limit ran out; the files don't say which wrote which). Six tasks: E4 blending, E5 shadows, E6 ambient, E7 sky, E8 clouds, E9 fog. Kept as delivered in `incoming/grok/`.

Checked against the code and against the papers we've read in full: Schuster 2020, Burley 2019, Gibson 1998, Hošek & Wilkie 2012, Vlachos 2010.

**Overall grade: good.**
- All six kept to theory: no code, and assumptions were labelled.
- Most sources are real, well-known works with correct details.
- Weaknesses: E4's sources are vague, several items misattribute or overstate, nobody gave links, and each note missed the one key finding our own reading turned up.

## Per task

| Task | Grade | Right | Wrong or missing | Keep |
|---|---|---|---|---|
| E5 shadows (`shadows.md`) | Good | Accurate account of cascades, PCF, PCSS, VSM, ESM; real sources (Reeves 1987, Donnelly & Lauritzen 2006, Lauritzen & McCool 2008, Zhang 2006); fair verdict that our single map with crossfade is a good cost point | PCSS is cited as *GPU Gems 2* ch. 17; it's Fernando's 2005 SIGGRAPH sketch / NVIDIA white paper. ESM is cited as "Convolution Shadow Maps" (EGSR 2007); ESM is a separate 2008 paper by the same group. Its "9-tap, assumption" is right: 3 × 3 hardware-compared taps. | **A near cascade** (about 1024², the first 20–30 blocks), to pair with far terrain later, since our map spends 0.11 blocks per texel. PCSS parked: a sun's penumbra is small. |
| E6 ambient (`ambient.md`) | Good | Real sources (Zhukov 1998, Landis 2002, Mittring 2007, Bavoil & Sainz 2008, McGuire 2011–12); explains why SSAO dimples faceted ground (depth discontinuities read as contact); bent normals agree with our overview | Assumes our sky view is stored as 8 directional values (labelled); it's one number per corner. Its first suggestion (chunk-seam continuity) doesn't apply: our copy margins cover the bake's reach (2 cells for openness, 10 blocks for sky view). Kontkanen & Laine 2005 is "ambient occlusion fields", not bent cones. **Missed the per-triangle interpolation wedges** (our VERTEX_DARKENING note). | **Sky-lit ambient from a few spherical-harmonic numbers per frame** (4 or 9, computed on the processor from the sky), in place of our two-colour hemisphere: smoother, sky-coloured ambient for a handful of operations. A good candidate for batch 4. |
| E8 clouds (`clouds.md`) | Good | Real sources (Perlin 1985 and 2002, Ebert et al. textbook, Bridson et al. 2007 curl noise, Harris & Lastra 2001, Dobashi 2000, Schneider 2015, Hillaire 2016); correct cost ranking | Its flow maps "accumulate the flow vectors": naive accumulation smears, which is exactly what Vlachos's two phases prevent. It cites a domain-warping blog (a practitioner source, which D66 doesn't count as reliable). Doesn't mention Vlachos. | **Two cheap extras for step 5**, alongside Vlachos's phases: give each noise octave its own drift speed, and slowly move the density threshold so clouds form and dissolve. |
| E9 fog (`fog.md`) | Good | Correct transmittance and height-fog maths; a sound list of ways to hide the world's edge; its assumptions about our fade are nearly right (ours is a smooth step, not linear) | Suggests making the haze fully opaque at the edge. At our short default render distance that would fog nearby ground: the owner's "white distance" complaint (D64). "*GPU Gems 3*, Aerial Perspective" is doubtful; the known chapter is O'Neil, "Accurate Atmospheric Scattering", *GPU Gems 2* ch. 16. | Height-dependent haze (already R1's item 3), and a distant shell (our far-terrain idea). Nothing new. |
| E7 sky (`sky_models.md`) | Fair to good | Correct physics (Rayleigh λ⁻⁴ and phase 1 + cos²θ; Mie forward peak); real sources, including Hošek & Wilkie's 2013 solar-radiance follow-up | Says Hošek–Wilkie gives "better twilight colours"; the paper we read says after-sunset isn't modelled. "No tables required at run time" contradicts its own "larger coefficient set". Suggests "full Preetham for the whole day–night cycle"; Preetham is invalid with the sun below the horizon. | **"Clarity / haze" (turbidity), "horizon warmth" and "sun size" as the look controls**, matching our turbidity decision. An analytic glow near the sun only is worth a trial. |
| E4 blending (`blending.md`) | Weak | Describes our blend correctly (additive: weight + height + noise); explains dithering and weight sharpening | **No real citations** ("survey papers…", "classic descriptions"): fails D66. Rates our additive method "high streak resistance", when additive scoring is what made the orange lines. Misses the multiplicative max-blend (Schuster 2020) and histogram-preserving blending (Heitz & Neyret; Burley). | Nothing beyond what we already have. |

## What changes in our plans

- **Step 5 (clouds):** add per-octave drift speeds and a slowly moving threshold to the Vlachos phases.
- **Batch 4 / R1:** sky-lit ambient from a few spherical-harmonic numbers per frame, as an alternative to the two-colour hemisphere.
- **Far terrain (idea):** pair it with a near shadow cascade.

## A question for the owner (rule D66)

Published sky models come with fitted coefficient tables: numbers in the papers themselves (Preetham's appendix; Hošek & Wilkie's data). Our rule says never copy anyone's data. Are published scientific constants (physical facts in a paper) allowed, like using the speed of light? Or should we keep fitting our own gradient to references? Until decided, we don't use them.

**Answered (D68):** tables aren't copied. We describe what the data shows (its trends, shapes, ranges) and match that with our own numbers.

## What this round teaches about helpers

The theory tasks work well: sound summaries with mostly real sources. They still need our own reading, because every note missed the one specific finding that mattered:
- the multiplicative blend;
- Vlachos's two phases;
- the triangle wedges;
- the twilight limit.

Keep asking for full citations with links; E4 shows what happens without them.
