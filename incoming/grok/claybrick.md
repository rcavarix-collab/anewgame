=== FILE: REPORT.md ===

# Task A20: Clay brick

## Files

- `brick.md` — Generator-ready visual brief for hand-worked fired clay bricks in a running bond, including palette, marks, top and side treatment, height, variation, and generation steps.

## Done when

* Every hex colour is listed: Yes — `brick.md`, Palette.

* Every size is in pixels at 64: Yes — `brick.md`, Marks and Generator recipe.

* Nothing depends on a tool we don't have: Yes — noise, scatter, colour ramps, and ordinary Python image operations suffice.

## Assumptions

* ASSUMPTION: A brick block uses a 64 × 64 texture, with the same world-projected, repeating-per-block mapping as the existing ground materials.

* ASSUMPTION: The future building layer supports a top texture, a side texture, and a height map, even though brick is not a ground material.

* ASSUMPTION: A brick block represents a wall surface. Its side texture carries the running-bond pattern, while its top texture represents the exposed upper faces of bricks.

* ASSUMPTION: Brick height values use the established 36-level scale, with 0 as the lowest and 35 as the highest. The height map is used for blending rather than geometric displacement.

* ASSUMPTION: Running-bond alignment is handled by the building layer or texture coordinates. If adjacent blocks cannot share a continuous bond, the generator must use edge-compatible patterns to prevent visibly broken joints.

## Self-check

1. Hard rules kept: Yes

2. Everything I say about the game comes from the task's facts or is labelled ASSUMPTION: Yes

3. What I'm least sure of: How the future building layer handles texture orientation, block-edge continuity, and whether brick height maps participate in material blending.

## Questions for the owner or Claude

* Does the building layer support continuous texture coordinates across adjacent blocks, or must each block generate edge-compatible brick courses?

* Are brick height maps used only for material blending, or can they also affect surface shading?

=== FILE: brick.md ===

# Clay Brick — Fired, Hand-Worked Running Bond

## Design intent

A warm, fired-clay brick surface made from individually readable rectangular bricks, laid in a running bond. Each brick should feel shaped and fired by hand: slightly irregular edges, restrained colour variation, and occasional shallow surface pits.

The signature is the staggered masonry rhythm: broad horizontal courses, with vertical joints offset by half a brick between neighbouring courses. At 64 × 64 pixels, the viewer should read brickwork immediately, even at a distance.

Keep the pixel-art treatment crisp and deliberate. The texture supports walkgrid's faceted world; it must not make the surface look like a photograph or a perfectly manufactured modern wall.

This is a built-piece material, not a natural ground material. Its marks should communicate fired clay and masonry, not soil, stone, or gravel.

## Palette

Use these 12 colours as the complete palette. Hex values are sRGB, with roles chosen to preserve separation between bricks and mortar under changing illumination.

|
Hex

|

Role

|
| --- | --- |
|

`#A94F35`

|

Base — principal fired-clay red

|
|

`#B96040`

|

Base light — warm, slightly lighter brick

|
|

`#93432F`

|

Base dark — deeper terracotta brick

|
|

`#C8754D`

|

Highlight — sun-warmed orange clay

|
|

`#D18A5B`

|

Highlight light — occasional warm fired edge

|
|

`#78392D`

|

Shadow — dark brick recesses and undersides

|
|

`#642F29`

|

Deep shadow — deepest joint pixels and pits

|
|

`#B87557`

|

Variation — muted red-brown brick

|
|

`#C18A69`

|

Variation light — subdued dusty clay

|
|

`#865044`

|

Variation dark — weathered brown-red brick

|
|

`#795D4C`

|

Mortar — warm, muted brown

|
|

`#9A7861`

|

Mortar light — occasional exposed mortar edge

|

Do not introduce additional colours during generation. Blend or select from these palette entries rather than creating arbitrary new RGB values.

## Marks

All dimensions below are in pixels on a 64 × 64 block texture.

### 1. Brick faces

* Shape: broad, squat rectangles with slightly uneven edges and occasional clipped corners.

* Typical face size: 27–30 px wide × 12–14 px high.

* Face count: approximately 6–8 visible brick faces per block, depending on course alignment and edge cropping.

* Spread: arrange in horizontal courses. Use a half-brick horizontal offset on every alternate course.

* Face colours: assign one of the base and variation colours to each brick. Keep neighbouring bricks distinct, but avoid a strict repeating colour sequence.

* Edge treatment: vary the face boundary by 0–1 px at a few corners and edges. Keep the overall rectangular silhouette clear.

* Internal marks: each brick may have 0–3 tiny colour patches, each 1–3 px across. These are broad, sparse variations, not dense noise.

### 2. Mortar joints

* Shape: continuous horizontal seams and staggered vertical seams.

* Horizontal joint thickness: 2–3 px.

* Vertical joint thickness: 2–3 px.

* Course count: 4–5 horizontal courses across the 64 px texture, with partial courses allowed at the top and bottom.

* Vertical joints: one per brick face, placed at the running-bond interval and offset by half a brick on alternating courses.

* Colour: primarily `#795D4C`, with occasional 1 px highlights in `#9A7861`.

* Edge treatment: joints should be slightly irregular, but remain continuous and legible. Avoid perfectly straight, mechanically uniform lines.

* Priority: joint readability is more important than small surface detail.

### 3. Fired-clay pits

* Shape: small square or L-shaped dark marks.

* Size: 1–2 px across, with rare 3 px marks.

* Count: 3–7 per block.

* Spread: scatter over brick faces only, with at least 3 px separation from the mortar wherever practical.

* Colour: `#78392D` or `#642F29`.

* Contrast: use sparingly. Pits should suggest rough fired clay, not holes or damage.

### 4. Clay flecks

* Shape: compact square or short rectangular flecks.

* Size: 1–3 px across.

* Count: 4–9 per block.

* Spread: distribute unevenly across brick faces. Avoid placing flecks in a uniform grid or in every brick.

* Colour: `#C8754D`, `#D18A5B`, `#B87557`, or `#865044`.

* Grouping: allow occasional pairs, but avoid long aligned streaks.

### 5. Hand-worked edge chips

* Shape: tiny notches or single-pixel steps along selected brick edges.

* Size: 1–2 px.

* Count: 0–3 per block.

* Spread: favour exposed brick corners and occasional face edges; never damage every brick.

* Colour: use a neighbouring face colour or `#78392D`.

* Limit: the brick silhouette must remain rectangular at a glance.

## Top vs side

### Side texture

The side texture is the primary brickwork view.

* Show the running bond clearly, with horizontal courses and alternating half-brick offsets.

* Keep mortar joints continuous and 2–3 px thick.

* Use the full brick palette, but keep most faces within the three base colours and two muted variation colours.

* Place pits and flecks inside faces, not across the mortar.

* Make the upper and lower boundaries tile cleanly if the building layer repeats the texture vertically.

### Top texture

The top texture shows the exposed upper faces of bricks.

* Use the same palette and brick-face language.

* Show broad rectangular brick tops separated by 2–3 px mortar joints.

* Use a staggered arrangement where the construction permits it, but do not force a side-wall course pattern onto a horizontal surface.

* Keep the face rectangles slightly broader and the internal marks sparser than on the side.

* Avoid drawing vertical wall joints as if they continued across the top surface.

### Shared rules

* Keep the material recognisably the same fired clay in both orientations.

* The side should read as masonry first; the top should read as exposed brick tops first.

* Do not add perspective, bevel shading, or directional lighting into the texture. The game's lighting should provide illumination.

* ASSUMPTION: The renderer selects top versus side texture based on surface orientation, as it does for ground materials.

## Height

Use the established 36-level height scale, from 0 to 35.

|
Feature

|

Height level

|

Reason

|
| --- | --- | --- |
|

Mortar bed

|

8

|

Recessed relative to the brick faces

|
|

Main brick face

|

18

|

Shared reference height

|
|

Slightly raised face patches

|

19

|

Subtle hand-worked variation

|
|

Small pits

|

7

|

Shallow depressions

|
|

Occasional edge chip

|

12

|

Slightly lowered damaged edge

|
|

Deepest joint pixels

|

5

|

Local recess, kept restrained

|

### Blending rules

* Keep height changes concentrated at mortar boundaries and tiny surface imperfections.

* Brick faces should be broad, mostly level regions.

* Do not give every fleck its own height peak.

* Never make mortar taller than the adjacent brick face.

* Avoid narrow, tall ridges along the joints. They could appear as streaks when another material meets the brick.

* Height describes relative surface relief for blending; it must not turn the texture into a deeply sculpted surface.

The main height contrast should be between the recessed mortar and the broad brick faces. This makes the masonry structure legible without introducing noisy relief.

## Variation

The texture must avoid looking like a repeated stamp.

### Between neighbouring blocks

* Vary the brick-face colour assignments while keeping the overall warm red-clay balance.

* Shift individual face widths by 1–2 px and heights by 0–1 px, without changing the intended course layout.

* Change the positions and counts of pits and flecks within the specified ranges.

* Vary the edge irregularities: some blocks should have almost intact bricks, while others have a few small chips.

* Vary mortar colour locally between `#795D4C` and `#9A7861`, without changing the joint thickness dramatically.

* Use a deterministic per-block seed so that the same block remains stable during play.

### Avoiding visible block boundaries

* Align the running bond across adjacent wall blocks wherever the building layer permits.

* Do not restart the course offset at every block if that would create a repeated seam or a broken bond.

* Keep the outermost brick shapes compatible with neighbouring blocks. If continuous coordinates are unavailable, generate edge-aware patterns with matching course heights and joint positions.

* Avoid placing distinctive pits, bright flecks, or chips at the same local coordinates in every block.

* Do not use a single repeated colour sequence such as light, dark, medium, light, dark, medium.

ASSUMPTION: The generator can receive a block coordinate or stable seed, allowing repeatable variation that does not flicker or change between frames.

## Must never look like

* A perfectly uniform, machine-made modern brick wall.

* A photographic or physically scanned masonry texture.

* A wall made from large stone blocks.

* A dirt or clay soil surface.

* A surface covered in random red noise.

* A set of isolated red rectangles with no readable mortar.

* Deeply eroded, crumbling ruins.

* A direct imitation of another game's materials or art style.

## Generator recipe

1. Create a 64 × 64 pixel canvas. Use only the 12 listed palette colours.

2. Establish the running-bond layout. Set horizontal mortar seams 2–3 px thick, with 4–5 courses across the texture. Use brick faces approximately 27–30 px wide and 12–14 px high.

3. Offset the vertical joints by half a brick on alternating courses. Preserve consistent course heights and bond alignment at texture boundaries.

4. Fill each brick face with one of the five base or variation colours. Use a seeded random choice with constraints that discourage identical colours in adjacent bricks.

5. Add slight rectangular irregularity. Adjust a few corners or edges by 1 px, and vary face dimensions only within the specified ranges.

6. Draw mortar in `#795D4C`. Add occasional 1 px accents in `#9A7861`, ensuring that the mortar remains visibly recessed and continuous.

7. Scatter 3–7 pits per block. Use 1–2 px marks, with rare 3 px marks, in `#78392D` or `#642F29`. Keep them inside brick faces.

8. Scatter 4–9 clay flecks per block. Use compact 1–3 px marks selected from the highlight and variation colours. Keep them sparse and avoid aligned streaks.

9. Add 0–3 tiny edge chips, each 1–2 px, using a neighbouring face colour or the shadow colour.

10. Generate the height map from the structural mask: mortar at level 8, brick faces at level 18, raised face patches at level 19, pits at level 7, edge chips at level 12, and deepest joint pixels at level 5. Clamp all values to the 0–35 range.

11. Generate top and side variants from the same palette and mark rules. The side prioritises the running bond; the top prioritises exposed brick faces.

12. Validate the final 64 × 64 output at native size. The running bond must read immediately, the mortar must remain legible, and the surface must not become noisy or photographic.

13. Check edge continuity against neighbouring blocks. Where the building layer supports shared texture coordinates, preserve the bond across block boundaries. Otherwise, use matching edge courses and joints.

14. Keep all variation deterministic for a given block coordinate or seed.

## Final acceptance checklist

* Uses a fired-clay palette with 12 explicit hex colours.

* Defines brick faces, mortar, pits, flecks, and chips in pixels at 64 × 64.

* Specifies both top and side treatments.

* Defines a 36-level height map with restrained relief.

* Explains block-to-block variation and edge continuity.

* Uses ordinary Python generator operations: masks, seeded random choices, scatter, and palette selection.

* Produces a hand-worked running bond that reads clearly at native texture size.

END OF DELIVERY
