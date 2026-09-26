# Snow — Material Brief

## Material identity

Snow is a bright, cool, softly varied surface. Its defining mark language is **broad, quiet snow patches with sparse, crisp sparkle points**.

The surface should read as snow immediately, without relying on pure white everywhere, deep blue shadows, or a field of tiny glitter marks. The faceted ground remains the dominant visual feature: snow provides a readable surface across the angular triangles without competing with their silhouettes.

Snow appears above a certain world height. Its colour must remain distinguishable under direct sunlight, overcast-looking sky illumination, dusk, dawn, and moonlight.

ASSUMPTION: Snow's upper elevation boundary is controlled by existing world-generation rules. This brief defines the material appearance only.

## Palette

Use these 12 colours as the complete palette. All colours are opaque sRGB hex values. Assign colours by role rather than selecting randomly without constraints.

| Hex | Role | Intended use |
|---|---|---|
| `#E8EDF0` | Base | Main snow surface; cool off-white, not pure white |
| `#DCE4E9` | Base variation | Broad, subtle variation across otherwise flat snow |
| `#F2F4F3` | Highlight | Soft-lit snow patches; reserve the brightest colour |
| `#CDD8E0` | Shadow | Gentle cool shading |
| `#B9C8D4` | Deep shadow | Small, restrained shadow pockets |
| `#D5E4EA` | Cool highlight | Light-facing patches with a faint icy cast |
| `#E7F0F2` | Highlight variation | Breaks up large highlight regions |
| `#C4D4DC` | Cool midtone | Transition between base and shadow |
| `#F7F8F5` | Sparkle | Tiny, rare sparkle points; never use as a broad fill |
| `#EAF7F8` | Sparkle variation | Occasional cool sparkle point |
| `#AFC2CF` | Crevice accent | Sparse, short indentations and compressed-snow marks |
| `#D0DEE5` | Soft transition | Blends neighbouring patches without hard outlines |

### Palette constraints

- Do not use pure white (`#FFFFFF`) in the texture.
- Keep `#F7F8F5` restricted to isolated sparkle points, at most a few pixels per mark.
- `#B9C8D4` and `#AFC2CF` must occupy only a small fraction of the surface.
- At least 70% of the texture should use the four principal colours: `#E8EDF0`, `#DCE4E9`, `#F2F4F3`, and `#CDD8E0`.
- Avoid broad areas of a single highlight colour. Even the brightest snow should retain visible cool variation.
- The palette is intentionally low in saturation. Do not push the blue channel so far ahead of red and green that snow appears icy blue.

## Marks

All dimensions below are in pixels on the 64 × 64 texture. Counts are per block, not per triangle. Marks should be distributed across the whole tile, with mild clustering and irregular spacing.

### 1. Broad snow patches

- Shape: irregular rounded polygons and softly stepped blobs.
- Size: 6–16 pixels across; 4–10 pixels high.
- Count: 8–16 patches per block.
- Spread: distribute across the tile, allowing small clusters and open gaps. Avoid a regular arrangement.
- Colour: primarily `#DCE4E9`, `#CDD8E0`, `#E7F0F2`, and `#F2F4F3`.
- Edge: use stepped pixel boundaries with 1–3 pixel irregularities; avoid smooth vector-like curves.
- Contrast: keep adjacent patch colours close enough that the texture still reads as snow rather than a mosaic.

These patches provide the primary visible variation. Their boundaries should not resemble individual stones, scales, or separate chunks of ice.

### 2. Shallow snow dimples

- Shape: small, irregular, shallow-edged patches, sometimes with a short darker edge.
- Size: 2–5 pixels across; 2–4 pixels high.
- Count: 12–24 per block.
- Spread: scatter unevenly, with occasional pairs or loose groups. Do not align them in rows.
- Colour: `#CDD8E0`, `#C4D4DC`, or `#B9C8D4`.
- Contrast: most dimples use the lighter two colours. Reserve the deepest shadow for occasional small marks.

Dimples should suggest a subtly uneven snow surface, not a field of holes.

### 3. Sparkle points

- Shape: single pixels, occasional 2-pixel pairs, and rare compact plus-shaped marks.
- Size: 1–3 pixels across; 1–3 pixels high.
- Count: 4–10 sparkle marks per block.
- Spread: sparse, irregular distribution. Avoid evenly spaced glitter and avoid clustering every sparkle around one bright patch.
- Colour: mainly `#F7F8F5`, with occasional `#EAF7F8`.
- Contrast: sparkles should be visible at close range but must not form a bright, continuous pattern.

A plus-shaped sparkle may use three pixels total: one central pixel and two short arms. Do not create large starbursts.

Sparkles are static texture marks. Do not require animation, bloom, particles, or additional lighting passes.

### 4. Shallow compression marks

- Shape: short, broken, gently curved or stepped dashes.
- Size: 3–7 pixels long; 1–2 pixels wide.
- Count: 3–7 per block.
- Spread: scattered, with varied orientations. No continuous parallel bands.
- Colour: `#C4D4DC`, `#CDD8E0`, or `#D0DEE5`.
- Contrast: low; these marks should be nearly lost at a distance.

These suggest minor surface compression without making the snow resemble layered rock or wind-carved dunes.

### 5. Tiny surface flecks

- Shape: isolated square pixels or compact 2-pixel clusters.
- Size: 1–2 pixels across; 1–2 pixels high.
- Count: 8–16 per block.
- Spread: unevenly distributed, with a mild tendency to appear near patch boundaries.
- Colour: `#D5E4EA`, `#D0DEE5`, or `#C4D4DC`.

Flecks add fine-grained variation, but should remain subordinate to the broad patches.

### Density and negative space

Keep at least 20% of the tile visually quiet: uninterrupted base-colour areas without prominent dimples, compression marks, or sparkles.

Marks may overlap the broad colour patches, but do not stack multiple high-contrast marks at the same pixel. Use a fixed priority order when resolving overlap:

1. Sparkle points
2. Deepest shadow marks
3. Dimple marks
4. Broad patches
5. Base colour

The sparkle layer must remain sparse even where the other marks overlap.

## Top vs side

### Top texture

Use the full palette and all five mark types.

The top should feel softly uneven, with broad patches doing most of the work. Keep compression marks short and infrequent. Sparkles are visible but rare.

Avoid a uniform noise field: the large patches should be legible at 64 × 64, while individual pixels remain crisp close to the player.

### Side texture

ASSUMPTION: The side texture represents compacted snow on steep terrain, not exposed ice or a geological cross-section.

Use the same palette, but reduce the number of bright marks and make the texture more compacted.

- Broad patches: 6–12 per block, 5–13 pixels across and 3–8 pixels high.
- Dimples: 8–16 per block, 2–4 pixels across and 2–3 pixels high.
- Sparkles: 1–4 per block, each 1–2 pixels across and 1–2 pixels high.
- Compression marks: 6–12 per block, 3–8 pixels long and 1–2 pixels wide.
- Tiny flecks: 6–12 per block, 1–2 pixels across and 1–2 pixels high.

Give compression marks a weak horizontal bias, but vary their lengths, vertical positions, and gaps. Do not create continuous horizontal stripes.

The side should be slightly darker on average than the top, primarily through increased use of `#DCE4E9`, `#CDD8E0`, and `#C4D4DC`. Keep `#F2F4F3` and sparkle colours rare.

Do not make the side uniformly blue or dramatically darker than the top. It must still read as snow under moonlight.

## Height

The height map has 36 levels, from 0 (lowest) to 35 (highest).

ASSUMPTION: The existing material system treats higher height values as winning at material boundaries, and uses this 0–35 range.

### Height assignment

| Feature | Height level | Reason |
|---|---:|---|
| Main snow surface | 16–20 | Stable, continuous material body |
| Broad raised patches | 18–22 | Gentle local variation without excessive overlap |
| Shallow dimples | 12–16 | Recessed surface detail |
| Compression marks | 13–17 | Slightly lower than the surrounding surface |
| Tiny surface flecks | 16–20 | Mostly colour detail, not protruding geometry |
| Sparkle points | 16–20 | Optical colour accents, not physical protrusions |
| Rare soft snow mounds | 21–24 | Occasional rounded high spots |

### Blending rules

- Snow must not consist entirely of maximum-height texels. Keep most of the surface in the middle of the range.
- Use broad, connected height regions rather than isolated tall pixels.
- Do not assign extra height to sparkle points. They are visual highlights, not crystals sticking out of the surface.
- Keep dimples and compression marks below the surrounding surface.
- Avoid tall, thin features. In particular, compression marks must not form narrow raised ridges.
- At boundaries with neighbouring materials, snow should blend as a broad surface rather than producing a jagged fringe of isolated winning pixels.
- Keep rare mounds broad enough to avoid one-pixel spikes at material boundaries.

The height map should communicate soft, shallow variation. It must not make snow appear like a field of sharp crystals or chunky ice.

## Variation

The texture repeats every block, so variation must be designed to conceal repetition without introducing visible seams.

### Within one block

- Use low-frequency value noise to establish broad light and shadow regions.
- Add medium-scale irregular patches using a combination of noise and irregular cell regions.
- Use scattered dimples, flecks, and sparkles for fine variation.
- Vary the size and orientation of compression marks.
- Keep the overall brightness distribution stable: variation should change the arrangement of snow marks, not turn some blocks grey and others nearly white.

### Between neighbouring blocks

- Use different random seeds for patch placement, dimples, and sparkles.
- Ensure the four edges of each tile have compatible base-colour distributions, so the repeat does not form a visible boundary.
- Avoid placing a large, distinctive patch or a dense sparkle cluster at the same relative position in every block.
- Do not force identical edge patterns. Instead, maintain similar colour and mark densities near opposite edges, allowing the texture to tile without an obvious grid.
- Avoid a dominant directional pattern. The top texture should not imply a repeated wind direction through identical parallel marks.
- Keep the brightest pixels rare across every tile, not merely rare in the overall texture set.

If the generator supports shared world-space noise, use it for broad colour variation across adjacent blocks. Keep fine scatter seeded per block. This lets large colour regions continue naturally across boundaries while small marks remain varied.

ASSUMPTION: The generator can use deterministic seeds and can sample noise in world-aligned coordinates. If it cannot, use deterministic per-tile seeds and matched edge-colour distributions instead.

## Must never look like

- Plain, featureless white.
- Deep blue ice or frozen glass.
- Wet reflective slush.
- A field of glitter, stars, or glowing crystals.
- Pebbles, gravel, or a field of separate white stones.
- Layered sandstone or slate.
- Tall spikes, icicles, or crystalline protrusions.
- Repeated parallel stripes or obvious square patches matching the tile boundary.
- Photographic snow, realistic photographic noise, or smooth painted gradients.
- A copy of another game's snow texture.

## Generator recipe

Use only the existing procedural building blocks: value noise, irregular cell regions, scatter, colour ramps, and the height-map output.

1. **Initialize the tile.** Create a 64 × 64 pixel texture. Seed the generator deterministically. Fill the tile with `#E8EDF0`.

2. **Build broad tonal variation.** Generate low-frequency value noise with a feature scale of approximately 16–28 pixels. Map the result to a restrained range of `#DCE4E9`, `#E8EDF0`, and `#F2F4F3`. Keep the brightest colour uncommon.

3. **Add broad snow patches.** Generate 8–16 irregular regions per block, each 6–16 pixels across and 4–10 pixels high. Use stepped, irregular boundaries. Assign colours from `#DCE4E9`, `#CDD8E0`, `#E7F0F2`, and `#F2F4F3`, weighted toward the base and midtones.

4. **Soften patch contrast without blurring pixels.** Add small intermediate regions using `#D0DEE5` and `#C4D4DC`. Preserve hard pixel edges. Do not apply a blur filter to the final texture.

5. **Scatter dimples.** Place 12–24 irregular marks per block, each 2–5 pixels across and 2–4 pixels high. Prefer `#CDD8E0` and `#C4D4DC`; use `#B9C8D4` sparingly. Keep dimples shallow in both colour contrast and height.

6. **Scatter compression marks.** Place 3–7 short dashes per block, each 3–7 pixels long and 1–2 pixels wide. Vary orientation and length. Use low-contrast colours and avoid alignment into continuous rows.

7. **Add tiny flecks.** Place 8–16 small marks per block, each 1–2 pixels across. Use `#D5E4EA`, `#D0DEE5`, and `#C4D4DC`. Bias placement slightly toward broad-patch boundaries, but do not outline every patch.

8. **Add sparkles last.** Place 4–10 sparkle marks per block, each 1–3 pixels across. Use `#F7F8F5` and `#EAF7F8`. Allow single pixels, two-pixel pairs, and rare three-pixel plus shapes. Reject placements that create a dense cluster or a repeated visual arrangement.

9. **Create the height map.** Initialize the surface around levels 16–20. Give broad raised patches levels 18–22, shallow dimples levels 12–16, and compression marks levels 13–17. Add rare, broad mounds at levels 21–24. Keep sparkles at the surrounding surface height. Never use the maximum level merely to make a mark visually prominent.

10. **Generate the side texture.** Reuse the same palette and mark vocabulary. Reduce bright patches and sparkles, increase the proportion of cool midtones, and give compression marks a weak horizontal bias. Keep all bands broken and irregular. Generate its height map using the same restrained range.

11. **Check tile edges.** Compare the colour distribution and mark density within 4 pixels of each edge. Adjust broad noise and patch placement to prevent obvious borders. Do not create a conspicuous frame or force identical marks onto opposite edges.

12. **Check the final 64 × 64 image at native size.** Confirm that broad patches are visible, sparkles remain rare, and dimples do not turn into dark holes. Ensure that the material still reads as snow without relying on pure white.

13. **Check lighting robustness.** Inspect the palette under representative warm daylight, neutral daylight, dusk, and cool moonlight colour conditions. Keep enough separation between the base, shadow, and highlight colours that snow does not collapse into a single white value in sunlight or become uniformly blue-grey at night.

14. **Keep the implementation inexpensive.** All noise, scatter, colour assignment, and height-map construction happen in the existing texture-generation pipeline. The runtime texture remains a conventional 64 × 64 repeating material texture. Do not require shaders, render passes, animation, or other new runtime features solely for snow.

## Acceptance checklist

- [ ] The main surface is cool off-white, never pure white.
- [ ] Broad patches provide the primary visual variation.
- [ ] Sparkle points are sparse and static.
- [ ] No thin raised marks produce streaks at material boundaries.
- [ ] The height map uses restrained levels and broad transitions.
- [ ] Top and side textures are distinguishable but clearly the same material.
- [ ] Neighbouring blocks do not form a visible grid.
- [ ] The texture remains readable under sun, dusk, dawn, and moonlight.
- [ ] All texture marks are defined at 64 × 64 resolution.
- [ ] The generator uses only the existing procedural techniques.