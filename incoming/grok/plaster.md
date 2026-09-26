=== FILE: REPORT.md ===

# Task A21: Adobe and plaster

## Files

- `adobe.md` — A procedural pixel-art material brief for hand-worked adobe and plaster walls, including palettes, marks, top and side treatment, height, variation, and generator recipe.

## Done when

* Every hex colour is listed: Yes — `adobe.md`, Palette.

* Every size is in pixels at 64: Yes — `adobe.md`, Marks and Generator recipe.

* Nothing depends on a tool we don't have: Yes — uses noise, scatter, colour ramps, and simple deterministic placement.

* Smoothed earth walls, cracked and patched: Yes — `adobe.md`, Marks and Generator recipe.

* Joints or courses are visible: Yes — `adobe.md`, Marks and Generator recipe.

* Readable at 64 pixels per block: Yes — `adobe.md`, Marks.

* Clearly original and consistent with walkgrid's faceted style: Yes — `adobe.md`, Must never look like and Generator recipe.

## Assumptions

* ASSUMPTION: Adobe and plaster share one material definition, with a selectable adobe or plaster palette and mark balance, rather than requiring separate material IDs.

* ASSUMPTION: Built pieces use a wall-facing texture projection and do not need the ground material's top-versus-side steepness rule.

* ASSUMPTION: The height map for this material is used for material blending or surface relief where supported; actual wall geometry and mortar depth are handled elsewhere.

## Self-check

1. Hard rules kept: Yes

2. Everything I say about the game comes from the task's facts or is labelled ASSUMPTION: Yes

3. What I'm least sure of: Whether the building layer supports distinct material variants, wall-specific texture projection, or visible height relief.

## Questions for the owner or Claude

* Should adobe and plaster be separate material variants, or should the generator expose them as two presets of one material?

* Does the building layer use the same 36-level height-map convention as ground materials?

=== FILE: adobe.md ===

# Adobe and plaster — material brief

## Purpose and identity

A hand-worked earth wall: compacted, smoothed surfaces, visible courses, softened cracks, and patches of repair. It should feel shaped by a person using simple tools, not poured as a perfectly uniform slab.

The signature mark language is broad earthen courses, shallow seams, blunt cracks, and irregular repair patches. Keep the wall calm at a distance. At 64 × 64 pixels per block, the courses and patches should read as deliberate construction, while small surface variation gives the wall life up close.

The texture is projected in world space and repeats every block, but should not look like a tiled stamp. Use the same broad visual family for adobe and plaster, with different colour ramps and mark frequencies.

ASSUMPTION: Treat adobe and plaster as two generator presets for one material definition. If the building layer requires separate material IDs, use the same recipe and assign each preset its own palette.

## Palette

Use one palette per preset. Every colour is an explicit hexadecimal RGB value. Keep contrast moderate: the wall must remain legible in bright sun, warm twilight, and cool moonlight without relying on hue alone.

### Adobe palette

|
Hex

|

Role

|
| --- | --- |
|

`#A98259`

|

Base: warm, muted earth

|
|

`#96734F`

|

Base variation: shaded earth

|
|

`#B9956B`

|

Base variation: sun-warmed earth

|
|

`#806044`

|

Course seam and recess shadow

|
|

`#70533D`

|

Deep crack and deepest seam

|
|

`#C4A47A`

|

Broad smoothed highlight

|
|

`#D0B48B`

|

Small warm highlight

|
|

`#B18A62`

|

Patch base

|
|

`#8D765B`

|

Patch shadow

|
|

`#C1A27B`

|

Patch highlight

|
|

`#A27650`

|

Exposed, slightly darker earth

|
|

`#D8C19A`

|

Rare pale fleck

|

### Plaster palette

|
Hex

|

Role

|
| --- | --- |
|

`#C8BFA9`

|

Base: warm mineral plaster

|
|

`#B5AD99`

|

Base variation: shaded plaster

|
|

`#D7CEB8`

|

Base variation: light plaster

|
|

`#928A79`

|

Course seam and recess shadow

|
|

`#746E62`

|

Deep crack and deepest seam

|
|

`#E2D9C4`

|

Broad smoothed highlight

|
|

`#EEE5D1`

|

Small pale highlight

|
|

`#B7AA91`

|

Patch base

|
|

`#9C927F`

|

Patch shadow

|
|

`#D0C4AC`

|

Patch highlight

|
|

`#A89B82`

|

Exposed substrate

|
|

`#DCD1BB`

|

Rare pale fleck

|

Use the adobe palette for earth-coloured walls and the plaster palette for pale, lime-like finishes. Do not blend the two palettes randomly within a block. Patches may expose the substrate colour, but should remain within the selected palette.

## Marks

All dimensions below are in pixels on a 64 × 64 texture. Counts are per block. Use deterministic scatter and low-frequency noise so that the texture can be regenerated consistently.

### 1. Broad courses

* Shape: irregular horizontal bands with slightly wavy, softened boundaries.

* Height: 5–10 px per course.

* Width: 64 px, continuing across the texture edges.

* Count: 5–9 courses per block.

* Spread: mostly horizontal; vary each boundary by 0–2 px over a 16–32 px horizontal distance.

* Appearance: alternate subtle base shades between courses. Use a 1 px shadow edge on selected boundaries, not every boundary.

* Construction logic: allow occasional offset joints, but avoid making every course a perfect brick row.

Courses should be visible at a glance without becoming dark stripes. A seam is a shallow tonal change, not a black outline.

### 2. Softened cracks

* Shape: branching, angular lines with blunt ends and occasional short forks.

* Width: 1–2 px; rare sections may reach 3 px.

* Length: 5–18 px.

* Count: 2–5 main cracks per block, with 0–2 short branches per main crack.

* Spread: irregular and sparse; avoid aligning cracks with every course seam.

* Colour: deep crack colour at the centre, with an optional 1 px lighter or darker edge.

* Form: use 2–5 px line segments with small directional changes. Avoid perfectly straight vertical or horizontal cracks.

* Frequency: leave large uncracked areas. Cracks should not form a dense network.

Cracks should appear as surface splits in smoothed earth, not as sharp black lightning bolts.

### 3. Repair patches

* Shape: rounded, uneven polygons with 5–10 corners; avoid circles and rectangles.

* Size: 7–18 px across, with occasional patches up to 22 px.

* Count: 2–5 patches per block.

* Spread: clustered loosely, but do not overlap so much that the wall becomes mottled.

* Interior: mostly a single patch colour with 1–3 low-contrast variation marks.

* Edge: a partial 1 px darker edge on one or two sides, not a complete outline.

* Optional repair seam: 1–3 px wide, short, and subdued.

Patches should suggest local repairs or changes in finish. They must not resemble masonry blocks or decorative inlays.

### 4. Smoothing strokes

* Shape: broad, low-contrast smears or shallow trowel marks, represented as short stepped bands.

* Size: 4–12 px long and 1–3 px thick.

* Count: 5–12 per block.

* Spread: mostly horizontal or gently diagonal, with no repeated directional pattern.

* Contrast: one or two palette steps from the local base.

* Placement: avoid placing every stroke at the same height or along course boundaries.

These marks provide the hand-smoothed finish. Keep them broad and quiet, never hairline scratches.

### 5. Exposed earth and worn edges

* Shape: irregular, blunt-edged areas where the finish has worn thin.

* Size: 3–9 px across.

* Count: 2–6 per block.

* Spread: favour course boundaries, patch edges, and occasional isolated spots.

* Appearance: use the exposed-earth colour with one neighbouring shade; avoid bright, high-contrast speckling.

### 6. Fine surface variation

* Shape: small, blocky colour clusters rather than individual noise pixels.

* Cluster size: 2–4 px across.

* Count: 8–18 clusters per block.

* Spread: low-density and irregular, with no evenly spaced distribution.

* Contrast: adjacent colours should differ by a modest tonal step.

This variation should disappear into the overall wall appearance at a distance. It must not turn the surface into gravel, stone, or a speckled stucco.

## Top vs side

ASSUMPTION: The building layer supports a wall-facing projection rather than the ground material's top/side selection based on face steepness.

* Wall-facing surfaces: use the full course, crack, patch, and smoothing recipe.

* Top-facing surfaces: use the same palette, but reduce the visibility of horizontal courses. Keep patches and smoothing marks; add only a few broad, shallow wear areas. Do not turn the top into a miniature ground texture.

* Narrow exposed edges: preserve the base colour and broad course bands where resolution permits. Simplify small patches and fine variation before reducing the course readability.

If the renderer only supports one texture per material, use the wall-facing version everywhere. Keep the palette and mark language consistent.

## Height

ASSUMPTION: Use the project's 36-level height convention if the building layer accepts the same height-map format. Otherwise, treat these values as relative relief guidance for the wall material.

The wall should remain mostly smooth. Height variation represents shallow surface relief, not the actual depth of the building or the thickness of its courses.

|
Feature

|

Relative height

|

Reason

|
| --- | --- | --- |
|

Smooth base

|

17–19

|

Dominant, quiet surface

|
|

Broad smoothing strokes

|

18–20

|

Slight raised or compressed finish

|
|

Repair patch interior

|

17–20

|

Nearly flush with surrounding surface

|
|

Patch edge

|

15–18

|

Slightly recessed or worn boundary

|
|

Course seam

|

13–16

|

Shallow separation between courses

|
|

Crack interior

|

10–14

|

Recessed split

|
|

Exposed worn area

|

15–18

|

Worn finish, close to the base

|

Keep all height transitions soft or stepped over 1–2 px where the format permits. Do not create tall, thin ridges along course seams, cracks, or patch edges. Such features would create distracting streaks or exaggerated relief.

The actual geometry of the wall and any deep joints belong to the building system, not this texture.

## Variation

The texture repeats every block, so variation must be designed into the generator rather than added as a uniform random overlay.

* Offset the course pattern vertically by 0–8 px between neighbouring blocks.

* Vary course heights by 1–3 px while preserving the overall 5–9 course count.

* Change the position, length, and branching of cracks between blocks. Never let a crack continue seamlessly across several block boundaries by default.

* Vary patch count by up to 2 from the preset's nominal count, within the specified limits. Change patch silhouettes, not just their colours.

* Shift smoothing strokes and exposed-earth marks independently of the course pattern.

* Use low-frequency value noise with a broad scale of 16–32 px, plus a smaller 4–8 px variation layer at low amplitude.

* Keep the palette fixed within a wall section. Avoid alternating entire blocks between conspicuously different light and dark values.

* If adjacent blocks use the same preset, vary their base value by at most one palette step and avoid identical mark placement.

* If the generator has a world-coordinate seed, use it to make neighbouring blocks differ deterministically while keeping the material continuous in its broad colour family.

The goal is a wall that reads as one continuous construction, not a checkerboard of unrelated tiles.

## Must never look like

* Perfectly uniform, factory-smooth plastic.

* A regular brick wall with identical rectangular units.

* A stone wall made of separate blocks.

* A dense web of black cracks.

* A surface covered in tiny pebbles or photographic grit.

* A high-contrast mosaic of patches.

* A set of repeated horizontal stripes with identical spacing.

* A wall with tall, thin ridges along every course.

* A copied texture or recognisable style from another game.

## Generator recipe

1. Choose the preset. Select either the adobe or plaster palette. Keep that palette consistent across the wall section.

2. Create the base field. Fill the 64 × 64 texture with the base colour. Add low-frequency value noise with a broad scale of 16–32 px, using the base-variation colours sparingly. Add a smaller 4–8 px noise layer at low amplitude.

3. Lay out the courses. Divide the block into 5–9 broad horizontal courses, each 5–10 px high. Use slightly irregular boundaries that drift by 0–2 px over 16–32 px. Vary the shade of each course subtly. Add a 1 px seam to selected boundaries, leaving other boundaries soft.

4. Break up the joints. Add occasional short vertical or angled joint marks, 1–2 px wide and 3–8 px long. Offset them irregularly. Do not align them into a regular brick pattern.

5. Place cracks. Scatter 2–5 main cracks, each 5–18 px long and 1–2 px wide. Build each from short 2–5 px stepped segments. Add 0–2 short branches where useful. Use the deep crack colour sparingly and avoid a dense network.

6. Place repair patches. Scatter 2–5 irregular polygons, 7–18 px across, with occasional patches up to 22 px. Fill them mostly with one patch colour, add 1–3 subtle interior variations, and shade only part of the boundary.

7. Add smoothing marks. Scatter 5–12 stepped strokes, 4–12 px long and 1–3 px thick. Keep them low contrast and vary their direction slightly.

8. Add wear. Place 2–6 worn areas, 3–9 px across, mostly near course seams or patch edges. Use exposed-earth colours with subdued contrast.

9. Add fine clusters. Place 8–18 clusters, 2–4 px across, using nearby palette colours. Avoid isolated single-pixel noise except where a few pixels are needed to sharpen a mark.

10. Construct the height map. Start with a mostly flat value of 17–19. Apply the relative heights in the Height table: patches nearly flush, seams and cracks recessed, smoothing marks only slightly raised. Keep transitions broad enough to avoid thin, tall ridges.

11. Make the texture tile. Ensure courses and base variation meet cleanly at all four edges. Avoid placing distinctive cracks or patches so that they form obvious repeated motifs at the seams. If a mark crosses an edge, continue it consistently on the opposite edge.

12. Generate neighbouring blocks. Use a deterministic world-coordinate seed. Offset courses by 0–8 px, alter mark positions and silhouettes, and vary the base by no more than one palette step. Keep the overall material family coherent.

13. Check at native size. Inspect the 64 × 64 result without enlargement. Courses should read immediately, cracks should remain sparse, and patches should be distinct but subdued. Reject outputs that look like brickwork, stone masonry, gravel, or a repeated striped pattern.

## Final acceptance checks

* The selected preset uses only its listed palette.

* Every listed mark is sized in pixels at 64 × 64.

* Course bands are visible without becoming heavy stripes.

* Cracks are sparse, stepped, and not hairline-thin.

* Repair patches have irregular silhouettes and partial edges.

* Height relief remains shallow, with no tall, thin features.

* Adjacent blocks differ in mark placement and course offset without forming a grid.

* The texture reads as hand-worked adobe or plaster at native resolution.

END OF DELIVERY
