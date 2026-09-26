=== FILE: REPORT.md ===

# Task A5: Sandstone, top and layered side (S)

## Files

- `sandstone.md` — Procedural texture brief for sandstone, covering its top surface, layered cliff sides, palette, marks, height map, variation, and generator recipe.

## Done when

* Every hex colour is listed: Yes — `sandstone.md`, Palette.

* Every size is specified in pixels at 64 × 64: Yes — `sandstone.md`, Marks and Generator recipe.

* Nothing depends on a tool we don't have: Yes — uses value noise, scatter, colour ramps, and ordinary procedural texture generation.

* Top texture and layered side texture are specified: Yes — `sandstone.md`, Top vs side.

* Side strata remain low in the height map: Yes — `sandstone.md`, Height.

* Layer thicknesses avoid a barcode-like appearance: Yes — `sandstone.md`, Marks and Generator recipe.

## Assumptions

* ASSUMPTION: The sandstone palette uses warm, muted ochre, tan, and rust colours to remain distinct from yellow sand, orange clay, and grey stone.

* ASSUMPTION: The generator can produce separate top and side textures and height maps for the same material.

* ASSUMPTION: Height values use the game's existing 36-level scale, with 0 as the lowest level and 35 as the highest.

## Self-check

1. Hard rules kept: Yes.

2. Everything I say about the game comes from the task's facts or is labelled ASSUMPTION: Yes.

3. What I'm least sure of: The exact colour-space and height-map conventions of the existing generator; Claude should map the brief to the current implementation.

## Questions for the owner or Claude

- None.

=== FILE: sandstone.md ===

# Sandstone — Top and Layered Side

## Design goal

Create warm, visibly sedimentary sandstone that supports walkgrid's faceted terrain rather than competing with it. The top reads as weathered, compacted stone with scattered granular marks and broad colour variation. The side reveals horizontal sedimentary layers, but those layers must remain subtle, irregular, and low-relief.

The material's signature mark language is broad, broken sediment bands with sparse angular mineral grains.

Use two separate 64 × 64 pixel textures:

* `sandstone_top`: exposed upper surfaces.

* `sandstone_side`: near-vertical cliff faces.

Both textures repeat every block and use world-projected coordinates. Preserve crisp pixel edges near the player and let the existing distance smoothing handle farther surfaces.

## Palette

Use these 12 colours. All hex values are sRGB.

|
Hex

|

Role

|

Intended use

|
| --- | --- | --- |
|

`#B9824E`

|

Base

|

Main warm sandstone body

|
|

`#C9945F`

|

Base light

|

Broad, sunlit tan variation

|
|

`#A97343`

|

Base dark

|

Warm midtone for natural variation

|
|

`#96643D`

|

Shadow

|

Recesses and sheltered areas

|
|

`#D8AD79`

|

Highlight

|

Sparse pale mineral grains

|
|

`#E2BD8B`

|

Highlight light

|

Very small, subdued sunlit chips

|
|

`#805638`

|

Deep shadow

|

Rare creases and deeper mineral pockets

|
|

`#BD8A58`

|

Warm accent

|

Secondary ochre patches

|
|

`#C17A4B`

|

Rust accent

|

Sparse iron-rich flecks

|
|

`#A98260`

|

Neutral tan

|

Muted sediment variation

|
|

`#D0A675`

|

Pale band

|

Light strata on side texture

|
|

`#8D6045`

|

Dark band

|

Dark strata on side texture

|

Keep the overall palette warm and moderately muted. Avoid relying on very bright highlights or near-black shadows. At noon, dawn, dusk, and under moonlight, the material should remain identifiable as warm sandstone through differences in value and hue, not through saturation alone.

### Palette distribution

For the top texture, target approximate proportions:

* `#B9824E`: 32%

* `#C9945F`: 17%

* `#A97343`: 17%

* `#96643D`: 8%

* `#D8AD79`: 7%

* `#E2BD8B`: 2%

* `#805638`: 2%

* `#BD8A58`: 6%

* `#C17A4B`: 3%

* `#A98260`: 6%

For the side texture, use the same underlying warm body, with strata colours replacing some of the ordinary base variation:

* `#B9824E`: 27%

* `#C9945F`: 12%

* `#A97343`: 15%

* `#96643D`: 8%

* `#D8AD79`: 5%

* `#E2BD8B`: 1%

* `#805638`: 2%

* `#BD8A58`: 5%

* `#C17A4B`: 3%

* `#A98260`: 6%

* `#D0A675`: 9%

* `#8D6045`: 7%

These are starting targets, not rigid per-pixel quotas. Allow noise and band placement to change the final proportions modestly.

## Marks

All dimensions below refer to pixels in a 64 × 64 texture.

### Top texture

1. Broad weathering patches

* Shape: irregular, soft-edged angular patches.

* Size: 8–20 pixels across.

* Count: 5–10 per block.

* Spread: distribute across the full texture, with overlap and varied spacing.

* Purpose: break up the base colour without forming obvious islands or a tiled pattern.

* Construction: low-frequency value noise, quantized into a small number of palette steps.

2. Compact mineral grains

* Shape: square, short rectangular, or small angular clusters.

* Size: 1–3 pixels across.

* Count: 18–36 per block.

* Spread: irregular scatter, with occasional pairs or small clusters; avoid even spacing.

* Colours: primarily `#D8AD79`, `#A98260`, and `#BD8A58`.

* Purpose: give the surface a granular stone character.

3. Weathered chips

* Shape: short, angular marks with stepped pixel edges.

* Size: 2–5 pixels long and 1–3 pixels wide.

* Count: 5–10 per block.

* Spread: clustered loosely around some weathering patches, while leaving broad quiet areas.

* Colours: mostly `#96643D`, `#D8AD79`, and `#A97343`.

* Purpose: suggest small surface fractures without resembling gravel.

4. Iron-rich flecks

* Shape: compact, irregular dots or short blocks.

* Size: 1–3 pixels across.

* Count: 3–7 per block.

* Spread: sparse and uneven; never form a line or continuous vein.

* Colour: `#C17A4B`.

* Purpose: add restrained mineral variation.

5. Rare pale inclusions

* Shape: tiny angular chips, never long streaks.

* Size: 1–2 pixels across.

* Count: 2–5 per block.

* Spread: widely separated.

* Colour: `#E2BD8B`.

* Purpose: provide small highlights without making the sandstone sparkle.

### Side texture

The side uses the same mineral and weathering vocabulary, but its defining feature is horizontal sedimentary layering.

1. Broad sediment bands

* Shape: horizontal, gently irregular bands with stepped edges.

* Thickness: 5–12 pixels vertically.

* Count: 4–7 bands per 64-pixel texture.

* Spread: fill the height, but vary thickness and spacing. Bands should not all have the same thickness.

* Colours: mainly the base colours, with occasional `#D0A675` or `#8D6045` bands.

* Purpose: establish the layered sandstone identity.

2. Narrow secondary seams

* Shape: broken horizontal seams, not continuous across the texture.

* Thickness: 2–3 pixels vertically.

* Length: 8–24 pixels horizontally.

* Count: 2–4 per block.

* Spread: place within or near selected broad bands, not at every boundary.

* Colours: `#D0A675`, `#8D6045`, or `#96643D`.

* Purpose: suggest changes in sediment without creating a barcode pattern.

3. Side-face mineral grains

* Shape: compact angular flecks.

* Size: 1–2 pixels across.

* Count: 10–20 per block.

* Spread: sparse scatter, avoiding a uniform distribution.

* Colours: `#D8AD79`, `#A98260`, and `#BD8A58`.

* Purpose: connect the side visually to the top.

4. Small erosion pockets

* Shape: squat, irregular patches.

* Size: 3–7 pixels across.

* Count: 3–6 per block.

* Spread: concentrate a few near selected band boundaries, but leave other boundaries clean.

* Colours: `#96643D` and `#805638`.

* Purpose: imply erosion and differential weathering without cutting deep grooves into the texture.

### Avoiding the barcode effect

Do not draw equally spaced horizontal lines. Instead:

* Use a sequence of unequal band heights, typically 5–12 pixels.

* Let each boundary wander vertically by 0–2 pixels over horizontal distances of 8–16 pixels.

* Break selected bands into offset segments, each 8–24 pixels long.

* Let a band change colour gradually through a few adjacent palette values rather than switching between two stark colours.

* Avoid repeating the same band sequence at the top and bottom edges.

* Do not make every band boundary equally dark or equally sharp.

* Keep the majority of bands broad and low-contrast.

The side should read as layered rock from a distance, but individual layers should not resemble evenly spaced stripes.

## Top vs side

|
Property

|

Top

|

Side

|
| --- | --- | --- |
|

Main read

|

Weathered, granular sandstone

|

Sedimentary sandstone with visible strata

|
|

Dominant structure

|

Broad irregular patches

|

Unequal horizontal bands

|
|

Small marks

|

Grains, chips, sparse iron flecks

|

Fewer grains, erosion pockets, broken seams

|
|

Contrast

|

Moderate, distributed across the surface

|

Moderate overall; stronger only at a few seams

|
|

Directionality

|

No preferred direction

|

Horizontal layering

|
|

Height relief

|

Mostly low, with rare small inclusions

|

Nearly flat; strata remain low-relief

|

Do not simply rotate the top texture to make the side. Generate the two independently while sharing the palette and mineral mark style.

## Height

Use the existing 36-level height scale, assumed to run from 0 (lowest) to 35 (highest).

### Top height map

* General weathering and base variation: levels 8–12.

* Broad mineral or compacted patches: levels 9–13.

* Ordinary mineral grains: levels 13–16.

* Small weathered chips: levels 12–16.

* Rare pale inclusions: levels 15–18.

* Absolute maximum: level 18.

Keep the surface mostly low and gently varied. The occasional raised grain may win against a neighbouring material, but sandstone should not produce a forest of tall spikes or a continuous raised rim.

### Side height map

* Main rock body: levels 8–12.

* Broad sediment bands: levels 8–12.

* Narrow secondary seams: levels 9–12.

* Erosion pockets: levels 6–9.

* Mineral grains: levels 12–15.

* Absolute maximum: level 15.

Critical: sediment bands must not form tall ridges. Their visual separation comes from colour, not height. Keep adjacent strata within a few height levels of one another. Narrow seams must never be elevated enough to become thin, protruding streaks when materials blend.

Use the same height convention for both textures and let the existing material blending decide which texels win.

## Variation

Neighbouring sandstone blocks must feel like parts of one continuous rock formation, not identical stamps.

### Top variation

* Offset the low-frequency noise field independently for each block while preserving world-space continuity where the projection requires it.

* Vary the number of broad weathering patches between 5 and 10.

* Change mineral grain counts within the specified ranges.

* Vary the positions, sizes, and orientations of chips.

* Change the local balance of tan, ochre, and rust accents slightly.

* Avoid placing the same pale inclusion or dark chip at the same relative position in successive blocks.

### Side variation

* Generate an independent band sequence for each side-texture block.

* Vary band thicknesses between 5 and 12 pixels.

* Change boundary offsets and segment breaks.

* Avoid matching the topmost and bottommost band positions across neighbouring blocks.

* Vary which bands receive pale or dark accents.

* Keep the overall horizontal sediment direction consistent, even when individual layers vary.

Do not introduce random rotation that turns horizontal strata vertical. Variation should alter the local geology while preserving the material's defining mark language.

Where world projection requires neighbouring faces to align, preserve the shared world-space pattern rather than introducing visible seams. Use deterministic variation from the block's world position if the generator needs a stable per-block seed.

## Must never look like

* Yellow beach sand with uniform fine grains.

* Orange clay or a smooth, featureless earthen surface.

* Grey granite, slate, or generic concrete.

* Gravel made from many separate rounded stones.

* A barcode of equally spaced, equally dark horizontal lines.

* Deeply grooved rock with tall, thin raised strata.

* A photographic rock scan or realistic photograph.

* A noisy, high-contrast collection of unrelated pixels.

* A repeating checkerboard or obvious 64 × 64 tile grid.

* A texture whose detail overwhelms the faceted terrain.

## Generator recipe

Use ordinary value noise, scatter, colour ramps, and integer pixel operations. No specialised tools or external assets are required.

### Shared preparation

1. Create separate 64 × 64 output buffers for the top and side.

2. Define the 12-colour palette above.

3. Use a deterministic seed derived from the material and world position, or the generator's existing equivalent, so repeated evaluation produces stable results.

4. Quantize all output colours to the listed palette. Do not introduce arbitrary intermediate RGB colours.

5. Generate colour and height independently, then check that the height map does not exaggerate small colour details.

### Top generation

1. Fill the texture with `#B9824E`.

2. Generate low-frequency value noise with a feature scale of 8–20 pixels. Quantize it into broad patches using `#C9945F`, `#A97343`, `#96643D`, and `#A98260`.

3. Add 5–10 irregular weathering patches. Vary their dimensions between 8 and 20 pixels, allowing partial overlap.

4. Scatter 18–36 compact mineral grains, each 1–3 pixels across. Use mostly `#D8AD79`, `#A98260`, and `#BD8A58`.

5. Scatter 5–10 angular chips, each 2–5 pixels long and 1–3 pixels wide. Keep them compact and avoid connecting them into lines.

6. Add 3–7 rust flecks in `#C17A4B`, each 1–3 pixels across.

7. Add 2–5 pale inclusions in `#E2BD8B`, each 1–2 pixels across.

8. Apply a subtle, low-frequency colour bias so some regions are more ochre and others more tan. Keep the overall palette distribution close to the targets.

9. Generate the height map: use levels 8–12 for the body, 9–13 for broad patches, 13–16 for ordinary grains and chips, and 15–18 for rare pale inclusions.

10. Inspect the tile at native 64 × 64 size. Remove any accidental long lines, obvious clusters of equally spaced marks, or isolated high-contrast pixels that dominate the surface.

### Side generation

1. Fill the texture with `#B9824E`, then apply low-frequency colour variation using the shared base palette.

2. Divide the 64-pixel height into 4–7 broad bands with unequal thicknesses, each typically 5–12 pixels. Adjust the final band to fit the texture without forcing all bands to the same size.

3. Assign each band a base colour from `#B9824E`, `#C9945F`, `#A97343`, `#96643D`, or `#A98260`. Keep neighbouring bands relatively close in value.

4. For selected bands only, introduce a pale `#D0A675` or dark `#8D6045` section. Keep these accents broad enough to read as sediment, not thin bright outlines.

5. Let selected boundaries wander by 0–2 pixels over horizontal distances of 8–16 pixels. Use stepped, pixel-aligned changes.

6. Break 2–4 secondary seams into segments 8–24 pixels long and 2–3 pixels thick. Place them selectively rather than at every band boundary.

7. Add 10–20 mineral grains, each 1–2 pixels across, using `#D8AD79`, `#A98260`, or `#BD8A58`.

8. Add 3–6 small erosion pockets, each 3–7 pixels across, using `#96643D` or `#805638`. Keep their outlines irregular and their height low.

9. Generate the height map independently of the band colours. Use levels 8–12 for the body and bands, 9–12 for seams, 6–9 for erosion pockets, and 12–15 for mineral grains.

10. Inspect the side tile both alone and beside a second independently seeded tile. Remove uniform stripe spacing, repeated boundary heights, and seams that line up into a continuous barcode across blocks.

### Final checks

* Both textures are exactly 64 × 64 pixels.

* Every output colour comes from the listed palette.

* All mark sizes and band thicknesses are specified in pixels.

* The top reads as granular, weathered sandstone.

* The side reads as horizontally layered sedimentary rock.

* Strata are predominantly colour-defined, not height-defined.

* No thin, tall features can protrude conspicuously through neighbouring materials.

* Repetition does not create an obvious block grid.

* The material remains recognisable under the game's changing daylight and moonlight.

* The faceted ground remains the dominant visual feature.

END OF DELIVERY
