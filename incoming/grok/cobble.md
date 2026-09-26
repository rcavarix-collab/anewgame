# Cobble paving

A hand-laid path of irregular, worn stone cobbles bedded in earthy joints. The stones should feel individually placed and shaped, not like a mathematically tiled floor. The defining marks are broad, angular cobble faces, uneven dark soil joints, and small, restrained chips. Keep the pixel-art treatment crisp and graphic. The faceted world geometry remains the main visual signature; the texture should support it rather than compete with it.

ASSUMPTION: Use the shared 64 × 64 pixels-per-block world-projected texture convention and 36-level height map for this built material. If the building layer has a separate projection pipeline, preserve the design and adapt the sampling convention.

## Palette

Use these 12 colours. Hex values are sRGB. Keep the palette earthy and moderately warm, with enough separation between stone faces and joints to read in daylight and moonlight. Avoid blue-grey shadows that make the path resemble slate.

| Hex | Role | Use |
|---|---|---|
| `#665D4C` | Deep shadow | Narrow creases, deepest joints, occasional chipped recesses |
| `#786D59` | Shadow | Shaded cobble edges and secondary joint pixels |
| `#8B806A` | Dark stone | Main colour for darker cobbles |
| `#A0947B` | Stone base | Main colour for medium cobbles |
| `#B2A68B` | Light stone | Main colour for lighter cobbles |
| `#C4B89B` | Highlight | Sparse upper-facing stone planes |
| `#D2C5A5` | Bright highlight | Very sparse sunlit chips and small face accents |
| `#82704F` | Earth shadow | Dark, compact soil patches |
| `#9A855D` | Earth base | Main soil-joint colour |
| `#B09A6B` | Earth light | Occasional exposed dry soil |
| `#6E7650` | Moss accent | Rare muted green in protected joints |
| `#C0AD7E` | Warm mineral accent | A few tiny warm inclusions on selected stones |

Do not use every colour equally. Stone should occupy most of the visible area. Soil colours belong primarily in joints; moss and mineral accents should be uncommon.

## Marks

All sizes are in pixels on a 64 × 64 texture.

### 1. Irregular cobble faces

- Size: mostly 10–22 pixels across; occasional smaller stones 6–9 pixels across.
- Count: typically 7–13 complete or partial cobble faces per block, depending on the seam layout.
- Shape: irregular polygonal outlines, usually 5–8 corners. Use softened corners made from short pixel steps rather than smooth curves.
- Spread: distribute stones across the block with varied orientations and uneven spacing. Avoid a uniform lattice.
- Construction: make each stone a coherent face with one dominant base colour and one or two broad value regions. The face should read as a single stone, not a collection of noisy pixels.
- Variation: vary the width, length, corner cuts, orientation, and face value. Most stones should be squat or roughly rounded polygons; a few may be elongated, but avoid long, narrow slabs.
- Edge: use a dark, irregular outline 1–2 pixels wide, interrupted in places by soil or a lighter worn edge. Do not give every cobble a complete, perfectly uniform outline.

### 2. Soil joints

- Width: usually 2–4 pixels; widen to 5–6 pixels at occasional junctions.
- Count: joints form a connected network around the cobbles, not isolated decorative marks.
- Spread: use irregular, branching seams. Vary their width and direction. Allow some three-way junctions and a few short dead ends.
- Colour: primarily `#9A855D`, with `#82704F` in the deepest portions and occasional `#B09A6B` pixels.
- Shape: seams should bend around individual stones. Avoid straight, parallel lines extending across the entire block.
- Readability: at least one continuous joint path should cross each block boundary, so the paving reads as a connected surface rather than separate stone islands.

### 3. Hand-worked stone facets

- Size: broad patches 3–8 pixels across; occasional 1–2-pixel chips.
- Count: 1–3 broad value patches per cobble, plus 0–3 tiny chips on selected stones.
- Spread: place broad patches asymmetrically, favouring one side of a cobble. Use a consistent notional light direction for the texture, but do not bake in strong directional lighting.
- Shape: angular patches with 2–5 corners, following the stone's face. Keep them broad enough to survive distance filtering.
- Colour: use neighbouring stone colours from the palette, generally one step lighter or darker than the face base.
- Avoid: thin parallel scratches, dense stippling, and noisy high-frequency speckling.

### 4. Worn edges and chips

- Size: chips 1–3 pixels across; worn edge bands 1–2 pixels wide.
- Count: 0–2 chips on most stones; some stones have none. Only a few stones per block should show bright edge wear.
- Spread: favour corners and exposed edges, not the centre of every stone.
- Shape: small angular bites or short, broken edge highlights.
- Colour: use `#786D59` or `#8B806A` for recesses, and `#C4B89B` sparingly for worn highlights.
- Height: chips are not raised features. They are small recesses or colour changes within the stone.

### 5. Soil and moss inclusions

- Size: soil patches 2–5 pixels across; moss marks 2–4 pixels across.
- Count: 2–6 soil variations per block; 0–3 moss marks per block.
- Spread: cluster soil variation near wider joints. Moss should occur only in a few sheltered joint corners, never as a continuous green border.
- Shape: compact, broken patches rather than long streaks.
- Colour: use `#B09A6B` for dry soil variation and `#6E7650` for rare moss.
- Keep inclusions subordinate to the cobble outlines.

## Top vs side

### Top texture

The top texture is the primary appearance: an irregular mosaic of hand-laid cobbles separated by visible soil joints.

- Use the full cobble-face and joint design.
- Keep the stone faces broad, readable, and individually distinct.
- Show small differences in stone colour and facet placement without making the path look patchy or dirty overall.
- Make the joint network the principal structural cue. It should remain visible when the texture is viewed at an oblique angle.
- Do not add long directional courses. The paving is irregular, not a brick pattern.
- Preserve coherent cobble shapes across texture boundaries. Stones cut by the edge of a block must continue naturally into the neighbouring block.

### Side texture

ASSUMPTION: The side texture represents the exposed edge of the paving and its soil bed.

- Show a shallow band of cobble edges above compacted earth.
- Stone-edge segments: 8–20 pixels wide and 4–10 pixels high.
- Soil-bed band: 8–18 pixels high where visible.
- Keep joints vertical or slightly angled between adjacent edge segments, with widths of 2–4 pixels.
- Use darker stone colours on the lower edges and restrained lighter accents along a few upper edges.
- The earth band should use `#82704F`, `#9A855D`, and occasional `#B09A6B`.
- Do not simply rotate or squash the top texture. Construct a separate side composition with a clear stone-to-soil boundary.
- Avoid thin, continuous horizontal highlight lines. Break any edge wear into short segments.

## Height

Use the shared 36-level height map, with values increasing from 0 to 35.

ASSUMPTION: These values are relative material heights, not world-space elevations.

- Soil bed and deepest joints: levels 2–7.
- Main cobble faces: levels 12–19.
- Broad face facets: levels 12–20, staying close to the parent stone's height.
- Slightly proud cobble centres or worn crowns: levels 20–22, used on only a few stones.
- Recessed chips: levels 8–11.
- Moss inclusions: levels 8–12; keep them low so they do not form raised green beads.
- Never use the highest levels for long outlines or thin highlights.

The height map should reinforce the sense of stones bedded into soil. Cobble faces sit above the joints, but the difference must remain modest: this is a walkable path, not a field of boulders. Keep high regions broad and stone-shaped. Avoid narrow high ridges around every cobble, which could create unwanted streaks when blended with neighbouring materials.

## Variation

The texture must not reveal a repeated 64 × 64 grid.

- Use a low-frequency value-noise field to vary the overall stone and soil balance across the block. Keep this variation subtle.
- Generate cobble shapes from a jittered arrangement of seed points, using irregular polygonal regions. Vary point spacing and boundary displacement so the layout does not form a regular grid.
- Let a small number of cobbles cross the texture boundary. Generate or sample neighbouring regions consistently so a crossing stone continues seamlessly.
- Vary the number of visible stones, their sizes, and their orientations within the stated ranges.
- Use a block-level palette bias: some blocks lean slightly warmer or lighter, others slightly darker or more neutral. Keep all changes within the listed palette.
- Vary facet placement independently for each stone, but keep the broad face structure coherent.
- Change soil-joint width gradually rather than making every seam identical.
- Keep moss and bright chips sparse and non-periodic.
- Do not mirror, rotate, or randomly shuffle a finished block as the main variation method. Those operations make seams and repeated motifs easier to notice.
- Ensure that neighbouring blocks can differ in stone arrangement without creating abrupt changes in overall brightness or stone-to-soil ratio.

## Must never look like

- A regular brick path with aligned rows and repeated rectangular stones.
- A perfectly fitted polygonal pavement with identical joint widths.
- A field of loose gravel or scattered pebbles.
- A polished marble floor or glossy decorative stone.
- A blue-black slate surface.
- A photographic scan, realistic material photograph, or physically rendered stone.
- A texture dominated by fine noise, scratches, or tiny speckles.
- A raised, jagged boulder field.
- A copied visual style, layout, or distinctive motif from another game.

## Generator recipe

1. Create a 64 × 64 working canvas in the material's world-projected texture space. Use a deterministic seed derived from the material and world location so adjacent blocks can share continuous features.

2. Generate a low-frequency value-noise field to control subtle variation in overall warmth, brightness, and soil exposure. Keep the variation broad; do not use noise to create fine surface grain.

3. Place irregular cobble seed points using a jittered spatial distribution. Target 7–13 visible cobble faces per 64 × 64 block, with typical face widths of 10–22 pixels and occasional 6–9-pixel stones. Avoid equal spacing and repeated rows.

4. Form polygonal cobble regions from the seed points. Perturb boundaries by small amounts, using short angular steps. Keep most stones compact and use only a few elongated forms.

5. Ensure the region layout is continuous beyond the 64 × 64 boundary. Generate enough neighbouring context that partial stones and their joints align across edges. Do not independently regenerate each block's border.

6. Build the soil-joint mask from the spaces between stone regions. Use 2–4-pixel joints, widening to 5–6 pixels at selected junctions. Add slight local irregularity without breaking the connected network.

7. Assign each cobble a base colour from `#8B806A`, `#A0947B`, and `#B2A68B`, with occasional darker or lighter stones. Bias the selection using the low-frequency field. Keep the palette discrete rather than interpolating into arbitrary photographic colours.

8. Add one or two broad angular facets per stone, generally 3–8 pixels across. Choose adjacent palette colours and place facets asymmetrically. Use the same notional light direction throughout the material, but keep contrast restrained.

9. Add sparse chips and edge wear. Use 1–3-pixel marks, limited to a few locations per stone. Ensure bright highlights are isolated and do not form continuous thin lines.

10. Fill the soil joints with `#9A855D`, varying selected pixels or compact patches toward `#82704F` and `#B09A6B`. Add 0–3 small moss patches per block using `#6E7650`. Keep all inclusions compact.

11. Construct a separate side texture. Lay out short, irregular cobble-edge segments above a compacted-earth band. Use the side dimensions and palette described above; do not reuse the top texture by rotating it.

12. Generate the height map from the masks: low soil bed, modestly raised cobble faces, shallow recessed chips, and only occasional slightly proud stone centres. Keep high values broad and avoid thin raised outlines.

13. Validate the result at native 64 × 64 size. Confirm that individual stones and the soil-joint network remain legible without zooming. Remove any mark that becomes meaningless noise at this size.

14. Inspect several neighbouring blocks together. Correct visible repetition, aligned seams, abrupt palette shifts, and discontinuities at texture boundaries. The path should read as one hand-laid surface extending across the world.

15. Check the material under bright daylight, warm dawn and dusk, and cool moonlight. Preserve separation between the stone faces and soil joints in every lighting condition. If necessary, adjust palette contrast rather than adding more marks.

16. Keep generation inexpensive: use a small number of noise samples, polygon masks, and scatter operations per block. Avoid expensive per-pixel simulation or effects that would threaten the 60 FPS target on a GTX 1060-class PC.