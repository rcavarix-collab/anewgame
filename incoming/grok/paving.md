=== FILE: REPORT.md ===

# Task A28: Built against natural: telling them apart (S)

## Files

- `built_vs_natural.md` — Visual design rules for distinguishing built materials from natural ground materials, with paired examples and generator guidance.

## Done when

* At least six rules: Yes — eight rules covering regularity, edges, colour ranges, mark language, alignment, repetition, height, and wear.

* Each rule has an example pair: Yes — every rule includes a built-versus-natural comparison.

* Built materials remain distinct from all 12 ground materials: Yes — rules address the shared visual characteristics of meadow grass, dry turf, moss, dirt, loam, clay, sand, gravel, stone, slate, sandstone, and snow.

* No unsupported tool dependencies: Yes — the guidance uses procedural noise, scatter, colour ramps, and geometric patterns.

* All game-facing text can be translated: Yes — this file specifies visual design only and introduces no player-facing text.

## Assumptions

* ASSUMPTION: Built materials include manufactured or deliberately assembled surfaces such as paving, brickwork, planks, masonry, and woven panels. The exact catalogue of built materials is not specified.

* ASSUMPTION: Built materials may use the same world-projected, 64 × 64-pixel-per-block texture system and height-map blending as natural materials.

* ASSUMPTION: The existing renderer supports the stated noise, scatter, colour-ramp, and geometric-pattern generation methods, but no additional shader or rendering features are required.

## Self-check

1. Hard rules kept: Yes

2. Everything I say about the game comes from the task's facts or is labelled ASSUMPTION: Yes

3. What I'm least sure of: The exact built-material catalogue and whether all built surfaces use the same texture projection and height-map blending as ground materials.

## Questions for the owner or Claude

* Which built materials are already implemented, and which are planned?

* Do built surfaces use the same top/side texture and height-map blending rules as natural ground?

=== FILE: built_vs_natural.md ===

# Built Against Natural: Telling Them Apart

## Purpose

Built materials should read as deliberately made or assembled, while natural materials should read as grown, deposited, eroded, or fractured.

The distinction must survive at 64 × 64 pixels per block, under daylight and moonlight, and when viewed across faceted terrain. It must come from the marks, their arrangement, the edges, and the colour structure—not from extra rendering effects.

The natural materials already have distinct mark languages: grass blades, moss patches, soil grains, sand ripples, gravel pebbles, stone fractures, slate layers, sandstone strata, and snow deposits. Built materials must not simply reuse these marks in regular rows and call them manufactured. Their geometry and organisation should communicate construction.

## Rules

### 1. Regularity: built marks follow a deliberate plan

Rule: Give built materials a repeated, recognisable unit with controlled dimensions and spacing. Natural marks may cluster or repeat statistically, but should not maintain exact spacing across a surface.

* Built: brick faces arranged in courses, with joints aligned along consistent rows.

* Natural: gravel pebbles scattered in uneven clusters, with variable gaps and orientations.

Generator guidance: Define a base unit and a spacing interval in pixels. Add small, bounded variation to individual units, but retain the overall arrangement. For natural comparison materials, use irregular scatter or noise without a fixed repeating lattice.

Example pair: A brick wall with aligned courses versus a gravel patch with irregularly distributed stones.

### 2. Edges: built boundaries are intentional

Rule: Built units should have clear, designed boundaries. Natural boundaries should emerge from fracture, growth, deposition, or gradual blending.

* Built: rectangular paving slabs with readable corners and consistent joint widths.

* Natural: angular stone fragments with varied outlines and uneven gaps.

Generator guidance: Use geometric masks for built units. Keep corners and joint widths consistent within a surface, allowing occasional chips or worn corners. Use irregular masks and varied outlines for natural material marks.

Example pair: A paved path with straight slab joints versus a natural stone field with uneven fragment edges.

### 3. Colour ranges: built surfaces use controlled families

Rule: Give each built material a compact, coherent palette, with deliberate separation between the main surface, joints, and wear. Natural materials should retain more local variation from deposits, growth, mineral changes, or moisture.

A compact palette does not mean a single flat colour. Built materials need enough value separation to show their construction under different lighting.

* Built: warm clay bricks with muted red-orange faces, darker mortar, and a few restrained pale chips.

* Natural: red-brown clay with diffuse ochre patches and irregular darker deposits.

Generator guidance: Use separate colour ramps for the unit face, joint, and wear. Keep all ramps within one material family. For natural materials, use broader, spatially irregular colour fields rather than assigning a separate fixed colour to every geometric unit.

Example pair: A brick wall with consistent terracotta faces and dark joints versus natural clay with uneven ochre and brown patches.

### 4. Mark language: construction marks are not natural marks

Rule: Built marks should describe manufacture, assembly, or deliberate finishing. Do not use the defining marks of a natural material as the main marks of a built surface.

* Built: repeated rectangular brick faces, cut-stone edges, plank seams, or orderly woven crossings.

* Natural: grass blades, moss cushions, sand ripples, or irregular mineral fractures.

Generator guidance: Select marks according to the material's construction method. Use rectangular units for masonry, long aligned seams for planks, and repeated over-under crossings for woven surfaces. Avoid generating these marks with unrestricted scatter.

Example pair: A timber-plank floor with parallel seams versus meadow grass with short, independently oriented blades.

### 5. Alignment: built features share a common direction

Rule: Give built features a dominant orientation or a small set of deliberate orientations. Natural features should follow environmental processes or local variation rather than a single construction axis.

* Built: parallel plank seams running in one consistent direction.

* Natural: sandstone strata that vary in thickness, continuity, and local shape.

Generator guidance: Establish a dominant axis for each built surface. Align seams, grain marks, or unit edges to it. For natural materials, allow orientation to vary with local features; avoid imposing a perfectly uniform direction unless that material specifically calls for it.

Example pair: A plank floor with parallel seams versus natural sandstone with uneven, interrupted bands.

### 6. Repetition: built repetition is legible, not mechanically tiled

Rule: Built surfaces may repeat units, but the texture must not reveal an obvious 64-pixel block boundary. The construction pattern should continue coherently across neighbouring blocks.

* Built: a brick course whose joints appear to continue naturally across block boundaries.

* Natural: a moss surface with irregular cushions and gaps that do not repeat at fixed intervals.

Generator guidance: ASSUMPTION: The generator can use a shared world-space pattern origin or equivalent coordinate-consistent pattern placement. Define the construction layout in world space rather than restarting it at every texture block. Vary face colour, small chips, and wear independently within bounded ranges. Do not randomly shift the whole pattern at each block edge.

Example pair: Brickwork with continuous courses across adjacent blocks versus moss with irregularly distributed patches.

### 7. Height: built relief follows the construction

Rule: Use height differences to reinforce joints, seams, and deliberate surface relief. Natural height should describe growth, loose deposits, or irregular geological features.

* Built: paving slabs with mostly level faces and shallow, narrow joints.

* Natural: gravel with rounded or angular pebbles rising individually above the ground.

Generator guidance: Keep the interior of a built unit relatively uniform in height. Put most height change at joints, bevels, and occasional wear. For natural surfaces, distribute height variation according to the material's marks. Avoid tall, thin ridges that could protrude conspicuously through neighbouring materials.

Example pair: Level paving faces separated by shallow joints versus gravel with individually raised pebbles.

### 8. Wear: built wear respects the manufactured form

Rule: Wear should modify a recognisable built unit without destroying its underlying construction pattern. Natural wear should not imply that the material was cut, laid, or assembled.

* Built: a chipped paving corner or a worn brick edge, with the original rectangular unit still readable.

* Natural: an irregularly fractured stone with no implied manufactured outline.

Generator guidance: Apply wear as a secondary mask over the unit geometry. Concentrate chips at corners and edges, and use restrained surface variation within each face. Keep the underlying joints and alignment legible. For natural materials, generate fractures and deposits independently of any implied manufactured grid.

Example pair: A chipped rectangular slab with its straight joints intact versus a naturally fractured stone with an irregular perimeter.

## Colour and readability safeguards

These safeguards apply across the built-material family.

* Use value differences, not hue alone, to distinguish units and joints. A surface that becomes nearly monochrome under moonlight should still retain its construction pattern.

* Avoid relying on very dark outlines. Joints should be visible through a modest value contrast with adjacent faces, not through near-black borders.

* Keep accents sparse. A few chips, pale inclusions, or weathered areas should add life without competing with the main unit pattern.

* Do not make every unit identical in colour. Use bounded variation in face colour, wear, and small inclusions.

* Do not give every built material the same joint pattern. Brick courses, slab joints, plank seams, and woven crossings should remain visually distinct.

* Preserve the faceted ground as the dominant large-scale form. Texture detail should explain the material, not make the ground appear smooth or photographic.

## Generator recipe

Use this procedure as a general starting point for a built-material texture. Adapt the unit geometry and marks to the material being generated.

1. Choose the construction unit. Define whether the surface is made of bricks, slabs, planks, blocks, or woven elements. Specify the unit's width, height, orientation, and joint layout in pixels at 64 × 64 pixels per block.

2. Establish the pattern layout. Generate the unit boundaries from a regular geometric arrangement. Keep the arrangement continuous across block boundaries. ASSUMPTION: The generator can derive pattern placement from world coordinates or receive a consistent pattern offset.

3. Create the base faces. Assign each unit a base colour from a compact palette. Use a small, bounded colour variation between neighbouring units so repetition remains legible without appearing mechanically identical.

4. Generate joints and edges. Draw joints as continuous gaps or mortar bands. Give them a consistent nominal width, with only small local variation. Keep the joint colour distinct enough to read in both bright and dim lighting.

5. Add construction-specific marks. Scatter small chips, tool-like facets, grain marks, or other appropriate details within unit boundaries. Align marks with the construction where appropriate. Do not let details regularly cross joints unless the material calls for it.

6. Apply restrained wear. Use low-density edge chips, corner damage, and subtle face variation. Preserve the main unit silhouette and layout.

7. Build the height map. Keep unit faces relatively level. Make joints slightly lower, and add only modest local relief for bevels, chips, or surface texture. Avoid tall, narrow ridges.

8. Check the repeating boundary. Compare adjacent 64 × 64-pixel blocks. The construction pattern must continue coherently, and colour variation must not create a visible block grid.

9. Check the natural comparison. Place the texture conceptually beside its closest natural counterpart. If both read as the same material at a glance, strengthen the built unit geometry, alignment, or joint structure rather than simply increasing contrast.

10. Check lighting robustness. Inspect the palette as if viewed at dawn, noon, dusk, and under moonlight. Preserve differences between faces, joints, and wear through value separation as well as hue.

11. Check the final scale. At 64 × 64 pixels per block, confirm that the unit pattern, joints, and major marks remain clear. Remove details too small to survive the intended display scale.

## Must never look like

* A natural ground texture with straight lines drawn over it.

* A perfect, sterile checkerboard of identical units.

* A photographic material sample.

* A surface distinguished only by being darker or more saturated than its natural counterpart.

* A pattern that restarts visibly at every 64 × 64-pixel block.

* A field of tall, thin ridges that creates streaks through neighbouring materials.

* A smooth, featureless plane whose construction cannot be read at a glance.

## Scope note

These are visual rules, not a requirement to make every built material rigid or pristine. Old paving can be uneven, weathered brick can have softened corners, and timber can show irregular grain. The key distinction is that wear modifies a recognisable construction pattern; it does not erase the fact that the material was deliberately made or assembled.

END OF DELIVERY
