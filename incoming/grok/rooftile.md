# Roof Tile — Material Brief

## Purpose and visual identity

A material for built structures: overlapping, hand-worked fired-clay roof tiles.

The signature is a field of broad, slightly irregular tiles, each with a softly bowed body, a visible lower edge, and dark recessed joints. Tiles read as individual pieces rather than a continuous striped surface.

The mark language is **rounded rectangles, shallow arcs, and short dark joints**. Keep the silhouettes chunky and the surface marks sparse. Small irregularities suggest hand production without making the roof look damaged or ancient.

At 64 × 64 pixels per block, the viewer should immediately read a roof made from overlapping fired tiles. The tile courses must remain visible at a distance, but the individual surface marks should merge into a coherent warm surface rather than visual noise.

Use crisp, hard-edged pixel shapes. Avoid photographic clay, smooth gradients, and excessive fine detail.

## Palette

Use the following 12 colours. Every colour is an explicit sRGB hex value.

| Hex | Role | Use |
|---|---|---|
| `#713E32` | Deep shadow | Recessed joints and the darkest undersides |
| `#89503C` | Shadow | Tile overlap edges and shaded tile ends |
| `#A65F43` | Dark body | Cooler or less-lit fired clay |
| `#B86D49` | Base | Main tile body |
| `#C77C51` | Warm base | Main body variation |
| `#D58D5D` | Light body | Broad, restrained tile highlights |
| `#E2A873` | Highlight | Small sun-facing patches |
| `#EBC092` | Bright highlight | Rare, small fired-clay glints |
| `#A94E38` | Red accent | A few warmer tiles or patches |
| `#C16A45` | Orange accent | Occasional warmer tile variation |
| `#9A6B50` | Muted accent | Dusty, brownish tiles |
| `#D0A17B` | Pale accent | Sparse faded or lightly weathered patches |

Do not add pure black, pure white, or unlisted colours to the texture. Lighting is applied by the game, so the palette should retain separation under daylight, dusk, and moonlight.

The overall impression is warm terracotta, not vivid orange-red. Most pixels should use the three base colours. Dark outlines must be narrow enough that the roof does not become a dark grid.

## Marks

All dimensions below refer to pixels in a 64 × 64 texture block.

### 1. Tile bodies

- Shape: broad rounded rectangles with slightly bowed long edges. Use stepped pixel contours, not antialiased curves.
- Size: 18–23 px wide × 12–17 px high.
- Count: typically 8–12 visible tile bodies per block, depending on course offset and overlap.
- Distribution: arrange in staggered horizontal courses, with approximately 15–19 px between course starts.
- Silhouette: vary widths by 1–3 px and heights by 1–2 px. Keep each tile recognisably separate.
- Colour: primarily `#B86D49`, `#C77C51`, and `#A65F43`; vary the dominant colour between neighbouring tiles.
- Interior: a broad, mostly flat colour field. Add one or two small patches of adjacent palette colours, each approximately 3–7 px wide × 2–4 px high.
- Avoid outlining every tile on all four sides. The overlap edge and joint should define the silhouette.

### 2. Lower lips and overlap edges

- Shape: short, shallow arcs or stepped bands following the lower edge of each tile.
- Size: 9–19 px long × 2–3 px thick.
- Count: one per visible tile, with some edges partially hidden.
- Distribution: aligned with the staggered courses. Do not make every arc identical.
- Colour: mostly `#89503C` for the shaded lip, with occasional 2–5 px patches of `#D58D5D` on the upper-facing edge.
- Form: the lip should imply one tile resting over another. It is a broad edge, not a thin black contour.

### 3. Joints

- Shape: short, dark, stepped seams between adjacent tiles.
- Size: 2–4 px wide × 3–7 px long.
- Count: approximately 8–14 visible joint segments per block.
- Distribution: place at tile junctions and in short sections between tile edges. Break up long continuous seams.
- Colour: `#713E32` or `#89503C`.
- Keep joints visibly recessed. They should be darker than the tile bodies but should not form a perfectly regular checkerboard.

### 4. Hand-worked surface marks

- Shape: squat chips, small flattened patches, and short, irregular smears.
- Size: 2–5 px wide × 2–3 px high.
- Count: 5–10 marks per block.
- Distribution: scatter within tile interiors, keeping marks at least 2 px away from most visible edges. Leave some tiles nearly plain.
- Colour: use `#D58D5D`, `#E2A873`, `#A94E38`, or `#9A6B50`.
- Keep marks compact. Do not use long thin streaks.

### 5. Firing and weather variation

- Shape: irregular, softly stepped patches contained within tile bodies.
- Size: 4–9 px wide × 3–6 px high.
- Count: 3–6 patches per block.
- Distribution: place across different tiles, avoiding a repeated light-dark-light pattern.
- Colour: use `#C16A45`, `#9A6B50`, and `#D0A17B`.
- These patches should suggest small differences in firing and surface wear, not dirt painted over the entire roof.

### 6. Rare edge chips

- Shape: a small notch or missing corner in a tile silhouette.
- Size: 1–3 px wide × 1–2 px high.
- Count: 0–2 per block.
- Distribution: use sparingly and irregularly.
- Colour: reveal the darker tile beneath, usually `#89503C`.
- Most tiles should remain intact. The roof is maintained and functional, not ruined.

## Top vs side

### Top texture

The top texture shows the roof surface viewed from above or obliquely.

- Use the staggered courses as the primary structure.
- Tile bodies should occupy most of the surface.
- Make the lower lips and recessed joints clearly readable.
- Use small interior colour patches for hand-worked variation.
- Keep the course direction consistent within a single roof surface.
- The texture should not contain a simulated perspective vanishing point. Geometry and the camera establish perspective.

### Side texture

The side texture shows the exposed edge of a tiled roof, such as the eaves or a cut edge.

- Use a compact sequence of overlapping tile ends.
- Tile ends: 8–13 px wide × 5–9 px high.
- Show 4–7 visible tile ends per 64 × 64 block, depending on orientation.
- Use dark gaps of 2–3 px between some tile ends.
- Place a darker underside band, 3–6 px thick, beneath the tile ends.
- Use `#713E32`, `#89503C`, and `#A65F43` for the underside and recesses.
- Keep the exposed upper edges warm, using `#B86D49` and `#C77C51`.
- Do not draw the entire side as a stack of thin, evenly spaced horizontal stripes.

The side texture must read as the thickness and overlapping edges of real tile pieces, not as a second copy of the top texture.

## Height

Use the material's 36-level height map, with 0 as the lowest level and 35 as the highest.

- Recessed joints: levels 2–5.
- Tile body: levels 12–19.
- Raised central or upper tile surface: levels 18–22.
- Overlap lips: levels 24–29.
- Rare chipped or lifted corners: levels 30–32.
- Absolute maximum: level 32; reserve levels 33–35 for no feature in the standard material.

The main height change should occur at the tile boundaries and overlap lips. The tile bodies should remain comparatively even.

Use broad, connected height regions for each tile. Do not encode every colour fleck as a height change.

The height map represents the local relief of the tile surface, not the full physical thickness of the roof assembly. The building geometry supplies the overall roof shape.

### Blending rationale

The tile lips are raised enough to read as overlapping pieces. The recessed joints remain low so the seams are legible.

Keep the highest features broad and short. Never create long, thin, raised lines: these could appear as unwanted streaks when the material meets another surface.

If the building layer uses the same taller-texel-wins blending rule as ground materials, the tile lips should win locally without allowing the entire roof to overwhelm neighbouring materials.

## Variation

Variation must preserve the course structure while preventing the 64 × 64 block boundary from becoming visible.

### Between neighbouring blocks

- Offset the course pattern by 3–7 px between neighbouring blocks, while keeping courses aligned enough to read as continuous rows.
- Shift tile widths by 1–3 px and tile heights by 1–2 px.
- Vary the position of joint segments by 1–3 px.
- Change the dominant body colour of approximately 2–4 tiles per block.
- Move or replace at least half of the small interior marks between neighbouring blocks.
- Vary the number of firing patches within the stated range.
- Avoid placing the same distinctive chip, bright patch, or dark joint at the same relative position in repeated blocks.

### Continuity

The staggered course pattern should continue across block boundaries where the building texture mapping permits it. Do not independently restart every row at the edge of each block.

If texture generation cannot preserve a continuous course across block boundaries, use different course offsets and tile layouts for adjacent blocks. Ensure that no vertical or horizontal seam remains aligned across multiple repeated blocks.

### Large-scale colour balance

- Keep the overall surface predominantly mid-value terracotta.
- Distribute dark and light tiles rather than grouping all of one value together.
- Avoid a repeating checkerboard of tile colours.
- Keep the brightest colour rare.
- Maintain enough value separation that joints remain visible under dim light.

## Must never look like

- A modern, perfectly uniform factory-made roof.
- A roof made of flat, overlapping rectangles with no visible lips.
- A continuous orange surface with decorative stripes.
- A regular black grid.
- A roof of narrow, parallel ridges.
- Glazed ceramic with sharp mirror-like reflections.
- Rusted metal, wooden shingles, or stone slabs.
- A photographic scan or realistic high-frequency noise.
- A copy of another game's material style.
- A damaged ruin with missing tiles everywhere.

## Generator recipe

Use only the existing classes of generator operations: seeded randomness, value noise, scatter, colour ramps, and simple geometric masks. No external tools, learned models, or hand-painted assets are required.

1. **Establish the course layout.**
   - Create staggered horizontal courses using a nominal spacing of 15–19 px.
   - Alternate the horizontal offset of adjacent courses.
   - Use a deterministic seed per texture variant.
   - Continue the course phase across block boundaries when possible.

2. **Generate tile silhouettes.**
   - Place tile bodies 18–23 px wide × 12–17 px high.
   - Give each tile a shallow bowed contour using stepped pixel masks.
   - Vary dimensions by the ranges specified in Marks.
   - Clip overlapping portions so the lower tile is partially concealed by the tile above it.

3. **Assign tile body colours.**
   - Select from the 12-colour palette.
   - Prefer `#B86D49`, `#C77C51`, and `#A65F43`.
   - Use seeded variation so approximately 2–4 tiles per block receive a different dominant body colour.
   - Keep the bright highlight colours uncommon.

4. **Draw overlap lips.**
   - Add a 2–3 px thick stepped band along each visible lower edge.
   - Use `#89503C` for the shadowed edge.
   - Add small `#D58D5D` highlights to selected edges.
   - Break the bands at joints and vary their lengths.

5. **Draw recessed joints.**
   - Add 2–4 px wide × 3–7 px long dark segments at tile junctions.
   - Use `#713E32` and `#89503C`.
   - Avoid continuous outlines and uninterrupted lines spanning the block.

6. **Add surface variation.**
   - Apply low-frequency value noise to tile interiors, using broad regions rather than individual-pixel noise.
   - Scatter 5–10 compact hand-worked marks and 3–6 firing patches.
   - Use the specified palette only.
   - Keep marks within their tile masks and away from most edges.
   - Do not use long, thin features.

7. **Add rare chips.**
   - With low probability, remove a 1–3 px × 1–2 px notch from a tile corner.
   - Reveal the darker underlying tile colour.
   - Limit this to 0–2 chips per block.

8. **Build the height map.**
   - Assign low values to joints, medium values to tile bodies, and higher values to overlap lips.
   - Use broad masks matching the tile geometry.
   - Do not convert every colour mark into relief.
   - Clamp all values to the 0–35 range and keep the ordinary maximum at 32.

9. **Create the side texture.**
   - Generate a separate mask of overlapping tile ends.
   - Use 4–7 visible ends, each 8–13 px wide × 5–9 px high.
   - Add a 3–6 px underside band.
   - Keep the tile-end pattern irregular but recognisable.

10. **Check the result at native resolution.**
    - Inspect the full 64 × 64 block at 1:1 pixel scale.
    - Confirm that tile bodies, joints, and lips remain distinct.
    - Check repeated blocks side by side for obvious seams or a regular grid.
    - Check the texture under bright, warm, dim, and cool lighting.
    - Reject variants whose joints dominate the surface or whose courses resemble thin stripes.

11. **Keep generation reproducible.**
    - Use a stable seed for each generated variant.
    - Derive colour, silhouette, and mark variation from that seed.
    - Regenerate with a different seed when a layout is too regular, too dark, or too repetitive.