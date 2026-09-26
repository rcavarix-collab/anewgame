# Sun Shadows over Large Terrain

## Ours

We cast sun shadows from a single 2048² depth map that covers a square region of roughly ±48 to ±112 blocks centred on the player. The map is rebuilt over four successive frames while the sun continues to move; the previous and current maps are cross-faded so that the transition is continuous. Sampling uses a fixed 9-tap filter (ASSUMPTION: a regular 3×3 or Poisson kernel of that size). A normal-offset bias is applied to the receiver position before the depth comparison to reduce shadow acne on slopes. The whole arrangement is designed to stay inside a modest fraction of the 2–3 ms world-graphics budget on a GTX 1060-class GPU and to keep temporal stability under the slow, continuous motion of the sun.

## The theory

### Cascaded shadow maps

Cascaded shadow maps (CSMs) divide the view frustum into several depth ranges (cascades). Each cascade receives its own orthographic shadow map whose resolution is concentrated on the nearer geometry. The classic formulation appears in the work of Zhang et al. (Parallel-Split Shadow Maps, 2006) and was popularised for real-time use by Microsoft and NVIDIA technical reports around the same period. The split planes can be chosen uniformly in view space, logarithmically, or by a hybrid scheme that balances the two; the logarithmic choice equalises the projected texel size more evenly along the depth axis. At run time a fragment selects the cascade that contains it, transforms into that cascade’s light space, and performs the depth test. Blending or hysteresis between adjacent cascades reduces visible seams when the camera moves.

Because each cascade is independent, the technique scales to large outdoor scenes: near cascades keep high texel density on the player’s feet while far cascades cover the horizon at lower density. The cost is roughly linear in the number of cascades (extra render targets, extra draw calls or multi-view rendering, and extra texture fetches at sample time).

### Filtering techniques

#### Percentage-Closer Filtering (PCF)

Reeves, Salesin and Cook introduced percentage-closer filtering in 1987. Instead of a single binary depth comparison, several neighbouring texels in the shadow map are tested and the fraction that pass is returned as a soft coverage value. In modern hardware this is performed with a small fixed kernel (typically 3×3 to 5×5) or with a Poisson-disk set of offsets. The result is a soft edge whose width is constant in shadow-map space; projected onto the world it therefore grows with distance from the light, which is physically plausible for a directional sun only in an approximate sense. Hardware support for percentage-closer comparisons (via comparison samplers) makes a modest kernel essentially free on contemporary GPUs.

#### Percentage-Closer Soft Shadows (PCSS)

Fernando’s 2005 paper extends PCF by estimating a contact-hardening penumbra. A blocker search first averages the depths of occluders inside a larger kernel; the average blocker depth, the receiver depth and an assumed light size then determine the width of the subsequent PCF kernel. Near contact the kernel collapses and the shadow edge sharpens; farther away the edge softens. The method therefore approximates the soft-shadow behaviour of an area light while remaining a pure post-process on a conventional shadow map. The extra blocker search roughly doubles the number of texture fetches relative to plain PCF.

#### Variance Shadow Maps (VSM)

Donnelly and Lauritzen (2006) store the first two moments of the depth distribution instead of a single depth value. Chebyshev’s inequality then supplies an upper bound on the probability that a sample is occluded, which is used as the shadow factor. Because the moments are linear, the map can be filtered with ordinary mip-mapping or separable Gaussian blur, yielding soft edges at almost no extra cost at sample time. The principal artefacts are light bleeding (when the depth distribution is multimodal) and the need for a higher-precision render target. Exponential variants and layered variants later mitigated the bleeding problem at additional storage or computation cost.

#### Exponential Shadow Maps (ESM)

Annen et al. (2008) and subsequent refinements replace the depth value with an exponential of depth. The shadow test becomes a simple multiplication and comparison that can again be filtered with ordinary texture filtering. The exponential base controls the fall-off; a larger base reduces light bleeding at the expense of precision. ESMs are cheaper to filter than VSMs because only a single channel is required, yet they still suffer from bleeding on complex depth distributions and require careful choice of the exponential constant for a given depth range.

### Handling a moving sun

A directional light that rotates continuously invalidates every shadow map. Three practical strategies appear in the literature:

1. Full rebuild every frame. Simple but expensive for large maps.
2. Temporal accumulation / multi-frame update. The map is refreshed in tiles or over several frames while the previous result is still used; a short cross-fade hides the transition. This is the approach we already employ.
3. Dual or multi-map temporal supersampling. Two complete maps are maintained and swapped; the older map is faded out while the newer one is faded in. Variants appear in production engines that must support slow-moving lights under a strict frame budget.

When cascades are used, the same temporal scheme can be applied independently to each cascade, or the far cascades can be updated less frequently because their projected error is smaller.

## Comparison

| Method | Cost (relative, GTX 1060 class) | Quality | Complexity | Fit to our single-map temporal design |
|--------|----------------------------------|---------|------------|---------------------------------------|
| Ours (1×2048², 4-frame rebuild + cross-fade, 9-tap PCF, normal offset) | Baseline (well inside 2–3 ms world budget) | Hard shadows with modest soft edge; good stability under slow sun motion; acne controlled by normal offset | Low | Reference |
| Cascaded (3–4 cascades, each ~1024²–2048²) | 2–4× higher render cost; similar sample cost | Higher near-field resolution; possible cascade seams | Medium–high | Orthogonal; can be combined with our temporal update |
| Plain PCF (fixed 9-tap) | Already included | Soft edges of constant map-space width | Low | Exact match to our filtering |
| PCSS | ~2× sample cost of PCF | Contact-hardening penumbrae | Medium | Possible upgrade of the filter stage |
| VSM / ESM | Similar render cost; cheaper or comparable sample cost after blur | Soft edges via ordinary filtering; risk of light bleeding | Medium (extra precision / constants) | Possible replacement of the depth representation |
| Full rebuild every frame | 4× our present update cost | No temporal lag | Low | Unnecessary given slow sun motion |

The table shows that our design already occupies a favourable point on the cost–quality curve for a large but finite terrain region. Cascades would improve texel density near the player at a measurable increase in both memory and fill rate; the filtering upgrades (PCSS, VSM/ESM) trade sample cost or bleeding artefacts for softer, more physically motivated edges.

## What we might try (ordered by value)

1. **Increase the temporal update rate or refine the cross-fade.** Because the sun moves slowly, a four-frame rebuild already keeps lag imperceptible. A two-frame or even continuous partial update (e.g., updating one quadrant per frame) would further reduce any residual lag at almost no extra cost. Highest value: preserves the existing cheap single map while improving temporal quality.

2. **Replace the fixed 9-tap kernel with a modest PCSS blocker search.** Contact hardening improves the visual match to real sunlight without requiring a larger shadow map. The extra fetches remain modest if the blocker kernel is kept small. Second-highest value: quality gain inside the existing map resolution.

3. **Add one or two cascades for the near field only.** A 1024² near cascade covering the first 20–30 blocks, combined with the existing large map for the middle and far distances, would raise texel density under the player’s feet while still fitting the budget if the far map is updated less often. Medium value: addresses the fundamental resolution fall-off of a single map, at the price of extra bookkeeping and potential seams.

4. **Experiment with a variance or exponential representation.** If light bleeding can be controlled (layered VSM or careful ESM constant), ordinary hardware filtering could replace the 9-tap PCF and free texture bandwidth. Lower priority because the current PCF already meets the soft-edge requirement and bleeding is a new artefact to manage.

5. **Full multi-cascade system with independent temporal rates.** Highest theoretical quality for very large terrain, but the cost and complexity exceed the present single-map solution and would need a dedicated quality setting to disable on lower-end hardware. Lowest immediate value under the stated 60 fps / 2–3 ms constraint.

## Sources

- Zhang, F., Sun, H., Xu, L., & Lun, L. K. (2006). Parallel-Split Shadow Maps for Large-Scale Virtual Environments. *Proceedings of the 2006 ACM International Conference on Virtual Reality Continuum and its Applications*.
- Reeves, W. T., Salesin, D. H., & Cook, R. L. (1987). Rendering Antialiased Shadows with Depth Maps. *Computer Graphics (SIGGRAPH ’87 Proceedings)*, 21(4), 283–291.
- Fernando, R. (2005). Percentage-Closer Soft Shadows. *GPU Gems 2*, Chapter 17. Addison-Wesley.
- Donnelly, W., & Lauritzen, A. (2006). Variance Shadow Maps. *Proceedings of the 2006 Symposium on Interactive 3D Graphics and Games*, 161–165.
- Annen, T., Mertens, T., Bekaert, P., Seidel, H.-P., & Kautz, J. (2008). Convolution Shadow Maps. *Rendering Techniques 2007 (Eurographics Symposium on Rendering)*, 51–60. (Exponential formulation refined in subsequent ESM papers by the same group.)
- Engel, W. (various years). Cascaded Shadow Maps. *ShaderX* series and GPU Pro articles (practical real-time presentation of the cascade technique).
- Lauritzen, A., & McCool, M. (2008). Layered Variance Shadow Maps. *Graphics Interface 2008*. (Addresses light-bleeding limitations of basic VSM.)

All sources are peer-reviewed conference papers, standard textbook chapters, or widely recognised industry technical reports. No code, art or proprietary engine material has been reproduced.