# Clay — Material Brief

## Identity

Clay is a smooth, dense, compact earth found at the edges of sand and gravel lowlands. Its defining mark language is **broad, irregular dry cracks separated by quiet, solid clay plates**.

The surface should feel fine-grained and cohesive, not rocky or sandy. Most of each block is uninterrupted clay. Cracks provide the main visual structure; sparse colour variation gives the plates life without making the material noisy.

The clay must remain recognisably clay when viewed alongside sand, gravel, dirt, and loam.

Target format: 64 × 64 pixels per block, world-projected, repeating every block. Preserve crisp square texels nearby and let the existing distance smoothing handle the far view.

## Palette

Use these 12 colours. All colours are opaque sRGB hex values. Roles describe their intended use; the generator may vary their proportions within the stated visual intent.

| Hex | Role | Use |
|---|---|---|
| `#A9654C` | Base | Main warm, muted terracotta clay |
| `#B87559` | Base light | Broad areas of sunlit, dry clay |
| `#92533F` | Shadow | Shaded clay plates and subdued surface variation |
| `#784435` | Deep shadow | Crack interiors and the darkest plate edges |
| `#C88A69` | Highlight | Small, restrained warm highlights on plate surfaces |
| `#D49A77` | Highlight light | Rare pale clay accents |
| `#B98265` | Accent | Subtle orange-brown plate variation |
| `#9C6D58` | Accent | Muted brown plate variation |
| `#80594A` | Accent shadow | Darker, compact plate variation |
| `#C18B70` | Accent light | Soft, dusty pink-brown variation |
| `#6D493D` | Crack edge | Occasional dark side of a crack |
| `#E0AE8C` | Rare highlight | Very sparse warm flecks, never a continuous line |

Use the base colours across most of the surface. The lightest colours should occupy only small areas. Keep the overall material moderately light and warm, with enough value separation to survive dawn, noon, dusk, and moonlight.

Do not make the surface uniformly orange or red. The muted brown accents keep it earthy.

## Marks

All dimensions are in pixels on the 64 × 64 texture.

### 1. Dry crack network — signature mark

- Shape: irregular, branching, gently angular lines separating broad clay plates.
- Width: usually 1 px; occasional 2 px sections.
- Branch junctions: 2–4 px across.
- Total coverage: approximately 3–6% of the texture.
- Layout: a connected but irregular network, with several large enclosed or partially enclosed plates per block.
- Direction: varied; avoid a dominant horizontal, vertical, or diagonal direction.
- Colour: primarily `#784435`, with selective `#6D493D` along one edge and rare `#92533F` transitions.
- Crack edges must remain short and discontinuous. Do not outline every plate with a bright rim.

Cracks should read as narrow recesses in compact clay, not as black ink drawn on top.

### 2. Clay plates — main surface shape

- Shape: broad, uneven polygonal patches separated by the crack network.
- Typical width: 10–24 px.
- Typical height: 8–20 px.
- Count: approximately 6–16 substantial plates per block, depending on the crack layout.
- Spread: irregularly across the texture; vary the plate sizes and avoid a repeated tessellation.
- Colour: predominantly `#A9654C` and `#B87559`, with gentle shifts to `#92533F`, `#B98265`, and `#9C6D58`.

Plates are regions of subtly different clay colour, not raised stones. Their boundaries should be defined mainly by the cracks, not by strong outlines.

### 3. Broad surface mottling

- Shape: soft, irregular patches with no hard outlines.
- Size: 6–18 px across.
- Count: approximately 8–20 patches per block.
- Spread: low-frequency variation across plates, with occasional smaller patches nested inside larger ones.
- Colour: blend among `#A9654C`, `#B87559`, `#92533F`, `#B98265`, and `#C18B70`.

Keep the mottling subdued. It should enrich the plates without competing with the cracks.

### 4. Small mineral and clay flecks

- Shape: compact squares, short dashes, and tiny uneven clusters.
- Size: 1–2 px; occasional clusters up to 3 × 2 px.
- Count: approximately 12–28 flecks per block.
- Spread: sparse and irregular, with no even scatter or obvious clustering into rows.
- Colour: use `#9C6D58`, `#80594A`, `#C88A69`, and very rarely `#D49A77` or `#E0AE8C`.

Flecks must be low-profile colour details. They should not resemble gravel, coarse grit, or embedded pebbles.

### 5. Subtle plate shading

- Shape: broad, low-contrast patches near selected crack edges.
- Width: 2–5 px from the crack.
- Length: 4–12 px, discontinuous.
- Count: roughly 3–8 shaded sections per block.
- Spread: vary which edges receive shading; do not shade every plate consistently.
- Colour: `#92533F`, `#9C6D58`, and `#80594A`.

This shading suggests slight surface irregularity without turning the material into chunky, raised tiles.

## Top vs side

### Top texture

The top is the primary clay appearance:

- Broad, smooth clay plates with irregular dry cracks.
- Use the full palette, keeping the warm base colours dominant.
- Make the crack network legible at 64 × 64 without allowing it to overwhelm the surface.
- Keep plate shading subtle and uneven.
- Avoid directional patterns that would reveal the block boundaries when world-projected.

### Side texture

Use the same clay identity, but make the surface slightly darker and more compact-looking:

- Preserve the broad plates and narrow cracks.
- Reduce bright highlights and small flecks.
- Increase the relative use of `#92533F`, `#784435`, and `#80594A`.
- Use fewer visible cracks, with most 1 px wide and occasional 2 px sections.
- Keep the side visually cohesive; do not add sedimentary bands or stacked layers.

ASSUMPTION: The side texture uses the same palette and mark language as the top, adjusted for darker vertical faces.

## Height

The height map has 36 levels, from low to high. Use only the existing height-map output; no additional rendering features are required.

### Low areas

- Crack interiors: lowest values, approximately levels 0–5.
- Main clay plates: low values, approximately levels 5–10.
- Broad plate shading: no meaningful height increase; colour variation only.

### Slightly raised areas

- Occasional compact plate centres: approximately levels 10–13.
- Small, broad surface irregularities: approximately levels 10–14, spread sparsely.

### Height rules

- Keep the overall height range narrow.
- Cracks must be shallow recesses, not deep trenches.
- Plate boundaries must not form tall ridges.
- Do not assign high values to flecks or highlights.
- Do not create raised, thin crack-edge lines.
- Keep the highest clay values well below the levels used for prominent tufts, pebbles, or other tall material features.

Clay is dense and cohesive. Its height map should support gentle surface variation and clear recessed cracks, not aggressive relief.

## Variation

The texture must not appear as a repeated square when adjacent blocks meet.

For neighbouring blocks:

1. Vary the broad mottling field so the clay's base colour and large patches do not repeat at the same positions.
2. Change the crack network's branching, plate sizes, and junction locations.
3. Vary the number and placement of small flecks.
4. Change which plate edges receive subtle shading.
5. Keep the palette and overall crack density consistent so the material remains coherent across the terrain.

Where possible, derive variation from the generator's existing world-position or block-seed inputs. Do not introduce visible seams or abruptly change the material's overall colour between blocks.

The crack network should not form a recognisable repeating motif every 64 px. Avoid identical corner cracks, repeated central junctions, and matching large plates across neighbouring blocks.

## Must never look like

- Bright red brick, terracotta tiles, or masonry.
- Dry mud broken into deep, dark, oversized polygons.
- Gravel, pebbles, or coarse granular soil.
- Layered sandstone or horizontally banded sediment.
- Glossy, wet, reflective clay.
- Uniform orange or featureless brown.
- A regular Voronoi diagram, geometric tile pattern, or repeated grid.
- Thin raised ridges that streak into neighbouring materials.
- Photographic soil or realistic scanned material.
- Another game's distinctive material style.

## Generator recipe

Use only value noise, Voronoi-style partitioning, scatter, colour ramps, and ordinary pixel operations already available to the texture generator.

1. **Create the broad clay field.** Generate low-frequency value noise over the 64 × 64 texture. Map it mainly between `#92533F`, `#A9654C`, and `#B87559`. Keep the contrast modest and the changes broad.

2. **Create irregular plate regions.** Generate a jittered Voronoi partition or an equivalent irregular cell field. Use a characteristic plate width of 10–24 px and height of 8–20 px. Perturb the cell boundaries so they do not look mechanically geometric. Do not expose the complete partition as a bright outline.

3. **Form the crack network.** Convert selected partition boundaries into connected, irregular crack paths. Keep most cracks 1 px wide, with occasional 2 px sections. Make the network branching and varied, with roughly 3–6% total crack coverage. Use `#784435` for most crack pixels and `#6D493D` sparingly. Break up overly uniform segments and junctions.

4. **Soften plate colours.** Apply a second low-frequency noise field to the plate interiors. Shift colours subtly among `#A9654C`, `#B87559`, `#B98265`, `#9C6D58`, and `#C18B70`. Keep the plate regions visually distinct enough to read, but avoid a patchwork of strongly contrasting colours.

5. **Add small flecks.** Scatter approximately 12–28 compact marks, mostly 1–2 px, with occasional clusters up to 3 × 2 px. Use the muted accent colours. Keep the rare lightest colours sparse. Reject long, thin marks and any arrangement that forms rows.

6. **Add restrained crack-edge shading.** Select a few short sections of crack edges and apply a 2–5 px-wide, low-contrast shadow patch. Keep it discontinuous. Do not add a continuous bright or dark border around every plate.

7. **Build the height map.** Start with low, gently varying plate heights around levels 5–10. Set crack interiors to levels 0–5. Add sparse broad irregularities around levels 10–14. Do not make the flecks, crack edges, or plate boundaries tall. Keep all values within the 36-level map.

8. **Create the side variant.** Start from the same material identity, reduce the lightest colours, and favour darker base and shadow colours. Retain the crack language but reduce crack frequency and small flecks. Do not add horizontal strata.

9. **Check the repeat.** Inspect the 64 × 64 tile repeated in both directions. Adjust the field and marks so no obvious grid, repeated central plate, matching corner crack, or periodic crack junction appears. Keep texture details compatible with world projection across adjacent blocks.

10. **Check the material in context.** View it beside sand, gravel, dirt, and loam under bright, dim, warm, and cool lighting. Preserve the warm clay identity without relying on saturated red or on highlights that disappear under moonlight.

11. **Keep the output simple.** Export the colour texture and 36-level height map in the existing generator's expected format. Do not require new tools, shaders, or runtime effects.