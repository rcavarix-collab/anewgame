# walkgrid material-texture brief

Paste everything below the line into the art conversation. It is also the
format spec the engine's texture loader reads, so the two stay in step:
anything written to this spec drops into `assets/textures/` and loads with
no conversion.

---

I'm building a first-person game on faceted ground: the world is a grid of
one-block cells, but the ground is drawn as angular facets through them, not
cubes and not smooth blobs. Textures are projected onto the ground from the
world (from above and from two sides), so they never stretch on a slope, and
they're filtered smoothly (blended and mip-mapped), not point-sampled. I'd
like you to turn my art style into material textures for it. The engine reads
a plain-text texture format, described below. Please follow it exactly, since
a malformed texture is rejected and the engine falls back to a placeholder.

## Output format (`.vtex`)

Give each file as its own fenced code block, with the file name on the line
above it (for example `stone.vtex`). A file may hold any number of `texture`
and `block` entries, and every `.vtex` file in the folder is loaded.

```
# Lines starting with # are comments. Indentation is optional.

texture stone                 # name: lowercase a-z, 0-9, _
size 16                       # square, 16 or 32 pixels per side
palette
  a 7c7c82                    # one key character, then 6-digit hex RGB
                              # (or 8 digits, RGB + alpha, for see-through blocks)
  b 6a6a70
  c 8f8f95
pixels                        # exactly `size` rows of exactly `size` keys
  aabacaabaaacabaa
  ...                         # (16 rows in total for size 16)
end

block stone                   # must be one of the block names listed below
  all stone                   # texture used on every face
end

block chest
  all   chest_side            # fallback for any face not named below
  top   chest_top
  front chest_front           # the face that looks toward the player on placement
end
```

Rules:
- Faces you can name in a `block` entry are `all`, `top`, `bottom`, `side`
  (all four sides) and `front`. The more specific name wins: `front` beats
  `side`, and `side` beats `all`.
- A palette key is any single printable character except space and `#`.
  Keys only need to be unique within their own texture. Around 16 colours
  or fewer per texture reads best.
- Every pixel row must be exactly `size` characters with no spaces, there
  must be exactly `size` rows, and every character must be in that
  texture's palette. Please re-check row counts and lengths before sending.
- The game's materials are **32 pixels per block**, lit per pixel (a
  height map becomes surface relief). There are two good ways to contribute:
  1. **A material brief** (preferred): a name, a one-line description, a
     palette of 4–8 colours and a note on its structure (plates, strata,
     pebbles, fibres, veins, a framed motif…). It's turned into a 32-px
     material by `tools/natural_textures.py`, following the rules below.
  2. **Pixel art at 32** (or 16, which reads chunkier), optionally with the
     maps below. Keep one size per set.
- Optional maps, after a texture's `pixels` grid and before `end`, each a
  grid the same size as the pixels:
  - `height` — `0`–`9` then `a`–`z` (36 levels, low to high): relief.
  - `shine` — `0`–`9`: how glossy (wet, icy, polished).
  - `glow` — `0`–`9`: what lights up by itself (veins, cores, runes).
- Transparency is only for see-through blocks (`glass`, `crystal`): give
  those palette entries 8 hex digits, `rrggbbaa`, where `aa` is how solid
  the pixel is (`00` invisible, `ff` solid). Glass reads best mostly clear
  (`aa` around `20`–`40`) with a firmer frame; everything else stays opaque
  (6 digits), and alpha on an opaque block is ignored.

## Art guidance for this engine

- **Nothing anyone owns.** No logos, brand marks, trademarks, real
  currencies or crypto symbols, company or product names, official
  insignia, or recognisable characters, creatures or artwork from other
  works — and don't recreate another game's textures or distinctive look.
  Original motifs, natural materials and traditional public-domain
  patterns (knotwork, florals, geometric ornament) are all fine. When
  unsure, make it more original.
- **No regular dither.** Don't fill a background with a repeating cycle
  of keys (`abca` / `cabc`, a checkerboard). Up close it reads as woven
  fabric; at a distance it turns into shimmering diagonal lines across the
  landscape. Build backgrounds from irregular clumps of 2–4 pixels in 2–4
  close tones.
- **No details in fixed spots.** Every tile repeats on every neighbouring
  block, so a flower, crystal, knot or hole in the same place on each tile
  becomes a perfect grid across a wall. Scatter details unevenly, vary
  their size, and think of the tile as a random patch of a larger surface.
- **Lines wrap.** Cracks, veins, strata and grain must leave one edge and
  re-enter at the matching point of the opposite edge, so they continue
  unbroken across blocks. A line that stops short of the edge becomes a
  row of loose dashes when tiled. Strata and bands: one set per tile, in
  uneven thicknesses (a band that repeats every 8 pixels reads as ruled
  paper on a cliff).

- **Tile seamlessly.** A texture repeats across the ground in every
  direction, so its left edge must continue into its right edge and its top
  edge into its bottom edge. Natural materials never have a border.
- **Top and side.** A material has a top (seen on ground up to about 50
  degrees) and a side (on steeper ground and cliffs). Strata, layers and
  anything with a direction belong on the side, where they stay level along
  a whole cliff; tops are seen from above in any rotation, so they must work
  either way round.
- **Height maps matter most.** Every natural material should have a
  `height` map. It gives the surface relief, and it decides how materials
  meet: at a border the taller parts of each (grass tufts, pebbles, cobbles)
  show through the other, so borders come out ragged instead of blurred.
- **Smoothly filtered.** Textures are seen blended, not as crisp pixels.
  Detail reads best in clusters of 2–4 pixels; single-pixel noise blurs to
  mud up close and shimmers far away.
- **No baked light direction.** Facets are lit per pixel by the sun and the
  sky, and hollows are darkened. A highlight painted on one edge will look
  wrong, so keep any shading even and texture-level only.
- **Leave headroom.** Shade darkens by up to about 60%, so avoid pure black
  and pure white. A range of roughly `141414` to `f0f0f0` works well.
- **Orientation.** For side textures, row 0 is the top (toward the sky).

## The materials walkgrid starts with (M1.3, provisional until the owner confirms)

| material | top texture | side texture | lumpiness | notes |
|---|---|---|---|---|
| meadow grass | `meadow_grass` | `dirt` | 0.07 | the bright one; may be toned down |
| dry turf | `dry_turf` | `dirt` | 0.06 | dry, straw-coloured grass |
| moss | `moss` | `dirt` | 0.06 | deep, soft green |
| dirt | `dirt` | `dirt` | 0.05 | |
| loam | `loam` | `loam` | 0.05 | dark earth |
| clay | `clay` | `clay` | 0.03 | orange-red |
| sand | `sand` | `sand` | 0.012 | smooth: never cut into detail |
| gravel | `gravel` | `gravel` | 0.035 | |
| stone | `stone` | `stone` | 0.045 | pale grey |
| slate | `slate` | `slate` | 0.04 | dark blue-grey |
| sandstone | `sandstone_top` | `sandstone_layered` | 0.015 | strata on the side |
| snow | `snow` | `snow` | 0.02 | |

Lumpiness is how far the ground's fine detail is pushed in or out, in blocks
(facetmesh.h). The picks came from contact sheets of 32 candidates
(`docs/pictures/m1_3/`). The art is `natural.vtex` (from
`tools/natural_textures.py`) and `batch_sept.vtex`; anything authored under the
same texture names replaces it.

## Voxistics' blocks

Replaced by the materials above in M1.9. Their textures are parked in
`assets/textures/parked/` (not loaded), kept intact for reuse. The loaded
files hold only the materials' textures and no `block` entries: the
registry (`blocks.h`) names each material's top, side and bottom.

Feel free to propose additional materials that suit my art, using the same
format with a new name plus a one-line description of what each is.
