# walkgrid — Height maps and material borders

## Purpose

Define a consistent height hierarchy for all 12 ground materials.

The goal is to let distinctive, compact material features show through at boundaries without creating noisy, spiky borders. Broad height variation should make neighbouring blocks feel naturally interwoven rather than separated by a visible grid.

The faceted ground remains the main visual feature. Height maps add material identity and subtle contact variation; they must not make the terrain look furry, jagged at every texel, or covered in thin raised lines.

## Height-map convention

ASSUMPTION: Height values are integer levels from 0 to 35, where 0 is the lowest and 35 is the highest.

Use the following interpretation:

| Height band | Levels | Intended role |
|---|---:|---|
| Recessed | 0–5 | Small pits, gaps, and low spaces between features |
| Low | 6–11 | Quiet background variation |
| Base | 12–17 | Main surface of the material |
| Raised | 18–23 | Broad surface features and ordinary protrusions |
| High | 24–29 | Distinctive compact features that should win some borders |
| Very high | 30–33 | Rare, broad, strongly distinctive features |
| Extreme | 34–35 | Reserved; do not use for ordinary ground materials |

These bands are shared reference points, not a requirement that every material use every band.

### Border comparison

ASSUMPTION: At overlapping projected texels, the material with the greater height value wins.

- Use the same height-map convention for every material.
- Keep each material's ordinary base within its assigned band.
- Make border-winning features sparse and spatially compact.
- A feature should win because it is a meaningful part of its material, not because a single pixel has an arbitrary extreme height.
- Do not use a continuous tall ridge to force a material to win along an entire boundary.
- Avoid height discontinuities that create a sawtooth outline at the scale of individual texels.
- Do not introduce a new renderer rule or a material priority system as part of this task.

If equal heights require a tie-break, retain the renderer's existing deterministic behaviour.

## Material height profiles

The base range describes the typical surface. The feature range describes the raised details that may poke through a neighbouring material.

| Material | Base levels | Feature levels | Border behaviour |
|---|---:|---:|---|
| Meadow grass | 12–17 | 20–27 | Compact grass clumps rise above low, smooth ground; individual blades must not become tall, isolated spikes. |
| Dry turf | 12–17 | 18–24 | Short, flattened tufts rise modestly above the base; remain lower than lush meadow clumps. |
| Moss | 13–18 | 19–25 | Soft, rounded cushions create broad, low protrusions; avoid sharp peaks. |
| Dirt | 12–17 | 18–22 | Small, rounded clods provide occasional raised details without making dirt look rocky. |
| Loam | 12–18 | 18–23 | Broad, soft clumps and shallow mounds; slightly more varied than dirt, but not sharply raised. |
| Clay | 12–17 | 17–21 | Mostly even surface with shallow, broad lumps; no raised cracks or narrow ridges. |
| Sand | 12–17 | 18–23 | Rounded grains and low, broad ripples; ripples are colour and value marks, not tall ridges. |
| Gravel | 12–17 | 22–29 | Individual compact pebbles rise above neighbouring surfaces; keep them rounded and separated. |
| Stone | 13–18 | 20–27 | Broad, angular chips and shallow fractured plates; avoid needle-like points. |
| Slate | 13–18 | 19–25 | Broad, flat chips and shallow plate edges; keep the height restrained so the surface reads as slate rather than rubble. |
| Sandstone | 12–17 | 19–26 | Broad grains and shallow, blocky surface relief; the side's strata remain colour bands, not tall ridges. |
| Snow | 14–19 | 19–25 | Soft, rounded drifts and compact snow clumps; no sharp peaks or thin raised crests. |

## Border winners and feature policy

The winner column specifies which material's raised features should generally win when they overlap the other material's base surface.

This is not a claim that every texel of the winning material must be higher. Low areas and recessed marks remain below the neighbour where their heights are lower.

| Material pair | Border winner | Features that poke through | Height relationship | Reason |
|---|---|---|---|---|
| Meadow grass / Dry turf | Meadow grass | Compact lush grass clumps | Meadow 20–27; dry turf 18–24 | Lush clumps give the greener material a slightly fuller profile; dry turf remains flatter. |
| Meadow grass / Moss | Meadow grass | Compact grass clumps and rounded moss cushions | Grass 20–27; moss 19–25 | Grass blades and clumps should be recognisable, while moss remains a low, soft mat. |
| Meadow grass / Dirt | Meadow grass | Grass clumps; occasional dirt clods in low areas | Grass 20–27; dirt 18–22 | Vegetation sits above the soil, but exposed soil remains visible between clumps. |
| Meadow grass / Loam | Meadow grass | Grass clumps and broad loam mounds | Grass 20–27; loam 18–23 | Grass is the visible surface cover; loam adds subdued relief beneath it. |
| Meadow grass / Clay | Meadow grass | Grass clumps | Grass 20–27; clay 17–21 | Vegetation rises above the comparatively even clay surface. |
| Meadow grass / Sand | Meadow grass | Grass clumps and low sand ripples | Grass 20–27; sand 18–23 | Grass should read as growing above sand without being obscured by raised ripples. |
| Meadow grass / Gravel | Gravel | Compact pebbles and occasional grass clumps | Gravel 22–29; grass 20–27 | Distinct pebbles can interrupt the grass cover; grass remains visible between them. |
| Meadow grass / Stone | Stone | Broad stone chips and grass clumps | Stone 20–27; grass 20–27 | Broad stone fragments can protrude through grass, while grass softens the surrounding ground. |
| Meadow grass / Slate | Meadow grass | Grass clumps and shallow slate plates | Grass 20–27; slate 19–25 | Vegetation covers the flatter slate surface; slate plates remain visible in gaps. |
| Meadow grass / Sandstone | Meadow grass | Grass clumps and broad sandstone grains | Grass 20–27; sandstone 19–26 | Grass is the surface cover; sandstone relief stays subdued. |
| Meadow grass / Snow | Snow | Rounded snow clumps and occasional grass clumps | Snow 19–25; grass 20–27 | Snow cover should dominate the ground, with only occasional compact grass details showing through. |
| Dry turf / Moss | Moss | Rounded moss cushions and short dry tufts | Moss 19–25; dry turf 18–24 | The soft moss mat rises gently above the flatter dry turf. |
| Dry turf / Dirt | Dry turf | Short turf tufts and small dirt clods | Dry turf 18–24; dirt 18–22 | Sparse vegetation sits just above the exposed soil without forming a thick canopy. |
| Dry turf / Loam | Dry turf | Short turf tufts and broad loam mounds | Dry turf 18–24; loam 18–23 | Turf remains the surface cover while loam contributes shallow variation. |
| Dry turf / Clay | Dry turf | Short turf tufts | Dry turf 18–24; clay 17–21 | Dry vegetation rises slightly above the smooth clay. |
| Dry turf / Sand | Dry turf | Short turf tufts and low sand ripples | Dry turf 18–24; sand 18–23 | Flattened tufts remain legible without letting sand ridges dominate. |
| Dry turf / Gravel | Gravel | Compact pebbles and short turf tufts | Gravel 22–29; dry turf 18–24 | Pebbles are distinct raised objects; dry turf fills spaces around them. |
| Dry turf / Stone | Stone | Broad stone chips and short turf tufts | Stone 20–27; dry turf 18–24 | Stone chips interrupt the flatter turf surface. |
| Dry turf / Slate | Dry turf | Short turf tufts and shallow slate plates | Dry turf 18–24; slate 19–25 | The low vegetation generally covers slate, while slate plates remain visible in gaps. |
| Dry turf / Sandstone | Dry turf | Short turf tufts and broad sandstone grains | Dry turf 18–24; sandstone 19–26 | Turf remains the cover; sandstone features are broad and low. |
| Dry turf / Snow | Snow | Rounded snow clumps and occasional dry tufts | Snow 19–25; dry turf 18–24 | Snow cover is dominant; dry vegetation may appear only in sparse patches. |
| Moss / Dirt | Moss | Rounded moss cushions and small dirt clods | Moss 19–25; dirt 18–22 | Moss forms a soft cover over the lower, more compact soil. |
| Moss / Loam | Moss | Rounded moss cushions and broad loam mounds | Moss 19–25; loam 18–23 | Moss remains the soft upper layer; loam contributes broad relief. |
| Moss / Clay | Moss | Rounded moss cushions | Moss 19–25; clay 17–21 | Moss cushions rise above the relatively even clay. |
| Moss / Sand | Moss | Rounded moss cushions and low sand ripples | Moss 19–25; sand 18–23 | Moss forms a soft cover; sand relief remains subdued. |
| Moss / Gravel | Gravel | Compact pebbles and rounded moss cushions | Gravel 22–29; moss 19–25 | Pebbles can protrude through the moss mat without turning it into a rocky field. |
| Moss / Stone | Stone | Broad stone chips and rounded moss cushions | Stone 20–27; moss 19–25 | Broad stone fragments protrude; moss softens the spaces around them. |
| Moss / Slate | Moss | Rounded moss cushions and shallow slate plates | Moss 19–25; slate 19–25 | Moss generally covers the flat slate, with both materials retaining low surface variation. |
| Moss / Sandstone | Moss | Rounded moss cushions and broad sandstone grains | Moss 19–25; sandstone 19–26 | Moss forms a soft cover; sandstone's raised marks remain broad and sparse. |
| Moss / Snow | Snow | Rounded snow clumps and occasional moss cushions | Snow 19–25; moss 19–25 | Snow cover dominates overall; moss may show through at low edges and exposed patches. |
| Dirt / Loam | Loam | Broad loam mounds and small dirt clods | Loam 18–23; dirt 18–22 | Loam has a softer, fuller profile; dirt remains comparatively compact. |
| Dirt / Clay | Dirt | Small rounded dirt clods | Dirt 18–22; clay 17–21 | Dirt clods provide modest relief above the smoother clay. |
| Dirt / Sand | Dirt | Small dirt clods and low sand ripples | Dirt 18–22; sand 18–23 | Dirt clods should be locally distinct, while sand ripples remain broad and low. |
| Dirt / Gravel | Gravel | Compact pebbles and small dirt clods | Gravel 22–29; dirt 18–22 | Pebbles are the defining raised features; dirt fills the spaces between them. |
| Dirt / Stone | Stone | Broad stone chips and small dirt clods | Stone 20–27; dirt 18–22 | Stone chips rise above the finer soil surface. |
| Dirt / Slate | Dirt | Small dirt clods and shallow slate plates | Dirt 18–22; slate 19–25 | Dirt fills and softens the flatter slate surface; slate remains visible in exposed patches. |
| Dirt / Sandstone | Sandstone | Broad sandstone grains and small dirt clods | Sandstone 19–26; dirt 18–22 | Sandstone's compact grains create slightly stronger relief than dirt. |
| Dirt / Snow | Snow | Rounded snow clumps and occasional dirt clods | Snow 19–25; dirt 18–22 | Snow cover dominates, with exposed soil appearing in low areas. |
| Loam / Clay | Loam | Broad loam mounds | Loam 18–23; clay 17–21 | Loam has a fuller, softer surface than clay. |
| Loam / Sand | Loam | Broad loam mounds and low sand ripples | Loam 18–23; sand 18–23 | Loam mounds provide compact relief; sand ripples remain shallow. |
| Loam / Gravel | Gravel | Compact pebbles and broad loam mounds | Gravel 22–29; loam 18–23 | Pebbles stand above the softer soil. |
| Loam / Stone | Stone | Broad stone chips and broad loam mounds | Stone 20–27; loam 18–23 | Stone fragments have stronger relief than the soil. |
| Loam / Slate | Loam | Broad loam mounds and shallow slate plates | Loam 18–23; slate 19–25 | Soft soil generally fills and covers the flat slate relief. |
| Loam / Sandstone | Sandstone | Broad sandstone grains and broad loam mounds | Sandstone 19–26; loam 18–23 | Sandstone's compact surface features rise above the softer loam. |
| Loam / Snow | Snow | Rounded snow clumps and occasional loam mounds | Snow 19–25; loam 18–23 | Snow is the dominant cover, while broad loam features may remain visible in sparse patches. |
| Clay / Sand | Sand | Low, broad sand ripples and shallow clay lumps | Sand 18–23; clay 17–21 | Sand's rounded surface marks provide slightly more relief than the even clay. |
| Clay / Gravel | Gravel | Compact pebbles and shallow clay lumps | Gravel 22–29; clay 17–21 | Pebbles protrude above the smooth clay surface. |
| Clay / Stone | Stone | Broad stone chips and shallow clay lumps | Stone 20–27; clay 17–21 | Stone fragments provide stronger relief than clay. |
| Clay / Slate | Slate | Broad, shallow slate plates and shallow clay lumps | Slate 19–25; clay 17–21 | Slate's flat plates rise modestly above the smooth clay. |
| Clay / Sandstone | Sandstone | Broad sandstone grains and shallow clay lumps | Sandstone 19–26; clay 17–21 | Sandstone has slightly more surface relief than clay. |
| Clay / Snow | Snow | Rounded snow clumps and shallow clay lumps | Snow 19–25; clay 17–21 | Snow cover sits above the smoother clay surface. |
| Sand / Gravel | Gravel | Compact pebbles and low sand ripples | Gravel 22–29; sand 18–23 | Pebbles clearly rise above sand; ripples remain low enough not to create streaks. |
| Sand / Stone | Stone | Broad stone chips and low sand ripples | Stone 20–27; sand 18–23 | Stone fragments protrude above the sand surface. |
| Sand / Slate | Slate | Broad, shallow slate plates and low sand ripples | Slate 19–25; sand 18–23 | Slate plates remain distinct without making sand ripples look like raised ridges. |
| Sand / Sandstone | Sandstone | Broad sandstone grains and low sand ripples | Sandstone 19–26; sand 18–23 | Sandstone has slightly stronger compact relief than loose sand. |
| Sand / Snow | Snow | Rounded snow clumps and low sand ripples | Snow 19–25; sand 18–23 | Snow is the dominant cover; sand ripples remain shallow. |
| Gravel / Stone | Gravel | Compact pebbles and broad stone chips | Gravel 22–29; stone 20–27 | Individual pebbles are the more prominent raised features; broad stone chips remain visible. |
| Gravel / Slate | Gravel | Compact pebbles and shallow slate plates | Gravel 22–29; slate 19–25 | Pebbles rise above the flatter slate plates. |
| Gravel / Sandstone | Gravel | Compact pebbles and broad sandstone grains | Gravel 22–29; sandstone 19–26 | Pebbles have the stronger relief; sandstone grains remain broad and low. |
| Gravel / Snow | Gravel | Compact pebbles and rounded snow clumps | Gravel 22–29; snow 19–25 | Pebbles may protrude through thin snow, while snow remains visible between them. |
| Stone / Slate | Stone | Broad stone chips and shallow slate plates | Stone 20–27; slate 19–25 | More irregular stone chips rise above flatter slate plates. |
| Stone / Sandstone | Stone | Broad stone chips and broad sandstone grains | Stone 20–27; sandstone 19–26 | Stone chips have slightly stronger relief; sandstone remains broad and layered. |
| Stone / Snow | Snow | Rounded snow clumps and broad stone chips | Snow 19–25; stone 20–27 | Snow cover dominates overall, with occasional stone chips exposed through sparse snow. |
| Slate / Sandstone | Sandstone | Broad sandstone grains and shallow slate plates | Sandstone 19–26; slate 19–25 | Sandstone has slightly stronger relief; slate remains flat and plate-like. |
| Slate / Snow | Snow | Rounded snow clumps and shallow slate plates | Snow 19–25; slate 19–25 | Snow covers the flatter slate surface, with slate visible in low areas. |
| Sandstone / Snow | Snow | Rounded snow clumps and broad sandstone grains | Snow 19–25; sandstone 19–26 | Snow is the dominant cover, with occasional sandstone grains visible through sparse coverage. |

## Special case: sandstone sides

Sandstone uses a top texture and a layered side texture.

- The side's strata must be colour and value bands, not raised height ridges.
- Keep the height map broad and restrained across the side.
- Do not give each stratum a separate tall ridge.
- Avoid thin, continuous height peaks following the strata.
- Broad, shallow variations may break up the side, but the layered colour pattern should carry the material identity.

## Special case: slate

Slate is currently too dark and blue, nearly black.

- Height must not be used to compensate for its colour.
- Use broad, shallow plate relief.
- Keep the raised features below gravel and broadly comparable to stone, but flatter.
- Avoid thin, sharp plate edges that create a streaky border.
- The slate palette correction belongs to the slate material brief; this task only defines height behaviour.

## Tall-feature restrictions

No material may use a tall, thin feature to win a border.

Do not create:

- One-pixel-wide peaks that rise far above the base.
- Long, narrow ridges used to force a material to win.
- Continuous raised lines along a texture edge.
- Repeated parallel height ridges that form a comb-like silhouette.
- Tall, thin blades whose height map produces isolated spikes.
- Sharp, needle-like stone or gravel peaks.
- Raised sandstone strata.
- Tall snow crests or narrow snow horns.

For elongated marks that are important to the material's identity, such as sand ripples or sandstone strata, express the mark primarily through colour and value. Their height should remain near the base range.

## Generator guidance

The generator should create the height map independently of the colour map, but use the same underlying feature locations where a visible mark needs physical relief.

1. Start with a material-specific base height from the table.
2. Add broad, low-frequency noise for gentle surface variation.
3. Scatter compact feature masks appropriate to the material.
4. Give each feature a shallow centre-to-edge height profile, avoiding isolated high pixels.
5. Clamp the result to the material's feature range.
6. Keep elongated colour marks low in height, regardless of their contrast.
7. Reserve the upper levels for sparse, broad features; do not use levels 34–35 for ordinary materials.
8. Check neighbouring material pairs using the border table.
9. Reject height maps that produce thin streaks, sharp spikes, or a visibly serrated material boundary.
10. Keep variation spatially coherent across a block so the repeating texture does not create an obvious height grid.

## Validation checklist

- [ ] All 12 materials use the same 0–35 height convention.
- [ ] Every unique material pair has a border policy.
- [ ] Each pair identifies a winner and explains why.
- [ ] Each pair identifies the features that may poke through.
- [ ] Feature heights remain within the stated material ranges.
- [ ] No tall, thin features are used to win borders.
- [ ] Sand ripples and sandstone strata remain low-relief marks.
- [ ] Gravel pebbles are compact rather than needle-like.
- [ ] Grass blades do not become isolated height spikes.
- [ ] Snow, moss, and soil features remain broad and rounded.
- [ ] Sandstone's layered side is defined by colour, not raised ridges.
- [ ] Slate remains flatter than gravel and avoids sharp plate edges.