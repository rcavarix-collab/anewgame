# Worked slate slabs

## Material intent

A hand-worked, split-slate building material for floors and roofs. The surface should read as broad, thin stone slabs, not as a field of small stones or a single uninterrupted sheet.

The visual identity is **overlapping angular plates, chipped corners, and restrained recessed joints**. Use broad, quiet faces with a few short, angular split marks. The slab boundaries provide the main structure; the internal marks provide the natural cleft character.

Keep the palette lighter and less blue than the existing slate. Aim for weathered charcoal-grey, muted grey-brown, and pale mineral-grey highlights. The surface should remain readable under daylight, warm dusk, and moonlight.

This is a material for built pieces, not ground cover. Do not add grass, soil, loose gravel, or terrain-like scatter.

## Palette

Use these 12 colours. Hex values are sRGB.

| Hex | Role | Use |
|---|---|---|
| `#77766F` | Base | Main mid-value slate face |
| `#85847C` | Base light | Broad, softly varied slab faces |
| `#696A65` | Base dark | Alternate slab faces and restrained cloudy patches |
| `#5B5D59` | Shadow | Recessed joints and deep cleft marks |
| `#484B48` | Shadow deep | Small joint intersections and the deepest chips |
| `#96958B` | Highlight | Broad, sparse mineral-light planes |
| `#AAA99E` | Highlight light | Tiny pale cleft accents |
| `#6D6258` | Warm accent | Subtle grey-brown mineral variation |
| `#81776B` | Warm light | Occasional warm face variation |
| `#A19A8C` | Warm highlight | Rare muted warm flecks |
| `#626C6C` | Cool accent | Very restrained cool-grey variation |
| `#929B98` | Cool highlight | Rare neutral-cool edge glints |

Use the base colours for most pixels. Warm and cool accents should be sparse and subdued. Avoid making the whole material blue, purple, brown, or nearly black.

Suggested approximate coverage:
- Main base colours: 65–75%.
- Shadow and deep-shadow colours: 12–18%, concentrated at joints and clefts.
- Highlights: 8–12%.
- Warm and cool accents combined: 3–6%.

These are target ranges for the generator, not strict per-pixel quotas. Keep the darkest colour local to recesses rather than using it as a broad face colour.

## Marks

All dimensions below are in pixels on a 64 × 64 block texture.

### 1. Slab boundaries

The dominant mark is the joint between neighbouring slabs.

- Joint width: 2–3 px.
- Occasional widened joint or chipped opening: 4 px.
- Joint colour: primarily `#5B5D59`, with `#484B48` only at selected intersections.
- Joint path: mostly straight, with small angular offsets of 1–3 px.
- Avoid perfectly uniform, uninterrupted outlines around every slab.

For a typical 64 × 64 block, show 2–4 substantial slab faces, depending on the course layout. Do not subdivide the entire block into many small tiles.

### 2. Slab faces

- Typical visible face: 20–42 px across.
- Typical face length: 24–54 px.
- Face shape: irregular quadrilateral or broad polygon, with 4–7 corners.
- Edge offsets: 2–6 px from an ideal straight edge.
- Face-to-face value difference: usually one palette step, occasionally two.
- Keep most face interiors broad and calm.

Slabs should feel thin and split from larger stone, with angular edges rather than smoothly rounded outlines.

### 3. Split and cleft marks

- Short split marks: 5–14 px long, 1–2 px wide.
- Small angular chips: 2–5 px across.
- Shallow face patches: 6–16 px across.
- Use 2–5 short split marks per 64 × 64 block.
- Use 3–8 small chips per block.
- Use 2–4 broad, low-contrast patches per block.

Split marks should be short, discontinuous, and aligned loosely with the slab's natural cleavage. Do not create long, thin lines that cross several slabs.

### 4. Edge highlights

- Width: 1–2 px.
- Length: 4–12 px.
- Use 2–5 highlights per block.
- Place them along selected slab edges, not around every edge.
- Use `#96958B` most often; reserve `#AAA99E` for tiny accents.

Highlights should imply a chipped, uneven edge catching light, not a continuous bevel or cartoon outline.

### 5. Face variation

Use low-frequency patches to stop each slab from reading as a flat, identical tile.

- Patch diameter: 8–20 px.
- Contrast: subtle, normally one palette step from the surrounding face.
- Use broad patches rather than dense speckling.
- Allow some faces to remain nearly uniform.

Do not put a regular repeated pattern inside every slab.

## Top vs side

### Top texture

Use for horizontal floors and the upper-facing surface of roof slabs.

- Show broad, flat slab faces with angular outlines.
- Make joints the clearest structural marks.
- Keep split marks short and sparse.
- Use the full neutral-grey palette, with occasional warm mineral variation.
- Preserve broad quiet areas so the surface reads as laid slate rather than rubble.

For roofs, use staggered courses. Offset the joints between adjacent courses so they do not form a continuous grid. Keep individual slabs broad enough to read at 64 px.

For floors, use a less directional arrangement than the roof: irregular polygonal slabs or broad staggered courses. Avoid a perfect checkerboard.

### Side texture

Use for exposed slab edges, roof edges, and near-vertical faces.

- Show 2–4 horizontal or gently angled courses across a 64 × 64 block.
- Course boundary width: 2–3 px.
- Individual visible course height: 10–24 px.
- Add occasional short vertical breaks, 3–8 px long, where a slab edge is chipped or uneven.
- Keep the side darker than the top by using more `#696A65` and `#5B5D59`, but retain enough mid-value face area to read in shade.
- Use sparse 1–2 px highlights along a few exposed edges.

The side must communicate stacked, thin slate layers. Do not turn it into a brick wall: courses should vary in height, and vertical joints should be irregular and infrequent.

## Height

Use the existing 36-level height map. The values below describe relative relief, not physical dimensions.

- Broad slab faces: levels 17–20.
- Low face patches: levels 16–18.
- Raised split edges: levels 21–23.
- Small chipped ridges: levels 22–24.
- Recessed joints: levels 10–14.
- Deep joint intersections: levels 8–11.
- Tiny cleft lips: levels 21–22.

Keep most of each face close to the base level. The purpose of the height map is to suggest shallow, uneven split surfaces and recessed joints, not thick boulders.

Joints should remain narrow and recessed. Do not make every slab boundary a tall ridge. That would create an overly embossed, tiled appearance.

Do not use tall, thin height features. Split marks should be primarily colour marks, with only slight height changes. This avoids thin raised streaks when the material blends or is viewed at a distance.

## Variation

The generator must avoid obvious repetition when a 64 × 64 texture repeats.

- Vary the number of visible slab faces between 2 and 4 per block.
- Vary face widths and lengths within the ranges above.
- Shift joint positions by 3–8 px between neighbouring blocks.
- Vary course heights by 2–5 px.
- Alternate between polygonal and staggered-course arrangements, while keeping each individual block internally coherent.
- Vary face values by one palette step across neighbouring slabs.
- Change the position, length, and direction of split marks for every block.
- Use low-frequency value noise with a broad scale of 16–32 px.
- Add a second, weaker noise layer with a scale of 5–10 px.
- Keep the noise amplitude low enough that the slab boundaries remain the dominant marks.
- Avoid repeating the same joint intersection, chip cluster, or highlight placement at the same coordinates.

For a continuous roof or floor, neighbouring blocks should connect plausibly. Do not simply randomize each block independently if that would cause joints to terminate abruptly at block boundaries. Generate course and joint offsets from a shared layout seed or edge-compatible parameters.

ASSUMPTION: The building layer can provide neighbouring-piece or shared-layout information to the texture generator. If it cannot, use deterministic per-piece variation and avoid placing conspicuous joint endings directly on texture boundaries.

## Must never look like

- Nearly black, saturated blue slate.
- Polished black marble or glossy stone.
- Smooth, featureless grey concrete.
- A regular ceramic tile grid.
- A brick wall with uniform rectangular units.
- Loose gravel or small rubble.
- A photographic stone scan.
- Long, bright, continuous cleavage stripes.
- Deeply embossed, heavily beveled paving.
- A repeating checkerboard or obvious 64 px texture grid.

## Generator recipe

1. **Choose a layout.** Select either a broad irregular polygon layout or a staggered-course layout. For a 64 × 64 block, target 2–4 visible slab faces. For a roof, prefer staggered courses; for a floor, allow either layout.

2. **Establish the joints.** Draw 2–3 px recessed joint paths in `#5B5D59`. Use mostly straight segments with angular offsets of 1–3 px. Add occasional 4 px openings at chipped corners. Use `#484B48` sparingly at a few intersections.

3. **Build the slab faces.** Fill each face with `#77766F` or `#85847C`. Assign some faces `#696A65` to create variation. Keep each face a broad, coherent region rather than a mosaic of small patches.

4. **Apply low-frequency colour variation.** Generate value noise with a 16–32 px scale. Map small deviations to adjacent palette colours. Add weaker 5–10 px noise for limited local variation. Keep noise out of the deepest joint pixels.

5. **Add mineral patches.** Scatter 2–4 broad patches per block, each 8–20 px across. Use muted warm-grey or neutral-grey variants. Blend by selecting palette colours rather than introducing arbitrary new colours.

6. **Add cleavage marks.** Scatter 2–5 short marks per block, each 5–14 px long and 1–2 px wide. Keep them within a single slab face. Make most marks low-contrast; use darker marks more often than bright ones.

7. **Add chipped corners.** Scatter 3–8 chips per block, each 2–5 px across. Place some near joints and some within faces. Use shadow colours for cuts and mid-value colours for exposed chips.

8. **Add selective edge glints.** Place 2–5 highlights per block, each 4–12 px long and 1–2 px wide. Align them to selected slab edges. Leave most edges unhighlighted.

9. **Construct the side texture separately.** Lay down 2–4 irregular courses, each 10–24 px high. Use 2–3 px dark course boundaries, varying course heights and the positions of vertical breaks. Use a darker overall ramp than the top, but preserve visible midtones.

10. **Create the height map.** Start faces at levels 17–20. Lower joints to levels 10–14 and selected intersections to 8–11. Raise only occasional split edges and chips to levels 21–24. Keep the height changes shallow and avoid long, thin raised marks.

11. **Prevent visible tiling.** Change the layout, joint offsets, face values, and mark placements between blocks. Where pieces meet, use shared layout information or compatible edge parameters so courses and joints continue plausibly without creating a repeated grid.

12. **Check at native scale.** Inspect the material at 64 × 64 pixels per block. The broad slab faces and recessed joints must read immediately. If the surface looks too busy, remove internal marks before weakening the joints. If it looks too dark, lift the face colours rather than brightening the joints.

13. **Check lighting robustness.** Confirm that the material remains distinguishable under warm low-angle light, neutral daylight, and cool moonlight. Keep the palette's overall identity in the neutral-grey range; do not rely on blue saturation to make it read as slate.