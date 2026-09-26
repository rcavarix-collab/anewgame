=== FILE: REPORT.md ===

# Task A11: Moss

## Files

- `moss.md`: Pixel-art moss material brief covering palette, marks, top and side textures, height, variation, blending, and generator recipe.

## Done when

* Every hex colour is listed: Yes — `moss.md`, Palette.

* Every size is in pixels at 64: Yes — `moss.md`, Marks and Generator recipe.

* Nothing depends on a tool we don't have: Yes — uses value noise, scatter, colour ramps, and simple procedural masks.

* Soft, dense, deep green: Yes — `moss.md`, Palette and Marks.

* Cushion texture unlike grass blades: Yes — `moss.md`, Marks and Must never look like.

## Assumptions

* ASSUMPTION: Moss uses the same texture for top and side initially, with a separate side treatment only where needed to suggest a thicker, shaded cushion.

* ASSUMPTION: The existing height-map generator accepts 36 discrete levels and combines material heights by taking the taller texel.

## Self-check

1. Hard rules kept: Yes.

2. Everything I say about the game comes from the task's facts or is labelled ASSUMPTION: Yes.

3. What I'm least sure of: The precise height-map range and how the existing renderer interprets intermediate height levels.

## Questions for the owner or Claude

- Confirm whether the material pipeline supports separate top and side colour ramps, or whether both textures must share one palette.

=== FILE: moss.md ===

# Moss — Material Brief

## Design target

Soft, dense, deep-green moss with a compact, cushioned surface. Its visual language is made of overlapping rounded pads, small clustered lobes, and occasional tiny pits. It should read as a continuous living carpet, not as individual grass blades or a field of separate stones.

The moss texture supports the faceted ground rather than competing with it. Large ground triangles remain clearly visible; the texture supplies fine surface life and colour variation.

At 64 × 64 pixels per block, the moss should be rich but calm. Use broad, soft-edged pixel-art shapes with crisp individual texels. Avoid smooth photographic shading, hairline details, and uniform repeating patterns.

## Palette

Use these 12 colours. All values are hexadecimal RGB.

|
Colour

|

Hex

|

Role

|
| --- | --- | --- |
|

Deep forest

|

`#203D29`

|

Deepest creases and rare contact shadows

|
|

Dark moss

|

`#294B30`

|

Main shadow colour

|
|

Moss base

|

`#355C38`

|

Dominant cushion colour

|
|

Cool moss

|

`#3D6740`

|

Secondary base variation

|
|

Leaf green

|

`#477348`

|

Lit cushion faces

|
|

Soft green

|

`#568052`

|

Broad highlights

|
|

Pale moss

|

`#668B59`

|

Small upper-facing highlights

|
|

Yellow-green

|

`#789653`

|

Sparse warm accents

|
|

Muted olive

|

`#596B3B`

|

Subtle shadow-side colour variation

|
|

Cool deep green

|

`#304A3C`

|

Cool shadow variation

|
|

Earthy green

|

`#465B35`

|

Low-frequency colour variation

|
|

Dew tint

|

`#87A06A`

|

Rare tiny highlights, not literal droplets

|

Keep the overall image dark-to-mid green. The pale and yellow-green colours are accents, not large patches. The palette should remain distinguishable under warm daylight, cool dusk, and moonlight; avoid relying on hue alone to separate marks.

## Marks

All sizes refer to pixels in the 64 × 64 texture.

### 1. Cushion pads

* Shape: irregular rounded blobs, soft polygonal ovals, and joined lobes. Use stepped pixel contours, not circles with mathematically smooth edges.

* Size: typically 7–15 pixels across; occasional larger connected pads up to 20 pixels across.

* Count: approximately 12–22 visible pads or lobes per block, with some merging into larger cushions.

* Spread: clustered and overlapping, with irregular gaps. Avoid evenly spaced rows or a regular Voronoi-cell appearance.

* Colour: moss base and cool moss, with leaf green on selected upper-facing portions.

* Purpose: the primary mark that makes moss read as a dense cushion rather than grass.

### 2. Cushion highlights

* Shape: broad, broken patches following the upper-facing portion of a pad. Use stepped edges and slightly uneven coverage.

* Size: 3–8 pixels across, usually wider than tall.

* Count: approximately 10–18 patches per block.

* Spread: distribute unevenly across pads. Leave many pads without highlights; do not outline every pad.

* Colour: soft green and pale moss, with yellow-green used sparingly.

* Purpose: imply soft, gently raised surfaces without a glossy or wet appearance.

### 3. Crease pockets

* Shape: short, irregular dark notches and compact pits between adjacent pads.

* Size: 1–4 pixels across.

* Count: approximately 16–30 per block.

* Spread: favour joins between pads, but keep some areas uninterrupted. Vary their orientation and spacing.

* Colour: dark moss and deep forest. Keep deep forest rare.

* Purpose: separate overlapping cushions and provide depth without drawing hard outlines around every shape.

### 4. Tiny lobe clusters

* Shape: groups of 2–4 small rounded lobes, each lobe merging into its neighbours. These are compact clumps, never individual blades.

* Overall cluster size: 3–7 pixels across.

* Count: approximately 8–16 clusters per block.

* Spread: fill some open spaces between larger cushions, with irregular local density.

* Colour: moss base, leaf green, and occasional soft green.

* Purpose: break up broad areas and add fine-grained life at the texture's native resolution.

### 5. Subtle colour flecks

* Shape: small blocky flecks or short, chunky marks.

* Size: 1–3 pixels across.

* Count: approximately 8–20 per block.

* Spread: scattered irregularly, biased toward cushion faces rather than creases.

* Colour: earthy green, muted olive, and rare dew tint.

* Purpose: prevent broad colour fields from feeling flat. Keep these marks subordinate to the cushions.

### 6. Avoided marks

Do not use blade-shaped marks, long strands, narrow parallel lines, or tall thin ridges. Do not outline every cushion. The texture's identity comes from soft, connected masses and their shallow creases.

## Top vs side

### Top texture

Use the full cushion language:

* Broad, overlapping pads with uneven stepped contours.

* A dense carpet of mid-to-deep greens.

* Small, irregular highlight patches on selected pad faces.

* Dark creases where cushions meet.

* Occasional compact lobe clusters filling gaps.

The top should feel gently domed and continuous. Avoid a field of isolated circular blobs: neighbouring pads should often touch or merge.

### Side texture

ASSUMPTION: Use the same underlying cushion language, but make the side slightly darker and more compressed to imply a thicker mat of moss.

* Reduce pale and yellow-green highlights.

* Increase dark moss and cool deep green in the lower half.

* Use irregular, overlapping horizontal-ish cushion edges, but never continuous stripes.

* Keep the same rounded pads and short crease pockets; do not introduce grass-like hanging strands.

* Preserve enough mid-green area that the side remains recognisably moss rather than dark soil.

If the material system requires one texture for both orientations, use the top texture unchanged and rely on the world lighting and facet normals to darken vertical faces.

## Height

Use the existing 36-level height map. The exact numeric mapping is an ASSUMPTION because the task does not specify which level corresponds to the renderer's baseline.

* Low: deepest crease pockets and the shaded gaps between cushions. Keep these close to the local base height.

* Low-to-middle: the main cushion surface, forming a gently uneven carpet.

* Middle: broad domed centres of larger pads.

* Locally higher: a few compact, rounded lobe clusters and selected pad centres.

* Highest: only rare, tiny rounded tufts formed by connected lobes; never long or narrow features.

Keep height changes gradual across each cushion. Use broad plateaus and short transitions rather than sharp spikes. Moss should blend as a shallow, soft mat, not as a collection of rocks.

The highest marks should be compact and rounded so they do not produce thin streaks when taller texels win at material boundaries.

## Variation

Make neighbouring blocks look like parts of one continuous moss carpet.

* Use a low-frequency value-noise field to vary the dominant base colour over areas roughly 16–32 pixels across.

* Add a second, smaller noise field with a scale of roughly 5–10 pixels to vary local cushion density and colour.

* Vary cushion size, shape, orientation, and clustering between blocks. Keep the usual 7–15-pixel range, but shift the distribution so adjacent blocks do not repeat the same proportions.

* Let some cushions cross the texture's edges. Generate the pattern with wrapped coordinates so opposite edges meet seamlessly.

* Avoid a conspicuous central cushion, repeated corner marks, or evenly spaced clusters.

* Vary highlight coverage between roughly one-third and two-thirds of the cushion pads, with no fixed pattern.

* Change crease density locally; some areas should feel more tightly packed, while others have broader, quieter pads.

* Keep overall colour balance stable. Variation should come from local structure and subtle green shifts, not from turning some blocks yellow, blue, or nearly black.

## Must never look like

* Grass blades, reeds, or a miniature meadow.

* A regular grid of identical round bumps.

* A field of isolated pebbles or stones.

* Long parallel ridges, strands, or thin streaks.

* Bright lime-green carpet or fluorescent vegetation.

* Wet, glossy moss with sharp specular spots.

* Photographic moss, realistic macro imagery, or another game's texture style.

* A flat, uniform green tile with no readable cushion structure.

## Generator recipe

Use only the existing procedural building blocks: value noise, scatter, colour ramps, and simple masks.

1. Create a seamless coordinate field. Generate the 64 × 64 texture using wrapped coordinates so opposite edges match. Use the same wrapped sampling for every noise field and scattered mark.

2. Build the broad colour foundation. Sample low-frequency value noise at a scale of approximately 16–32 pixels. Map its values through the green palette, mostly using moss base, dark moss, and cool moss. Keep deep forest limited to the darkest creases.

3. Create the cushion layout. Scatter approximately 12–22 irregular pad centres per block. Give each pad a typical diameter of 7–15 pixels, with occasional pads up to 20 pixels. Distort their contours using low-amplitude noise. Allow pads to overlap and merge; do not force a uniform spacing or a clean Voronoi partition.

4. Fill the gaps. Add approximately 8–16 compact clusters of 2–4 connected lobes, each cluster 3–7 pixels across. Place them in selected gaps and partially merge them into neighbouring pads. Avoid isolated circular dots.

5. Add cushion shading. Use each pad's irregular mask to apply a broad highlight over a selected upper-facing portion. Make highlights 3–8 pixels across. Use soft green and pale moss, with yellow-green and dew tint only for rare accents. Keep highlights patchy rather than outlining pad boundaries.

6. Add creases. Place approximately 16–30 short dark pockets, 1–4 pixels across, near pad junctions. Use dark moss most often and deep forest sparingly. Do not connect these into long lines.

7. Scatter restrained colour flecks. Add approximately 8–20 flecks, 1–3 pixels across, in earthy green, muted olive, or rare dew tint. Bias them toward cushion faces. Reject any mark that forms a long, thin line.

8. Build the height map. Start with a shallow, uneven base. Raise broad pad interiors slightly, raise selected compact lobes a little more, and lower crease pockets. Keep transitions gradual and use only a few of the upper height levels. Ensure every high feature is compact and rounded.

9. Apply the side treatment. If separate side textures are supported, reduce highlight coverage and shift the lower side toward dark moss and cool deep green. Keep the cushion shapes intact and avoid horizontal banding. Otherwise, reuse the top texture.

10. Check seamless repetition. Inspect the texture tiled in a 3 × 3 arrangement. If a cushion, crease, or highlight creates an obvious repeating cross, corner, or central motif, adjust the scatter and noise before accepting the texture.

11. Check readability at native scale. View the 64 × 64 texture without enlargement and on the faceted ground. Cushion masses should read immediately; small flecks must not overpower them. Confirm that the material remains deep green under bright and dim lighting.

12. Check material blending. Place the moss beside grass, dirt, and stone. Ensure the highest moss texels are compact, do not form thin streaks inside neighbouring materials, and allow the material's soft cushion structure to remain visible at the boundary.

=== END OF DELIVERY ===
