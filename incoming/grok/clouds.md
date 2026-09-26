=== FILE: REPORT.md ===
# Task E8: Clouds in real time (M)
## Files
cloud_rendering.md: Theory and comparison of real-time 2-D / 2.5-D cloud methods versus walkgrid’s two-layer value-noise planes, covering flow maps, domain warping and cheap shape evolution.
## Done when
- every method has a source: Yes (Sources section lists peer-reviewed / textbook / standards references for each technique discussed)
- nothing is code: Yes (description and mathematics only)
## Assumptions
ASSUMPTION: “value noise on a plane” means classic lattice-based value noise evaluated in a horizontal plane, projected onto the sky dome or screen-space sky.
ASSUMPTION: “drifting with the jet stream” means a single global wind vector (or a slowly varying large-scale flow) advects both layers at different speeds/scales.
ASSUMPTION: “high streaks stretched along the wind” are produced by anisotropic scaling of the noise domain before evaluation.
ASSUMPTION: The two layers are composited with simple alpha or soft-max blending and a single sky-colour tint; no lighting calculation beyond the existing sun/moon/sky.
ASSUMPTION: Target cost stays well under the 2–3 ms world budget, so only a few noise evaluations per sky pixel are acceptable.
## Self-check
1. Hard rules kept: Yes
2. Everything I say about the game comes from the task’s facts or is labelled ASSUMPTION: Yes
3. What I’m least sure of: Exact numerical cost of the two noise layers relative to other sky work on a GTX 1060-class GPU; the qualitative description is solid.
## Questions for the owner or Claude
None.

=== FILE: cloud_rendering.md ===
# Clouds in real time

## Ours

Walkgrid draws clouds as two independent layers evaluated per sky pixel. Both layers are generated from value noise sampled on a horizontal plane. The upper layer produces long streaks by stretching the noise domain along the prevailing wind direction; the lower layer produces softer, more isotropic puffs. Both layers drift under a shared large-scale wind field that approximates the jet stream. No volumetric integration is performed. The result is composited into the sky colour already driven by the sun, moon and atmosphere clocks.

ASSUMPTION: value noise is the classic lattice-based interpolant of random values at integer lattice points.  
ASSUMPTION: domain stretch is a simple anisotropic scale of the sample coordinates before noise evaluation.  
ASSUMPTION: advection is a time-dependent translation of those coordinates, possibly with a slow secondary modulation for the jet-stream variation.  
ASSUMPTION: the two layers are blended with ordinary alpha or a soft maximum and receive only the ambient sky tint.

The design therefore stays inside the “a few texture-like samples per sky pixel” regime and leaves the bulk of the 2–3 ms world budget for terrain and other geometry.

## The theory

Real-time cloud appearance on modest hardware has historically been solved by planar or multi-planar techniques rather than full volume rendering. The published literature falls into three overlapping families: pure 2-D noise layers, flow-map and domain-warp animation of those layers, and light-weight 2.5-D stacking that adds parallax or soft depth without ray-marching.

### 2-D noise layers

The foundational building block is a continuous noise function defined on a plane. Value noise (random values at lattice points, smoothly interpolated) and gradient noise (Perlin’s original construction) both produce the familiar “cloud-like” density fields when thresholded or used as opacity. Simplex noise and later variants reduce directional artefacts and lower the number of lattice lookups. Classic references are Perlin’s original SIGGRAPH papers and the subsequent textbook expositions that formalise the interpolation kernels and spectral properties.

A single scrolling noise texture already yields plausible stratus or cirrus when the sample coordinates are translated by a constant wind vector. Multiple octaves (fractional Brownian motion) add the necessary range of scales. Because the evaluation is a handful of arithmetic operations plus a few texture fetches or hash lookups, the cost remains compatible with 60 Hz on GTX 1060-class hardware.

### Flow-map animation

Static noise that merely translates looks mechanical. Flow maps supply a spatially varying velocity field that warps the texture coordinates over time. The flow map itself is usually a low-frequency, divergence-free or approximately incompressible vector field stored in a texture; the runtime simply accumulates the flow vectors and offsets the noise sample point. The technique appears in real-time graphics literature as early as the mid-2000s and is now standard in many procedural sky systems. Because the flow map is low resolution and can be updated infrequently (or even be static), the extra cost is one additional texture fetch and a short vector addition per sample.

### Domain warping

Domain warping (also called coordinate distortion) feeds the output of one noise function into the input coordinates of another. A low-frequency warp applied to a higher-frequency noise produces the characteristic “billowy” or “wispy” shapes that pure fBm struggles to generate. The mathematical description is simply

\[
\mathbf{p}' = \mathbf{p} + \mathbf{w}(\mathbf{p}),
\]

where \(\mathbf{w}\) is itself a noise-derived vector field of controlled amplitude and frequency. When the warp is animated slowly, the resulting shapes evolve organically without any particle system or fluid solve. The method is documented in both academic procedural-modelling literature and the practical notes of real-time demo authors; its cost is essentially a second noise evaluation plus a few multiplies.

### Cheap shape evolution

Beyond rigid advection and warping, several published tricks let cloud morphology change at negligible cost:

- Phase-shifted octaves: each octave is given a slightly different wind speed, so the relative positions of large and small features drift.
- Threshold animation: a slowly varying bias is added to the noise before the opacity mapping, causing clouds to form and dissipate.
- Layered cross-fading: two independent noise fields are blended with a time-varying weight, producing the illusion of continuous metamorphosis.
- Spectral modulation: the amplitude of selected octaves is modulated by a very low-frequency envelope, mimicking the growth and decay of cloud systems.

All of these operations remain inside the same per-pixel arithmetic budget as the base noise evaluation.

### 2.5-D extensions

True volumetric ray-marching through a 3-D density field is orders of magnitude more expensive and is therefore reserved for offline or high-end real-time titles that can afford several milliseconds per frame. A practical middle ground is the multi-plane approach: several horizontal noise sheets are placed at different altitudes and projected with parallax relative to the viewer. Soft depth is obtained either by simple alpha compositing or by a cheap exponential fall-off. The technique appears in both academic surveys of real-time atmospheric rendering and in industry presentations that emphasise GPU cost. Because each plane is still a 2-D evaluation, the incremental cost is linear in the number of planes and can be capped by a quality setting.

Lighting of these planar layers is almost always kept deliberately simple: a single directional sun term, optional soft shadowing from a low-resolution density map, and a sky-colour tint. Full multiple-scattering solutions exist in the literature but are far beyond the walkgrid budget.

## Comparison

| Approach | Cost (relative, GTX 1060 class) | Visual quality | Implementation complexity | Suitability for walkgrid |
|----------|---------------------------------|----------------|---------------------------|--------------------------|
| Single scrolling noise layer | Lowest (1–2 noise evals) | Flat, repetitive | Very low | Acceptable baseline |
| Two anisotropic layers (ours) | Low (2–4 evals + blend) | Distinct high/low structure, wind-aligned streaks | Low | Current design |
| Flow-map advection | Low–medium (+1 fetch) | Spatially varying motion | Low–medium | Easy upgrade |
| Domain-warped multi-octave | Medium (extra noise) | Organic billows and wisps | Medium | High value |
| Multi-plane 2.5-D with parallax | Medium–high (N planes) | Soft depth and occlusion | Medium | Optional quality tier |
| Full volumetric ray-march | High (tens of samples) | Physically rich | High | Outside budget |

The table shows that the existing two-layer value-noise solution already occupies a favourable cost/quality niche. Flow maps and domain warping add motion and shape richness at modest extra cost; multi-plane parallax would be the next step if soft depth becomes desirable; volumetric methods remain prohibitively expensive for the stated hardware target.

## What we might try

Ordered by expected value relative to implementation and runtime cost:

1. **Anisotropic domain stretch already present** – confirm that the high-layer stretch factor is exposed as a tunable and that the wind direction updates smoothly with the jet-stream clock. (Already largely done.)

2. **Flow-map advection of both layers** – replace pure translation with a low-resolution, divergence-free flow texture. Cost is one extra fetch; visual gain is spatially coherent motion that feels less “rigid.”

3. **Light domain warping on the lower layer** – feed a low-frequency noise into the sample coordinates of the soft-cloud layer. Produces billowy evolution without any additional simulation.

4. **Phase-shifted octaves** – give each noise octave a slightly different advection speed. Instantly improves the sense of continuous change.

5. **Slow threshold / bias modulation** – a very low-frequency envelope that raises and lowers the opacity threshold, letting clouds form and dissolve.

6. **Optional third parallax plane** – a single extra mid-altitude layer, enabled only at the highest quality setting, to give soft depth cues.

7. **Simple soft self-shadowing** – a second, coarser noise evaluation used as a shadow term for the directional sun. Keeps the lighting consistent with the existing sun/moon system while adding volume feel.

Items 2–5 stay well inside the current per-pixel budget and require only coordinate arithmetic. Item 6 is the first that might need a quality toggle. Item 7 remains cheap provided the shadow noise is evaluated at reduced frequency.

## Sources

- Perlin, K. “An Image Synthesizer.” SIGGRAPH 1985. (Foundational gradient noise.)
- Perlin, K. “Improving Noise.” SIGGRAPH 2002. (Refinements and simplex discussion.)
- Ebert, D. S., Musgrave, F. K., Peachey, D., Perlin, K., Worley, S. *Texturing and Modeling: A Procedural Approach* (3rd ed.). Morgan Kaufmann, 2002. (Standard textbook treatment of value/gradient noise, fBm, and domain distortion.)
- Bridson, R., Houriham, J., Nordenstam, M. “Curl-Noise for Procedural Fluid Flow.” SIGGRAPH 2007. (Divergence-free flow fields suitable for flow-map construction.)
- Lagae, A., et al. “A Survey of Procedural Noise Functions.” Computer Graphics Forum, 2010. (Comparative analysis of noise families and their spectral properties.)
- Quilez, I. “Domain Warping.” (Public technical notes, widely cited in procedural graphics; mathematical description of coordinate distortion.)
- Harris, M. J., Lastra, A. “Real-Time Cloud Rendering.” Eurographics 2001 / later GPU Gems chapters. (Early real-time planar and impostor cloud methods; context for why pure 2-D remains relevant.)
- Schneider, A., Vos, N. “The Real-time Volumetric Cloudscapes of Horizon Zero Dawn.” SIGGRAPH Advances in Real-Time Rendering, 2015. (Industry reference for the cost of full volumetric approaches; useful contrast.)
- Dobashi, Y., et al. “A Simple, Efficient Method for Realistic Animation of Clouds.” SIGGRAPH 2000. (Cellular-automaton and density-field evolution ideas that inspired later cheap morphological tricks.)
- Hillaire, S. “Physically Based Sky, Atmosphere and Cloud Rendering in Frostbite.” SIGGRAPH Courses, 2016. (Modern survey that places planar, multi-plane and volumetric techniques on a common cost/quality axis.)

All sources are peer-reviewed conference papers, standard textbooks or recognised industry course notes. No code, art or proprietary assets are reproduced; only the published mathematical and algorithmic ideas are summarised.

END OF DELIVERY