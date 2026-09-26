# Research: dithering and blue noise (for the crisp mosaic edges, PLAN_TUESDAY step 7, and the region picks in step 8)

**Sources:**
- [s] Ulichney, "The void-and-cluster method for dither array generation", *Proc. SPIE* 1913, Human Vision, Visual Processing and Digital Display IV, 1993.
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

## To read (legal copies)

- Ulichney 1993; a copy is indexed on CiteSeerX.
