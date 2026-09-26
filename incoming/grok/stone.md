# Stone — Procedural Texture Brief

## Purpose

Create a cool grey, naturally fractured stone texture for walkgrid.

The stone should feel like solid, weathered rock, with broad angular faces, restrained chips, and occasional cracks. Its marks should reinforce the game's faceted ground rather than compete with the triangles of the terrain.

The texture is projected from the world, at 64 × 64 pixels per block, and repeats every block. It must read clearly at 64 × 64 pixels, with crisp texels nearby and smoothing at distance.

Use a restrained cool-grey palette. Keep the midtones sufficiently light and the shadows neutral enough that stone remains distinguishable at dawn, noon, dusk, and under moonlight.

## Palette

Use these 12 colours. All hex values are opaque sRGB colours.

| Hex | Role | Use |
|---|---|---|
| #343A3E | Deep shadow | Rare deep creases and the darkest undersides of chips |
| #41494D | Shadow | Recessed facets and crack interiors |
| #50595D | Dark cool grey | Secondary shadow planes |
| #626C70 | Cool dark grey | Lower-value stone faces |
| #737D80 | Base shadow | Shaded portions of broad faces |
| #858F91 | Base | Main stone body |
| #969FA0 | Light base | Large, gently lit faces |
| #A7AFB0 | Highlight | Upper-facing planes |
| #B8BFC0 | Bright highlight | Sparse small facet highlights |
| #69777C | Cool accent | Subtle cool variation between adjacent faces |
| #8C999B | Pale cool accent | Occasional mineral flecks |
| #C5C9C7 | Pale neutral accent | Very rare tiny chips |

Use #858F91 as the dominant colour. The palette should not be distributed evenly: most pixels belong to the base and its nearby values. Dark and bright extremes should be uncommon.

Avoid a strong blue cast. The cool accents should be subtle enough that the overall material still reads as grey.

## Marks

All sizes below are measured in pixels on the 64 × 64 texture.

### 1. Broad fractured faces

- Shape: irregular polygons with 4–8 sides.
- Size: 10–28 pixels across.
- Count: 5–12 major faces per block.
- Spread: cover the texture with a jittered Voronoi partition, then merge or subdivide selected cells to prevent a uniform mosaic.
- Colour: assign each face a nearby midtone from the palette, usually #737D80 through #A7AFB0.
- Edge: use a mostly clean boundary, softened only by occasional 1-pixel irregularities.
- Purpose: establish the stone's broad planes without making the material look like a regular tiled floor.

The faces should have varied sizes. Avoid equally sized cells, evenly spaced vertices, or a repeating polygon pattern.

### 2. Facet transitions

- Shape: narrow, angular bands along selected face boundaries.
- Width: 1–3 pixels.
- Count: 8–18 selected boundary segments per block.
- Spread: distribute across the texture, with no more than roughly half of all face boundaries receiving a strong transition.
- Colour: choose one adjacent palette value, usually one or two steps lighter or darker than the face.
- Purpose: make the stone look fractured and faceted without outlining every polygon.

Do not draw a continuous dark outline around every face. Most boundaries should be defined by a change in fill colour alone.

### 3. Hairline cracks

- Shape: short, broken, angular lines, with occasional one-pixel branches.
- Length: 3–9 pixels.
- Width: 1 pixel; exceptionally 2 pixels at a junction.
- Count: 3–7 cracks per block.
- Spread: place mainly along selected facet boundaries. A few may cross a face, but keep them short and discontinuous.
- Colour: #41494D or #50595D.
- Height: keep close to the surrounding stone height.
- Purpose: suggest fractures in solid rock without making the surface look like a web of black lines.

Cracks should not all point in the same direction. Prefer bends and short angular branches over long straight diagonals.

### 4. Small chips

- Shape: angular triangles, quadrilaterals, and tiny irregular polygons.
- Size: 1–4 pixels across.
- Count: 10–24 per block.
- Spread: scatter irregularly, with a mild preference for facet edges and corners. Avoid uniform spacing.
- Colour: use #626C70, #969FA0, #B8BFC0, or #C5C9C7.
- Purpose: add small-scale variation and a sense of broken mineral edges.

Most chips should be subtle. Use #C5C9C7 sparingly, with no more than 2–4 pale-neutral chips per block.

### 5. Mineral flecks

- Shape: compact square or angular clusters.
- Size: 1–3 pixels across.
- Count: 4–10 per block.
- Spread: scatter across broad faces, avoiding evenly spaced rows.
- Colour: #8C999B, #69777C, or a nearby base tone.
- Purpose: prevent broad faces from appearing like flat, solid colour fields.

Flecks should be low contrast. Do not create bright, glittering specks or a granular sand-like surface.

## Top vs side

### Top texture

Use the full broad-face pattern.

- Make 5–12 major faces per 64 × 64 block.
- Use the complete palette, weighted toward #737D80, #858F91, and #969FA0.
- Keep most cracks short and aligned loosely with face boundaries.
- Use highlights on selected faces, not as a universal upper-left lighting effect.
- Allow a few small chips and mineral flecks within each face.

The top should read as a collection of solid, angular rock planes. It should not look like loose gravel or a field of small stones.

### Side texture

Use the same underlying stone identity, but simplify the pattern for near-vertical faces.

- Use 4–9 broad faces per 64 × 64 block.
- Keep the same 10–28-pixel face scale where possible, allowing some larger faces to merge.
- Use fewer bright highlights and more mid-to-dark grey faces.
- Keep cracks short and irregular; do not turn them into continuous horizontal strata.
- Use the same chips and restrained mineral flecks, at slightly lower density if the side appears visually busy.

ASSUMPTION: The material system can provide a distinct side texture. If it cannot, use the top texture for both orientations.

## Height

Use the game's 36-level height map, with levels 0–35.

ASSUMPTION: Height level 0 is the lowest and level 35 is the highest.

Stone is mostly solid and relatively even. Its texture height should not create pronounced protrusions.

- Main stone faces: levels 16–20.
- Slightly raised broad facets: levels 19–22.
- Shallow recessed cracks: levels 13–16.
- Small chips and mineral flecks: levels 17–22.
- Rare raised chips: levels 23–24.
- Deep-looking crack pixels: levels 12–14, limited to very short sections.

Do not assign height according to brightness alone. A lighter face is not automatically higher; height changes should describe small physical irregularities, not lighting.

Keep the total height variation modest. In particular:

- No tall isolated peaks.
- No long, thin raised lines.
- No raised crack networks.
- No large height steps between adjacent broad faces.

This keeps stone from producing unwanted protrusions when it meets grass, soil, gravel, or other materials. The small height changes should add texture without making stone behave like a collection of tall objects.

## Variation

The 64 × 64 texture repeats every block. Neighbouring blocks must not reveal a regular grid or obvious repeated fractures.

Generate each block using a stable variation seed derived from its world position and the material identity.

Vary these features between neighbouring blocks:

1. Voronoi seed positions and cell shapes.
2. Number and size of broad faces.
3. Face colour assignments.
4. Which facet boundaries receive transition bands.
5. Crack positions, bends, branches, and lengths.
6. Chip and fleck locations.
7. The distribution of lighter and darker faces.

Keep the palette and overall mark language consistent. Variation should change the arrangement, not the identity of the material.

Avoid matching the same prominent crack or large polygon across opposite texture edges. When the projection requires spatial continuity, vary the texture's internal marks while keeping the material's world-space colour and height behaviour stable.

Do not rely on rotation alone to disguise repetition. The underlying face layout and marks must change.

## Must never look like

- A regular grid of identical polygonal tiles.
- A brick wall or neatly stacked masonry.
- Loose gravel or a pile of pebbles.
- A smooth, featureless grey plane.
- A dense black spiderweb of cracks.
- Layered sediment with long horizontal stripes.
- Brightly glittering crystal or metallic ore.
- Blue slate or nearly black rock.
- Photographic stone, realistic scanned rock, or another game's texture.
- A surface whose pattern competes with the terrain's large faceted triangles.

## Generator recipe

Use the existing procedural building blocks: value noise, Voronoi cells, scatter, and colour ramps.

1. **Set the canvas.** Create a 64 × 64 texture. Use a deterministic seed based on world position and material identity.

2. **Create broad faces.** Generate a jittered Voronoi field with an initial target of 5–12 major regions. Use irregular seed placement and vary cell sizes. Merge or subdivide selected regions so the result does not resemble a uniform polygon mosaic.

3. **Assign base colours.** Give each region a colour selected from the 12-colour palette. Bias selection toward #737D80, #858F91, and #969FA0. Use darker and brighter colours less frequently.

4. **Add low-frequency variation.** Apply subtle value noise to broad regions. Keep the change restrained so the individual faces remain legible. Avoid noise that creates a mottled or cloudy appearance.

5. **Shape facet boundaries.** Select 8–18 boundary segments. Add 1–3-pixel-wide angular transition bands using nearby palette values. Leave most boundaries unoutlined.

6. **Draw cracks.** Scatter 3–7 short, angular cracks, each 3–9 pixels long and 1 pixel wide. Prefer selected facet boundaries. Add occasional one-pixel branches. Use #41494D or #50595D. Break lines where they would otherwise become long, continuous features.

7. **Scatter chips.** Add 10–24 angular chips, each 1–4 pixels across. Bias their positions slightly toward face boundaries, but retain some interior chips. Use the designated shadow and highlight colours.

8. **Scatter mineral flecks.** Add 4–10 compact flecks, each 1–3 pixels across. Use restrained cool accents and nearby base tones. Avoid regular spacing and high-contrast sparkle.

9. **Build the height map.** Start with main face heights at levels 16–20. Apply small, irregular adjustments for raised facets and chips, and shallow depressions for cracks. Keep most pixels between levels 13 and 22; permit only rare chips to reach levels 23–24. Do not create tall thin features.

10. **Create the side variant.** Generate a separate 64 × 64 layout with 4–9 broad faces. Merge some regions, reduce highlights, and retain only short cracks. Avoid horizontal bands that could read as sedimentary layers.

11. **Check the repeat.** Tile the texture in a 3 × 3 arrangement for inspection. Adjust the seed and feature placement if the same large face, crack, or chip pattern becomes conspicuous at block boundaries. Do not add a visible border or seam.

12. **Check readability.** Inspect the texture at native 64 × 64 resolution and at a reduced view. Broad faces must remain distinct, cracks must remain subordinate, and the material must read as cool grey stone rather than gravel.

13. **Check lighting compatibility.** Preview the palette under representative dawn, noon, dusk, and moonlight colour conditions. Preserve enough separation between the base, shadows, and highlights that the stone remains readable. If a lighting condition makes it too blue or too dark, adjust the palette weighting rather than introducing a new texture effect.

14. **Check material blending.** Verify that cracks and chips do not create unintended tall protrusions when stone meets neighbouring materials. Reduce their height contrast if necessary, while preserving their visible colour marks.

The final texture should have a small number of clear, angular faces, restrained fractures, and enough irregularity to avoid obvious repetition. The terrain's geometry remains the dominant visual feature.