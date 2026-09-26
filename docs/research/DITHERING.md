# Research: dithering and blue noise (for the crisp mosaic edges, PLAN_TUESDAY step 7, and the region picks in step 8)

**Sources:**
- [r] Ulichney (Digital Equipment Corporation), "The void-and-cluster method for dither array generation", *Proc. SPIE* 1913, Human Vision, Visual Processing and Digital Display IV, 1993, pp. 332–343. Read in full from a scanned copy supplied by the owner.
- [k] Bayer, "An optimum method for two-level rendition of continuous-tone pictures", 1973 (ordered dithering).

## What the theory says

- **Dithering** turns a smooth value into a pattern of on/off choices so that, from a distance, their average matches the value.
- **Ordered (Bayer) patterns** are regular: cheap, but they show a visible grid texture.
- **Blue-noise patterns** place choices so that no two nearby ones cluster: no low-frequency blotches, no grid. The eye reads them as smooth.
- **Ulichney's void-and-cluster method** builds such a pattern by repeatedly moving a point from the tightest cluster to the largest void, giving an even, isotropic arrangement.
- **It's a threshold array:** build it once, store it as a small tiling texture, and compare against it per pixel. That's cheap at run time.

## What it means for walkgrid

- **Step 7, the crisp mosaic edge.** Near the player, the border between two materials is decided per texel. To make that border neither a straight line nor salt-and-pepper speckle, compare the blend's winning margin against a **blue-noise threshold per texel** (a small tiling array addressed by the texel's world position, so it's stable as the camera moves). The result is a ragged but even border made of whole texels: what a pixel artist would draw by hand.
- **Step 8, near regions.** The irregular, texel-aligned regions that each get their own texture offset can use the same blue-noise array to jitter their borders, so neighbouring blocks never share a straight seam.
- **Cost:** one small texture read per pixel; the array (for example 64 × 64) is built once, offline or at load.
- **Our own:** we build the array with our own implementation of the published method (a tool in `tools/`), never taking someone else's array or code.

## From the paper itself (read in full)

**Why not Bayer's patterns:** the classic ordered-dither patterns are homogeneous, but they "suffer from rigid regular structures that introduce an artificial processed look". That's exactly the grid look we don't want at material edges.

**The method, in words:**
- **A threshold array M × N, periodic in both directions,** so it tiles the plane seamlessly. Any size works; arrays **as small as 32 × 32** gave very high-quality patterns, free of directional artefacts.
- **Voids and clusters are found with a Gaussian filter,** wrapping around the array's edges. The best filter width found was **σ = 1.5 pixels**.
- **Start** from any sparse pattern (random is fine). Repeatedly move the point with the tightest cluster into the largest void, until removing a point would itself create the largest void. That gives an even starting pattern.
- **Then rank every cell, in three phases:**
  - remove points one at a time from the tightest cluster, giving the ranks below the start;
  - add points one at a time into the largest void, up to half full;
  - past half, the roles reverse, filling the tightest clusters of empty cells.

  Every cell gets a unique rank from 0 to MN − 1.
- **To use it:** normalise the ranks to the input range. For a two-way choice, output 1 wherever the input is greater than the cell's normalised rank. The average of the output always equals the input.
- **Compared with error diffusion,** it has no directional artefacts, needs no neighbouring pixels (a pure per-pixel lookup), and is just as homogeneous.

**For walkgrid, confirmed:**
- **A 32 × 32 or 64 × 64 array, σ = 1.5,** built by our own tool from these steps (the paper describes a method; the array we build is ours). Stored as a tiny tiling texture and addressed by texel world position, so it's stable as the camera moves.
- **Step 7:** at a material border near the player, compare the winning margin (or the blend weight) against the array value, per texel. The border becomes even and grain-free, and its average position follows the true blend.
- The periodic, wrap-around property is what lets it tile across blocks without seams.
