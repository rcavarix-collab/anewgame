# Dry Turf — Material Brief

## Design intent

Dusty, ochre ground with sparse, short, desiccated grass. It should feel like soil that has dried out but still supports scattered remnants of vegetation.

Its identity comes from three things:
- A warm, muted ochre-brown ground.
- Small, irregular straw-coloured grass blades grouped into sparse tufts.
- Broad, subdued patches of dry soil that break up the colour without forming stripes or obvious repeating patterns.

Dry turf must remain a ground material, not a field of upright grass. Its marks should support the angular facets of walkgrid's terrain rather than compete with them.

At a glance:
- Meadow grass is greener, denser, and more visibly alive.
- Dry turf is warmer, dustier, browner, and more sparsely vegetated.
- Sand is more uniformly granular and lacks the distinctive blade-shaped marks of dry turf.

All dimensions below refer to a 64 × 64 pixel texture representing one block.

## Palette

Use these 12 colours as the complete material palette. All colours are opaque sRGB hex values.

| Hex | Role | Use |
|---|---|---|
| `#A98A52` | Base | Main dusty ochre ground |
| `#987A46` | Shadow | Broad, subdued soil variation |
| `#82683F` | Deep shadow | Small, low-lying soil pockets |
| `#B99B61` | Highlight | Sun-dried earth |
| `#C7AA70` | Bright highlight | Sparse dusty ground accents |
| `#D2B77E` | Accent | A few pale, dry soil flecks |
| `#74613E` | Deep accent | Tiny embedded soil chips |
| `#B69A59` | Grass base | Main straw-coloured blade |
| `#C8AC6A` | Grass highlight | Lit side of a blade or tuft |
| `#D6BD7F` | Pale grass | Occasional bleached blade |
| `#947B48` | Grass shadow | Darker blade edges and bases |
| `#A18B60` | Neutral transition | Muted dust and soil-grass transition |

Palette guidance:
- Keep the overall appearance warm, dry, and moderately muted.
- The ground base should occupy most of the texture.
- Use the brighter straw colours sparingly; they identify vegetation rather than turning the ground yellow.
- Keep the deepest brown confined to small marks and sheltered portions of tufts.
- Avoid pure black, pure white, saturated orange, and vivid green.
- Do not introduce additional colours during generation. Blend or select from these palette entries.

## Marks

The mark language is **short, broken straw blades over broad dusty soil patches**. Avoid long lines, dense hair-like strokes, and uniformly distributed speckles.

### 1. Dusty ground patches

- Shape: Irregular, softly stepped blobs with uneven, angular boundaries.
- Size: 7–20 pixels across; 4–12 pixels high.
- Count: 5–10 patches per 64 × 64 pixel block.
- Spread: Use low-frequency noise to create broad areas of slightly lighter or darker soil. Allow patches to merge into the base, but avoid a single dominant patch.
- Colour: Primarily `#987A46`, `#B99B61`, and `#A18B60`.
- Contrast: Keep patch boundaries subdued. They should be visible on close inspection, not read as separate stones.

These patches provide large-scale variation without creating obvious stripes or a tiled pattern.

### 2. Dry grass tufts

- Shape: Small clusters of 3–6 separate, tapered, angular blades. Each blade should be a short wedge or narrow quadrilateral, not a smooth curve.
- Individual blade size: 2–6 pixels long and 1–2 pixels wide.
- Tuft footprint: 5–10 pixels across.
- Count: 5–9 tufts per block.
- Spread: Place tufts in irregular groups. Vary their spacing, rotation, and blade count. Leave substantial bare ground between groups.
- Colour: Use `#B69A59` as the main blade colour, `#C8AC6A` for occasional highlights, and `#947B48` for small shadowed bases.
- Orientation: Vary blade directions. Most blades should lean or lie diagonally; avoid making every tuft point upward in the texture.
- Silhouette: Keep the blades short and broken. No tuft should form a continuous thin line.

Tufts should be the primary identifying feature of dry turf, but the soil must remain visually dominant.

### 3. Isolated straw fragments

- Shape: Short, angular dashes or tiny tapered chips.
- Size: 1–3 pixels long and 1 pixel wide.
- Count: 8–18 fragments per block.
- Spread: Scatter unevenly, with some fragments near tufts and others isolated. Avoid even spacing.
- Colour: Mostly `#B69A59` and `#D6BD7F`; use the pale colour sparingly.

Fragments suggest broken vegetation without making the whole surface grassy.

### 4. Soil chips

- Shape: Small, irregular angular flecks.
- Size: 1–3 pixels across.
- Count: 10–20 chips per block.
- Spread: Cluster lightly within darker soil patches, with occasional isolated chips elsewhere.
- Colour: Use `#74613E`, `#82683F`, and `#D2B77E`.
- Contrast: Keep most chips low-contrast. Bright chips should be rare.

These marks distinguish dry turf from sand's more uniform granular texture. Do not turn them into pebble-sized features.

### 5. Mark hierarchy

From most to least visually important:

1. Sparse straw-coloured tufts.
2. Broad, low-contrast dusty patches.
3. Tiny soil chips and broken straw fragments.

At normal viewing distance, the viewer should first read dry ochre ground with sparse vegetation. Fine chips should not dominate.

## Top vs side

ASSUMPTION: Use the same underlying dry-turf texture for both top and side surfaces.

The top texture is the reference appearance:
- Preserve the full density of 5–9 tufts per block.
- Keep broad dusty patches and scattered chips.
- Maintain the warm ochre base.

For near-vertical faces:
- Use the same palette and mark shapes.
- Reduce tuft density to approximately 2–5 tufts per 64 × 64 pixel block.
- Reduce isolated straw fragments to approximately 4–10 per block.
- Keep soil patches broad and subdued.
- Do not rotate the entire texture into horizontal strata or create a layered cliff pattern.

This keeps vertical dry-turf faces related to the ground while avoiding the appearance of grass growing densely sideways.

## Height

Use the material's 36-level height map, where lower values represent recessed features and higher values represent features that should win material blending.

Suggested height assignments:

| Feature | Height level | Reason |
|---|---:|---|
| Main ground | 14–18 | Establishes the base surface |
| Broad dusty patches | 13–19 | Shallow variation; should not create hard relief |
| Dark soil pockets | 10–14 | Slightly recessed appearance |
| Soil chips | 16–20 | Minor surface irregularities |
| Straw fragments | 17–21 | Slightly raised, but not dominant |
| Grass blade bodies | 19–23 | Distinguishable from the soil |
| Tuft bases | 21–24 | Small local accumulation |
| Highest blade tips | 25–27 | Occasional small features that can win blending |

Height rules:
- Most pixels should remain between levels 14 and 20.
- Reserve levels 25–27 for isolated blade tips.
- Do not use levels 28–35 for ordinary dry-turf marks.
- Keep high values spatially compact within tufts.
- Never make an entire blade a tall, thin ridge. Its height should be modest, and its footprint should remain a short, broad-enough wedge.
- Soil chips should not rise high enough to resemble gravel or stone.

The modest height range lets a few dry grass tips show through neighbouring materials without producing conspicuous streaks at material boundaries.

## Variation

The texture repeats every block, so neighbouring blocks must not look like identical copies.

Use deterministic, independently seeded variation for each block, while preserving the same palette and mark language.

Vary the following:

- Tuft count: Choose within 5–9 per block.
- Tuft footprint: Choose within 5–10 pixels across.
- Blade count: Choose 3–6 blades per tuft.
- Blade length: Choose within 2–6 pixels.
- Patch count: Choose within 5–10 per block.
- Patch scale: Choose within 7–20 pixels across.
- Soil-chip count: Choose within 10–20 per block.
- Fragment count: Choose within 8–18 per block.
- Palette balance: Shift the proportion of base, shadow, and highlight subtly between blocks.
- Tuft orientation: Vary angles and lean directions.

Prevent visible repetition:
- Do not place tufts at fixed grid points.
- Do not use a regular checkerboard, rows, or evenly spaced clusters.
- Do not align patch boundaries with the 64-pixel texture edges.
- Let a few marks approach texture boundaries so the surface does not have a conspicuous empty border.
- Avoid placing a distinctive large tuft in the same relative location in every block.
- Use a stable world-coordinate seed so neighbouring blocks differ without shimmering or changing randomly between frames.
- Keep the average material identity consistent across blocks; variation should not make some blocks look like meadow grass or sand.

## Must never look like

- Lush green meadow grass.
- A dense carpet of upright grass blades.
- Uniform yellow sand or a field of evenly distributed grains.
- Gravel, with large, individually readable stones.
- Layered sandstone or striped sediment.
- Burnt, nearly black earth.
- Bright orange clay.
- Photographic soil, realistic scanned ground, or smooth painted illustration.
- A regular tiled pattern, checkerboard, or repeating tuft arrangement.
- Tall, thin ridges that produce streaks when blended with neighbouring materials.

## Generator recipe

1. **Initialize the block.** Create a 64 × 64 pixel texture and a matching 64 × 64 pixel height map. Use a deterministic seed derived from the block's world coordinates.

2. **Establish the base.** Fill the texture with `#A98A52`. Initialize the height map around level 16, allowing small seeded variation between levels 14 and 18.

3. **Generate broad soil variation.** Evaluate low-frequency value noise across the block. Convert it into 5–10 irregular patches, each 7–20 pixels across and 4–12 pixels high. Use `#987A46`, `#B99B61`, and `#A18B60`. Keep contrast modest and boundaries irregular. Assign patch heights between levels 13 and 19.

4. **Add darker soil pockets.** Scatter small, irregular areas using `#82683F`. Keep them subordinate to the broad patches. Use height levels 10–14, with no large, connected depressions.

5. **Scatter soil chips.** Place 10–20 angular flecks, each 1–3 pixels across. Use `#74613E`, `#82683F`, and occasional `#D2B77E`. Cluster some within darker patches. Keep their heights between levels 16 and 20.

6. **Place grass tufts.** Scatter 5–9 tuft centres with a seeded, non-uniform distribution. Reject placements that create a regular pattern or merge too many tufts into a continuous mass. Each tuft contains 3–6 blades, each 2–6 pixels long and 1–2 pixels wide. Use short tapered wedges with irregular angles and lean directions.

7. **Colour the blades.** Use `#B69A59` for most blade pixels, `#C8AC6A` for occasional highlights, `#D6BD7F` for rare bleached tips, and `#947B48` for a few darker bases. Keep bright pixels concentrated in small groups rather than outlining every blade.

8. **Assign tuft heights.** Set blade bodies to levels 19–23, tuft bases to levels 21–24, and isolated tips to levels 25–27. Keep height features compact. Do not draw a long, narrow high-height line through a blade.

9. **Scatter broken straw.** Add 8–18 short fragments, each 1–3 pixels long and 1 pixel wide. Use `#B69A59` and occasional `#D6BD7F`. Avoid aligning fragments into lines or placing them at regular intervals. Set their heights to levels 17–21.

10. **Apply subtle local variation.** Use `#B99B61`, `#C7AA70`, and `#A18B60` sparingly to introduce small shifts in the soil. Use `#C7AA70` and `#D2B77E` only for isolated dusty highlights. Do not apply a uniform brightening pass that turns the texture into sand.

11. **Create the side variant.** Reuse the same texture-generation logic and palette, but reduce tufts to 2–5 per block and fragments to 4–10 per block. Keep the same subdued soil pattern. Do not introduce horizontal bands.

12. **Check the material identity.** At 64 × 64 pixels, confirm that the ochre soil dominates, sparse straw blades are immediately recognizable, and the texture remains distinct from greener meadow grass and granular sand.

13. **Check repetition and blending.** Preview adjacent blocks using their world-coordinate seeds. Confirm that no obvious 64-pixel grid appears, that the texture remains legible when projected onto faceted triangles, and that high blade tips do not form thin streaks at material boundaries.

14. **Check lighting readability.** Inspect the texture under dawn, noon, dusk, and moonlight. Preserve separation between the ochre base, brown shadows, and straw highlights. If necessary, adjust palette usage or the renderer's existing colour response rather than adding colours outside the listed palette.