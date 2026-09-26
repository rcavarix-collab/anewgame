# Research: baked per-corner darkening and its triangle artefacts (the noon patches, PLAN_LOOK G)

**Sources:**
- [m] First-principles maths (checkable here).
- [p] Practitioner write-ups on block-world meshing: not peer-reviewed. D66 prefers stronger sources; these are used only because the maths below can be checked independently:
  - "Ambient occlusion for Minecraft-like worlds", 0 FPS blog (M. Lysenko), 2013;
  - an issue in the public stb_voxel_render tracker.

## The mechanism

- **[m]** A quad whose four corners carry different darkening values is drawn as two triangles. Each triangle interpolates its three corners **linearly**, so the fourth corner has no say inside it. The two possible diagonals give two different pictures:
  - Split through the dark corner: the darkness spreads into both triangles.
  - Split through the bright pair: one triangle can have **no dark corner at all**, and the dark corner shows as a hard triangular wedge.
- Across a surface this reads as **triangle-shaped patches that follow the mesh, not the terrain**. They're visible even at noon, because the darkening is in the ambient light, not in the shadows.
- **[p]** Block-world practice confirms it: the fix is to pick the diagonal through the darker pair, i.e. split along a–c when darkness(a) + darkness(c) is greater than darkness(b) + darkness(d). The alternative is to interpolate bilinearly in the shader from all four corner values.

## Our case (from the code)

- Each ground corner stores openness and sky view, and the shader interpolates them per triangle.
- The quad's split is chosen **by shape** (`FoldAC`, facetmesh.cpp: neither triangle inside out, prefer the fold that bulges outward), never by darkening.
- The same split is used for collision (`FacetBaseFace`), so drawing and walking agree.
- So wherever the bulge rule picks the bright diagonal, a dark corner makes a wedge. This matches the owner's screenshots: dark triangular smudges on slate and stone in full light.

## Options (for after the debug view confirms it)

1. **Choose the split by darkening where the shape allows.** Keep the bulge rule's safety (never fold inside out), but when both folds are safe and the bulge is slight, prefer the darker diagonal. The rule is computed from cells, so collision can use the same rule and they stay in agreement. It changes the ground's shape a little on those faces; tests must cover both.
2. **Smooth the darkening before storing it.** Average each corner's value with its neighbours', so differences across one quad are small and the wedges fade. No change to shape or collision; costs a little meshing time.
3. **Rely on fine detail.** Finer faces (more vertices) shrink each wedge, but on large calm faces the problem stays. Not enough alone.
4. **Bilinear in the shader.** Needs all four corner values at every vertex, and the vertex has no room (16 bytes, full). Not now.

**Recommendation:** first confirm with the debug view (darkening only). Then try option 2 (cheap, no geometric change) and, if wedges remain, option 1. Check with the preview renderer before and after.
