# Dressed Stone — Squared, Hand-Worked Masonry

## Purpose and identity

A material for built structures: squared stone blocks laid in orderly courses, with joints that remain legible at 64 × 64 pixels per block.

The stone should feel quarried and shaped by hand. Its identity comes from broad, subtly uneven block faces, restrained chisel marks, and recessed mortar joints. The overall structure is orderly, but the individual stones are not machine-perfect.

The mark language is:
- Rectangular blocks.
- Horizontal course joints.
- Short, shallow dressing marks.
- Small corner chips.
- Broad, quiet variations in stone colour.

The material must support the game's angular, faceted world without making the masonry itself look like a collection of miniature terrain facets. Keep the block layout readable and the surface marks subordinate to the stonework.

## Palette

Use these 12 colours. All colours are opaque sRGB hex values.

| Hex | Role | Use |
|---|---|---|
| #77766D | Base | Main warm-neutral grey of the stone |
| #858377 | Light base | Broad, lightly illuminated stone faces |
| #68675F | Shadow | Recessed and downward-facing stone areas |
| #55554F | Deep shadow | Joint interiors and deep corner cuts |
| #969184 | Highlight | Sparse broad face highlights |
| #A69F90 | Warm highlight | Occasional sunlit upper edges |
| #625E53 | Warm midtone | Slightly brown-grey stone variation |
| #817B6D | Warm light | Subtle warmer patches on selected blocks |
| #777C78 | Cool midtone | Restrained neutral-cool variation |
| #626761 | Cool shadow | Cool variation in shaded stone |
| #A09A8B | Pale mineral | Tiny mineral flecks and exposed fresh cuts |
| #494A45 | Joint accent | Darkest mortar and narrow joint cores |

Keep the material predominantly neutral grey. Warm and cool variants should be close enough to read as one masonry type, not separate coloured stones.

Use the deep shadow and joint accent sparingly. The darkest values belong in recesses, not across entire stone faces.

Do not introduce pure white, pure black, saturated blue, or saturated orange.

## Marks

All dimensions below are in pixels on a 64 × 64 texture.

### 1. Squared stone blocks

- Shape: broad rectangles with slightly irregular edges.
- Typical face width: 28–46 px.
- Typical face height: 13–23 px.
- Edge deviation: 1–2 px, with occasional 3 px irregularities.
- Face count: usually 2–4 visible blocks per 64 × 64 texture, depending on the projection and course arrangement.
- Distribution: arrange in horizontal courses. Keep the courses approximately level, with small natural deviations rather than perfectly straight CAD-like lines.

The stone faces should occupy most of the texture. Avoid subdividing each block into many small tiles.

### 2. Horizontal course joints

- Width: 2–3 px.
- Depth indication: a dark 1–2 px core, with a 1 px lighter or intermediate edge on selected joints.
- Direction: predominantly horizontal.
- Frequency: 2–4 horizontal joint lines across the 64 px texture, including partial lines at texture boundaries where needed.
- Distribution: course lines should be continuous enough to establish masonry, but small offsets and local interruptions should prevent a mechanically drawn appearance.

Course joints should be the clearest structural marks. Keep them dark enough to read at normal viewing distance without becoming thick black stripes.

### 3. Vertical block joints

- Width: 2–3 px.
- Frequency: usually 1–3 visible vertical joints per course.
- Placement: offset the vertical joints between adjacent courses.
- Shape: mostly vertical, with small 1–2 px steps or shallow irregularities.
- Distribution: avoid aligning vertical joints through every course. Use a running-bond arrangement, with neighbouring courses staggered.

Vertical joints should meet horizontal joints cleanly. Do not add decorative outlines around every stone.

### 4. Dressing marks

- Shape: short, shallow rectangular dashes or small angled chips.
- Length: 2–6 px.
- Width: 1–2 px.
- Count: 4–10 marks per 64 × 64 texture.
- Distribution: place on stone faces, not inside joints. Group 2–3 marks loosely on a few blocks; leave other blocks unmarked.

Marks should suggest hand dressing without depicting detailed tool strokes. Vary their orientation slightly, but keep most marks aligned loosely with the stone's edges.

Use a nearby midtone or shadow colour. Reserve the palest mineral colour for only 1–3 tiny marks per texture.

### 5. Edge chips

- Shape: small rectangular notches or angular bites taken from block corners.
- Size: 1–3 px across.
- Count: 1–4 chips per 64 × 64 texture.
- Distribution: favour occasional exposed corners and edges. Do not damage every block.

Chips should reveal a slightly lighter or darker stone tone, not a contrasting material.

### 6. Face variation

- Shape: broad, low-contrast patches with irregular, rounded or angular boundaries.
- Size: 8–20 px across.
- Count: 2–5 patches per texture.
- Distribution: use low-frequency value noise, with patches spanning substantial portions of individual faces.

Face variation should be subtle. The blocks must remain visually distinct from one another without looking mottled.

### 7. Mineral flecks

- Shape: isolated square pixels or tiny 2 × 2 px clusters.
- Size: 1–2 px.
- Count: 0–5 flecks per texture.
- Distribution: sparse and irregular, avoiding obvious rows or clusters repeated in the same position.

Flecks are optional supporting detail, never a defining feature.

## Top vs side

### Top texture

Use the same dressed-stone identity, adapted to a horizontal upper surface.

- Keep the squared block boundaries and recessed joints.
- Let the face interiors occupy most of the area.
- Use broad, low-contrast tonal variation.
- Place dressing marks and chips away from the majority of joint intersections.
- Do not simulate strong directional lighting in the texture; lighting comes from the game's sun, moon, and sky.

The top should read as a flat, worked stone surface, not as a pile of loose rocks.

### Side texture

Emphasize the stacked courses.

- Keep horizontal course joints at 2–3 px.
- Make vertical joints stagger between courses.
- Use slightly stronger local value contrast around recessed joints than on the top.
- Let the lower edge of each course carry a restrained 1 px shadow accent where appropriate.
- Keep individual faces broad and quiet.

The side texture should read as a wall built from squared blocks. Do not add long, thin strata bands across the stone faces; those could read as streaks rather than joints.

### Relationship between top and side

The two textures must share the same palette, stone scale, and dressing-mark language.

The top can be quieter and more open. The side can show a more explicit stacked arrangement. Avoid making them look like different stone species.

## Height

ASSUMPTION: The building material does not participate in the terrain system's texel-height winner blending. Treat the following as a surface-relief guide unless the implementation requires an actual height map.

If a 36-level height map is required:

- Level 18: nominal stone face.
- Levels 16–17: broad, shallow face depressions.
- Levels 19–20: subtle dressing marks and gentle raised patches.
- Levels 14–15: recessed joint shoulders.
- Levels 10–13: narrow joint cores and occasional deeper chips.
- Levels 21–22: rare tiny raised mineral grains.

Keep most pixels between levels 16 and 20.

The joints should read primarily through colour and a small, coherent depression, not through extreme height differences. Broad block faces must remain the dominant surface.

Avoid tall, narrow ridges along course joints. If the height map is used for blending with other materials, these ridges could produce unwanted thin streaks.

Do not use height to create exaggerated bevels around every block.

## Variation

The material should avoid obvious repetition when neighbouring blocks use the same texture.

### Course layout

- Vary course heights by 1–3 px between texture instances.
- Vary the horizontal offset of vertical joints by 3–12 px.
- Change the number of visible blocks per course where the tiling layout permits.
- Keep the overall horizontal course direction consistent.

### Stone faces

- Vary each face's base colour among the neutral, warm, and cool midtones.
- Change the size and placement of broad tonal patches.
- Vary dressing-mark counts and orientations.
- Place chips on different corners.
- Omit mineral flecks from many instances.

### Tiling safeguards

- Use the stable block coordinate and material seed to select variations deterministically.
- Avoid repeating the same joint intersections, chip positions, or dressing-mark clusters on every block.
- Ensure texture edges tile seamlessly: course lines that leave one edge must meet compatible lines at the opposite edge, and face variation must wrap without a visible seam.
- Do not simply rotate the entire texture randomly. Keep courses horizontal and vary their internal arrangement instead.

ASSUMPTION: The generator can derive repeatable variations from block coordinates and a material seed.

## Must never look like

- Smooth, poured concrete.
- Glossy marble or polished granite.
- A random pile of rubble or loose cobblestones.
- Perfectly uniform manufactured bricks.
- Exaggerated medieval-fantasy masonry with oversized bevels.
- A photographic scan of real stone.
- Strongly blue-black slate.
- High-contrast black outlines around every block.
- Thin horizontal strata streaks across otherwise continuous faces.
- A miniature tiled floor with many tiny squares.

## Generator recipe

1. Start with a 64 × 64 pixel texture and the 12-colour palette above.

2. Establish the course layout.
   - Choose a course height within 13–23 px.
   - Arrange broad rectangular stone faces across each course.
   - Offset vertical joints between neighbouring courses.
   - Allow small 1–2 px edge irregularities, with occasional 3 px deviations.
   - Make the pattern tile seamlessly at the texture boundaries.

3. Draw recessed joints.
   - Use 2–3 px joint widths.
   - Give the joint a dark core and, on selected edges, a restrained intermediate transition.
   - Use the darkest palette colours only in the narrowest recesses.
   - Keep course joints horizontal and vertical joints staggered.

4. Assign face colours.
   - Give each stone one principal base colour.
   - Choose nearby neutral, warm-grey, or cool-grey alternatives for neighbouring stones.
   - Keep colour differences restrained so the masonry reads as one material.

5. Add broad face variation.
   - Generate low-frequency value noise.
   - Use 8–20 px patches.
   - Limit changes to nearby palette values.
   - Mask the noise to stone faces so the joints remain crisp.

6. Add hand-dressing marks.
   - Scatter 4–10 short marks, each 2–6 px long and 1–2 px wide.
   - Keep them within face boundaries.
   - Vary their orientation slightly.
   - Leave many faces completely unmarked.

7. Add occasional chips.
   - Place 1–4 angular notches, each 1–3 px across.
   - Restrict them mostly to corners and exposed edges.
   - Use a neighbouring stone tone to suggest a small fresh break.

8. Add optional mineral flecks.
   - Scatter 0–5 marks, each 1–2 px.
   - Keep their contrast low and their distribution irregular.

9. If generating a height map, begin at level 18.
   - Depress joints to levels 10–15.
   - Keep face variation around levels 16–20.
   - Allow only rare tiny details to reach levels 21–22.
   - Avoid continuous raised joint ridges.

10. Generate the top and side variants.
    - Keep the same palette and basic stone identity.
    - Make the top quieter and more open.
    - Make the side more explicitly course-based.
    - Preserve the staggered joints and restrained dressing marks.

11. Apply deterministic variation.
    - Use the block coordinate and material seed to vary course offsets, face colours, noise, marks, and chips.
    - Ensure adjacent blocks do not repeat the same distinctive arrangement.
    - Preserve seamless texture edges and a consistent course direction.

12. Final readability check.
    - Inspect the texture at its native 64 × 64 size.
    - Confirm that the blocks and staggered joints are immediately recognizable.
    - Confirm that dressing marks remain secondary.
    - Confirm that the palette reads as neutral stone under both warm and cool illumination.
    - Remove any detail that becomes visual noise or creates an obvious repeating grid.