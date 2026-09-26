=== FILE: REPORT.md ===

# Task A26: Rammed earth (S)

## Files

- `rammed_earth.md` — Pixel-art material brief for layered, hand-compacted earth walls, covering palette, marks, top and side textures, height, variation, exclusions, and a procedural generator recipe.

## Done when

* Every hex colour is listed: Yes — `rammed_earth.md`, Palette.

* Every size is in pixels at 64: Yes — `rammed_earth.md`, Marks and Generator recipe.

* Nothing depends on a tool we don't have: Yes — uses noise, scatter, colour ramps, and deterministic procedural variation.

## Assumptions

* ASSUMPTION: Rammed earth is a built-wall material with a dedicated texture, separate from the existing ground materials.

* ASSUMPTION: Wall faces are generated with world-projected top and side textures, and the building layer can supply stable coordinates for texture placement.

* ASSUMPTION: Courses are broad, compacted-earth lifts, with subtle horizontal construction seams rather than prominent masonry joints.

* ASSUMPTION: The material uses the existing 64 × 64 texture and 36-level height-map conventions, even though it is intended for built pieces rather than ground.

## Self-check

1. Hard rules kept: Yes.

2. Everything I say about the game comes from the task's facts or is labelled ASSUMPTION: Yes.

3. What I'm least sure of: How the building layer handles texture coordinates, corners, and course alignment across separate wall pieces.

## Questions for the owner or Claude

* Confirm whether building pieces share world-projected texture coordinates or use local per-piece coordinates.

* Confirm whether the building layer supports separate top and side textures and height maps.

=== FILE: rammed_earth.md ===

# Rammed Earth — Material Brief

## Identity

Rammed earth is a warm, compacted soil material for built walls. Its signature is broad, slightly irregular horizontal courses, pressed-earth grain, and occasional embedded grit.

The wall should feel shaped by human labour: dense, matte, and solid, with construction layers visible at a glance. It must remain recognisably earth rather than brick, stone, or exposed sedimentary rock.

The mark language is built from three elements:

* Compaction courses: broad, uneven horizontal bands that describe how the wall was built.

* Packed-earth mottling: compact, low-contrast patches that break up large flat areas.

* Embedded grit: sparse, chunky flecks that add scale and material character without making the surface look gravelly.

The courses are the dominant feature. Grain and grit support them; neither should compete with the layered construction.

## Palette

Use the following 12 colours. Keep the overall material warm and earthy, with enough value separation for the courses to remain readable under daylight, dusk, and moonlight.

|
Hex

|

Role

|
| --- | --- |
|

`#B99A70`

|

Base — warm, sunlit compacted earth

|
|

`#A98960`

|

Base variation — slightly darker ochre earth

|
|

`#C9AD83`

|

Highlight — softly lit upper faces of compacted bands

|
|

`#D8BE94`

|

Highlight accent — sparse pale earth grains

|
|

`#92734F`

|

Mid-shadow — recessed course surfaces

|
|

`#7B6044`

|

Shadow — deeper packed-earth variation

|
|

`#674F3A`

|

Deep shadow — narrow seams and sheltered recesses

|
|

`#C18C55`

|

Accent — warm ochre soil patches

|
|

`#A66E45`

|

Accent — iron-rich brown patches

|
|

`#8D795B`

|

Accent — muted mineral inclusions

|
|

`#D0B58A`

|

Accent — dry, compacted-earth flecks

|
|

`#756A52`

|

Accent — sparse grey-brown grit

|

Avoid using the deepest colours as large regions. The wall should remain medium-light overall, not become a dark brown mass.

## Marks

All sizes refer to pixels in a 64 × 64 texture.

### 1. Compaction courses

* Shape: Broad horizontal bands with gently uneven upper and lower boundaries.

* Thickness: 7–13 px per course.

* Count: 4–7 courses per 64 × 64 block.

* Spread: Run across most of the texture width. Offset boundaries by 1–3 px with low-frequency noise. Vary the thickness between neighbouring courses.

* Edge treatment: Mostly continuous, with occasional shallow notches or compacted bulges 1–3 px deep. Avoid sharp zigzags.

* Colour: Alternate between the base colours and mid-shadow. Use a 1–2 px shadow edge on selected course boundaries, not on every boundary.

* Readability: At least three courses should remain identifiable at normal viewing distance.

### 2. Course seams

* Shape: Short, subdued horizontal recesses, occasionally interrupted.

* Thickness: 1–2 px.

* Length: 8–28 px.

* Count: 3–7 seams per block, concentrated near course boundaries.

* Spread: Distribute unevenly. Do not create a continuous dark line across every course.

* Colour: `#92734F` for ordinary seams; `#674F3A` for a few tiny recessed sections.

* Purpose: Suggest compacted lifts meeting one another, not mortar between separate blocks.

### 3. Packed-earth mottling

* Shape: Blunt, irregular patches with rounded or stepped pixel edges. Use small clusters rather than isolated single pixels.

* Size: 3–9 px wide × 2–6 px high.

* Count: 10–18 patches per block.

* Spread: Cluster gently within courses, with uneven spacing. Keep the course boundaries visually dominant.

* Colour: Use `#A98960`, `#92734F`, and `#C18C55`. Keep adjacent patches close in value.

* Purpose: Break up broad bands without making the wall look spotted or noisy.

### 4. Embedded grit

* Shape: Squat, angular flecks; mostly short dashes and compact chips.

* Size: 1–3 px wide × 1–2 px high.

* Count: 8–16 flecks per block.

* Spread: Sparse and irregular, with no evenly spaced rows. Allow occasional pairs, but avoid dense clusters.

* Colour: Use `#8D795B`, `#756A52`, and `#D8BE94`.

* Purpose: Suggest coarse particles pressed into the earth. Keep flecks small and low-profile.

### 5. Compaction highlights

* Shape: Short, broad patches following the course direction, never thin continuous stripes.

* Size: 4–12 px wide × 2–4 px high.

* Count: 4–8 patches per block.

* Spread: Place unevenly along selected course tops. Leave many course edges unhighlighted.

* Colour: `#C9AD83` and, very sparingly, `#D8BE94`.

* Purpose: Give the compacted lifts a subtle pressed and smoothed surface.

## Top vs side

### Side texture

The side texture is the defining rammed-earth surface.

* Make the 4–7 horizontal courses immediately readable.

* Keep course boundaries broadly horizontal, with small irregularities rather than perfectly straight lines.

* Use subdued shadowing at selected boundaries to imply compacted lifts.

* Keep mottling and grit subordinate to the courses.

* Do not make the texture resemble stacked stone blocks, fired bricks, or geological strata.

### Top texture

The top texture represents the exposed upper surface of a rammed-earth wall.

* Use a denser, more even field of compacted-earth mottling.

* Retain faint, broad directional traces of compaction, but do not draw full-width horizontal courses.

* Use the same base and shadow colours as the side so adjoining faces belong to the same material.

* Allow slightly more pale compacted patches and sparse grit than on the side.

* Keep the surface matte and packed, with no grass, loose soil piles, or large stones.

ASSUMPTION: The material system supports distinct top and side textures for built pieces. If it does not, use the side texture as the fallback and keep the top-face course contrast subdued through the generator's existing face-orientation handling.

## Height

ASSUMPTION: The building material can use the same 36-level height-map convention as the ground materials. These levels describe texture relief, not the physical dimensions of the wall.

* Lowest — 0–5: Deepest seam pixels and small recessed pockets.

* Low — 6–11: Ordinary course boundaries and shadowed compacted depressions.

* Middle — 12–20: Main packed-earth surface and most mottling.

* Raised — 21–25: Occasional compacted bulges and broad surface irregularities.

* Highest — 26–28: Sparse, chunky grit inclusions and a few pressed-earth lumps.

* Unused — 29–35: Leave these levels unused for the standard material.

Keep the relief shallow. Rammed earth is a dense, compacted surface, not a field of protruding stones.

Course boundaries may be lower than the centres of their bands, but they must not become tall, thin ridges. This prevents narrow features from producing unwanted streaks when the material meets neighbouring materials.

The height map should reinforce the compacted surface without changing the silhouette of the built wall.

## Variation

Variation must prevent visible repetition while preserving the material's construction logic.

### Between neighbouring blocks

* Vary course thickness by 1–3 px.

* Shift the vertical position of the course pattern by 2–6 px, while keeping the overall rhythm coherent.

* Vary course-boundary irregularity by 1–3 px.

* Change the positions, sizes, and colours of mottling patches.

* Move and replace grit flecks rather than simply recolouring them.

* Vary the number of highlighted course sections.

* Change the relative proportions of the two base colours slightly.

### Avoiding obvious repetition

* Do not reuse an identical course arrangement in adjacent blocks.

* Avoid placing a dark seam at exactly the same height across every neighbouring block.

* Do not create a single continuous, perfectly aligned horizontal stripe at every block boundary.

* Keep the broad course rhythm consistent enough that the wall reads as one construction material.

* Use deterministic variation from the block's stable coordinates or seed, so the texture does not shimmer or change between frames.

ASSUMPTION: The generator can obtain a stable per-block seed or equivalent deterministic coordinate input.

### Palette stability

* Keep most pixels in the two base colours and the mid-shadow range.

* Reserve the brightest highlight for sparse flecks and selected course tops.

* Use the deepest shadow sparingly.

* Preserve warm brown and ochre relationships; avoid pushing the material toward red brick, grey stone, or yellow sand.

* Keep neighbouring colour values close enough that the material remains legible under changing sun, moon, and sky lighting.

## Must never look like

* Fired brick with regular rectangular units or mortar joints.

* A stone wall made from discrete blocks.

* Natural cliff rock or exposed geological sedimentary strata.

* Loose dirt, mud, or a churned soil surface.

* A uniformly smooth, featureless brown wall.

* A wall covered in dense gravel or protruding stones.

* A stack of perfectly straight, evenly spaced horizontal stripes.

* A photographic or realistic scanned-earth texture.

## Generator recipe

Use only the existing procedural building blocks: value noise, scatter, colour ramps, and deterministic variation.

1. Choose a stable seed. Derive a deterministic seed from the wall piece's stable coordinates or material seed. Use it to generate variation that remains fixed between frames.

2. Establish the base field. Generate low-frequency value noise across the 64 × 64 texture. Keep the amplitude modest so it creates broad packed-earth variation rather than clouds or large stains.

3. Build the side courses. Divide the texture into 4–7 horizontal bands, each 7–13 px thick. Perturb their boundaries with low-frequency noise by 1–3 px. Keep the bands broad and mostly continuous.

4. Colour the bands. Use the base colours `#B99A70` and `#A98960` as the dominant values. Use `#92734F` and `#7B6044` to distinguish selected courses and create restrained tonal depth.

5. Add interrupted seams. Scatter 3–7 short, 1–2 px-high seam marks near course boundaries. Use `#92734F` most often and reserve `#674F3A` for a few short recessed sections. Avoid outlining every band.

6. Add packed-earth mottling. Scatter 10–18 blunt patches, each 3–9 px wide × 2–6 px high. Bias their placement toward course interiors. Colour them with `#A98960`, `#92734F`, and `#C18C55`, with occasional `#A66E45` patches.

7. Add sparse grit. Scatter 8–16 compact flecks, each 1–3 px wide × 1–2 px high. Use `#8D795B`, `#756A52`, and occasional `#D8BE94`. Reject placements that form rows or dense clusters.

8. Add selective compaction highlights. Scatter 4–8 short patches, each 4–12 px wide × 2–4 px high, along a subset of course tops. Use `#C9AD83`; use `#D8BE94` only for rare small accents.

9. Build the top texture separately. Reuse the base noise and palette, but replace the full-width courses with denser mottling and faint, broad compaction traces. Keep the same overall warmth and avoid strong directional stripes.

10. Build the height map. Start from the middle height range, 12–20. Lower seams and depressions to 0–11, raise broad compacted bulges to 21–25, and allow only occasional grit to reach 26–28. Never create tall, narrow course ridges.

11. Apply deterministic block variation. Change the course phase, thickness, boundary noise, patch positions, and fleck placement for each block. Keep the course rhythm recognisable without aligning every boundary across neighbouring pieces.

12. Check the 64 × 64 result. Ensure the side reads as compacted earth at a glance, with visible but imperfect courses. Ensure the top reads as packed earth rather than a striped wall. Remove any repeated motifs, excessively dark seams, dense grit clusters, or tall thin marks.

13. Check lighting readability. Inspect the palette under bright, warm, cool, and low-light conditions. If the material collapses into a dark mass, reduce the area covered by `#674F3A` and `#7B6044` rather than brightening every highlight.

14. Keep generation inexpensive. Use the existing noise and scatter operations on the small texture. Do not add runtime geometry, costly shaders, or additional rendering passes for this material.

## Acceptance checklist

* The side texture reads as hand-compacted earth with broad, visible courses.

* The top texture reads as dense, packed earth without full-width stripes.

* The palette contains 12 explicitly listed hex colours.

* Every mark has explicit pixel dimensions and a per-block count or range.

* Height relief is shallow, with no tall, thin course features.

* Neighbouring blocks vary without losing a coherent construction rhythm.

* The texture remains readable at 64 × 64 pixels.

* The result is procedural, deterministic, and compatible with the stated generator tools.

* No photographic texture, copied artwork, or other game's visual language is used.

END OF DELIVERY
