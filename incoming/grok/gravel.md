# Gravel

## Design goal

Loose, naturally scattered mineral fragments in a muted, varied grey-brown palette. The surface should read immediately as gravel: many small, individually legible angular stones, separated by narrow seams of darker grit.

The signature is the irregular distribution of pebble shapes, not a field of uniform dots or fitted cobbles. Keep the texture subordinate to the game's faceted ground. Individual stones should remain visible at 64 × 64 pixels per block, with enough variation to prevent adjacent blocks from forming an obvious repeating pattern.

Generate a seamless 64 × 64 pixel texture. Use crisp, hard-edged pixel-art marks. Avoid photographic detail, smooth gradients, and antialiased outlines.

## Palette

Use these 12 colours. All values are sRGB hex colours.

| Colour | Hex | Role |
|---|---|---|
| Deep crevice | `#393936` | Darkest gaps between tightly packed stones |
| Dark shadow | `#4B4A44` | Shadowed stone edges and occasional dark fragments |
| Cool charcoal | `#5D5D56` | Dark grey pebbles |
| Earth shadow | `#665C4E` | Warm shadowed stones and earthy seams |
| Base grey | `#77766B` | Main gravel field |
| Warm grey | `#898575` | Common mid-value stone |
| Brown-grey | `#827565` | Earth-stained fragments |
| Pale grey | `#A09D8E` | Lighter stone faces |
| Sand-grey | `#B1AA96` | Warm pale fragments |
| Light highlight | `#C3BDA9` | Sparse upper-facing facets |
| Mineral accent | `#A6A18A` | Occasional pale mineral inclusions |
| Rust accent | `#96704F` | Rare iron-stained fragments |

Keep the palette mineral and subdued. Most pixels should use Base grey, Warm grey, Cool charcoal, and Earth shadow. Use the lightest colours sparingly. Rust accent should occupy only a small number of pixels, never forming continuous lines.

Maintain clear value separation between the stones and their crevices. The darkest colour should form thin, broken seams rather than broad black areas. The overall texture must remain readable under both warm daylight and cool moonlight.

## Marks

All sizes below refer to pixels in the 64 × 64 texture.

### 1. Main pebbles

- Shape: irregular angular polygons, usually 5–9 pixels across and 4–7 pixels high.
- Quantity: approximately 22–34 per block.
- Distribution: scattered with controlled randomness. Allow occasional small clusters and occasional wider gaps.
- Shape language: squat, chipped, asymmetrical fragments. Vary the number of corners and the direction of the longest edge.
- Colour: mainly Base grey, Warm grey, Cool charcoal, and Brown-grey.
- Internal detail: give some larger pebbles a single 1–3 pixel lighter facet or a 1–2 pixel darker edge.
- Avoid outlines around every pebble. Use a mixture of direct colour boundaries and short, broken shadow edges.

### 2. Small chips

- Shape: angular fragments, 2–4 pixels across and 2–3 pixels high.
- Quantity: approximately 18–30 per block.
- Distribution: fill some of the spaces between main pebbles, but leave a few open patches.
- Colour: alternate between dark, middle-value, and occasional pale tones.
- Detail: mostly solid-colour marks. A chip should not need an outline or internal shading to read.

### 3. Fine grit

- Shape: compact pixel clusters, 1–2 pixels across.
- Quantity: approximately 20–40 clusters per block.
- Distribution: irregularly scattered, with local variation in density. Keep some clusters close to larger stones and leave others in small open pockets.
- Colour: primarily Deep crevice, Dark shadow, and Base grey.
- Purpose: break up empty spaces and soften the transition between neighbouring stones without creating a uniform speckle field.

### 4. Pale mineral flecks

- Shape: compact angular marks, 1–2 pixels across.
- Quantity: approximately 4–8 per block.
- Distribution: sparse and uneven. Never arrange them in rows or evenly spaced groups.
- Colour: Light highlight or Mineral accent.
- Purpose: give the gravel subtle mineral variety. Keep these marks small enough that they do not compete with the main pebbles.

### 5. Warm stained fragments

- Shape: small patches, 2–4 pixels across.
- Quantity: approximately 3–7 per block.
- Distribution: irregularly scattered, occasionally near another warm fragment but never joined into long bands.
- Colour: Rust accent or Brown-grey.
- Purpose: introduce natural variation without making the gravel look predominantly red or sandy.

### 6. Broken crevice marks

- Shape: short, irregular gaps, usually 1–3 pixels wide and 1–4 pixels long.
- Quantity: distributed around and between pebbles; do not force a fixed count.
- Distribution: follow some pebble boundaries, but interrupt the gaps frequently. Allow stones to touch directly in places.
- Colour: Deep crevice or Dark shadow.
- Purpose: separate the loose stones while avoiding a repeated network of outlines.

### Density and silhouette rules

- Main pebbles should occupy approximately 55–70% of the texture.
- Small chips should occupy approximately 8–15%.
- Fine grit and crevice marks should occupy most of the remaining area.
- Pale highlights and rust accents should remain sparse.
- No pebble should have the same shape as every other pebble.
- Avoid a uniform size distribution: most pebbles are small or medium, with a few larger fragments and many tiny chips.
- Keep individual stones chunky. Do not create long, thin slivers.

## Top vs side

ASSUMPTION: Use the same basic gravel mark language for both top and side textures unless the material system requires separate images.

### Top texture

- Use the full palette.
- Show the clearest angular pebble silhouettes.
- Use occasional pale facets on the upper-facing portions of larger stones.
- Keep crevices narrow and discontinuous.
- Preserve a varied distribution of stone sizes and open spaces.

### Side texture

- Retain the same loose-fragment structure; do not turn the gravel into sedimentary layers.
- Use fewer pale highlights and more middle-to-dark values.
- Let a few fragments appear partially embedded in the darker ground between them.
- Keep the same angular, scattered mark language, but reduce the contrast of the smallest grit.
- Do not add long horizontal strata or a stacked-cobble pattern.

If separate top and side textures are required, derive them from the same palette and mark rules rather than simply darkening every pixel uniformly. Keep the side recognisably gravel.

## Height

Use the existing 36-level height map, with levels 0–35.

ASSUMPTION: Level 0 is the lowest and level 35 is the highest.

Height represents local material prominence, not geometric displacement of the ground mesh.

- Deep crevices: levels 0–4.
- Embedded fine grit: levels 3–8.
- Small chips: levels 7–14.
- Main pebble bodies: levels 10–21.
- Raised pebble facets: levels 18–25.
- Occasional prominent pebble crowns: levels 24–29.
- Absolute maximum: level 29; do not use levels 30–35 for ordinary gravel.

Give most main pebbles a coherent, compact height footprint. The centre may be slightly higher than the edges, but avoid smooth, rounded height ramps. Use a few discrete steps to suggest a chunky stone surface.

Some pebbles should rise above the surrounding gravel, allowing their taller texels to win when gravel meets another material. Spread these prominent pebbles across the block rather than concentrating them at the edges.

Keep high areas broad enough to read as pebbles. Never assign maximum height to a 1-pixel-wide line, a thin outline, or a long narrow fragment. Tall, thin features can create unwanted streaks when materials blend.

Crevices should remain low, so neighbouring taller pebbles define a broken, natural boundary rather than a continuous raised ridge.

## Variation

The texture repeats every block, so each block needs meaningful internal variation while remaining seamlessly tileable.

- Use a different deterministic seed for each generated texture instance.
- Vary pebble positions, sizes, angles, corner shapes, and colours.
- Vary the balance of warm grey, cool grey, and brown-grey within the palette.
- Change the local density: one area may contain several close-set pebbles, while another has a few larger stones separated by grit.
- Vary the number and placement of pale mineral flecks and rust-stained fragments.
- Shift the broad colour balance subtly using low-frequency value noise. Keep the noise weak enough that it does not turn the texture into broad muddy patches.
- Avoid repeating the same pebble silhouettes or the same arrangement of highlights.
- Make the texture seamless at all four edges. Shapes crossing an edge must continue correctly on the opposite edge.
- Do not force a large pebble, bright accent, or dark crevice to occur at a fixed position.
- Ensure that neighbouring blocks differ in detail without producing an obvious brightness boundary between them.

The texture must not reveal the 64 × 64 block boundary through a repeated ring of dark crevices, a matching corner pebble, or a distinctive highlight cluster.

## Must never look like

- A regular cobblestone pavement.
- Uniformly sized round pebbles arranged in rows.
- A mosaic of equally sized polygons.
- Polished marble, gemstones, or crystalline facets.
- A sandy texture dominated by fine noise.
- A dark, nearly black field with indistinct stones.
- A field of isolated dots with no readable stone silhouettes.
- Long, thin streaks of raised material.
- Photographic gravel or realistic scanned texture.
- A texture whose repeated block boundaries are immediately visible.

## Generator recipe

1. Create a seamless 64 × 64 pixel canvas. Choose a deterministic seed for this texture instance.

2. Generate low-frequency value noise over the canvas. Use it to make subtle, broad changes in the local colour balance and pebble density. Tile the noise seamlessly. Keep the variation restrained.

3. Scatter approximately 22–34 main pebble centres. Use irregular spacing rather than a grid or a regular Poisson-like packing pattern. Permit small clusters and uneven gaps. Wrap placement across the texture boundaries so edge-crossing shapes continue seamlessly.

4. For each main pebble, choose a width of 5–9 pixels and a height of 4–7 pixels. Construct an irregular polygon with 5–9 vertices. Perturb the vertices enough to avoid repeated silhouettes, but keep the outline chunky and angular. Reject shapes that become thin slivers.

5. Assign each pebble a colour from the main mineral palette, weighted toward Base grey and Warm grey. Use local noise to bias some stones toward cool grey or brown-grey. Do not let adjacent pebbles all share the same value.

6. Add a compact, stepped height profile to each main pebble. Use levels 10–21 for the body, with a few pixels or small clusters reaching levels 18–25. Select a small subset of pebbles for broader raised crowns reaching levels 24–29. Keep these high regions compact.

7. Add a small number of 1–3 pixel light facets to selected larger pebbles. Place them asymmetrically, as short angular patches rather than consistent top-edge highlights. Use Pale grey, Sand-grey, or Light highlight. Do not outline every stone.

8. Scatter approximately 18–30 small chips, each 2–4 pixels across and 2–3 pixels high. Place them in gaps and occasional clusters. Give them heights of 7–14 and colours drawn from the same palette.

9. Scatter approximately 20–40 fine-grit clusters, each 1–2 pixels across. Use dark and middle-value colours. Assign heights of 3–8. Vary the spacing so the grit does not form an even speckle pattern.

10. Add approximately 4–8 pale mineral flecks and 3–7 warm stained fragments. Keep each mark compact. Avoid aligning accents, and ensure that warm marks do not join into streaks.

11. Construct the crevices from the spaces between stones, not from a complete outline around every polygon. Add short, broken dark gaps 1–3 pixels wide and 1–4 pixels long. Use levels 0–4. Interrupt gaps and allow some stones to touch.

12. Check the overall coverage. Aim for main pebbles to occupy 55–70% of the image and chips approximately 8–15%. Adjust overlaps and gaps to preserve readable stone silhouettes.

13. Make the colour and height maps seamless. Any shape or noise crossing an edge must wrap to the opposite edge. Avoid edge-specific darkening or highlighting.

14. Inspect the texture at native 64 × 64 resolution. If it reads as a uniform collection of dots, increase the silhouette variety and the number of angular facets. If it reads as cobblestone, break up the spacing, vary the sizes more, and remove consistent outlines.

15. Inspect the height map independently. Confirm that raised features are compact and pebble-shaped, that the highest areas are distributed across the block, and that no long, thin high feature can produce a streak at a material boundary.

16. Check the texture under warm and cool lighting. Preserve enough separation between the main stones, highlights, and crevices that the gravel remains legible at dawn, noon, dusk, and under moonlight.

17. Export the colour texture and its corresponding 36-level height map using the existing material pipeline. Do not add a new rendering effect or require any tool beyond the stated noise, scatter, colour-ramp, and pixel operations.