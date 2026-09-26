# Woven Reed Panels

## Material identity

Woven reed panels are hand-worked building screens: bundles of dried reeds interlaced into a firm, slightly irregular sheet, held together by occasional cross-bindings and a simple perimeter frame.

The defining mark language is **broad vertical reed bundles crossed by horizontal weavers**. The weave should read immediately at 64 × 64 pixels per block, with the larger construction pattern visible before the individual fibres.

This is a built material, not a ground surface. Its texture should communicate a lightweight, hand-assembled panel. It must sit naturally beside timber, clay, adobe, and thatch without becoming visually identical to any of them.

The signature comes from the woven construction, uneven reed widths, and occasional bindings—not from photographic fibre detail.

ASSUMPTION: The panel represents dried natural reeds or split rushes woven into a screen, with simple structural edges.

## Palette

Use the following 12 colours. Every colour is a fixed hexadecimal sRGB value.

| Name | Hex | Role |
|---|---|---|
| Deep reed | `#493B27` | Deepest weave gaps, frame joints, and limited contact shadows |
| Dark umber | `#625039` | Shadowed reed faces and recessed crossings |
| Reed shadow | `#796344` | Dark side of broad reed bundles |
| Dry bark | `#92784F` | Secondary dark reed colour |
| Reed base | `#AD9160` | Main dried-reed body colour |
| Pale reed | `#C1A575` | Main lit reed faces |
| Sun-dried straw | `#D3B987` | Highlights on broad reed surfaces |
| Warm straw | `#E0C99B` | Sparse bright reed accents |
| Old binding | `#776044` | Fibre ties and dark binding sections |
| Binding light | `#B69A68` | Lit binding sections |
| Weathered reed | `#9B966C` | Muted grey-olive variation in older reeds |
| Cut end | `#D0B78B` | Exposed reed ends and occasional cut faces |

### Palette rules

- Use `#AD9160` as the dominant colour.
- Keep `#C1A575` and `#D3B987` as the principal light-facing colours.
- Reserve `#493B27` for small, deliberate gaps and contact shadows. It must not become a broad background.
- Use `#9B966C` sparingly to interrupt the warm palette with subdued, weathered reeds.
- Use `#E0C99B` and `#D0B78B` only for small highlights and exposed cut ends.
- Bindings should generally use `#776044`, with `#B69A68` on their lit faces.
- Keep the colour distribution warm and earthy, with enough value separation for the weave to remain legible in changing daylight and moonlight.
- Do not introduce pure black, saturated orange, vivid yellow, or blue-grey as additional colours.

The palette is intentionally brighter than dark timber. At night, lighting may darken it naturally, but the unlit texture must retain visible separation between reeds, weave gaps, and bindings.

## Marks

All sizes are in pixels on a 64 × 64 texture representing one block.

### 1. Main vertical reed bundles

These establish the material's primary direction and structure.

- Width: 3–7 px.
- Visible length: 24–58 px.
- Typical count: 7–11 bundles per block.
- Orientation: predominantly vertical, with slight bends or lean.
- Spread: distribute across the full block, but vary the spacing. Avoid a regular picket-fence rhythm.
- Shape: long, mostly continuous strips with gently irregular edges. Some bundles may narrow or widen by 1–2 px along their length.
- Colour: primarily `#AD9160`, `#C1A575`, `#92784F`, and `#D3B987`.
- Internal detail: occasional 1 px dark or light edge marks, never a continuous outline on every bundle.

A bundle should read as several reeds grouped together, not as a single perfectly straight plank.

Allow a few bundles to be slightly crooked or kinked. Limit each kink to a lateral displacement of 1–2 px over a 12–20 px length.

Do not make the bundles uniformly spaced, identical in width, or identical in colour.

### 2. Horizontal woven reeds

These make the interlacing unmistakable.

- Width: 2–4 px.
- Length: 12–42 px.
- Typical count: 5–8 visible horizontal courses per block.
- Spread: uneven vertical spacing, usually 7–13 px between course centres.
- Orientation: horizontal, with occasional 1 px rises or falls.
- Shape: flattened bands that pass alternately in front of and behind the vertical bundles.
- Colour: `#92784F`, `#AD9160`, `#C1A575`, and occasional `#D3B987`.

At each crossing, alternate the apparent over-under relationship. A horizontal reed passing in front should interrupt or cover the vertical reed locally; a reed passing behind should disappear beneath it.

Use small, clean interruptions rather than drawing a complete grid of dark crossing lines.

At 64 × 64, the weave should remain readable without zooming in. The horizontal courses must be broad enough to survive distant filtering.

### 3. Reed edge facets

Give the bundles a modest angular, faceted appearance.

- Width: 1–2 px.
- Length: 5–18 px.
- Typical count: 10–18 marks per block.
- Spread: mostly along one edge of selected vertical bundles, with a few on horizontal reeds.
- Shape: short straight segments, small stepped facets, or compact wedges.
- Colour: `#625039`, `#796344`, `#C1A575`, and `#D3B987`.

These marks suggest flat split-reed faces catching light. Keep them discontinuous and irregular.

Do not outline every reed. Most bundle edges should be defined by neighbouring colours and narrow gaps.

### 4. Weave gaps

Use gaps to reveal the depth of the interlacing.

- Width: 1–2 px.
- Length: 2–7 px.
- Typical count: 8–16 per block.
- Spread: concentrated at selected crossings and between adjacent bundles.
- Shape: short, broken dark notches rather than long continuous lines.
- Colour: `#493B27` and `#625039`.

Keep these gaps sparse. The texture should read as a dense woven screen, not a lattice of holes.

No dark gap should form a continuous line longer than 7 px unless it belongs to a genuine panel joint or perimeter boundary.

### 5. Binding ties

Bindings provide evidence of hand assembly.

- Width: 2–3 px.
- Length: 4–9 px.
- Typical count: 2–5 ties per block.
- Spread: place ties at a few horizontal courses, with irregular intervals.
- Shape: compact bands wrapping across a bundle or around a crossing. Use a short light segment beside a darker segment to suggest a loop.
- Colour: `#776044`, `#B69A68`, and occasionally `#625039`.

A tie should read as a deliberate fastening, not another full-width woven course.

Do not put a tie at every crossing. Their scarcity makes them useful construction details.

### 6. Exposed reed ends

Use these only where the panel terminates or where a small break reveals a cut end.

- Width: 2–5 px.
- Length: 2–5 px.
- Typical count: 2–6 per block on exposed-edge variants; 0–2 on uninterrupted panel faces.
- Spread: cluster near panel edges or a small damaged area.
- Shape: blunt, squared ends with a slightly uneven edge.
- Colour: `#D0B78B`, `#C1A575`, and `#92784F`.

Do not scatter cut ends across the centre of an intact panel.

### 7. Panel joints and courses

The weave is the surface construction; the panel's perimeter and assembly joints establish that it is a building component.

- Joint width: 2–4 px.
- Joint length: 12–64 px, depending on the edge or seam.
- Typical count: 0–2 internal seams per block, plus perimeter details when the texture represents an exposed panel boundary.
- Shape: a narrow dark seam, a slightly raised binding, or a change in reed alignment.
- Colour: `#493B27`, `#625039`, `#776044`, and `#92784F`.

Internal seams should be uncommon. When present, make them look like two separately woven sections brought together, not like a regular tile border.

ASSUMPTION: The texture can distinguish an uninterrupted panel face from a panel edge or joint. If the mapping system cannot do this, omit explicit perimeter marks from the shared face texture and retain the edge treatment in the side texture.

## Top vs side

### Top texture

Use the top texture for upward-facing panel surfaces, such as a horizontal woven screen or the upper face of a panel.

- Keep the weave direction clear: long reed bundles run in one dominant direction, crossed by shorter horizontal weavers.
- Preserve the 3–7 px bundle widths and 2–4 px course widths.
- Show only a few binding ties.
- Use a restrained mix of warm and weathered reeds.
- Keep the surface relatively flat, with small raised overlaps at crossings.
- Do not introduce a strong checkerboard pattern from alternating reed colours.

The top should read as a woven sheet viewed from above, not as a field of parallel timber planks.

### Side texture

Use the side texture for vertical panel faces and exposed edges.

For a vertical woven screen, the side texture may use the same weave construction as the top, rotated or remapped to match the intended physical orientation.

For a narrow exposed panel edge:

- Make the panel thickness visible as a compact band of bundled reed ends.
- Use short 2–5 px cut-end marks.
- Group the ends into irregular clusters rather than a uniform row of identical circles.
- Include occasional 2–3 px binding bands that wrap around the panel thickness.
- Use `#625039` and `#493B27` for small gaps between bundles, with `#C1A575` and `#D0B78B` for cut faces.

The edge must look like a bundle of reeds held together, not like solid timber.

ASSUMPTION: The building layer can select a side treatment according to whether the viewer sees a woven face or an exposed panel edge. If it cannot, use the woven-face treatment for both and reserve the cut-end treatment for separately generated edge geometry.

## Height

Use the material's 36-level height map, where low values represent recesses and high values represent raised features.

The height map should describe the physical weave, not cast a deep relief across the whole panel.

### Low features

- Recessed weave gaps: 2–5 levels above the local minimum.
- Shallow grooves along selected reed edges: 3–7 levels above the local minimum.
- Background between bundles: low and comparatively smooth.

These allow the woven construction to remain legible without turning every colour boundary into a ridge.

### Mid-height features

- Main reed surfaces: 10–17 levels above the local minimum.
- Horizontal weavers: 12–19 levels above the local minimum.
- Slightly raised bundle edges: 14–20 levels above the local minimum.

Most of the panel should sit within this middle band. Adjacent reeds should have small height differences, enough to suggest overlapping strips without producing jagged relief.

### High features

- Over-under crossings: 18–23 levels above the local minimum.
- Binding ties: 20–25 levels above the local minimum.
- Occasional raised reed tips: 18–22 levels above the local minimum.

Keep these features compact. Bindings and crossing overlaps may rise above their neighbours, but they must not become isolated spikes.

### Blending rules

- Keep the height map low-contrast overall.
- Make the broad reed bodies continuous, with gradual transitions at their edges.
- Give horizontal weavers a shallow raised profile rather than a tall ridge.
- Restrict the highest values to small crossings and bindings.
- Do not create long, narrow, high ridges along every reed. They may produce unwanted streaks when this material meets another material.
- Avoid isolated high texels that would appear as sharp bumps in a blended surface.

ASSUMPTION: The height map is used for material blending or relief in the building layer. If built pieces do not use height-based blending, retain these values as relative surface-relief guidance rather than allowing the ground-material blending rules to alter building geometry.

## Variation

The material should look hand-woven and irregular, while preserving a coherent construction pattern.

### Within one block

- Vary each vertical bundle's width by 1–2 px.
- Shift horizontal course spacing by 1–3 px from the local average.
- Give a few bundles a 1–2 px bend.
- Vary the colour of neighbouring reeds without making adjacent bundles strongly striped.
- Place bindings irregularly, avoiding a repeating tie pattern.
- Use small changes in the visible over-under sequence.
- Let some bundles partially obscure their neighbours.

Keep the weave recognisable. Variation must not destroy the dominant vertical-and-horizontal structure.

### Between neighbouring blocks

- Use deterministic procedural variation so the same block seed always produces the same texture.
- Change bundle widths, reed colours, course spacing, small bends, and tie positions between blocks.
- Offset the internal weave phase so horizontal courses do not form continuous stripes across every block boundary.
- Avoid repeating the same distinctive dark gap or binding position on neighbouring blocks.
- Keep the general material direction consistent unless the physical panel is deliberately rotated.
- Do not draw a full border around each 64 × 64 texture.

When adjacent blocks represent one continuous panel, avoid visible seams caused only by the texture boundary. The generator should support a shared or coordinate-based variation seed for continuous surfaces.

When adjacent blocks represent separate panels, allow a genuine construction joint or slight alignment change to communicate that they are separate pieces.

ASSUMPTION: The generator can receive a stable seed or world-space coordinate for each block. If it cannot, use a repeatable per-block seed derived from the existing material-generation inputs.

## Must never look like

- A perfectly regular basket-weave checkerboard.
- A uniform fence made from evenly spaced sticks.
- Solid timber planks with painted-on horizontal lines.
- A thatch roof made from overlapping straw bundles.
- A smooth, featureless tan wall.
- A photographic scan of real reeds.
- A high-contrast black lattice with large open holes.
- A bright yellow straw surface.
- A tiled texture with obvious square borders.
- A collection of thin, tall ridges that protrude unnaturally through neighbouring materials.
- Another game's recognisable material style.

## Generator recipe

Use the existing Python generator approach: seeded value noise, scatter, simple geometric masks, and colour ramps. No external assets, image downloads, hand-painted source images, or specialised tools are required.

### 1. Establish the base

1. Create a 64 × 64 pixel texture.
2. Fill it with `#AD9160`.
3. Generate a low-amplitude value-noise field with a broad scale of 16–32 px.
4. Use that field to make gentle patches of `#92784F`, `#C1A575`, and `#9B966C`.
5. Keep the base variation subtle. Do not let noise obscure the woven marks.

### 2. Lay out the vertical bundles

1. Divide the texture width into 7–11 irregular bundle regions.
2. Use widths of 3–7 px, with gaps or overlaps of 0–2 px.
3. Jitter each bundle's centre by 1–3 px from an even starting layout.
4. Extend most bundles for 24–58 px, allowing some to begin or end outside the texture boundary.
5. Give selected bundles a gentle 1–2 px bend over 12–20 px.
6. Fill each bundle with one or two colours selected from the reed palette.
7. Add occasional 1–2 px edge facets, but leave most edges unoutlined.

Use a combination of broad bundle masks and narrow internal strips to suggest grouped reeds. Avoid drawing every reed as an independent, uniformly thin line.

### 3. Add horizontal weavers

1. Place 5–8 horizontal courses.
2. Use 2–4 px thickness and 12–42 px visible lengths.
3. Space the courses irregularly, generally 7–13 px apart.
4. Break each course into sections where it passes behind vertical bundles.
5. Let front-facing sections overlap the vertical bundles by a clean 1–2 px margin.
6. Alternate front and back sections across the bundle sequence.
7. Select course colours from `#92784F`, `#AD9160`, `#C1A575`, and `#D3B987`.

Avoid a rigid, repeated over-under sequence that creates a mechanical checkerboard. Introduce small, controlled deviations in the crossing pattern.

### 4. Add relief and edge accents

1. Place 10–18 short edge facets, each 1–2 px wide and 5–18 px long.
2. Place 8–16 dark gap marks, each 1–2 px wide and 2–7 px long.
3. Keep dark marks discontinuous, with no long uninterrupted outlines.
4. Use height values to distinguish recessed gaps, main reeds, and raised crossings.
5. Keep most pixels in the mid-height range.

### 5. Add bindings

1. Scatter 2–5 bindings per block.
2. Use 2–3 px thickness and 4–9 px length.
3. Align each binding across a crossing or around a selected bundle.
4. Give each one a dark section in `#776044` and a lit section in `#B69A68`.
5. Avoid placing bindings at uniform intervals.

### 6. Add cut ends and joints

1. For an exposed-edge variant, place 2–6 cut ends near the edge.
2. Give each cut end a 2–5 px width and 2–5 px length.
3. Use `#D0B78B`, `#C1A575`, and `#92784F`.
4. Add a narrow 2–4 px joint only when the texture represents a genuine panel seam.
5. Do not add an automatic border to every texture block.

### 7. Build the height map

1. Start with a low, smooth base.
2. Raise the broad reed masks into the main 10–17 level band.
3. Raise horizontal weavers slightly above the reeds they cross.
4. Raise selected crossing overlaps into the 18–23 level band.
5. Raise bindings into the 20–25 level band.
6. Keep grooves and gaps low.
7. Blur only the height transitions enough to avoid harsh one-pixel spikes; preserve the crisp pixel-art colour boundaries.
8. Inspect the result for long, narrow high ridges and reduce them if necessary.

### 8. Generate the side variant

1. Reuse the same material palette and mark language.
2. For a woven face, preserve the vertical bundles and horizontal courses.
3. For an exposed edge, replace the broad weave with compact groups of cut reed ends.
4. Add occasional binding bands around the edge.
5. Keep the edge profile shallow and the gaps narrow.

### 9. Apply deterministic variation

1. Seed the generator from the available stable material or block seed.
2. Vary bundle widths, colours, course offsets, small bends, and binding locations.
3. For continuous panels, derive variation from shared surface coordinates where available.
4. Prevent the same distinctive marks from repeating at identical positions on every block.
5. Preserve the dominant weave direction and overall colour balance.

### 10. Final checks

- Inspect at exactly 64 × 64 pixels.
- Confirm the woven structure reads before the fine details.
- Confirm that vertical bundles and horizontal courses remain distinct.
- Confirm that joints and bindings look like construction details.
- Confirm that the palette remains readable when darkened by lighting.
- Confirm that no full-block border reveals the texture's repeating square.
- Confirm that no narrow high feature creates a long streak in blending.
- Confirm that the texture reads as woven reed rather than timber, thatch, or a regular basket pattern.