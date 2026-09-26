# Dirt — Material Brief

## Purpose and identity

Dirt is the exposed earth beneath grass. It appears on the vertical sides of grass-covered ground and wherever the player digs.

Its identity is **compact, irregular soil clods, small embedded stones, and scattered fine-grained flecks**. It should feel dense, earthy, and slightly varied, without looking muddy, sandy, rocky, or artificially patterned.

The texture must support the game's faceted ground. Its marks should follow the projected texture rather than trying to imitate the shape of the triangles. At a glance, dirt should read as warm brown earth.

Use a crisp pixel-art style at 64 × 64 pixels per block. Every mark must remain legible at this resolution. Avoid photographic noise and overly fine detail.

## Palette

Use these 12 colours. Hex values are exact, with roles assigned to keep the material readable under daylight, dusk, dawn, and moonlight.

| Hex | Role | Use |
|---|---|---|
| `#493126` | Deep shadow | Small creases between clods and the darkest soil recesses |
| `#5B3B2B` | Shadow | Undersides of clods and shaded soil patches |
| `#704832` | Dark base | Broad darker areas and the shadow-side portions of clods |
| `#805238` | Base | Main soil colour |
| `#90603F` | Warm base | Broad, subtle variation within the soil |
| `#A16E49` | Light base | Exposed clod surfaces |
| `#B17D53` | Highlight | Sparse upper-facing clod edges |
| `#C08D60` | Bright highlight | Very occasional small, sun-facing flecks |
| `#604A35` | Neutral brown | Embedded grit and muted inclusions |
| `#786047` | Light neutral brown | Small mineral grains and secondary soil marks |
| `#987B58` | Pale mineral accent | Sparse, subdued embedded grains |
| `#3E3930` | Dark mineral accent | Rare dark inclusions, never dominant |

Do not introduce pure black, pure white, saturated orange, or strongly red colours. Keep the palette earthy and restrained.

The darkest colours should occupy little area. The two main base colours, `#805238` and `#90603F`, should dominate. Highlights must be visible as pixel-art accents, not as a continuous bright coating.

## Marks

All dimensions are in pixels on the 64 × 64 texture.

### 1. Soil clods

The primary mark language.

- Shape: irregular, rounded-angular patches, with stepped pixel edges. Avoid perfect circles and smooth gradients.
- Size: typically 4–9 pixels wide and 3–7 pixels high.
- Large examples: up to 12 × 9 pixels, but rare.
- Count: approximately 18–30 distinct clods per block, including partially merged clods.
- Spread: irregularly distributed, with small clusters and occasional open spaces. Avoid evenly spaced rows.
- Colour: mostly `#704832`, `#805238`, `#90603F`, and `#A16E49`.
- Shading: use one darker edge or a small shadow notch on some clods, not all of them.
- Highlights: add a 1–3-pixel highlight to a minority of clods. Do not outline every shape.

Clods should merge visually into a continuous soil surface. They must not resemble separate pebbles scattered across a flat background.

### 2. Fine soil flecks

Small marks that provide texture between clods.

- Shape: single pixels, short stepped pairs, or compact 2 × 2-pixel patches.
- Size: 1–2 pixels across; occasional 3 × 2-pixel marks.
- Count: approximately 45–75 marks per block.
- Spread: broadly distributed, with density gently following the underlying noise. Avoid uniform random coverage.
- Colour: primarily `#5B3B2B`, `#90603F`, and `#A16E49`.
- Contrast: moderate. Most flecks should differ from the local base by only one or two palette steps.

Flecks should make the surface feel granular without producing television-like static.

### 3. Embedded mineral grains

Small, subdued inclusions that distinguish dirt from a flat brown fill.

- Shape: compact, angular clusters; never long streaks.
- Size: 2 × 2 to 4 × 3 pixels.
- Count: approximately 5–10 grains per block.
- Spread: irregularly scattered, with no preferred direction.
- Colour: `#604A35`, `#786047`, and occasionally `#987B58`.
- Brightness: keep pale grains sparse and low contrast.

These are grains embedded in soil, not loose gravel sitting on top.

### 4. Tiny cracks and creases

Use sparingly to suggest compacted earth.

- Shape: short, broken, angular notches; no continuous outlines.
- Size: 2–5 pixels long and 1 pixel wide.
- Count: approximately 3–7 per block.
- Spread: place them near a few larger clods, with irregular spacing.
- Colour: `#493126` or `#5B3B2B`.
- Orientation: vary freely; do not align them into parallel bands.

Cracks should be secondary details. Avoid a dry, deeply cracked desert appearance.

### 5. Local colour patches

Broad colour variation beneath the marks.

- Shape: irregular, soft-edged regions, rendered with stepped pixel boundaries.
- Size: approximately 12–28 pixels across.
- Count: 3–6 broad regions per block.
- Spread: overlap and blend irregularly; avoid distinct circular blobs.
- Colour: use the main base colours, especially `#805238` and `#90603F`, with occasional darker or lighter regions.
- Contrast: low. The regions should be felt as variation rather than read as separate objects.

## Top vs side

### Top texture

The top texture represents exposed earth viewed from above.

- Use the full palette, with the base colours dominating.
- Keep clods relatively broad and irregular.
- Distribute mineral grains and flecks across the surface.
- Allow occasional small, darker creases.
- Use sparse highlights on exposed clod surfaces.
- Keep the overall texture balanced: no single corner should be consistently darker or brighter.

The top should read as compact soil, not a field of stones or a smooth patch of mud.

### Side texture

The side texture represents the exposed vertical soil face beneath a grass-covered block.

- Reuse the same palette and mark language.
- Increase the proportion of connected clods and compact, irregular soil masses.
- Reduce the number of isolated bright grains and highlights.
- Use slightly more dark creases between adjoining clods.
- Keep the overall value close to the top texture so that a block's side remains recognisably the same material.
- Do not add horizontal strata, long bands, or regularly spaced layers.

The side may feel more compact and shaded, but it must not become a separate dark-brown material.

**ASSUMPTION:** The renderer handles face orientation and lighting separately. Therefore, the side texture should not bake in a strong directional shadow that would conflict with the scene lighting.

## Height

Use the game's 36-level height map, with integer levels from 0 to 35.

**ASSUMPTION:** Level 0 is the lowest height and level 35 is the highest.

Dirt is primarily a low-relief material. Its height map should add enough shape for blending without creating conspicuous protrusions.

### Height assignments

| Feature | Height level | Reason |
|---|---:|---|
| Main soil background | 10–14 | Establishes the continuous soil surface |
| Broad colour patches | 10–15 | Keeps colour variation mostly flat |
| Dark creases | 7–10 | Recesses between compacted clods |
| Main clod surfaces | 14–18 | Gives clods modest relief |
| Clod edge pixels | 16–20 | Adds small, irregular raised areas |
| Embedded mineral grains | 13–18 | Keeps inclusions mostly flush with the soil |
| Rare compact clod peaks | 21–23 | Provides limited local relief |

Do not use levels 24–35 for ordinary dirt features. Dirt should not compete with tall grass, large pebbles, or other pronounced surface details.

### Blending rules

- Keep the majority of the surface within levels 10–18.
- Keep raised areas broad and compact.
- Do not make highlights automatically taller than their surrounding pixels; colour and height are separate properties.
- Avoid isolated, high single-pixel spikes.
- Never create tall, thin cracks, ridges, or mineral streaks.
- Keep the side texture's height variation restrained so it does not produce a jagged silhouette along exposed block edges.

## Variation

The texture must remain natural when the same material repeats across neighbouring blocks.

### Within a block

- Combine broad, low-contrast colour regions with clods, flecks, and sparse inclusions.
- Vary clod sizes and shapes. Avoid repeating a small set of identical templates.
- Use irregular clusters rather than uniform scatter.
- Allow some areas to be relatively plain, balanced by more detailed areas elsewhere.
- Avoid a single dominant feature in the centre of every block.

### Between neighbouring blocks

- Shift the broad colour regions and the clod distribution.
- Vary the local balance of warm base colours while preserving the overall brown identity.
- Change the number and placement of mineral grains and creases within the specified ranges.
- Avoid copying the same large clod arrangement from block to block.
- Keep the average brightness and palette consistent so neighbouring blocks still read as one material.
- Do not create a dark border or bright border at texture edges.

### Seam and repetition constraints

- The 64 × 64 texture must tile seamlessly.
- Features crossing one texture edge must continue correctly at the opposite edge.
- Use wrapped coordinates when generating broad noise and placing marks.
- Avoid obvious repeated clusters, checkerboard layouts, or evenly spaced marks.
- Preserve crisp pixel boundaries at close range.

## Must never look like

- Wet mud, glossy clay, or a reflective surface.
- Loose sand or a field of tiny grains.
- Gravel, cobblestones, or a layer of separate rocks.
- Deeply cracked desert earth.
- Red-orange baked earth.
- Uniform chocolate-brown noise.
- Horizontal sedimentary strata.
- Photographic soil, scanned dirt, or realistic photographic noise.
- A regular checkerboard, repeating clod stamp, or obvious tiled grid.
- A surface covered in tall, thin ridges or streaks.

## Generator recipe

Use the existing Python generator's value noise, scatter, colour ramps, and height-map output. No external tools or new dependencies are required.

### 1. Establish the base

1. Create a 64 × 64 pixel texture.
2. Generate a low-frequency, seamless value-noise field using wrapped coordinates.
3. Use the field to distribute the main base colours, with `#805238` and `#90603F` as the dominant colours.
4. Keep broad colour transitions subtle and irregular. Avoid smooth, obvious circular blobs.
5. Ensure the noise wraps cleanly at all four edges.

### 2. Create the clod pattern

1. Generate an irregular field of approximately 18–30 clod centres per block.
2. Use a second, higher-frequency noise field to distort clod boundaries into stepped, angular shapes.
3. Give most clods dimensions of 4–9 × 3–7 pixels, with only a few reaching 12 × 9 pixels.
4. Allow nearby clods to merge into connected patches.
5. Assign clod colours from the dark and warm base colours, using local noise to prevent identical colour distributions.
6. Add small shadow notches to some clods, but do not outline every clod.
7. Add sparse 1–3-pixel highlights to a minority of clods.

### 3. Add fine flecks

1. Scatter approximately 45–75 fine marks across the texture.
2. Choose mostly 1–2-pixel marks, with occasional 3 × 2-pixel patches.
3. Bias the scatter density gently using the noise field.
4. Choose colours from the shadow and base colours, with occasional light-base marks.
5. Prevent dense, evenly distributed static by allowing small clusters and quiet areas.

### 4. Add mineral grains

1. Scatter approximately 5–10 compact grains.
2. Give them dimensions of 2 × 2 to 4 × 3 pixels.
3. Choose colours from `#604A35`, `#786047`, and rarely `#987B58`.
4. Keep each grain compact and angular.
5. Reject or redraw any grain that becomes a long streak or a dominant bright patch.

### 5. Add creases

1. Scatter approximately 3–7 short creases.
2. Make each crease 2–5 pixels long and 1 pixel wide.
3. Use `#493126` or `#5B3B2B`.
4. Place creases near selected clods, but do not outline their full boundaries.
5. Avoid long connected cracks and any repeated directional alignment.

### 6. Build the height map

1. Start with a low-relief background at levels 10–14.
2. Give broad colour patches only small height changes.
3. Lower creases to levels 7–10.
4. Raise clod surfaces to levels 14–18, with occasional edge pixels reaching 20.
5. Allow rare compact peaks to reach levels 21–23.
6. Keep mineral grains close to the surrounding soil height.
7. Clamp all heights to the intended range and inspect for accidental tall, thin features.

### 7. Derive the side texture

1. Generate a separate 64 × 64 side layout using the same palette.
2. Use more connected clods and fewer isolated bright grains.
3. Increase the frequency of short, dark creases slightly.
4. Preserve the same broad value range as the top texture.
5. Do not add horizontal layers, long streaks, or regular bands.
6. Generate a restrained side height map, avoiding sharp protrusions along the block silhouette.

### 8. Final checks

- Confirm that every colour comes from the listed 12-colour palette.
- Confirm that all marks fit the specified pixel-size ranges.
- Confirm that the texture tiles seamlessly in both directions.
- Inspect repeated blocks side by side for obvious shared clusters or grid patterns.
- Check that dirt remains recognisably brown under bright and dim lighting.
- Check that no mineral grain, crease, or clod creates a tall, thin feature.
- Check the texture at native 64 × 64 resolution and at distance.
- Ensure the texture supports the faceted ground rather than competing with its angular geometry.