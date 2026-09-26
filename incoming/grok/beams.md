# Squared Log and Beam Timber

## Purpose and identity

A worked timber material for constructed pieces in walkgrid.

The material represents squared beams cut from logs, shaped by hand, and fitted together into a structure. It should read as timber immediately, but its visual identity must come from a deliberate pixel-art language rather than photographic wood grain.

The signature is a combination of:
- Broad, slightly irregular longitudinal grain marks.
- Chunky, squared-off tool facets.
- Dark, recessed joint seams.
- Distinctive end grain built from broken, angular growth rings.

The result should feel like solid wood shaped into useful construction pieces. The material must complement walkgrid's angular geometry without competing with the faceted ground.

ASSUMPTION: Timber is rendered on constructed beam geometry, so face orientation can determine whether the texture uses longitudinal grain or end grain.

## Palette

Use the following 12 colours. All values are opaque sRGB hex colours.

| Colour | Hex | Role |
|---|---|---|
| Pale sapwood | `#D9B982` | Sparse warm highlights on cut surfaces |
| Sunlit wood | `#C69B62` | Main highlight and exposed cut-face colour |
| Honey wood | `#AF804B` | Main timber base |
| Warm oak | `#94663B` | Secondary base and broad grain |
| Deep amber | `#7D512F` | Dark grain marks and tool facets |
| Bark brown | `#68452D` | Recessed joints and deep grain |
| Cut-face tan | `#D0AA72` | End-grain base variation |
| Ring ochre | `#B88A50` | End-grain growth-ring marks |
| Ring shadow | `#885B37` | Dark sides of growth rings |
| Fresh cut | `#E2C995` | Small cut-face highlights |
| Weathered wood | `#A18A68` | Occasional muted, aged patches |
| Knot brown | `#553A29` | Rare knots and deepest recesses |

Keep the palette warm and earthy. The lightest colours should occupy small areas, and the deepest browns should be reserved for knots, seams, and narrow shadow marks.

Do not introduce pure black. Do not rely on blue or saturated orange to communicate the material.

The colours must remain distinguishable under daylight, dusk, dawn, and moonlight. Preserve value differences between the base, grain, joints, and end-grain rings.

## Marks

All dimensions below are in pixels on a 64 × 64 texture.

### 1. Longitudinal grain

The primary mark language on beam sides is a set of broad, discontinuous wood-grain bands.

- Width: 2–5 px.
- Length: 9–30 px.
- Count: 7–13 marks per 64 × 64 face.
- Spread: mostly parallel to the beam's long axis.
- Shape: stepped, angular bands with occasional short branches or offsets.
- Colour: Honey wood, Warm oak, and Deep amber.
- Distribution: irregular clusters separated by broad areas of quiet base colour.

Allow occasional grain bands to widen or narrow by 1–2 px. Break long marks into offset segments rather than drawing continuous stripes across the face.

The bands should suggest the direction of the wood fibres without resembling fine drawn lines.

### 2. Squared tool facets

Add broad, low-contrast patches representing hand-worked surfaces.

- Size: 6–15 px across.
- Count: 3–7 patches per face.
- Spread: irregularly distributed, with no repeated arrangement.
- Shape: angular polygons and stepped rectangles.
- Colour: Honey wood, Sunlit wood, Warm oak.
- Contrast: subtle; each patch should remain part of the same piece of timber.

These marks imply that the beam has been squared and dressed by hand. They are surface variation, not deep gouges.

Avoid regular diagonal hatching. The facets should feel like a few broad planes, not a decorative pattern.

### 3. Knots

Knots provide occasional interruptions to the longitudinal grain.

- Size: 4–9 px long and 3–6 px wide.
- Count: 0–2 per face.
- Spread: irregular, avoiding corners and joint seams.
- Shape: compact, angular ovals or short nested lozenges.
- Colour: Warm oak, Bark brown, and Knot brown.

Use a dark central mark with one incomplete lighter edge. Add at most two short grain branches around a knot.

Knots must remain chunky and readable at 64 px. Do not use thin, concentric rings or intricate spirals.

### 4. End-grain growth rings

Cut faces use broken, angular rings around an off-centre core.

- Core: 5–9 px across.
- Ring bands: 2–4 px thick.
- Ring count: 3–6 visible partial rings.
- Overall ring cluster: 34–50 px across, adjusted to the face dimensions.
- Spread: centred approximately on the cut log's growth axis, with a slight offset.
- Shape: stepped, squared-off loops, interrupted at irregular intervals.
- Colour: Cut-face tan, Ring ochre, Ring shadow, and Fresh cut.

The rings should be visibly nested but imperfect. Each ring may shift by 1–3 px between corners, with occasional missing segments.

Use broad, interrupted bands rather than thin outlines. Leave enough of the Cut-face tan base visible between rings to prevent the end from reading as a solid dark target.

For smaller visible cut faces, reduce the number of rings before reducing their thickness.

### 5. Joints and courses

Construction seams must distinguish separate pieces of timber.

- Seam width: 2–3 px.
- Secondary contact shadow: 1–2 px, only where needed.
- Count: determined by the actual joint or course layout.
- Spread: aligned to real beam boundaries, not scattered decoratively.
- Shape: straight or stepped seams following the constructed joint.
- Colour: Bark brown and Knot brown.

Add a narrow, uneven highlight on one side of selected seams using Warm oak or Sunlit wood.

For a beam that meets another beam, make the seam terminate at the actual contact boundary. Do not draw a false seam through the middle of a continuous beam.

If joints are represented by texture alone, use the same seam widths and alignment. If geometry already provides a clear gap or recess, reduce the painted seam to avoid a double-dark outline.

ASSUMPTION: The renderer can distinguish real beam boundaries from continuous faces, or Claude can supply the corresponding seam mask.

### 6. Small chips and cut marks

Add a few broad marks where the beam has been shaped.

- Size: 2–4 px wide and 3–7 px long.
- Count: 2–5 per face.
- Spread: mostly near edges, corners, and occasional joints.
- Shape: short stepped notches or compact wedge-like facets.
- Colour: Deep amber, Warm oak, and Sunlit wood.

Keep these sparse. They should communicate hand-working, not damage or decay.

No chip may form a long, thin streak.

## Top vs side

Timber is not ground material. Its top and side designations refer to beam-face orientation, not terrain slope.

### Long side faces

Use the longitudinal-grain pattern.

- Broad grain bands follow the beam's long axis.
- Tool facets create slight plane-to-plane variation.
- Knots appear occasionally.
- Joints and contact seams remain aligned to construction.

The side should read as the length of a squared log, with the fibre direction visible.

### Upper faces

Use the same longitudinal grain language, but make the surface slightly lighter and less contrasty.

- Increase Sunlit wood and Honey wood coverage.
- Reduce the frequency of Deep amber marks.
- Keep grain broad and discontinuous.
- Use occasional shallow tool facets to suggest a dressed upper surface.

Do not create a bright outline around the entire upper face. Its brightness should come from the palette and lighting, not a cartoon border.

### Cut end faces

Use the end-grain pattern instead of longitudinal grain.

- The stepped growth rings surround a compact core.
- The cut-face base is lighter than the side-face base.
- A few irregular radial divisions may connect ring sections, but they must be broad and sparse.
- Do not continue the longitudinal grain bands across the cut face.

The cut should read as a cross-section of solid timber, not a target, eye, or perfect geometric bullseye.

### Joint faces

Where a beam end meets another beam, preserve the end-grain identity if the cut surface is visible.

Where the joint is hidden or covered, use the appropriate side-face texture and a dark contact seam.

ASSUMPTION: The material system supports selecting a texture variant by face orientation. If it supports only one texture, generate a shared atlas containing side, top, and end-grain regions and select the appropriate region per face.

## Height

This material is for built pieces, not ground blending. Its height information should describe shallow surface relief, not alter the beam's overall dimensions.

Use the existing 36-level height convention if the building layer consumes the same kind of height map.

- Lowest values: recessed joint seams, knot centres, and deep grain marks.
- Low values: broad base wood.
- Medium values: broad grain bands and tool facets.
- High values: occasional cut-face ring ridges and small exposed chips.
- Highest values: very sparse, broad tool facets or cut ridges, never thin lines.

Keep the height range restrained. Most texels should remain close to the base level, with marks differing by only a few levels.

End-grain rings may rise slightly above the surrounding cut face, but their relief must be broad enough to avoid narrow, high ridges.

Do not use height to create deep bark-like cracks, sharp splinters, or exaggerated grooves.

If height maps are used only for terrain blending and not for constructed pieces, omit the timber height map rather than applying terrain blending behaviour to the building layer.

ASSUMPTION: The building renderer either supports shallow material relief or can ignore height data for timber.

## Variation

The goal is to make neighbouring beams feel individually worked without making the structure visually noisy.

### Between beams

Vary these properties deterministically:

- Base colour: shift the dominant base between Honey wood and Warm oak.
- Grain density: vary the count by roughly 20–30%.
- Grain width: favour different combinations within the 2–5 px range.
- Knot placement: change the number and position of knots.
- Tool facets: vary their placement and orientation.
- End grain: change ring offsets, interruptions, and local thickness.
- Weathering: use Weathered wood sparingly on selected beams.

Keep all beams within the same warm material family. Variation should suggest different pieces cut from similar timber, not a mixture of unrelated wood species.

### Across block boundaries

Avoid restarting the grain pattern at every 64 × 64 block.

Use a deterministic world-space or beam-space seed to control the pattern. Continue grain direction across adjacent texture tiles when they belong to the same beam.

For separate beams, change the seed so their grain, knots, and tool facets differ.

Use low-frequency variation for broad colour changes and higher-frequency variation only for small surface marks. Do not introduce a regular checkerboard of light and dark blocks.

Joints must remain tied to actual construction boundaries, regardless of texture variation.

ASSUMPTION: The generator can receive a stable beam identifier or world-space seed. If not, use the existing deterministic coordinate-based seed convention.

## Must never look like

- Photographic wood, scanned timber, or realistic high-frequency grain.
- Long, thin, continuous stripes.
- Perfectly concentric end-grain rings.
- A target, eye, or bullseye on cut faces.
- Bark-covered logs when the intended material is squared timber.
- Bright orange planks or uniformly yellow wood.
- Nearly black timber with unreadable grain.
- Repeating identical knots or identical ring patterns on every block.
- Decorative outlines around every beam face.
- Deep cracks, splinters, or noisy damage.
- Another game's recognisable timber style.

## Generator recipe

1. **Choose the face type.** Determine whether the face is a long side, upper face, cut end, or joint surface. Select the corresponding mark recipe.

2. **Choose a stable seed.** Use a deterministic seed based on the beam identity and face orientation. Adjacent tiles on the same beam must share a coherent pattern. Separate beams should receive distinct seeds.

3. **Build the base colour field.** Start with Honey wood or Warm oak. Add low-frequency value noise to create broad, restrained warm and cool-neutral variation. Keep the result inside the specified palette.

4. **Create broad tool facets.** Scatter 3–7 angular patches, each 6–15 px across. Use small shifts in brightness and warmth. Avoid a regular grid or uniform spacing.

5. **Lay in longitudinal grain on side and upper faces.** Scatter 7–13 broad, stepped marks, 2–5 px wide and 9–30 px long. Align them with the beam's long axis. Break them into irregular segments and vary their spacing. Use the darker grain colours sparingly.

6. **Place knots.** On side and upper faces, place 0–2 compact knots, each 4–9 px long and 3–6 px wide. Use a dark centre and an incomplete lighter edge. Allow nearby grain to bend or terminate around the knot.

7. **Generate end grain on cut faces.** Select a core location near the centre, offset it by a few pixels, and create 3–6 broken, stepped rings. Use 2–4 px ring thickness. Vary the shape and interrupt different parts of each ring. Avoid perfectly concentric squares.

8. **Add small tool marks.** Scatter 2–5 short, broad chips or cut facets, 2–4 px wide and 3–7 px long. Keep them sparse and avoid long thin shapes.

9. **Apply construction seams.** Use the joint layout to place 2–3 px seams and optional 1–2 px contact shadows. Align them to actual beam boundaries. Do not scatter seams randomly.

10. **Apply restrained height.** If supported, assign the lowest values to recessed seams and knot centres, slightly raised values to grain and tool facets, and occasional modest ridges to end-grain rings. Keep the majority of the surface near the base height. Never create tall, thin features.

11. **Introduce beam-to-beam variation.** Use the stable seed to vary the base shade, grain density, knot positions, tool facets, and end-grain interruptions. Keep the palette and mark sizes consistent.

12. **Check the 64 px result.** Inspect each face at native 64 × 64 size. The side must read as squared timber, the cut end as end grain, and the joints as separate constructed pieces. Remove marks that merge into noise or become thin streaks.

13. **Check lighting robustness.** Ensure that the material remains legible under bright daylight, dawn, dusk, and moonlight. Preserve the contrast between base wood, grain, seams, and cut-face rings without depending on strong coloured lighting.

14. **Check performance.** Generate textures offline or during asset creation. Runtime rendering should use the resulting textures and ordinary material sampling, without requiring expensive procedural shading or additional per-pixel effects.

## Acceptance checks

- A side face reads as worked, squared timber at 64 × 64.
- A cut face reads as wood end grain without resembling a target.
- Joints and courses remain visible without heavy black outlines.
- Grain is broad, angular, and discontinuous.
- Adjacent beams differ without forming a checkerboard.
- No tall, thin height features are present.
- The palette remains readable in bright and dim lighting.
- The material uses only deterministic noise, scatter, colour ramps, and simple texture selection.