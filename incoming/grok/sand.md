# Sand — Material Brief

## Palette

Use a warm, restrained palette. Sand should feel fine and softly varied, with enough separation between light and dark marks to remain readable at dawn, noon, dusk, and under moonlight.

Use these 12 colours. All hex values are opaque sRGB colours.

| Role | Hex | Use |
|---|---|---|
| Base | #C9A66B | Main warm sand colour; covers most of the texture. |
| Base light | #D6B77F | Broad, subtle warm variation. |
| Base shade | #B9955D | Quiet variation in the underlying sand. |
| Shadow | #9C794B | Small, soft-edged shadow marks between grains. |
| Deep shadow | #795B3C | Rare darkest grain marks; never form long lines. |
| Highlight | #E8D09A | Scattered sunlit grains and tiny glints. |
| Highlight light | #F0DDB2 | Very rare pale grains; avoid a speckled white appearance. |
| Accent warm | #D5A05F | A few warmer grains, distinct from the base without becoming orange. |
| Accent muted | #B88D57 | Slightly darker warm grains and small clusters. |
| Cool neutral | #B9A47F | Subtle neutral variation that helps the material remain readable in cool light. |
| Cool shadow | #88795F | Muted shadow grains; use sparingly. |
| Pale neutral | #D8C7A4 | Occasional quiet, pale grains between the warmer marks. |

Do not introduce additional colours in the generator. Lighting should come from the game's material and lighting system, not baked-in artificial highlights or shadows.

## Marks

All dimensions below are in pixels on a 64 × 64 texture.

Sand's mark language is fine, scattered grains over a gently varied surface. The grain should read as individual dots and tiny irregular chips, not as a field of uniform circles.

### 1. Fine grains
- Size: 1–2 px across.
- Quantity: approximately 180–300 grains per 64 × 64 block.
- Shape: mostly irregular single pixels, short 2 px flecks, and occasional compact 2 × 2 px clusters.
- Spread: distributed across the whole block using jittered scatter. Avoid a regular lattice or evenly spaced stippling.
- Colour: primarily base shade, shadow, highlight, accent warm, and cool neutral.
- Height: grains may be distinctly raised, but only as small, isolated features.

### 2. Medium grains
- Size: 2–3 px across; occasional 3 × 2 px chips.
- Quantity: approximately 35–65 per block.
- Shape: irregular, compact flecks with no sharp, elongated tails.
- Spread: scattered with mild clustering. Allow small groups of 2–4 grains, separated by quieter areas.
- Colour: mostly base light, base shade, muted accent, and highlight.
- Height: moderately raised, with a few of the tallest grains in the material.

### 3. Rare coarse grains
- Size: 3–4 px across.
- Quantity: approximately 4–10 per block.
- Shape: compact, uneven chips. Keep them broad rather than narrow.
- Spread: irregularly scattered; never arrange them into rows or repeated motifs.
- Colour: shadow, deep shadow, highlight, or accent warm.
- Height: can reach the highest grain levels, but must remain sparse.

### 4. Fine surface variation
- Scale: broad patches approximately 12–28 px across, with no fixed shape.
- Quantity: 3–7 overlapping patches per block.
- Shape: low-contrast, diffuse regions generated from value noise.
- Spread: overlap freely. Do not create distinct islands with obvious borders.
- Colour: shift the base among base, base light, and base shade, occasionally mixing in pale neutral or cool neutral.
- Height: nearly flat; these patches alter colour, not the silhouette of the surface.

### 5. Low ripples
- Width: approximately 8–20 px.
- Length: approximately 12–32 px.
- Quantity: 2–5 faint, broad ripple traces per block.
- Shape: shallow, gently curved bands with irregular edges. Prefer short arcs and broken, offset segments over continuous parallel lines.
- Spread: vary orientation and curvature. Allow a few traces to overlap, but do not form a repeated wave pattern.
- Colour: base light on one side and base shade on the other, with only a small colour difference.
- Height: very low. Ripples are primarily a colour and shading cue, not raised geometry in the height map.

Ripples must never become long, narrow, high-contrast orange streaks. Their colour and height should be subdued enough that they do not project visibly into neighbouring materials.

## Top vs side

### Top texture
- Use the full grain distribution described above.
- Keep the surface warm and fine, with broad low-contrast colour variation.
- Include 2–5 low, broken ripple traces per block.
- Make the base colour dominant; individual grains should provide detail without overwhelming the surface.
- Keep large quiet areas between some grain clusters so the texture can breathe.

### Side texture
- Retain the same warm sand identity and palette.
- Reduce the ripple count to 0–2 per block. Ripples should not read as horizontal strata.
- Use a slightly denser distribution of small shadow grains, especially in shallow depressions.
- Use broad, irregular patches of base shade to suggest compacted sand.
- Avoid continuous horizontal bands, stacked layers, or long vertical streaks.
- Keep the same grain size limits as the top texture.

The side should look like exposed compacted sand, not a separate rock material.

## Height

ASSUMPTION: The generator stores height as an integer from 0 to 35, where 0 is lowest and 35 is highest.

Sand needs a mostly quiet height map. The fine grains provide the material's raised detail; ripples must remain low enough that blending does not turn them into streaks inside stone.

| Feature | Height range | Purpose |
|---|---:|---|
| Broad base | 8–13 | Establish a gently uneven sand surface. |
| Low colour patches | 8–14 | Allow subtle variation without forming raised islands. |
| Ripple traces | 9–12 | Keep ripples nearly level with the surrounding sand. |
| Fine grains | 13–22 | Give the surface its characteristic fine, raised texture. |
| Medium grains | 17–27 | Add occasional small, distinct raised grains. |
| Coarse grains | 22–31 | Provide rare, more prominent grains. |
| Rare tallest grains | 29–33 | Add a few isolated peaks without making the material rocky. |

Height rules:

- Keep most pixels between levels 8 and 15.
- Grain height should be spatially compact: the raised area must fit within the grain's visible footprint.
- A grain's surrounding pixels should return quickly to the local base height.
- Never extend a raised grain into a thin tail or line.
- Do not assign a continuous height ridge to a ripple.
- Do not use height to encode the broad noise patches.
- Avoid large connected regions above height 20.
- Reserve levels 29–33 for a small number of isolated grains.
- Do not use levels 34–35 for ordinary sand. They are unnecessary for this material and risk excessive dominance during blending.

The desired result is a surface that feels finely granular, not lumpy. Grain peaks may win against nearby material at isolated points, but ripples should not.

## Variation

The texture repeats every block, so each 64 × 64 tile must avoid obvious landmarks that reveal the repetition.

For neighbouring blocks:

- Use a different random seed for scatter positions, grain shapes, and grain colours.
- Offset and rotate the broad noise field so neighbouring blocks do not share the same patch boundaries.
- Vary the number of fine grains within the specified range.
- Vary the proportion of warm, neutral, and cool grains slightly while keeping the base colour dominant.
- Change ripple orientation, curvature, spacing, and length for every block.
- Avoid matching ripple endpoints across block edges.
- Do not place a large grain at a consistent location in every block.
- Do not repeat the same cluster arrangement or coarse-grain silhouette.
- Preserve the same overall brightness and warmth so adjacent blocks still read as one material.

When a texture wraps at its edges, make the underlying colour field tileable. Scatter may cross an edge, but the wrapped continuation must not create a conspicuous seam or a straight line of grains.

Variation should look naturally irregular, not like a collection of visibly different tiles.

## Must never look like

- Orange streaks or high, narrow ripple ridges.
- A regular field of evenly spaced dots.
- A uniformly noisy, grain-covered surface with no quiet areas.
- Gravel, with large angular stones dominating the texture.
- Clay, with broad smooth slabs or cracked plates.
- Layered sandstone, with strong horizontal bands.
- Metallic gold, glitter, or sparkling crystals.
- A photographic scan or realistic image texture.
- A high-contrast checkerboard or visible repeating grid.
- A texture whose detail overwhelms the faceted ground.

## Generator recipe

1. Create a 64 × 64 base colour field using low-frequency value noise. Use a broad feature scale of approximately 12–28 px. Map the noise gently among #B9955D, #C9A66B, and #D6B77F. Keep the changes gradual and low contrast.

2. Add a smaller-scale noise layer with a feature scale of approximately 4–10 px. Blend it into the base colour field at low strength. Use it to break up smooth areas, not to create visible blobs.

3. Generate 180–300 fine grains. Scatter them across the tile with jittered positions and mild clustering. Use 1–2 px irregular marks, primarily single pixels and short flecks. Draw from #B9955D, #9C794B, #E8D09A, #D5A05F, and #B9A47F. Keep bright and dark grains interspersed rather than grouped into separate regions.

4. Generate 35–65 medium grains. Use compact 2–3 px marks, with occasional 3 × 2 px shapes. Draw from the base light, base shade, muted accent, and highlight colours. Avoid repeated shapes and consistent orientation.

5. Generate 4–10 coarse grains, each 3–4 px across. Use irregular compact chips. Use #795B3C and #F0DDB2 only sparingly, and avoid placing the darkest or palest marks beside one another often enough to look like glitter.

6. Add 2–5 top-texture ripple traces. Construct each as a short, gently curved band approximately 8–20 px wide and 12–32 px long. Break the band into uneven sections, vary its orientation, and keep its contrast low. Use a subtle base-light/base-shade colour shift. Do not generate a narrow centreline or a raised ridge.

7. For the side texture, omit most ripples and instead use a few broad, irregular shade patches. Increase the density of small shadow grains slightly, while retaining the same grain-size limits and warm palette.

8. Build the height map independently from colour noise. Start with a base height between 8 and 13. Keep broad colour patches and ripples close to the base height. Add compact height bumps beneath grain marks: fine grains generally reach 13–22, medium grains 17–27, and coarse grains 22–31. A few isolated grains may reach 29–33.

9. Keep each height bump confined to its grain footprint. Do not blur it into an elongated feature. In particular, ensure ripple traces have no continuous raised ridge and no narrow high pixels.

10. Check the completed height map for connected high regions. Reduce any broad region above height 20, and remove any long, thin raised feature. The highest points should be isolated grains, not lines or patches.

11. Make the colour field tileable at the 64 × 64 boundary. Wrap scatter positions and ensure grains crossing an edge continue cleanly on the opposite edge. Avoid visible seams in the noise field.

12. Generate neighbouring tiles with different seeds and small changes in grain count, colour balance, and ripple layout. Keep the same palette and height ranges across all tiles.

13. Preview the texture at its intended world scale on a faceted ground surface. Check that individual grains read clearly nearby, the sand remains warm at a distance, and the ripple marks do not appear as orange streaks inside adjacent stone.

14. Keep the recipe limited to value noise, scatter, colour ramps, and simple shape masks. No external tools, hand-painted assets, or additional rendering effects are required.