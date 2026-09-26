# Research: terracing and constrained surface nets (for W055, D62)

**Read at the source [r]:** Sarah F. F. Gibson, "Constrained Elastic Surface Nets: Generating Smooth Surfaces from Binary Segmented Data", MERL (Mitsubishi Electric Research Lab), in *Medical Image Computing and Computer-Assisted Intervention* (MICCAI) 1998, pp. 888–898. A legal copy was supplied by the owner. Theory only (D66): our understanding, in our words.

## What the paper establishes

- **Its problem is ours.** Binary data (each cell in or out) gives "terracing artifacts, where sloped surfaces appear as flat terraces separated by sharp elevation changes", worst when the slope is gentle. The paper's data is medical scans; ours is one-block cells. Same cause.
- **Local smoothing can't remove terraces.** Filtering "reduces the slopes of terraces", but "unless the filter extent is significantly wider than the terraces, terracing artifacts are not removed". A gentle slope has wide terraces, so any local fix fails there or erases real detail. The paper shows terraces surviving even a 19 × 19 × 19 filter.
- **The method, in words:**
  - put one node in each "surface cube" (a cube of 8 neighbouring cells that aren't all the same);
  - link each node to its neighbours in adjacent surface cubes (up to 6);
  - repeatedly move each node toward the point midway between its linked neighbours, which lowers the sum of squared link lengths;
  - **but never let a node leave its own cube.**

  Without the constraint the net would shrink to a point; with it, the surface stays faithful to the cells, keeps cracks and thin protrusions, and smooths terraces away.
- **How many iterations:** the examples use 10–30. The net's energy "decreases quickly to a minimum and then increases slowly" to a slightly higher level. At the minimum the surface is smoothest; corners sharpen afterwards.
- **Ambiguous cubes** (diagonal cells touching only at a corner) are neither bridged nor separated: the surface is pinched at that node, so no arbitrary choice is made.
- **Other energies are possible:** one that reduces curvature gives smoother surfaces with softer corners.

## What this means for walkgrid

**Our mesher is already a surface net with one step.** It places a corner where cells meet (one node per surface cube, in effect) and eases it halfway toward its neighbours: a single relaxation. That's why terraces remain on gentle slopes. The paper explains exactly why one local step can't remove wide terraces, and why our `FacetShape::terrace` pass, which is also local, only softens them.

**The constraint answers the worry about edits.** Each node stays inside its own cube, so the drawn surface never leaves its cells. Picking, digging and collision stay honest, and a one-cell edit still reads as one cell. Pinning edited cells (R3's flag) makes player work stay crisp as well.

**What makes it hard for us: chunk seams.** Relaxation spreads one node per iteration. Smoothing a terrace ten cells wide needs on the order of ten iterations, and a corner near a chunk's edge then depends on nodes ten cells into the next chunk. Our mesher copies only a 2-cell margin. Three ways through:
1. **Bigger margins with capped iterations.** Copy about 8 cells of margin and relax about 8 times. This removes the widest terraces only partly, and costs copy and meshing time on the job threads (measurable).
2. **A target from the terrain's own smooth surface, clamped to the cube. No iterations; the most promising.** The generator's heights come from continuous noise that's only rounded to whole cells at the end. For ground the player hasn't edited:
   - take the unrounded height at each node's (x, z);
   - clamp it into the node's own cube (the paper's constraint);
   - use that point.

   It's a pure function of world position, so neighbouring chunks agree exactly with no margins and no iterations. Edited cells fall back to today's corner. The terrain's cells, and so each version's output, stay unchanged; only the drawn surface of unedited ground improves.
3. **A hybrid:** option 2 for natural ground, plus one or two constrained relaxation steps across the boundary between edited and natural cells, to blend the join.

**What to keep:**
- stop where the surface is smoothest (few steps, or the direct target), not after many;
- pinch, don't guess, at ambiguous corners.

**Risks to test:**
- Where the continuous height is far from a cell's rounded value (cliffs), clamping pins it to the cube's edge, which is fine and matches the paper's corners.
- Mesas and the sandstone rule aren't pure heights, so the target applies only where a column's top matches the smooth field (natural slopes).
- Old worlds' generators must expose the same continuous value without changing their output. That needs a check, v1 and v2 alike.

## Next steps (when W055 is picked up)

- A feature card (SOP 1) with this note as its theory.
- A preview with the offline renderer (`tools/facet_preview.sh hills`), gentle slopes before and after, with an edited pit to show edits stay crisp.
- Measure meshing time on the job threads.

## Sources

- [r] Gibson 1998, above: abstract, sections 1, 2.2 and 3.1–3.2, figures 2–6.
