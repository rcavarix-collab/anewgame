# Ambient Light and Occlusion

## Ours

Walkgrid uses a fully baked ambient solution computed at meshing time together with a simple runtime hemisphere ambient term. At each mesh corner (vertex) two pieces of data are stored:

- A scalar (or small-vector) openness factor that approximates how much of the surrounding solid angle is unoccluded by nearby geometry.
- An 8-direction sky-view representation that records approximate visibility toward the sky in eight fixed azimuthal sectors.

These values are generated while the terrain mesh is built on background threads. Because the world is faceted and angular, the bake can examine the local cell neighbourhood and the resulting triangle connectivity without needing expensive runtime ray marches.

At runtime the openness and sky-view factors modulate a hemisphere ambient term. The hemisphere is oriented upward and represents the average contribution of the sky (and, to a lesser extent, the distant ground). The optional screen-space ambient occlusion (SSAO) path exists but is disabled by default; when enabled it produced visible dimpling on the lumpy, faceted ground surfaces and therefore failed the visual-quality bar for the default configuration.

The design deliberately keeps the per-frame cost of ambient lighting extremely low: a few texture fetches or interpolants and a handful of arithmetic operations. All heavy geometric work occurs offline (relative to the frame) during meshing. This fits the overall budget in which the world itself is allowed roughly 2–3 ms of GPU time per frame on GTX-1060-class hardware while still targeting 60 fps.

ASSUMPTION: openness is primarily a geometric visibility factor; any bent-normal information is either absent or already folded into the same data.

ASSUMPTION: the 8-direction sky view is a compact set of directional visibility scalars rather than a full environment-map sample.

## The theory

Ambient occlusion (AO) approximates the fraction of the hemisphere above a surface point that is occluded by nearby geometry. It darkens creases, contact points and undersides without requiring a full global-illumination solution. Ambient lighting then multiplies or adds a low-frequency illumination term (sky, constant ambient, or low-order spherical harmonics) that has already been attenuated by the occlusion factor.

Three families of methods are especially relevant: baked (precomputed) AO, horizon-based and bent-normal techniques, and screen-space variants.

### Baked ambient occlusion

In a baked approach the occlusion factor is computed once when the geometry is created or imported and is stored with the mesh (per-vertex, per-texel, or in a light-map). Classic offline pipelines cast many rays or use rasterisation-based visibility from each sample point, then store a scalar AO value or a small set of directional coefficients. Because the computation is offline, high sample counts and sophisticated filtering are affordable.

For dynamic or procedurally generated worlds the bake is performed at mesh-construction time rather than in a separate offline light-map pass. The same geometric neighbourhood that is already being examined to emit triangles can be used to estimate openness. Storage is typically a few bytes per vertex; runtime cost is essentially free (the value is simply interpolated by the rasteriser).

The principal limitation is that the bake cannot react to moving objects or to large-scale changes that occur after meshing. For a static or slowly changing terrain this is acceptable. Artefacts appear mainly as discontinuities at chunk boundaries if the neighbourhood used for the bake is smaller than the visible radius of occlusion, or as over-darkening if the bake radius is chosen too large relative to the geometric detail.

Key references for the underlying mathematics and early practical systems include the original ambient-occlusion formulation of Zhukov et al. (1998) and the subsequent popularisation in film and games through ray-traced and rasterised precomputation (Landis, 2002; Christensen, 2003).

### Horizon-based ambient occlusion and bent normals

Horizon-based methods replace the full hemispherical integral with a set of horizon-angle measurements. From a surface point one walks outward along a number of directions in the tangent plane, recording the elevation angle of the highest occluder. The AO factor is then derived from the solid angle under those horizon angles. The same horizon data can be used to compute a bent normal: the average unoccluded direction, which points away from nearby blockers and can be used to look up a distant lighting environment more accurately than the geometric normal.

Because horizon search is essentially a 2-D problem in the tangent plane, it is cheaper than a full 3-D ray march for a given sample count. The method was popularised for real-time use by Bavoil & Sainz (2008) in the form of Horizon-Based Ambient Occlusion (HBAO), later refined into HBAO+ and related variants. Bent-normal ideas appear in earlier offline work and were brought into real-time pipelines in the same period (e.g. the “bent cone” representations of Kontkanen & Laine, 2005, and subsequent game-engine practice).

Artefacts typical of horizon methods include:

- undersampling of thin features or of occlusion that arrives from directions not aligned with the chosen search rays;
- halo or darkening artefacts when the horizon search radius is large relative to depth-buffer precision;
- loss of fine contact shadows if the angular step size is too coarse.

When the horizon data are baked rather than computed in screen space, the same directional information can be stored compactly (for example as eight azimuthal samples) and interpolated across the mesh. This is conceptually close to the 8-direction sky-view data used in walkgrid.

### Screen-space ambient occlusion and its variants

Screen-space AO (SSAO) estimates occlusion from the depth (and optionally normal) buffer of the current frame. For each pixel a set of sample points is distributed in a sphere or hemisphere around the surface point; the fraction of those samples that fail a depth test is taken as the occlusion factor. The original Crytek formulation (Mittring, 2007) used a simple spherical distribution and a handful of samples. Later variants improved quality and reduced noise:

- Horizon-Based Ambient Occlusion (HBAO) performed in screen space (Bavoil & Sainz, 2008) replaces random sphere samples with horizon searches in screen-space directions.
- Alchemy AO and Scalable Ambient Obscurance (McGuire et al., 2011–2012) reformulate the fall-off and use smarter sampling patterns.
- Multi-scale and temporal variants accumulate results across frames or across different sample radii to reduce noise while preserving both contact and larger-scale occlusion.

Because the input is only the current depth buffer, SSAO can react to every dynamic object and requires no precomputation. The cost is a full-screen pass whose expense scales with sample count, resolution and the complexity of the bilateral filter used to remove noise. On GTX-1060-class hardware a high-quality SSAO pass can easily consume 1–2 ms, which is a significant fraction of the 2–3 ms world budget.

Characteristic artefacts of screen-space methods include:

- missing occlusion for geometry that is outside the current view or occluded in depth (the classic “screen-space” limitation);
- haloing around objects when the depth discontinuity is large;
- noise that requires aggressive bilateral filtering, which in turn can blur fine detail or produce temporal instability;
- incorrect darkening on large continuous surfaces that happen to have high-frequency depth variation (the dimpling observed on walkgrid’s lumpy ground is a typical example of this class of artefact).

When the geometry is itself highly faceted, the depth buffer contains many small discontinuities; SSAO interprets those discontinuities as contact shadows and therefore over-darkens the facets. This matches the observed behaviour that led to disabling the optional SSAO path.

### Ambient lighting models that accompany occlusion

Once an occlusion factor (or a set of directional visibility factors) is available, it is multiplied into an ambient term. The simplest ambient term is a constant colour. A modest improvement is a hemisphere or “sky” ambient whose intensity varies with the upward component of the normal (or of the bent normal). More elaborate models store low-order spherical harmonics of the distant lighting or a small set of prefiltered environment-map samples. Because walkgrid already moves the sun, moon and sky on independent clocks, the hemisphere ambient can be tinted by the current sky colour without additional geometric work.

The combination of a baked directional visibility representation with a dynamic sky colour yields a form of cheap, time-of-day-aware ambient lighting that still respects local occlusion.

## Comparison

| Method                        | Runtime cost (GTX 1060 class) | Quality (static terrain) | Quality (dynamic objects) | Complexity (implementation & data) | Artefacts typical of the method |
|-------------------------------|-------------------------------|---------------------------|----------------------------|------------------------------------|---------------------------------|
| Ours (baked openness + 8-dir sky view + hemisphere) | Near zero (interpolants only) | Good local contact & sky visibility | None (static bake)        | Low–medium (bake at mesh time)    | Chunk-boundary seams if neighbourhood too small; no reaction to movers |
| Classic offline baked AO (ray-traced / high-sample) | Near zero                     | Excellent                 | None                      | High (offline pipeline)           | Same as above; possible over-darkening if radius large |
| Horizon-based / bent-normal (baked) | Near zero                     | Very good directional     | None                      | Medium (horizon search at bake)   | Undersampled thin features; directional quantisation |
| Screen-space AO (Crytek-style) | 0.5–1.5 ms                    | Moderate                  | Good                      | Low–medium                        | Noise, halos, missing off-screen geometry, dimpling on faceted surfaces |
| Screen-space HBAO / multi-scale | 1–2+ ms                       | Good                      | Good                      | Medium–high                       | Same screen-space limits; higher cost |
| Full real-time ray-traced AO   | Several ms (even with denoising) | Excellent               | Excellent                 | High                              | Noise requiring temporal accumulation; still expensive on 6-year-old hardware |

The table emphasises that any technique requiring a full-screen pass or many samples per pixel quickly consumes a large fraction of the world graphics budget. Baked methods keep that cost near zero at the price of ignoring dynamic occluders.

## What we might try

Ordered by estimated value relative to implementation effort and performance risk:

1. **Tighten the bake neighbourhood and add a simple cross-chunk continuity pass.**  
   If openness or sky-view discontinuities appear at chunk borders, extend the geometric neighbourhood used during meshing by one cell and, optionally, average or min-filter the boundary values with the neighbouring chunk. Cost is paid only at mesh build time; runtime remains free. Highest value because it directly improves the existing data without new runtime systems.

2. **Store a bent normal (or bent cone) alongside or instead of pure openness.**  
   The same horizon or visibility samples already gathered for the 8-direction sky view can yield an average unoccluded direction. Looking up the sky colour with the bent direction rather than the geometric normal reduces the “flat” look of pure scalar AO and costs only a few extra bytes per vertex plus a normalise at runtime. Medium effort, clear visual gain on angular geometry.

3. **Replace the optional SSAO with a much cheaper, lower-radius contact-shadow pass that is aware of the faceted surface.**  
   A minimal screen-space pass that only darkens immediate contact (very small radius, few samples, strong normal weighting) can restore dynamic contact shadows under moving objects while avoiding the large-scale dimpling that disabled the previous SSAO. Keep it behind a quality setting so it can be turned off on the lowest machines. Higher runtime cost than pure bake, but still far cheaper than full HBAO.

4. **Add a low-order spherical-harmonic ambient term driven by the current sky.**  
   Instead of a pure hemisphere, project the sky (and sun/moon contribution) into 4 or 9 SH coefficients once per frame on the CPU and evaluate them with the (possibly bent) normal. The 8-direction sky-view factors can still modulate the result. Gives smoother directional ambient at modest extra arithmetic; still compatible with the existing bake.

5. **Temporal accumulation of a very low-sample SSAO.**  
   If dynamic occlusion becomes important, a 4-sample SSAO accumulated over many frames with careful velocity weighting can approach the quality of a more expensive single-frame pass. Requires a reliable motion vector and history buffer; higher implementation complexity and still incurs a non-zero per-frame cost. Lowest priority while the world remains largely static.

None of the suggestions require abandoning the baked openness and sky-view data; they either improve that data or add an optional, scalable layer on top.

## Sources

- Zhukov, S., Iones, A. & Kronin, G. (1998). An ambient light illumination model. *Rendering Techniques ’98* (Eurographics Workshop on Rendering).
- Landis, H. (2002). Production-ready global illumination. *SIGGRAPH 2002 Course Notes* (RenderMan).
- Christensen, P. H. (2003). Global illumination and all that. *SIGGRAPH 2003 Course Notes*.
- Mittring, M. (2007). Finding next gen – CryEngine 2. *Advances in Real-Time Rendering in 3D Graphics and Games* (SIGGRAPH 2007 Course).
- Bavoil, L. & Sainz, M. (2008). Image-space horizon-based ambient occlusion. *ShaderX7* / NVIDIA technical report.
- Kontkanen, J. & Laine, S. (2005). Ambient occlusion fields. *Proceedings of the 2005 Symposium on Interactive 3D Graphics and Games*.
- McGuire, M., Osman, B., Bukowski, M. & Stenson, C. (2011). The Alchemy screen-space ambient obscurance algorithm. *Proceedings of the ACM SIGGRAPH Symposium on High Performance Graphics*.
- McGuire, M. (2012). Scalable ambient obscurance. *Journal of Graphics Tools* / HPG follow-up material.
- Standard reference texts for the underlying radiometry and spherical integrals: Pharr, Jakob & Humphreys, *Physically Based Rendering* (multiple editions); Akenine-Möller, Haines & Hoffman, *Real-Time Rendering* (4th ed., chapters on ambient occlusion and global illumination approximations).

All methods described above are taken from the cited publications or from the standard textbook treatments; no implementation code has been reproduced.