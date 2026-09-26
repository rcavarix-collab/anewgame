# Research: blending textures and hiding repetition (for PLAN_TUESDAY steps 7 and 8)

**Read at the source [r]:** Schuster, Trettner, Schmitz & Kobbelt, "A Three-Level Approach to Texture Mapping and Synthesis on 3D Surfaces", *Proc. ACM Comput. Graph. Interact. Tech.* 3(1), May 2020 (the authors' copy, RWTH Aachen; supplied by the owner), https://doi.org/10.1145/3384542.

The paper is about texturing meshes from a small sample, which is a bigger problem than ours. What matters for us is its account of blending (its section 3.4) and its comparison with Heitz & Neyret. Theory only (D66): what follows is our understanding in our words.

## What the paper establishes

1. **Plain averaging of texture copies loses contrast and ghosts.** Linear blending "reduces the contrast of the result and can produce ghosting artifacts", and it gets worse the more copies are blended (they cite Heitz & Neyret 2018 for this).
2. **Heitz & Neyret's histogram-preserving blending** (confirmed here, secondary):
   - mix the copies in a "gaussianized" colour space;
   - restore the variance the mixing lost;
   - convert back to colour.

   It "overcomes the downsides of linear blending". Two follow-ups improve it, and both belong on our reading list:
   - Deliot & Heitz 2019 (*GPU Zen 2*) reduce the preprocessing;
   - Burley 2019 (*Journal of Computer Graphics Techniques*, an open-access journal) reduces clipping, colour shifts and ghosting.
3. **It suits stochastic textures, not structured ones.** Heitz & Neyret's triplanar version "produces convincing results for stochastic input material", but "for structured or semi-structured textures, blending artifacts occur due to content misalignment". In our terms:
   - it suits sand grain, gravel, moss, loam, dirt, clay and snow;
   - it's risky for sandstone strata, slate layers and, later, bricks and planks.
4. **Max-blending with a grace band.** Between two materials with height maps:
   - each sample gets an effective height **h_e = w × h** (weight times its own height);
   - the tallest wins;
   - only samples within a small grace **Δh** of the tallest are blended (with the histogram-preserving blend), which avoids hard edges.

   A min-blending variant uses w × (1 − h).
5. **Tall structures should win.** For materials whose structures protrude, max-blending "often results in a more natural transition between structures" than any linear mix.

## What this means for walkgrid

**The orange lines (step 7).** Our blend is **additive**: a material's score is its weight plus 0.6 × its height plus noise. Two things follow:
- A material with little weight can still win wherever its height map peaks. The sand's ripple crests, at full height, poke through stone where the sand weight is small; the world noise sometimes tips the balance.
- The paper's rule is **multiplicative** (w × h): as a material's weight falls toward zero, its height stops mattering, so it can't poke through far from its own ground.

Proposed change for step 7:
- score = weight × height, with a floor on height so smooth materials still compete;
- a grace band of about 0.1;
- near the player, the choice decided per texel (the crisp mosaic);
- far away, a narrow smooth blend.

It also shifts part of the art fix (A15): no tall thin features will matter less.

**Repetition (step 8).** Two regimes, following the stochastic-against-structured finding:
- **Near the player (crisp, point-sampled texels):** blending three offset copies would mix texels into colours that aren't in the pixel art. Instead, **pick one copy per region**. Split each block's area into a few cells with irregular, texel-aligned borders, and give each cell its own random offset and rotation (texture bombing without the blend). The crisp look survives, and no copy repeats in step with its neighbours.
- **Further away (smooth sampling):** blend three offset copies **with histogram preservation** for stochastic materials. For structured ones (sandstone, slate) keep offsets **along** the layering only (shift horizontally, never rotate), so strata stay continuous.
- **Cost:** Heitz & Neyret need a small lookup table per texture for the colour-space conversion. Burley 2019 simplifies this; read it before building. Three reads per projection far away, one near.

**Per material** (tag each in its texture data):

| Stochastic: full treatment | Structured: shift along layers only |
|---|---|
| sand, gravel, moss, loam, dirt, clay, snow, dry turf, meadow grass (blades are near-stochastic at 64 px) | sandstone (layered side), slate, stone if its cracks form a pattern |

## Burley 2019: the practical version (read in full)

**Read at the source [r]:** Brent Burley (Walt Disney Animation Studios), "On Histogram-preserving Blending for Randomized Texture Tiling", *Journal of Computer Graphics Techniques* 8(4), 2019, pp. 31–53 (open access; CC BY-ND). Supplied by the owner.

**How the method works** (it also confirms Heitz & Neyret's method at the source's level of detail):
- Tile the plane with a triangle lattice, pick a random tile at each lattice point, and blend the three nearest with barycentric weights.
- Blending independent samples convolves their histograms. Blended Gaussians stay Gaussian with a smaller variance, which a linear stretch around the mean restores.
- So the texture is first made Gaussian, blended, stretched back by 1/W, where W = √(Σ wᵢ²), and finally un-Gaussianized.

**What Burley changes:**
- **1-D lookup tables per channel, built on load.** Heitz & Neyret needed a 3-D optimal-transport step (minutes for a 256² texture). Burley builds each table from the texture's histogram at load time: trivial. The Gaussian's width is σ = 1/6.
- **No clipping:**
  - With 8-bit textures and table lookups, the infinite Gaussian clips: about 0.3% of texels on load, and up to 8.2% of blended pixels where features align.
  - A **truncated Gaussian** plus a **soft-clipping contrast operator** removes this while keeping fixed-point textures and table lookups.
- **Colour shifts:** only a minority of textures show them. Blending in a luminance–chroma space (YCbCr) and preserving the histogram of **luminance only** avoids them, at the cost of some colour contrast.
- **Ghosting:** **raise the three weights to a power γ and renormalise** before blending.
  - At γ ≤ 2 ghosting shows; at γ ≥ 8 the tile structure shows; **γ = 4 kept structure well** in their example.
  - This also helps structured textures, which Heitz & Neyret handled poorly.

**What it means for walkgrid (step 8, far zone):**
- It's buildable within our rules: the lookup tables are built when textures load (64 × 64 textures, so microseconds, with nothing slow at boot), and there's no offline preprocessing.
- **Use luminance-only preservation.** Our materials are mostly one hue family each, and it's the safer choice against colour shifts.
- **Exponentiated weights, γ ≈ 4 to start.** This may let layered materials (sandstone, slate) use it too, as long as offsets stay along the layers; test.
- **Truncated Gaussian with soft clipping.** Our textures are 8-bit.
- **Not near the player.** Our textures are pixel art with 8–16 colours. Blending three copies makes colours that aren't in the palette, and Gaussianizing a few discrete values is coarse. Near the player stays "one copy per region, no blend", as planned.
- **Cost, to measure:**
  - Per projection: three colour reads plus the inverse-table reads.
  - Up to three projections, but the sharpened projection weights mean one usually dominates, so the others can be skipped below a threshold, as the shader already does.
  - Only in the single-material far path.
  - Budget check with F3 before and after.

## To read next (legal copies)

- Deliot & Heitz, "Procedural Stochastic Textures by Tiling and Blending", *GPU Zen 2*, 2019.
- Heitz & Neyret 2018 itself (Inria's open repository HAL, hal-01824773).

## Sources

- [r] Schuster et al. 2020, above, sections 1, 3.4, 3.5, 4 and 6, and references.
- [s] Heitz & Neyret, "High-Performance By-Example Noise using a Histogram-Preserving Blending Operator", *PACM CGIT* 1(2), 2018, https://dl.acm.org/doi/10.1145/3233304 (as described by Schuster et al. and its abstract).
- [r] Burley 2019, above, sections 1–7 and algorithms 1–2.
- [k] Deliot & Heitz 2019, as cited by Schuster et al. and Burley (not yet read).
