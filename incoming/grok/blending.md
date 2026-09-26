# Blending materials where they meet

## Ours

Walkgrid blends materials per pixel by height. Each material carries a height map. At every shaded fragment the height value of every active material is added to that material’s base weight. The material with the greatest adjusted height wins inside a narrow transition band; outside the band the weights fall to zero. A small world-space noise field is added to the heights so that the winning material is not locked to a perfectly regular contour. The result is a sharp, irregular boundary that follows the local relief of the height maps rather than a smooth linear fade.

This approach needs only the height-map samples already required for the material look-up, plus a handful of comparisons and a noise evaluation. It therefore stays inside the 2–3 ms GPU budget allocated to the world on a six-year-old mid-range card.

## The theory

### Height-based blending

Height-based (or “height-map”) blending treats the transition zone as a competition among local surface elevations. The classic formulation, described in the real-time rendering literature and in procedural-terrain surveys, is:

Let \( w_i \) be the base weight of material \( i \) (normally supplied by a splat map or by a simple distance function). Let \( h_i \) be the value sampled from material \( i \)’s height map. The adjusted height is

\[
H_i = w_i + k \cdot h_i + n(\mathbf{x})
\]

where \( k \) is a scale factor and \( n(\mathbf{x}) \) is a low-amplitude noise field evaluated at world position \( \mathbf{x} \). The final blend weights are obtained by a soft-max or by a simple threshold:

\[
w_i' = \operatorname{saturate}\Bigl(\frac{H_i - H_{\max} + b}{b}\Bigr)
\]

with \( H_{\max} \) the highest adjusted height and \( b \) the width of the narrow band. Materials whose \( H_i \) lie more than \( b \) below the maximum receive zero weight; only the top one or two materials survive. The surviving weights are renormalised.

The method appears in textbooks on real-time graphics and in survey papers on terrain texturing because it produces natural-looking transitions without requiring high-resolution weight textures. The height maps already exist for parallax or displacement, so the extra cost is essentially free.

### Splat maps

A splat map stores explicit blend weights in the channels of one or more textures. Four materials fit into a single RGBA map; more materials need additional maps or a compressed packing. At run time the weights are sampled, optionally raised to a power (see weight sharpening), and used to mix the material properties.

Splat maps give the artist direct control and can encode large-scale distribution patterns that height maps alone cannot express. Their principal drawbacks are memory cost, the need for a separate authoring or painting step, and the tendency of low-resolution maps to produce soft, “painted” edges that look artificial on angular geometry. Published descriptions of multi-texture terrain systems treat splat maps as the baseline against which height-based and procedural methods are measured.

### Weight sharpening

Weight sharpening is a post-process applied to any set of blend weights. The common formula is

\[
w_i' = \frac{w_i^\gamma}{\sum_j w_j^\gamma}
\]

with \( \gamma > 1 \). Raising the weights to a power greater than one suppresses intermediate values and expands the regions where a single material dominates. The technique is discussed in graphics programming texts and in papers on multi-layer texturing as a cheap way to hide the soft fall-off that linear blending produces. It does not introduce new texture samples; it only changes arithmetic. On angular, faceted ground the sharpened weights still follow the original splat contours, so thin linear streaks can remain if the underlying weight field is itself streaky.

### Dithering

Dithering replaces continuous weight interpolation with a stochastic or ordered pattern that forces each pixel to choose a single material (or a small set). Ordered dither matrices or blue-noise textures are sampled and compared against the continuous weight; the comparison yields a binary or sparse selection. The visual effect is a hard but irregular boundary whose statistical density matches the original weights.

Dithering appears in literature on texture filtering, temporal anti-aliasing, and stylised rendering. It is attractive when the number of simultaneous materials must be kept to one or two for performance, and when the artist prefers a discrete “facet” look over a smooth blend. The main cost is an extra texture sample (or a procedural hash) and the need to ensure that the dither pattern does not crawl under camera motion—usually solved by using screen-space or world-space stable noise.

### Avoiding thin streaks

Thin streaks arise when two materials meet along a nearly straight contour and the blend region is only a few pixels wide. Several published strategies mitigate them:

1. Height competition (the method used by walkgrid) automatically widens or breaks the contour wherever the height maps differ.  
2. Noise perturbation of either the weights or the decision threshold adds irregularity without enlarging the transition band.  
3. Multi-scale weight maps store a coarse distribution and a fine detail layer; the fine layer is allowed to override the coarse one only inside a limited distance.  
4. Distance-to-edge metrics measured in world space or in texture space clamp the blend width to a minimum physical size, preventing sub-pixel streaks.  
5. Temporal accumulation or morphological filters can thicken isolated pixels, but they add latency and are rarely used for static terrain.

Surveys of terrain rendering consistently note that pure linear splat blending produces the most visible streaks, while height-based or noise-perturbed methods reduce them at modest extra cost.

## Comparison

| Method              | GPU cost (relative) | Visual quality on angular ground | Implementation complexity | Streak resistance |
|---------------------|---------------------|----------------------------------|---------------------------|-------------------|
| Height-based (ours) | Low (existing samples + noise) | High – follows local relief     | Low                       | High              |
| Splat maps          | Medium (extra textures)        | Medium – soft painted edges     | Medium (authoring needed) | Low–medium        |
| Weight sharpening   | Negligible                     | Medium – still follows splat    | Very low                  | Low               |
| Dithering           | Low–medium (noise or matrix)   | High – discrete, irregular      | Low–medium                | High              |
| Multi-scale splat   | High (two or more maps)        | High if well authored           | High                      | High              |

The table reflects typical figures reported for mid-range GPUs of the last decade. Height-based blending sits at the favourable end of both cost and streak resistance while requiring almost no additional authoring beyond the height maps that materials already possess.

## What we might try

Ordered by expected value under the existing 60 fps / 2–3 ms world budget:

1. **Tighten the noise amplitude and band width.**  
   The current world-noise nudge already breaks regular contours. Measuring the resulting transition width in world units and exposing two scalar constants (band width, noise strength) lets the look be tuned without new textures or shaders. Cost: zero. Value: high.

2. **Optional power sharpening on the final weights.**  
   After the height competition, raise the surviving weights to a small power (\( \gamma \approx 1.5 \)–2). This further suppresses residual soft edges at almost no arithmetic cost and can be toggled by a quality setting. Cost: negligible. Value: high.

3. **Stable world-space dither as a quality option.**  
   When the player enables a “crisp facets” mode, replace the soft band with a blue-noise or hash-based dither that forces a binary choice. The same height values can still bias the probability, preserving the material order. Cost: one extra noise sample; must be gated. Value: medium–high for stylistic consistency.

4. **Coarse splat override for large-scale features.**  
   A low-resolution, artist-painted splat map can supply the base weights \( w_i \) that the height competition then modulates. This restores the ability to force a material into a valley or onto a ridge while still letting height maps control the fine boundary. Cost: one extra texture sample and a modest memory increase; should be optional. Value: medium.

5. **Multi-resolution height detail.**  
   A second, higher-frequency height map sampled only near transitions can break remaining long streaks. Because the extra sample occurs only inside the narrow band, the average cost stays low. Value: lower than the preceding items because the existing noise already addresses most streaks.

Items 1 and 2 can be prototyped immediately; 3–5 require quality-setting infrastructure and should be evaluated only if residual artefacts remain after the first two.

## Sources

- Real-Time Rendering (fourth edition) – chapters on multi-texturing and terrain.  
- Survey papers on procedural and height-field terrain texturing published in computer-graphics journals (ACM Transactions on Graphics, Computer Graphics Forum).  
- Classic descriptions of height-map blending and weight-power sharpening appearing in GPU programming collections and SIGGRAPH course notes on real-time terrain.  
- Literature on ordered and blue-noise dithering for texture selection and stylised rendering (IEEE Transactions on Visualization and Computer Graphics, journal articles on stochastic sampling).  
- Analyses of contour artefacts and mitigation strategies in multi-layer material systems (peer-reviewed papers on terrain rendering pipelines).

All sources are textbooks, peer-reviewed surveys or recognised conference notes; none are forum posts or proprietary engine manuals.