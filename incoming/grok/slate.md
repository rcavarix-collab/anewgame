# Slate — Material Brief

## Design intent

Slate is dark, fine-grained, layered rock that splits into broad, flat plates. Its pixel-art identity comes from overlapping angular slabs, short cleavage marks, and restrained mineral flecks.

The material should read as slate immediately, even in bright daylight. Lift the current near-black appearance substantially and remove the strong blue cast. The result is a neutral, slightly cool charcoal-grey, not blue-black.

The faceted terrain remains the main visual feature. Slate's marks should describe the rock surface without competing with the angular ground.

Texture size: 64 × 64 pixels per block.

## Palette

Use these 12 sRGB hex colours. Assign each colour the stated role.

| Hex | Role | Use |
|---|---|---|
| #555652 | Base | Main slate body; neutral charcoal-grey |
| #60615D | Light base | Broad, subtle plate surfaces |
| #484A47 | Shadow | Recesses between plates |
| #3D403D | Deep shadow | Small cracks and narrow crevices only |
| #696A64 | Highlight | Lit plate faces |
| #77786F | Bright highlight | Sparse, small exposed edges |
| #50534F | Cool-neutral shade | Slightly cooler transition, without a blue cast |
| #62635D | Warm-neutral shade | Slightly warmer transition |
| #454743 | Dark plate | Individual plate interiors |
| #5B5D57 | Mid plate | Secondary plate interiors |
| #707168 | Pale mineral fleck | Sparse mineral inclusions |
| #85857A | Rare edge glint | Tiny, isolated glints only |

Keep the palette tightly grouped around neutral grey. The cool-neutral and warm-neutral shades should differ subtly; neither should shift the material into obvious blue or brown.

Do not use pure black or pure white. The deepest colour, #3D403D, is reserved for narrow cracks and should occupy very little of the block.

### Value relationship to stone

ASSUMPTION: Stone's existing palette is lighter and more granular than the current slate.

Keep slate's main base (#555652) visibly darker than stone's typical midtone, but not dramatically darker. The slate base should sit in a readable middle-dark range rather than near black.

Use the following visual relationship as the target:
- Slate's base should be darker than stone's base or dominant midtone.
- Slate's plate highlights should approach the middle values of stone, allowing slate to remain readable in direct light.
- Slate's deepest cracks may be darker than stone's shadows, but must occupy only a small fraction of the texture.
- Avoid making the entire slate texture darker than stone's shadow range.

ASSUMPTION: If the current stone palette does not have a clear dominant midtone, compare the average luminance of the two textures and keep slate moderately darker, while preserving overlap between slate highlights and stone midtones.

## Marks

All dimensions are in pixels on a 64 × 64 texture.

### 1. Broad split plates

- Shape: irregular, angular polygons with mostly straight edges and occasional small steps.
- Size: 14–30 pixels across; 9–22 pixels high.
- Count: 5–9 visible plates per block, including partial plates cut off by the texture edges.
- Spread: distribute across the full block using jittered Voronoi regions. Vary region sizes so the plates do not form a regular tessellation.
- Colour: use #555652, #60615D, #454743, and #5B5D57.
- Contrast: adjacent plates should usually differ by one palette step, not several.
- Orientation: favour long edges at varied shallow and diagonal angles. Avoid repeated parallel bands.

These are the primary mark. They should make the surface look like rock that can split into sheets.

### 2. Cleavage seams

- Shape: short, broken angular lines following plate boundaries.
- Width: 1–2 pixels.
- Length: 4–15 pixels.
- Count: 7–13 per block.
- Spread: concentrate near plate edges, with occasional short interior seams. Leave many plate boundaries unmarked.
- Colour: #484A47 or, rarely, #3D403D.

Do not draw every plate boundary as a continuous dark outline. Break seams into separate segments, with small gaps of 2–6 pixels. Avoid intersections that create star-shaped cracks.

### 3. Flat mineral facets

- Shape: small, blunt polygons or clipped rectangles; never round, glitter-like dots.
- Size: 2–5 pixels across and 2–4 pixels high.
- Count: 12–24 per block.
- Spread: scatter irregularly across plate interiors. Use clustered scatter with a few isolated marks; do not distribute on a uniform grid.
- Colour: #62635D, #696A64, or #50534F.

These marks add fine-grained variation without making slate look sandy or crystalline.

### 4. Edge catches

- Shape: tiny angular strips or single-pixel steps along selected plate edges.
- Width: 1–2 pixels.
- Length: 3–8 pixels.
- Count: 4–8 per block.
- Spread: place on a minority of plate edges, with no consistent light-facing direction baked into the texture.
- Colour: #77786F or #85857A.

Keep the brightest colour rare. Edge catches should suggest a broken, flat surface, not polished or metallic rock.

### 5. Broad tone variation

- Shape: soft, irregular patches of value change, not separate drawn objects.
- Size: 12–28 pixels across.
- Count: 3–6 patches per block.
- Spread: overlap the plate layout subtly, using low-amplitude value noise.
- Colour: blend among #50534F, #555652, #62635D, and #60615D.

Keep variation broad and restrained. The texture must still read as slate rather than mottled stone.

## Top vs side

### Top texture

The top texture shows the broadest plate faces.

- Use 5–9 visible plates, with mostly broad, flat interiors.
- Keep cleavage seams discontinuous and relatively sparse.
- Use mineral facets sparingly, concentrated within plate interiors.
- Allow a few plate corners to chip or step inward by 1–3 pixels.
- Keep edge catches subtle; they must not resemble a bright outline around every slab.

The top should read as flat, split rock viewed from above, not as a pile of loose stones.

### Side texture

The side texture exposes stacked layers and broken plate edges.

- Use 6–11 visible horizontal-to-diagonal layer segments.
- Each layer segment should be 12–34 pixels long and 2–6 pixels high.
- Offset neighbouring segments vertically by 1–4 pixels.
- Break each layer into 2–5 sections, with gaps of 2–7 pixels.
- Use the same palette, allowing #484A47 and #3D403D to define recesses between layers.
- Add occasional blunt broken ends, 2–5 pixels across.
- Keep mineral facets less frequent than on the top: 6–12 per block, each 2–4 pixels across.

The side may have a stronger layered rhythm than the top, but the layers must remain broad enough to read as stacked plates rather than thin stripes.

ASSUMPTION: The side generator can use a separate orientation rule for its marks. If it cannot, use the same plate generator with a directional bias for the side texture.

## Height

The height map has 36 levels, from 0 (lowest) to 35 (highest).

Slate should remain predominantly low and relatively flat. Its defining structure is cleavage and layering, not tall protrusions.

| Feature | Height levels | Reason |
|---|---:|---|
| Deep cracks and recesses | 0–5 | Allow lower neighbouring material texels to show through at boundaries |
| Main plate interiors | 8–14 | Establish a consistent, low rock surface |
| Secondary plate lips | 14–18 | Give plate edges a slight raised profile |
| Broken corners | 18–21 | Add restrained local relief |
| Mineral facets | 10–16 | Keep small inclusions integrated into the surface |
| Rare edge catches | 18–22 | Provide subtle local height variation |

Do not use levels 23–35 for ordinary slate marks. Reserve the upper range for materials that genuinely need tall features.

The height map should follow the plate structure:
- Lower the narrow gaps between plates.
- Raise plate interiors only slightly above their recesses.
- Keep broad plate faces mostly level.
- Avoid tall, narrow ridges along cleavage seams.
- Feather transitions over 1–2 pixels where possible, without blurring the colour texture.

Slate must not produce tall, thin streaks when blended with neighbouring materials. No seam should form a continuous high ridge.

## Variation

Two adjacent blocks must not repeat the same plate arrangement or tone pattern.

Use a deterministic per-block seed, combined with the material seed, to vary:
- Voronoi cell positions by 3–8 pixels.
- Plate widths and heights by approximately 20–35%.
- Plate rotations by 10–25 degrees.
- Seam placement and length independently of the plate boundaries.
- Mineral-facet positions, count, and colour.
- Broad noise scale and offset.
- The starting vertical phase of side layers by 2–5 pixels.

Maintain similar average brightness and mark density between blocks. Variation should change the arrangement, not cause one block to appear conspicuously lighter or darker than its neighbour.

For seamless repetition:
- Generate the noise and Voronoi fields with wrapped coordinates across the 64 × 64 boundary.
- Clip marks at the edges and continue their wrapped counterparts on the opposite edge.
- Avoid placing a distinctive large plate corner exactly at a block corner.
- Ensure that no strong seam or layer line aligns continuously across several adjacent blocks.

## Must never look like

- Near-black rock with barely visible detail.
- Strongly blue or violet stone.
- Polished metal, glossy obsidian, or reflective glass.
- Rounded pebbles or loose gravel.
- A regular brick wall or evenly stacked masonry.
- Thin, continuous parallel stripes.
- A field of bright glitter or crystalline sparkles.
- Photographic stone, realistic scanned rock, or another game's texture style.

## Generator recipe

1. Create a 64 × 64 coordinate field with wrapped edges. Use a deterministic seed derived from the material seed and block coordinates.
2. Generate a low-amplitude value-noise field at a broad scale of 12–28 pixels. Use it to vary the base among #50534F, #555652, #60615D, and #62635D. Keep the average near #555652.
3. Generate a jittered Voronoi field with a typical cell width of 14–30 pixels and height of 9–22 pixels. Use the cells to establish broad, irregular plate regions. Vary cell sizes and positions; avoid uniform tessellation.
4. Assign each plate one of #454743, #555652, #5B5D57, or #60615D. Bias assignments toward #555652 and #5B5D57. Keep neighbouring plates close in value.
5. Add subtle internal tone patches using the broad noise field. Blend in #50534F, #62635D, or #696A64 without introducing high-frequency mottling.
6. Detect selected plate boundaries and draw 7–13 broken cleavage segments per block. Use 1–2-pixel widths and 4–15-pixel lengths. Choose #484A47 for most segments and #3D403D for only a few. Leave 2–6-pixel gaps between segments.
7. Scatter 12–24 blunt mineral facets, each 2–5 pixels across and 2–4 pixels high. Use #50534F, #62635D, or #696A64. Apply clustered scatter with irregular spacing.
8. Add 4–8 sparse edge catches, each 1–2 pixels wide and 3–8 pixels long. Use #77786F occasionally and #85857A very rarely. Do not outline complete plates.
9. For the top texture, keep the broad plate arrangement and sparse broken seams. For the side texture, use 6–11 horizontal-to-diagonal layer segments, each 12–34 pixels long and 2–6 pixels high, offset vertically and broken into sections.
10. Construct the 36-level height map separately from colour. Set recesses to levels 0–5, plate interiors to 8–14, plate lips to 14–18, broken corners to 18–21, and rare edge catches to 18–22. Do not use levels above 22.
11. Ensure the height map has no continuous tall ridges. Keep seams low and plate interiors broad, with transitions over 1–2 pixels where possible.
12. Check the rendered texture at its native 64 × 64 size. The plate structure must be clear without zooming, and the texture must remain visibly grey rather than blue-black.
13. Check the texture under bright, neutral lighting. Plate interiors, seams, and edge catches must remain distinguishable without relying on extreme dark-to-light contrast.
14. Compare the result with the existing stone material. Slate's base should be darker than stone's dominant midtone, while slate's highlights should approach stone's middle values. If necessary, adjust the slate palette's overall value, not its blue saturation.
15. Test adjacent blocks together. Repeated plate shapes, aligned seams, obvious value jumps, and continuous side-layer stripes must not appear.

All generator steps use value noise, Voronoi cells, scatter, palette assignment, and a 36-level height map. No hand-painted input, external assets, or additional tools are required.