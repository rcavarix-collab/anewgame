=== FILE: REPORT.md ===

# Task A22: Thatch

## Files

- `thatch.md` — Pixel-art material brief for bundled straw roofing, covering palette, marks, top and side textures, height, variation, and generator recipe.

## Done when

* Every hex colour is listed: Yes — `thatch.md`, Palette.

* Every size is specified in pixels at 64 × 64: Yes — `thatch.md`, Marks and Generator recipe.

* Nothing depends on a tool we don't have: Yes — uses noise, scatter, colour ramps, and deterministic procedural generation.

* Worked-by-hand character, visible joints or courses, and bundled straw roofing: Yes — `thatch.md`, Marks and Top vs side.

* Readable at 64 pixels per block and clearly ours: Yes — `thatch.md`, Palette, Marks, and Must never look like.

## Assumptions

* ASSUMPTION: Thatch is a building material rendered on constructed surfaces, with a top-facing roof texture and a side-facing texture.

* ASSUMPTION: The building layer can use the same world-projected, repeating 64 × 64 texture convention as the ground, while orienting the texture to the building surface.

* ASSUMPTION: The material height map uses the same 36-level scale as the ground materials, even though thatch is not ground. Its values describe local relief for material blending or surface shading, not collision geometry.

* ASSUMPTION: Roof courses run horizontally across the roof surface, with straw bundles laid predominantly downward and outward from each course.

## Self-check

1. Hard rules kept: Yes

2. Everything I say about the game comes from the task's facts or is labelled ASSUMPTION: Yes

3. What I'm least sure of: How the building layer maps textures and uses height maps; the brief keeps the material definition independent of those implementation details.

## Questions for the owner or Claude

- Confirm the building layer's texture orientation and whether its material height map affects blending, shading, or both.

=== FILE: thatch.md ===

# Thatch — Bundled Straw Roofing

## Design intent

Thatch is a hand-worked roofing material made from tightly packed bundles of dry straw. Its identity comes from overlapping courses, broad bundles, short straw ends, and dark recessed joints.

The surface should feel assembled rather than grown. It is orderly enough to show the craft of laying the roof, but irregular enough to suggest individual bundles tied and trimmed by hand.

Use a warm, muted straw palette with earthy shadows. Keep the contrast strong enough for the courses and bundles to remain legible at 64 × 64 pixels, without making every straw strand a separate high-contrast line.

The signature mark language is:

* Broad, overlapping straw bundles.

* Short, broken straw tips.

* Shallow, irregular course boundaries.

* Occasional darker binding marks.

* Small clusters of straw fibres, grouped rather than evenly scattered.

Avoid thin, continuous lines. Individual fibres should appear as short, chunky marks within larger bundles, not as long parallel streaks.

## Palette

Use these 12 colours. All hex values are sRGB.

|
Hex

|

Role

|

Use

|
| --- | --- | --- |
|

`#59432B`

|

Deep shadow

|

Recessed gaps between courses and bundles

|
|

`#705333`

|

Shadow

|

Undersides of bundles and deeper straw overlaps

|
|

`#87643A`

|

Dark straw

|

Shaded bundle faces

|
|

`#9D7844`

|

Mid-dark straw

|

Main shadow-facing straw

|
|

`#B18C50`

|

Base

|

Dominant dry straw colour

|
|

`#C3A05F`

|

Base light

|

Main sun-facing bundle faces

|
|

`#D2B575`

|

Highlight

|

Broad lit areas on straw bundles

|
|

`#DFC68D`

|

Bright highlight

|

Sparse accents on exposed straw

|
|

`#A47D43`

|

Golden accent

|

Warm variation within bundles

|
|

`#8B874F`

|

Olive accent

|

Small amounts of aged, weathered straw

|
|

`#75634A`

|

Binding shadow

|

Recessed ties and compact binding marks

|
|

`#B39A6A`

|

Binding light

|

Occasional visible tie highlights

|

Keep the two brightest colours sparse. The dark brown should be reserved for genuine recesses rather than used as a general outline.

The olive accent should be subtle and infrequent. It represents weathering, not green vegetation.

## Marks

All sizes below are in pixels on a 64 × 64 texture.

### 1. Overlapping roof courses

* Shape: Broad, shallow bands with irregular lower edges. Each course is made of several adjoining bundle masses.

* Size: Each course is 10–16 pixels tall. Its lower edge varies by 1–3 pixels.

* Count: 4–5 courses per 64 × 64 block.

* Spread: Run across the full width. Stagger the course boundaries so they do not form a perfect horizontal grid.

* Treatment: Use a 1–2-pixel shadow seam beneath selected portions of each course, broken into segments rather than drawn as a continuous line.

The course pattern is the primary large-scale identifier. Keep it readable even when small bundle details are ignored.

### 2. Straw bundles

* Shape: Long, flattened, slightly tapered masses with rounded or blunt ends. Each bundle contains several chunky straw groups.

* Size: 8–18 pixels long and 4–7 pixels wide.

* Count: 8–14 visible bundles per block, distributed across the courses.

* Spread: Stagger bundle ends. Adjacent bundles should overlap visually, with no uniform spacing.

* Treatment: Give each bundle a main straw colour, a shadow edge, and an occasional 1–3-pixel highlight patch.

Bundles should read as coherent masses, not as individual sticks laid side by side.

### 3. Straw tips

* Shape: Short, uneven rectangular or tapered dashes, sometimes split into two adjacent pixels.

* Size: 2–5 pixels long and 1–2 pixels wide.

* Count: 12–24 marks per block.

* Spread: Concentrate them near exposed bundle ends and selected course edges. Keep their direction broadly aligned with their parent bundle.

* Treatment: Use mid-dark straw, base light, or highlight colours. Avoid placing every tip on the same edge.

These marks suggest cut straw ends without turning the texture into a field of scratches.

### 4. Fibre clusters

* Shape: Small groups of 2–4 chunky dashes or connected pixels, following the direction of a bundle.

* Size: Each cluster occupies 3–7 pixels in length and 2–4 pixels in width.

* Count: 6–10 clusters per block.

* Spread: Place them unevenly within bundle faces, avoiding course seams and avoiding regular rows.

* Treatment: Use neighbouring straw shades with moderate contrast. Most clusters should be visible as small patches, not as individual bright lines.

### 5. Binding marks

* Shape: Compact, slightly irregular ties that cross or cinch a bundle. They should look like darkened straw cord or a small recessed tie, not metal hardware.

* Size: 3–6 pixels long and 1–2 pixels wide.

* Count: 2–4 marks per block.

* Spread: Place them at irregular intervals, usually near a bundle boundary. Do not repeat at the same horizontal coordinate in adjacent courses.

* Treatment: Use binding shadow, with at most one binding-light pixel group per mark.

Bindings are secondary details. They should never compete with the course pattern.

### 6. Weathering patches

* Shape: Soft-edged, irregular clusters of straw colour variation.

* Size: 4–9 pixels across.

* Count: 2–5 patches per block.

* Spread: Cluster within individual bundles rather than crossing seams.

* Treatment: Use golden accent, olive accent, or mid-dark straw. Keep the patches low contrast and avoid green-dominant areas.

Weathering should add local variety without making the roof look stained or rotten.

## Top vs side

### Top texture

Use this for the main visible roof slope.

* Make the overlapping courses the dominant pattern.

* Show broad bundles whose long axes follow the roof's fall, with their exposed ends forming broken, staggered course edges.

* Use 4–5 courses per 64 × 64 block.

* Keep the bundle shapes large enough to read at normal gameplay distance.

* Show sparse fibre clusters and occasional bindings.

* Let the brightest straw appear on exposed upper faces, not as a continuous line along every course.

Visual read: a thick, layered roof of bundled straw, laid by hand.

### Side texture

Use this for near-vertical edges, eaves, and exposed thatch thickness.

* Show the cut ends and compressed layers of straw.

* Make the courses appear as stacked, shallow layers rather than broad roof-slope bundles.

* Use 5–7 uneven horizontal layers per 64 × 64 block.

* Each layer should be 7–12 pixels tall, with 1–3-pixel irregularities at its exposed edge.

* Add small, clustered straw ends along the layer faces, using 2–5-pixel marks.

* Use deeper shadows between selected layers to suggest compacted depth.

* Include occasional short binding marks, but keep them sparse.

Visual read: a thick edge made from tightly packed, overlapping straw layers.

The side texture must remain recognisably related to the top texture through its palette and chunky straw marks. Do not simply rotate the top texture by 90 degrees.

## Height

Use the 36-level height map, where 0 is the lowest relief and 35 is the highest.

|
Feature

|

Height level

|

Reason

|
| --- | --- | --- |
|

Deep course recesses

|

3–7

|

Keep gaps visually recessed

|
|

Compressed bundle interiors

|

8–13

|

Establish the packed body of the roof

|
|

Main bundle surfaces

|

14–20

|

Give the material broad, low relief

|
|

Raised bundle edges

|

21–25

|

Separate overlapping bundles

|
|

Occasional straw-tip clusters

|

26–29

|

Suggest loose, raised ends

|
|

Rare compact fibre clumps

|

30–32

|

Add local texture without excessive protrusion

|
|

Maximum isolated accents

|

33

|

Reserve for very rare, small clumps

|

Do not use level 34 or 35 for ordinary straw details.

Height should describe broad, shallow relief. It must not turn every fibre into a raised spike.

### Blending rules

* Keep the majority of pixels between levels 8 and 20.

* Make course recesses lower than the bundles they separate.

* Keep straw tips raised only when they form compact clusters.

* Avoid long, thin high-level marks. Such marks can become streaks when the material meets another material.

* Do not make the entire course boundary tall; use a low seam with occasional raised bundle edges.

* If the building layer does not use material height for blending, retain the map as a material detail definition without changing collision or silhouette.

## Variation

Variation must disrupt repetition without destroying the hand-laid course structure.

### Per-block changes

For each 64 × 64 block:

1. Vary course heights within the 10–16-pixel range.

2. Offset each course boundary by 1–4 pixels, independently of neighbouring courses.

3. Change bundle lengths and widths within their specified ranges.

4. Vary the number of visible bundles within the 8–14 range.

5. Shift bundle boundaries so that no bundle ends align vertically across adjacent courses.

6. Change the location and shape of straw-tip clusters.

7. Randomise the positions of binding marks, avoiding fixed coordinates.

8. Change the location, size, and hue of weathering patches.

9. Vary the balance between base straw and lighter straw by a small amount, keeping the overall material warm and readable.

### Repeat-seam rules

* The texture must tile seamlessly on all four edges.

* Any mark crossing an edge must continue at the opposite edge with matching shape, colour, and height.

* Avoid placing the same distinctive binding mark or bright cluster at corresponding positions in neighbouring blocks.

* Generate courses and bundles in a tile-aware coordinate space so edge continuity does not create visible duplicated motifs.

* Use a deterministic seed derived from the material's world-space tile coordinates, if available.

### Avoiding a visible grid

* Do not use a fixed course height or fixed bundle width.

* Avoid repeating the same sequence of straw colours across every course.

* Break up long seams into irregular segments.

* Ensure weathering and fibre clusters vary independently of the course pattern.

* Keep the large-scale course rhythm consistent enough to read as thatch, but vary the exact boundaries and bundle arrangements.

## Must never look like

* A smooth, uniform yellow roof.

* A stack of identical rectangular tiles.

* A woven basket or tightly interlaced mat.

* A field of thin, parallel lines.

* Long, needle-like straw spikes.

* Bright gold metal or polished brass.

* Green moss or living grass.

* Rotting, blackened, or heavily mould-stained straw.

* Photographic straw with fine, noisy detail.

* A recognisable copy of another game's material style.

## Generator recipe

Use a deterministic procedural generator with noise, scatter, colour ramps, and simple shape masks. No hand-painted assets or external textures are required.

1. Create the base canvas. Start with a 64 × 64 image filled with the base straw colour, `#B18C50`.

2. Build the course layout. Divide the texture into 4–5 horizontal courses. Choose each course height independently within 10–16 pixels, then adjust the final boundaries so the texture closes cleanly at 64 pixels. Add small irregularities of 1–3 pixels along the course edges. Ensure the top and bottom edges tile.

3. Create bundle masks. Within each course, scatter 2–4 broad bundle shapes. Use lengths of 8–18 pixels and widths of 4–7 pixels. Give each a slightly tapered or blunt end. Overlap neighbouring bundles and stagger their endpoints.

4. Shade the bundle masses. Assign each bundle a base colour from `#9D7844`, `#B18C50`, and `#C3A05F`. Use `#87643A` and `#705333` for selected shadow-facing edges. Reserve `#59432B` for small, recessed gaps.

5. Add course seams. Place broken 1–2-pixel shadow segments below selected course edges. Vary their lengths and omit portions. Do not draw a continuous outline around every bundle.

6. Add broad highlights. Scatter irregular highlight patches, 1–3 pixels across, using `#D2B575` and, rarely, `#DFC68D`. Keep highlights inside bundle faces, with occasional patches near exposed ends. Do not form continuous bright lines.

7. Scatter straw tips. Place 12–24 short marks, each 2–5 pixels long and 1–2 pixels wide, near exposed bundle ends. Align them approximately with the bundle direction. Use `#9D7844`, `#C3A05F`, or `#D2B575`, selected according to local lighting and contrast.

8. Add fibre clusters. Scatter 6–10 compact clusters of 2–4 connected dashes. Each cluster occupies 3–7 × 2–4 pixels. Keep clusters within bundle masks and use adjacent straw shades.

9. Add bindings. Place 2–4 compact tie marks, 3–6 × 1–2 pixels, near selected bundle boundaries. Use `#75634A`, with rare `#B39A6A` accents. Avoid a repeated placement pattern.

10. Add weathering. Use low-frequency value noise to select 2–5 patches, each 4–9 pixels across. Tint them subtly with `#A47D43` or `#8B874F`. Keep weathering inside bundle masks and preserve the warm straw base.

11. Generate the side variant. Build 5–7 stacked layers, each 7–12 pixels tall, with irregular exposed edges. Use the same palette, but increase the frequency of dark recesses between layers. Add clustered cut ends along the exposed faces. Do not merely rotate the top texture.

12. Generate the height map. Start from a low-relief field, mostly levels 8–20. Lower course recesses to levels 3–7. Raise bundle edges to 21–25, compact straw-tip clusters to 26–29, and rare fibre clumps to 30–32. Cap isolated accents at level 33. Keep height transitions compact and avoid long, thin high-level streaks.

13. Enforce seamless tiling. Generate marks in tile-aware coordinates. Any shape crossing an edge must wrap to the opposite edge with identical colour and height. Verify all four edges for both texture and height map.

14. Apply restrained variation. Use seeded noise to vary course boundaries, bundle dimensions, colour balance, and detail placement. Preserve the large-scale layered read. Ensure neighbouring blocks do not share the same seed or distinctive motif.

15. Check readability. Inspect the texture at its native 64 × 64 resolution. The courses and bundles must be immediately legible; fibre details and bindings should remain secondary. Remove details that merge into noise or create thin streaks.

16. Check lighting resilience. Test the palette under warm dawn, neutral daylight, warm dusk, and cool moonlight. Preserve the distinction between deep recesses, mid-value straw, and highlights. Do not depend on bright yellow alone to identify the material.

17. Check performance. Generate textures offline or during asset preparation, not per frame. The runtime should sample the resulting textures using the existing material pipeline, with no additional heavy effects.

## Acceptance checklist

* The top reads as overlapping, hand-laid straw courses.

* The side reads as thick, compressed straw layers.

* All marks are specified in pixels at 64 × 64.

* Every palette colour has an explicit hex value and role.

* Course seams remain broken and irregular.

* No tall, thin marks create unwanted blending streaks.

* Texture and height map tile seamlessly.

* The material remains readable under daylight and moonlight.

* The result looks like bundled straw roofing, not generic yellow planks or a woven mat.

END OF DELIVERY
