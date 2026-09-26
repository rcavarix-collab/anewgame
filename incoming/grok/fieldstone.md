=== FILE: REPORT.md ===

# Task A17: Rough fieldstone wall

## Files

- `fieldstone.md` — A procedural pixel-art material brief for uncut, hand-fitted fieldstone masonry, including palette, stone shapes, joints, top and side treatment, height map, variation, exclusions, and generator recipe.

## Done when

* Every hex colour is listed: Yes — `fieldstone.md`, Palette.

* Every size is in pixels at 64: Yes — `fieldstone.md`, Marks and Generator recipe.

* Nothing depends on a tool we don't have: Yes — uses value noise, Voronoi cells, scatter, colour ramps, and ordinary pixel operations.

* Hand-worked, uncut stones with visible joints: Yes — `fieldstone.md`, Marks and Top vs side.

* Readable at 64 pixels per block: Yes — `fieldstone.md`, Marks and Generator recipe.

* Clearly original to walkgrid: Yes — `fieldstone.md`, Marks and Must never look like.

## Assumptions

* ASSUMPTION: A fieldstone block represents a section of a constructed wall, with the texture projected in world space at 64 × 64 pixels per block.

* ASSUMPTION: The future building layer can select a top texture and a side texture independently, following the existing material convention.

* ASSUMPTION: The height map is available to the building layer for material blending or relief, but does not physically displace the wall geometry.

* ASSUMPTION: Wall courses may be irregular and locally discontinuous, rather than following a strict brick bond.

## Self-check

1. Hard rules kept: Yes.

2. Everything I say about the game comes from the task's facts or is labelled ASSUMPTION: Yes.

3. What I'm least sure of: How the future building layer handles texture orientation, wall corners, and height-map blending.

## Questions for the owner or Claude

- Confirm whether the building layer supports independent top and side textures, and whether side textures can be rotated or mirrored to maintain plausible masonry around corners.

=== FILE: fieldstone.md ===

# Rough fieldstone wall

## Purpose and identity

A built-material texture for a wall assembled from uncut stones fitted together by hand. The stones are irregular, angular, and varied, with visible recessed joints. Their outlines should communicate weight and rough fitting, not precise cutting.

The signature mark is the uneven stone face: a broad, mostly solid shape with a few angular facets and a restrained highlight. Dark, broken mortar seams separate neighbouring stones. Occasional small chips and shallow pits add age without turning the wall into rubble.

The wall should read as masonry from a distance and as individual stones up close. At 64 × 64 pixels per block, the arrangement should feel deliberately composed but not mechanically repeated.

ASSUMPTION: The texture is projected in world space at 64 × 64 pixels per block, and the building layer can use separate top and side textures.

## Palette

Use these 12 hex colours. The palette is neutral, mineral, and slightly warm, with enough value separation to keep individual stones readable under changing daylight and moonlight.

|
Hex

|

Role

|
| --- | --- |
|

`#393A35`

|

Deep joint shadow

|
|

`#514F46`

|

Recessed mortar

|
|

`#69675B`

|

Stone underside and deep facet

|
|

`#807D6D`

|

Dark stone

|
|

`#96917E`

|

Main stone

|
|

`#ADA793`

|

Light stone

|
|

`#C2BAA4`

|

Pale stone highlight

|
|

`#D2C9B2`

|

Sparse brightest highlight

|
|

`#77796B`

|

Cool-neutral stone variation

|
|

`#8C8978`

|

Muted grey stone variation

|
|

`#A49D88`

|

Warm stone variation

|
|

`#B7AD96`

|

Warm light-stone variation

|

Keep the brightest colour sparse. Most pixels should use the main stone colours, not the highlight colours. Avoid a strong blue cast: this is ordinary weathered fieldstone, not slate.

Use the dark joint colours as narrow boundaries, not as broad black gaps. The darkest colour should occupy very little of the image.

## Marks

All dimensions below refer to a 64 × 64 pixel texture.

### 1. Main stone faces

* Shape: Irregular, closed polygons with 6–11 sides. Use unequal edge lengths and slightly bowed or stepped outlines. Avoid circles, perfect hexagons, and rectangles.

* Size: Most stones are 13–25 pixels wide and 10–20 pixels high. Occasional larger stones may reach 29 pixels wide or 23 pixels high. Small stones may be 7–12 pixels wide and 6–10 pixels high.

* Count: Aim for 7–13 substantial stone faces per 64 × 64 block, with partial stones entering and leaving at the texture boundaries.

* Spread: Use jittered Voronoi-like centres, with moderate variation in cell size. Bias some stones into short horizontal groupings, but avoid a uniform row of identical units.

* Outline: Separate stones with joints 1–2 pixels wide. Let a few joints widen to 3 pixels at irregular junctions. Keep the joints continuous enough to reveal the masonry structure.

* Face fill: Give each stone one dominant colour. Use one or two neighbouring palette colours for facets, not a noisy mosaic of many tiny colours.

The stone faces should be broad enough to read as individual masses at a glance. Their irregularity should come from their outlines and facet placement, not from excessive pixel-level noise.

### 2. Stone facets

* Shape: Broad angular patches following the implied plane of the stone. Use triangular or trapezoidal regions, sometimes with a short stepped edge.

* Size: 4–12 pixels across; 3–9 pixels high.

* Count: 1–3 facets per substantial stone. Leave some stones almost flat.

* Spread: Place facets asymmetrically, favouring one side or corner of each stone. Keep the implied light direction consistent within a texture.

* Contrast: Usually one palette step lighter or darker than the face. Use the deepest facet colour sparingly.

Facets should suggest rough, broken stone surfaces, not smooth bevels or glossy reflections. Do not outline every facet.

### 3. Chips and shallow pits

* Shape: Small angular notches, short dark cuts, or compact irregular spots.

* Size: Chips are 2–4 pixels across; pits are 1–3 pixels across.

* Count: 0–2 chips and 0–3 pits per substantial stone.

* Spread: Scatter unevenly, avoiding a regular pattern. Keep most marks away from the joint itself so the stone silhouette remains legible.

* Colour: Use the deep facet, dark stone, or a restrained light-stone colour. Bright chips should be rarer than dark ones.

Do not distribute pits uniformly. Some stones should have none; a few may have several.

### 4. Joint network

* Shape: Uneven, angular mortar seams that bend at stone junctions. Junctions should look fitted rather than like a clean geometric diagram.

* Size: 1–2 pixels wide for most joints; 3 pixels at occasional junctions or wider gaps.

* Count: One connected network separating all visible stones.

* Spread: Let the joints form irregular courses, with occasional short vertical joints and staggered junctions. Avoid long uninterrupted vertical seams.

* Colour: The recessed mortar colour forms the main seam. Use the deepest joint shadow only along selected edges and at a few junctions.

Joints are structural marks. Keep them visible even where neighbouring stones have similar colours.

## Top vs side

### Side texture

The side is the primary wall face.

* Use the full irregular-stone pattern, with visible horizontal tendencies but no rigid brick rows.

* Make course boundaries readable through slightly stronger horizontal joint segments.

* Stagger vertical joints between neighbouring courses. Avoid aligned vertical joints extending through multiple courses.

* Use the main stone palette across the whole face. Distribute warm and cool-neutral stones in small, uneven groups.

* Keep the darkest joints narrow and broken by occasional recessed corners.

* Facets should be broad and low-contrast enough that the wall reads as fitted stones rather than a pile of loose rocks.

### Top texture

The top shows the exposed upper surfaces of the same masonry.

* Use fewer visible stone faces: approximately 5–9 substantial faces per 64 × 64 block.

* Make stones somewhat broader, with typical widths of 15–27 pixels and heights of 11–21 pixels.

* Keep joints visible, generally 1–2 pixels wide.

* Reduce the number of dark side-facing facets. Use broad, quiet stone planes and occasional small chips.

* Do not add grass, moss, soil, or other ground-material marks by default.

ASSUMPTION: The top texture represents the upward-facing surface of a wall or masonry cap, rather than a separate decorative coping material.

## Height

Use a 36-level height map, where lower values represent recessed areas and higher values represent raised surfaces.

* Levels 0–5: Deepest joint intersections and a few narrow seam recesses.

* Levels 6–11: Main mortar joints and the deepest small pits.

* Levels 12–17: Shallow pits, chipped edges, and recessed facets.

* Levels 18–25: Main stone faces.

* Levels 26–31: Raised central planes and selected broad facets.

* Levels 32–35: Rare small stone corners or prominent raised chips.

Keep most of each stone face within levels 18–25. Facets should generally differ from the surrounding face by only a few levels. Use the top of the range sparingly.

The height map should reinforce the visual relief: joints are recessed, stone faces are raised, and chips or pits sit below the surrounding surface. Do not encode every colour boundary as a height discontinuity.

ASSUMPTION: The height map informs blending or surface appearance but does not change the wall's actual geometry.

## Variation

Variation should prevent a visible 64 × 64 repeating grid without destroying the continuity of the masonry.

* Generate stone centres and sizes from a seeded, low-frequency variation field.

* Vary the number of stones, their proportions, and their dominant colours between blocks.

* Shift course heights and vertical-joint positions by several pixels. Do not make every block begin or end with a complete stone.

* Allow partial stones to cross texture boundaries. Their shapes and colours must match across opposite edges when the texture tiles.

* Change facet placement and pit positions independently of the main stone layout.

* Vary warm and cool-neutral stones in small clusters, not in alternating stripes.

* Avoid repeating the same distinctive chip, large pale stone, or joint junction at a fixed position.

For seamless tiling, generate the stone layout on a periodic domain or explicitly reconcile the edges. The left and right edges must join, as must the top and bottom edges.

ASSUMPTION: The generator can use a fixed seed per material instance and can generate or reconcile tile boundaries.

## Must never look like

* Cut, polished, or uniformly rectangular blocks.

* A regular brick wall with identical courses and aligned joints.

* A wall made of perfectly round cobbles.

* Loose rubble with no stable fitted courses.

* A high-contrast black-and-white checkerboard of stones.

* Blue-black slate or polished marble.

* Photographic stone scans, realistic image noise, or fine-grained photographic texture.

* A direct imitation of another game's masonry.

* A noisy collection of tiny facets that loses the broad stone shapes at 64 pixels.

## Generator recipe

1. Create a 64 × 64 pixel tile and a separate 64 × 64 pixel height map with 36 discrete levels.

2. Establish a low-frequency variation field. Use it to vary stone size, orientation, colour selection, and course placement without creating visible bands.

3. Place jittered stone centres using a periodic layout. Use a Voronoi-like partition as a starting point, then perturb boundaries into uneven, angular outlines. Ensure the final layout wraps cleanly at all tile edges.

4. Introduce weak horizontal course tendencies. Let course boundaries wander by several pixels, and stagger vertical joints. Do not force every stone into a row or a rectangular cell.

5. Assign each stone one dominant colour from the middle of the palette. Choose neighbouring stones with related but non-identical colours. Use the warm and cool-neutral variants sparingly.

6. Draw the joint network over the boundaries. Make most joints 1–2 pixels wide, with occasional 3-pixel junctions. Use recessed mortar as the main colour and the deepest shadow only in selected narrow sections.

7. Divide selected stone faces into 1–3 broad facets. Use angular regions 4–12 pixels across and 3–9 pixels high. Keep facet contrast restrained and use a consistent implied light direction.

8. Scatter a small number of chips and pits within stone faces. Chips should be 2–4 pixels across and pits 1–3 pixels across. Leave many stones unmarked.

9. Assign height levels by structural role: joints low, faces mid-range, broad raised planes slightly higher, and rare chips or corners near the top of the range. Quantize all values to the 36 available levels.

10. Build the top texture from a broader, quieter stone layout with fewer faces and reduced side-facing facets. Build the side texture with more visible irregular courses and staggered joints.

11. Check the tile at native 64 × 64 size. Every stone should remain distinct, joints should remain visible, and the overall wall should read as hand-fitted masonry without relying on tiny details.

12. Check the repeated tile in a larger grid. Adjust edge continuity, colour grouping, stone size variation, and distinctive marks until no obvious repeating grid or repeated feature stands out.

13. Check the palette in light and dark conditions. Preserve separation between mortar and stone faces, and avoid relying solely on subtle colour differences to distinguish neighbouring stones.

The generator needs only the existing noise, scatter, cell-layout, colour-ramp, and pixel-drawing operations. No external assets, specialised tools, or sampled textures are required.

END OF DELIVERY
