# Loam — Material Brief

## Design goal

Loam is dark, rich, crumbly soil with a fine, irregular structure. It should feel moist and fertile without looking wet, muddy, or glossy.

Its visual identity is built from **rounded crumbs, compact dark clods, and small warm mineral flecks**. These marks must remain distinct at 64 × 64 pixels and read clearly across the faceted ground.

Loam must be recognisably darker, richer, and more granular than dirt. Dirt's broader, simpler earthy patches should not be reused as loam's primary mark language.

The texture must support the ground's angular silhouette rather than compete with it.

## Palette

Use the following 12 colours. Hex values are exact sRGB colour inputs.

| Hex | Role | Use |
|---|---|---|
| #30251D | Deep shadow | Small gaps between compact clods |
| #3D2D22 | Dark base | Main low-value soil colour |
| #493326 | Base | Primary soil field |
| #563B29 | Warm base | Broad colour variation |
| #62452F | Midtone | Crumb faces and soft patches |
| #705034 | Light midtone | Select crumb tops |
| #805D3B | Highlight | Sparse upper-facing crumb pixels |
| #946D45 | Warm accent | Occasional small mineral grains |
| #A17B50 | Bright accent | Very rare single-pixel flecks |
| #443A2B | Cool shadow | Neutral brown shadow variation |
| #514A36 | Muted olive-brown | Subtle mineral and organic variation |
| #6A6045 | Pale muted accent | Rare dry organic fragments |

Avoid using the brightest colours in broad areas. The overall texture must remain dark and brown-rich, not tan.

## Marks

All sizes below are in pixels on the 64 × 64 texture.

Use irregular clusters rather than evenly spaced dots. Marks should have broken outlines, asymmetrical silhouettes, and varied orientations. Avoid long, thin features.

### 1. Compact soil crumbs — primary mark

- Size: 2–5 pixels across.
- Count: 38–64 per block.
- Shape: Rounded or angular clusters, usually 3–12 pixels in total area.
- Colour: Mostly #563B29, #62452F, and #705034.
- Spread: Clustered in loose groups, with uneven gaps between groups.
- Structure: Give most crumbs a dark lower or side edge and one or two brighter pixels on the upper-facing area.
- Variation: Randomly vary their size, outline, and internal colour. Do not make every crumb a miniature oval.

These are the signature loam marks. They should make the surface feel friable and granular without becoming gravel.

### 2. Dark crumb gaps

- Size: 1–3 pixels across.
- Count: 24–42 per block.
- Shape: Short, irregular pockets; mostly 1–4 pixels in area.
- Colour: #30251D and #3D2D22.
- Spread: Concentrate around some crumb clusters, but leave other clusters open.
- Structure: Keep gaps broken and discontinuous. Do not outline every crumb.

Dark gaps give the soil depth and make the crumbs read as a loose aggregate rather than a flat speckled surface.

### 3. Fine granular flecks

- Size: 1–2 pixels across.
- Count: 45–75 per block.
- Shape: Single pixels and compact pairs.
- Colour: #443A2B, #514A36, and occasional #946D45.
- Spread: Irregularly scattered, with some local clustering.
- Structure: Keep most flecks low contrast. Avoid uniform peppering.

These marks provide fine-scale variation without creating a noisy, evenly stippled texture.

### 4. Broad organic-rich patches

- Size: 7–15 pixels across.
- Count: 3–6 per block.
- Shape: Blotchy, uneven patches with soft pixelated boundaries.
- Colour: #3D2D22, #493326, and #563B29.
- Spread: Overlap visually with the base field; avoid isolated circular spots.
- Structure: Use value noise to vary the base colour gradually, then interrupt the patches with crumbs.

These patches add richness and prevent the texture from reading as a repeated arrangement of individual particles.

### 5. Rare pale organic fragments

- Size: 2–4 pixels across.
- Count: 2–5 per block.
- Shape: Short, irregular chips or tiny broken fragments.
- Colour: #6A6045 and #A17B50, with the brightest colour used sparingly.
- Spread: Unevenly distributed; some blocks may contain none.
- Structure: Keep fragments compact, never threadlike.

These marks suggest occasional decomposed plant material or mineral inclusions. They are accents, not a dominant feature.

## Top vs side

### Top texture

Use the full palette, with the following emphasis:

- Main field: #493326 and #563B29.
- Darker crumb gaps: #30251D and #3D2D22.
- Crumb faces: #62452F and #705034.
- Highlights: #805D3B, used sparingly.
- Mineral and organic accents: use #946D45, #514A36, and #6A6045 in small amounts.

Keep the top relatively varied, with visible crumbs and broad patches. The surface should feel loose and fertile rather than compacted into a smooth slab.

### Side texture

Use the same 64 × 64 dimensions and palette, but make the side more compact and vertically compressed in appearance.

- Main field: #3D2D22 and #493326.
- Crumb marks: 2–4 pixels across, with fewer bright faces.
- Dark gaps: slightly more frequent than on the top.
- Highlights: mostly #62452F and occasional #705034.
- Pale fragments: very rare.

Keep the distribution irregular. Do not arrange marks into horizontal layers, stripes, or continuous bands.

ASSUMPTION: The side can use a separate generated texture. If the renderer shares one texture between top and side, use the top recipe and omit the side-specific adjustments.

## Height

Use the shared 36-level height range, with 0 as the lowest and 35 as the highest.

Height should represent compact soil structure, not tall vegetation or large stones.

| Feature | Height levels | Purpose |
|---|---:|---|
| Deep crumb gaps | 0–5 | Recesses between aggregates |
| Base soil field | 5–11 | Stable underlying surface |
| Broad organic-rich patches | 7–13 | Gentle, irregular relief |
| Fine granular flecks | 8–14 | Small surface grains |
| Compact crumbs | 12–21 | Main raised features |
| Larger crumb clusters | 18–25 | Occasional prominent aggregates |
| Rare pale fragments | 12–19 | Slightly raised inclusions |

Keep the tallest features compact and irregular. No mark should form a long, narrow ridge.

Do not use the top of the height range merely to make the texture more detailed. Most pixels should remain in the lower half of the range.

### Blending rules

- Loam should blend naturally into dirt, clay, and other ground materials.
- Crumbs may rise above neighbouring base soil, but their height should remain modest.
- Keep dark gaps low so they read as recesses rather than dark painted marks.
- Avoid tall, thin fragments that could project through neighbouring materials as streaks.
- The height map should remain visually coherent with the colour texture: brighter crumb faces are generally higher than the dark gaps.

## Variation

The texture repeats every block, so avoid any obvious 64 × 64 arrangement.

For each generated block variant:

1. Change the value-noise seed and the positions of the broad patches.
2. Shift crumb clusters by irregular amounts rather than moving the whole pattern uniformly.
3. Vary crumb counts within the specified ranges.
4. Vary the proportion of dark gaps and warm midtones.
5. Change the locations of the rare pale fragments; allow some blocks to have none.
6. Vary the height of individual crumbs by a few levels while preserving the same overall height distribution.
7. Avoid placing the largest crumbs near the same texture coordinates in every block.

Neighbouring blocks should retain the same overall dark brown identity while differing in local clump arrangement and colour balance.

Do not introduce obvious directional grain, repeated diagonals, or a regular grid of clods.

## Must never look like

- Ordinary light-brown dirt with only a darker tint.
- Gravel, with many distinct hard-edged stones.
- Wet mud, with glossy highlights or smooth reflective patches.
- Black soil, with the surface crushed into near-black values.
- Sand, with uniform fine grains and a pale, dry colour.
- A tiled checkerboard or a repeated polka-dot pattern.
- Soil with long roots, fibres, cracks, or thin raised streaks.
- Photographic soil, realistic scanned material, or another game's texture style.

## Generator recipe

Use only the existing generator techniques: value noise, Voronoi cells, scatter, colour ramps, and ordinary pixel operations.

1. **Create the base field.**
   Fill the 64 × 64 texture with #493326. Add low-frequency value noise to blend between #3D2D22, #493326, and #563B29. Keep the broad variation subtle and irregular.

2. **Add organic-rich patches.**
   Generate 3–6 irregular patches, each 7–15 pixels across. Use low-frequency noise to break their outlines. Blend them into the base using #3D2D22, #493326, and #563B29. Do not create isolated, perfectly round blobs.

3. **Generate loose crumb clusters.**
   Use scattered Voronoi sites or clustered scatter points to produce 38–64 compact crumbs, each 2–5 pixels across. Break up their outlines with simple pixel operations. Avoid a uniform cell size or regular spacing.

4. **Shade the crumbs.**
   Give most crumbs a dark edge or lower portion using #3D2D22. Use #62452F and #705034 for their main faces. Add #805D3B to a small minority of upper-facing pixels. Avoid outlines that completely encircle each crumb.

5. **Add recesses.**
   Scatter 24–42 small dark gaps, each 1–3 pixels across, using #30251D and #3D2D22. Prefer positions near crumb clusters, but keep the gaps discontinuous.

6. **Add fine grains.**
   Scatter 45–75 low-contrast flecks, each 1–2 pixels across. Use #443A2B and #514A36 most often. Occasionally use #946D45. Reject placements that create long connected lines or dense uniform stippling.

7. **Add rare inclusions.**
   Scatter 2–5 compact fragments, each 2–4 pixels across. Use #6A6045 or, very rarely, #A17B50. Do not stretch, rotate into thin slivers, or connect them into lines.

8. **Build the height map.**
   Start with base values of 5–11. Lower dark gaps to 0–5. Give broad patches values of 7–13 and fine grains 8–14. Raise crumbs to 12–21, with only occasional compact clusters reaching 25. Keep rare inclusions at 12–19. Clamp all values to 0–35.

9. **Check colour-height agreement.**
   Ensure raised crumbs generally use brighter colours than the gaps. Do not let a bright accent create a tall, narrow spike in the height map.

10. **Create the side variant.**
    If separate top and side textures are supported, use the same palette and generation approach. Darken the base emphasis, reduce bright crumb faces, and make crumbs slightly smaller. Keep the arrangement irregular and avoid horizontal strata.

11. **Check repetition.**
    Compare neighbouring 64 × 64 blocks. If clusters, patches, or pale fragments form a visible repeated arrangement, regenerate their positions with a different seed.

12. **Check at game scale.**
    View the texture on faceted ground at its intended scale. Loam should read as dark, rich, crumbly soil at a glance, while the angular ground remains the dominant visual feature. Confirm that its colours remain distinguishable in daylight, at dawn and dusk, and under moonlight.

13. **Keep rendering inexpensive.**
    Perform noise, scatter, and height-map generation during asset creation. The finished texture should require no additional per-frame effect.