# Timber planks — Material brief

## Identity

Sawn, weathered-but-maintained timber, made from broad boards with visible end joints, restrained grain, and small dark nail marks. The material should read immediately as assembled wood, not as a wood photograph.

Its signature mark language is **broad parallel grain interrupted by short, square-ended board joints**. Nail marks provide a small secondary rhythm. The boards are the structure; grain and nails explain how they were made and fastened.

Keep the pixel art crisp and angular. Use stepped contours, short rectangular marks, and a restrained range of warm, natural browns. The faceted geometry of walkgrid remains the dominant visual feature.

ASSUMPTION: The texture's long axis follows the board direction supplied by the building piece. If the material system cannot rotate or orient a texture per piece, use a fixed world-space board direction until the building layer supports orientation.

## Palette

Use these 12 colours. All values are sRGB hex colours.

| Hex | Role | Use |
|---|---|---|
| #806044 | Base | Main sawn timber colour |
| #99744F | Base light | Broad, sun-facing board variation |
| #6B4D36 | Base dark | Darker boards and broad timber variation |
| #B18A60 | Highlight | Sparse warm planes within boards |
| #C19A6C | Highlight light | Very sparse fresh-cut or worn edges |
| #59412F | Shadow | Board joints and deep grain |
| #453528 | Shadow deep | Occasional joint corners and nail recesses |
| #73543A | Grain | Main dark grain marks |
| #A68156 | Grain light | Occasional lighter grain marks |
| #8D6846 | Grain mid | Secondary grain and board variation |
| #382D24 | Nail dark | Dark centre of nail marks |
| #B69A76 | Nail glint | Tiny optional edge highlight on selected nails |

Do not add pure black, saturated orange, red, or yellow. The palette should remain recognisably timber under warm daylight, cool dusk, and moonlight.

The darkest colours are for small structural details, not broad board surfaces. Keep most pixels between #6B4D36 and #99744F. Highlights should occupy less area than the base colours.

## Marks

All sizes below are in pixels on the 64 × 64 texture.

### 1. Boards

- Shape: broad, elongated rectangular strips running parallel to the board direction.
- Width: 12–22 px.
- Length: 64 px, or the full available surface length.
- Count: usually 3–5 boards across a 64 px block, depending on the chosen widths and gaps.
- Spread: boards fill the surface evenly, but vary their widths slightly. Avoid perfectly equal strips.
- Edges: mostly straight, with occasional one-pixel steps or small two-pixel irregularities.
- Colour: assign each board one of the base colours. Adjacent boards should often differ by one palette step, not by extreme contrast.

The board boundaries are the strongest marks. They should be visible at normal viewing distance without making every board look outlined.

### 2. Board joints

- Shape: short, square-ended transverse seams, perpendicular to the grain.
- Width: 1–2 px.
- Length: across the full width of the affected board.
- Count: 1–3 visible joints per 64 × 64 block, with some blocks containing none.
- Spread: place joints at varied positions along the boards. Avoid lining up every joint across the whole texture.
- Colour: #59412F for the main seam; use #453528 only for a few pixels at selected corners.
- Optional edge: a 1 px strip of #B18A60 on one side of a small portion of a seam, suggesting a cut edge catching light.

Joints must remain distinct from grain. A joint crosses the board's width; grain follows its length.

Where a board continues across neighbouring blocks, do not introduce an artificial joint at the texture boundary. Where the construction calls for a board end, make the joint explicit.

### 3. Grain marks

Grain is broad, broken, and subdued. It must not become a set of long, thin, high-contrast stripes.

- Shape: stepped, gently wandering bands made from short rectangular segments.
- Segment size: 3–9 px long × 1–2 px wide.
- Group length: 8–24 px along the grain direction.
- Count: 4–9 grain groups per board per block.
- Spread: distribute irregularly along each board. Leave broad clear areas between groups.
- Colour: mainly #73543A and #8D6846; use #A68156 sparingly for a light grain segment.
- Continuity: break each group at least once. Do not draw uninterrupted lines across the block.

Allow occasional short forked or offset grain marks, but keep their total area small. A grain group should read as a mark in the wood, not a drawn outline.

No grain mark should be both long and thin and substantially taller than its surroundings in the height map. Grain is primarily colour variation.

### 4. Sawn and worn patches

- Shape: small, irregular rectangular patches with stepped edges.
- Size: 3–8 px across, with occasional patches up to 10 px.
- Count: 2–5 per board per block.
- Spread: favour the middle portions of boards; leave joints legible.
- Colour: #99744F, #B18A60, or #6B4D36.
- Contrast: keep these patches close to the board's base value.

These marks provide surface life without adding photographic texture. Avoid circular knots as a dominant feature. At most, use one small, blocky knot-like mark in an occasional board, 4–7 px across, with low contrast.

### 5. Nail marks

- Shape: tiny square or short rectangular dark marks, optionally with a single lighter pixel.
- Size: 2 × 2 px or 3 × 2 px.
- Count: 0–2 nails per board end where the construction uses nailed joints; normally 0–4 per block.
- Spread: position near board ends, set slightly in from the edges. Avoid placing every nail at exactly the same relative coordinate.
- Colour: #382D24 for the centre. A single #B69A76 pixel may sit on one edge when the lighting and contrast permit.

Nails should be visible on close inspection, not read as large black dots at a distance. Do not use circular metallic rings, bright silver, or elaborate screw-head symbols.

ASSUMPTION: Nail placement is a visual convention only; the texture generator does not need to simulate real fastening or structural load.

## Top vs side

### Top

Show the broad faces of the boards.

- Use 3–5 longitudinal boards across the 64 px surface.
- Make the seams and occasional staggered end joints clearly readable.
- Grain runs along the board direction.
- Nail marks appear near selected board ends.
- Use the full base palette, with restrained highlights and dark seams.

The top should communicate a worked, assembled surface from a glance. Do not fill it with grain so densely that the board layout disappears.

### Side

Show the exposed board edges or stacked courses.

- Use 2–4 horizontal or orientation-aligned courses across the 64 px surface, according to the piece's face orientation.
- Course boundaries are 1–2 px dark seams.
- Use shorter, quieter grain marks than on the top.
- Add occasional end-grain blocks, 4–9 px across, near board ends where appropriate.
- Keep nail marks rare on the side; use them only where a fastening would plausibly be visible.
- Make the side slightly darker on average than the top, using #6B4D36 and #59412F more often, but do not make it nearly black.

The side must read as timber thickness and assembled courses, not as a continuation of the top texture rotated onto a vertical plane.

ASSUMPTION: The renderer can select a distinct side texture and orient it to the building face. If it cannot, prioritise visible course seams and preserve the board direction consistently.

## Height

Timber is worked, comparatively flat material. Height variation should describe subtle surface wear and board edges, not create splinters or tall ridges.

Use the available 36-level height range:

- Board faces: levels 16–19.
- Broad worn or sawn patches: levels 17–20.
- Grain marks: levels 16–20, with most at the same height as the surrounding face.
- Board seams: levels 12–15.
- Nail recesses: levels 10–13.
- Slightly raised board edges: levels 19–21, restricted to narrow portions of the edge.

Keep the height differences shallow. The highest features should be broad edge portions, not narrow grain streaks. Do not make every board boundary a raised ridge.

The lower seam height helps the joints read as gaps. Slightly raised edge portions suggest board thickness and wear without creating exaggerated relief.

ASSUMPTION: These levels are relative material heights, with 0 as the lowest and 35 as the highest, matching the task's 36-level map. Confirm the actual interpretation in the material pipeline.

## Variation

Variation should prevent obvious repeated squares while preserving a coherent constructed surface.

1. Generate board widths with small differences. Keep the overall count and direction stable.
2. Vary board base colours by one palette step. Avoid random high-contrast alternation.
3. Place joints independently per board. Use varied distances from block boundaries and stagger joints across adjacent boards.
4. Let grain groups vary in length, spacing, colour, and small stepped offsets.
5. Change the positions and counts of worn patches.
6. Vary nail placement within a narrow area near the appropriate board ends.
7. Keep the broad board layout continuous across neighbouring blocks wherever the building piece represents continuous timber.
8. Where a new piece genuinely begins, align its board direction and course structure with adjacent pieces, but allow joints to occur at different positions when the construction permits it.

ASSUMPTION: The generator can use a stable world-space or piece-space seed so that adjacent surfaces agree at shared edges. If it only supports independent per-block seeds, add edge-aware rules so board boundaries do not jump at every block.

Avoid changing the board direction randomly from one block to the next. Variation belongs in widths, colour, joints, grain, and small wear marks, not in the overall construction.

## Must never look like

- Photographic wood, realistic high-frequency grain, or scanned timber.
- Long, uninterrupted, high-contrast grain stripes.
- A floor made of identical repeating rectangles.
- Perfectly uniform boards with every joint aligned.
- Nearly black wood with orange or red highlights.
- Metal plating, brickwork, stone courses, or bark.
- Oversized black nail heads or decorative screw symbols.
- Splintered, rotten, or heavily damaged wood as the default material.
- A direct imitation of another game's timber style.

## Generator recipe

Use ordinary procedural noise, scatter, palette ramps, and stepped pixel drawing. No external assets or special tools are required.

1. **Establish orientation.** Determine the board direction from the building piece. Work in local surface coordinates so boards remain aligned with the construction. Use a stable seed tied to the piece or world position.

2. **Lay out boards.** Divide the 64 × 64 surface into 3–5 longitudinal boards. Choose widths between 12 and 22 px, adjusting the set to fit the surface. Use small width variation while preserving a readable overall layout.

3. **Assign base colours.** Give each board a base colour from #806044, #99744F, and #6B4D36. Bias toward #806044 and #99744F. Use low-frequency value noise to produce broad, gentle variation within each board. Clamp the result to the listed palette rather than inventing new colours.

4. **Draw board edges.** Mark boundaries with 1–2 px seams in #59412F. Add a few small stepped irregularities, but keep most edges straight. Use #453528 only at occasional seam corners or deeper gaps.

5. **Place joints.** For each board, independently decide whether it has a visible end joint in this block. Place 1–3 joints across the block as a whole, with varied positions and staggered alignment. Draw each joint as a 1–2 px transverse seam. Do not force joints onto texture boundaries unless the construction calls for a board end.

6. **Generate grain groups.** Scatter 4–9 groups per board. Each group consists of short stepped segments, 3–9 px long and 1–2 px wide, with an overall length of 8–24 px. Follow the board direction, but introduce small offsets and breaks. Use #73543A and #8D6846 most often. Use #A68156 sparingly. Keep grain contrast lower than the contrast of board seams.

7. **Add worn patches.** Scatter 2–5 small stepped patches per board, generally 3–8 px across. Choose from the base and highlight colours. Use low-frequency noise to cluster some patches gently, while leaving large quiet areas.

8. **Add rare knots.** Occasionally place one small, blocky knot-like mark, 4–7 px across. Use a restrained combination of #6B4D36 and #8D6846. Keep it uncommon and low contrast.

9. **Add nail marks.** Near selected board ends, place 2 × 2 px or 3 × 2 px marks in #382D24. Limit the count to 0–4 per block in normal cases. Add a single #B69A76 pixel only where it remains subtle.

10. **Build the side texture.** For side-facing surfaces, arrange 2–4 courses with 1–2 px dark seams. Use shorter grain groups, occasional 4–9 px end-grain blocks, and fewer nails. Shift the overall colour distribution slightly toward the darker base and shadow colours.

11. **Create the height map.** Start board faces at levels 16–19. Keep grain and worn patches near the face height. Lower seams to levels 12–15 and nail recesses to 10–13. Add occasional shallow raised edge portions at levels 19–21. Avoid narrow raised features.

12. **Preserve continuity.** Where the building layer supports shared coordinates, derive board layout and edge marks from those coordinates so neighbouring blocks join cleanly. Where pieces have independent layouts, match the orientation and align courses at shared boundaries.

13. **Check at native scale.** Inspect the texture at 64 × 64 pixels without enlargement. Board boundaries, joints, and grain must remain distinct. Nail marks should be secondary. Remove any accidental long, thin, high-contrast lines.

14. **Check under lighting.** Preview the material under warm daylight, neutral daylight, dusk, and moonlight. If seams or board faces collapse into the same value, adjust the distribution among the existing palette colours rather than introducing a much darker shadow or brighter highlight.

15. **Check repetition.** Tile the texture in a grid. Look for repeated joint positions, identical grain clusters, uniform board widths, and obvious four-sided borders. Correct those patterns with stable variation, not unrestricted random noise.

## Acceptance checks

- At 64 × 64 pixels, the viewer can distinguish boards from grain and joints.
- Board direction remains consistent across connected building pieces.
- Joints are transverse, readable, and not mechanically aligned across every board.
- Grain uses broken, subdued marks rather than continuous thin stripes.
- Nail marks are small, dark, and secondary.
- The top and side read as different views of assembled timber.
- Height variation remains shallow and does not create spurious tall streaks.
- Repeated blocks do not produce an obvious checkerboard or repeated joint pattern.
- The material remains recognisably timber under daylight, dusk, and moonlight.