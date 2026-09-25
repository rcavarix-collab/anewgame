# walkgrid — design reference

The design of record for walkgrid. The engine came from Voxistics (docs/FOUNDATIONS.md, decision D1), and so did most of this document: the engine parts are carried over **with their Voxistics section numbers unchanged**, so code comments that cite "DESIGN.md 4.8" and so on still point to the right place. Parts that described Voxistics-only features are kept only in `reference/voxistics/DESIGN.md`, and are marked "not in walkgrid" below. walkgrid's own design starts at Part XXII.

A carried-over passage may still mention a Voxistics feature (The Line, pulse, fliers) until M0 takes that feature out of the code. The code is the truth; a passage like that is corrected when the step that removes the feature lands.

Companion documents:
- `docs/FOUNDATIONS.md`: working rules, layers, threads, budgets, data decisions.
- `docs/PROTOTYPE_OUTLINE.md`: what the first prototype is.
- `docs/DECISIONS.md`: every decision, dated.
- `docs/SCOPE_MOSCOW.xlsx`: scope.

## Part I — Vision and Constraints

### 1.1 What this is
walkgrid is a first-person game built on the Voxistics engine. Its world is a **faceted surface over a cell grid**: angular, crystalline ground, not cubes and not smooth blobs. You walk it, dig and build it one cell at a time, and hear it in step with procedural music. What the game becomes beyond the first prototype is still open: action is expected later, so everything is built to leave room for it.

### 1.2 Non-negotiable constraints, and why each exists
- **Carry the engine across; don't rewrite it.** Voxistics' engine code (rendering, lighting, textures, streaming, saves, audio, UI, profiler) is moved into walkgrid and unhooked from the old game, not re-written from memory. A rewrite of anything that already works needs the owner's approval and a reason (D1).
- **Reference material is ideas only.** The four seed prototypes (Prismative.cpp, drillder.cpp, LG2.cpp, cc_2_2_2.cpp) and Voxistics' own docs live in `reference/`, never compiled. Take ideas from them, not code; each is removed once nothing more is wanted from it (`reference/README.md`). *This rule is about `reference/` only.* It does not apply to the Voxistics engine, which is carried over as code.
- **Layers** (FOUNDATIONS.md 2): a file includes only its own layer or lower ones; the engine never names the game. Checked by `tools/check_layers.py`.
- **Resource discipline first.** Every system states its cost. Budgets per frame are set in FOUNDATIONS.md 4 and shown in F3.

### 1.3 What "optimal" means for this project specifically
**Cost scales with what's actually on screen or actually changing, never with total world size or total elapsed content.** Draw only what can be seen; load ahead what may be seen soon (D16).

---

## Part II — World Representation

### 2.1 Chunking
- 16×16×16 blocks per chunk (4096 cells), stored as flat `uint8_t[4096]` in the prototype.
- Chunks exist only when they contain at least one non-air block that was ever set — pure-air regions of the world allocate nothing. This is what makes "seemingly infinite" cheap: a `std::unordered_map<ChunkCoord, unique_ptr<Chunk>>` only grows with actual player activity, not with world size.
- **Compression upgrade path, deliberately deferred:** a palette-per-chunk scheme (distinct-block list + bit-packed indices sized to `ceil(log2(paletteSize))`) was designed and costed — a nearly-air chunk costs bytes, a 12-block-type terrain chunk costs ~4 bits/voxel (~16KB), versus 64KB for the flat array. Not implemented in the prototype because content variety (10 block types total) doesn't yet justify the complexity; the flat array is simpler and correct, and this is the documented point where compression pays for itself (once per-chunk distinct block counts start regularly exceeding ~16-20).

### 2.2 Coordinate systems
Three coordinate spaces exist and must never be silently mixed:
- **World space** — plain `int x, y, z`, the address of any block anywhere.
- **Chunk space** — `ChunkCoord{x,y,z}`, one chunk's identity in the sparse map.
- **Local space** — `[0,16)` per axis, a block's position inside its chunk.

`ToChunk()` must floor-divide correctly for negative coordinates (`v >= 0 ? v/16 : (v-15)/16`), not truncate — a naive `/` in C++ rounds toward zero and silently misassigns blocks near the origin on the negative side. This was an actual bug caught in the reviewed prototype (`UpdateLoaded` originally reimplemented this incorrectly instead of reusing `ToChunk`).


**The compass** (`common.h`: `kEast`, `kWest`, `kNorth`, `kSouth`). East is +X, west −X, north +Z, south −Z, up +Y. Everything with a direction is defined against these: the sun rises due east, passes straight overhead and sets due west (the world sits on its equator — the sky reads plainly, and noon shadows fall straight down); the moon's path leans a few degrees north of the sun's; the celestial pole lies on the northern horizon, so the stars rise straight up out of the east; the essence map shows north up and east to the right; a player's yaw of 0 faces north and grows turning east. F3 shows the compass point the player faces.
### 2.3 Height bound
Y is bounded to a fixed range (0–255 suggested). Rationale: true unbounded vertical buys nothing for this game's content (no infinite mining depth is planned) and costs real complexity in generation and eventual lighting; a generous fixed range is functionally infinite to a player while keeping Y a simple `uint8_t` if ever useful for compact storage.

### 2.4 Chunk loading
- A radius (`LOAD_RADIUS`, chunks) around the player's current chunk defines the loaded set.
- The loaded set is recomputed **only when the player's chunk coordinate actually changes**, not every frame — recomputing several hundred hash-set entries every single tick regardless of movement was an identified inefficiency in the reviewed prototype and is explicitly avoided.
- Newly-visible columns are *enqueued*, not generated immediately — actual terrain generation drains a capped number of columns per tick, the same bounded-work-queue pattern Part V's falling-block system established (Section 5.1). Generating the whole load radius synchronously (unavoidable at least once, for the initial spawn) would otherwise stall the first frame while every chunk in range generates and meshes at once. Columns are queued **ring by ring outward from the player's own column**, so the ground underfoot always generates first (a corner-to-corner raster order once put the spawn column dozens of ticks down the queue).
- **Player physics never runs on ground that doesn't exist yet.** Ungenerated space reads as air, so until the player's own column is resident they're held exactly in place (no gravity, no movement), and walking into a not-yet-resident column is refused like walking into a wall. A new game starts the player standing on the surface (terrain height is a pure function of x/z, so no chunk is needed to know it). The world's bottom layer (y = 0) can't be broken — there is nothing beneath it — and should a player ever end up below the world anyway, they're put back on the highest solid block of their column. As a backstop, a player whose box overlaps solid blocks — terrain appearing around an edge, a block falling onto them — is lifted one block per tick until free, and a placement that would overlap the player's own box is refused.
- Terrain is generated **one ring beyond the view radius**. A chunk's mesh reads all 8 neighbouring columns (face culling across shared faces, ambient occlusion across edges and corners — 4.2), so a chunk is only meshed once its whole 3×3 neighbourhood is resident; the extra ring is what lets the outermost visible ring be meshed, and waiting means each chunk is built once rather than once per neighbour arrival.
- Every chunk carries a **`modified`** flag: set by any edit, fall or load, i.e. whenever it stops matching what the generator produces. Columns leaving the radius (past a small hysteresis margin, so a player oscillating at the boundary doesn't thrash) are evicted: **unmodified chunks are simply freed** — the generator rebuilds them bit-for-bit on return (2.5) — and only modified chunks move to `g_evictedChunks` (minus GPU buffers). When a column becomes resident again, `GenerateColumn` generates its terrain and overlays any modified chunks held for it. So memory spent on places the player has left scales with what they *changed* there, not with distance walked, and the draw loop and rebuild queue stay bounded by the loaded area (Part 1.3). This is in-memory only, not disk-backed — a deliberate scope decision until the world's scale is settled. `SaveGame` walks both maps for modified chunks (7.2).

### 2.5 World generators
Because unmodified terrain is regenerated rather than stored — on eviction and on load — a world is only reproducible with the exact generator that made it. Each world therefore records its generator (`WorldGenParams`: type, version, seed) in its save, and a generator's output must be a pure function of (params, coordinates). Changing a generator's output means adding a new *version* alongside the old one, which existing worlds keep using; a save naming a generator or version this build doesn't have is refused with a clear reason rather than loaded onto the wrong terrain. Two exist: `hills` v1 (the original sin/cos terrain, which every pre-v5 save is tagged with) and `flat` v1 (surface at y = 12). New worlds currently use `flat` for testing (`DefaultNewWorldGen`, world.cpp). Every world gets a seed now even though neither generator reads one yet, so a seeded noise generator needs no format change.


**Flat v2 (new worlds): a patchwork plain.** The same flat plain at y = 12, but its top layer is a patchwork of three grounds chosen for how they feel underfoot — soft meadow grass, crunchy coastal sand, hard river pebbles — in fractal blobs: three octaves of value noise (48, 20 and 8 blocks across) seeded by the world's seed, summed and thresholded, so each world's patches differ and sand and pebbles sit apart in a sea of grass (about 68 / 14 / 18 %). It's the first generator to read the seed; a few hashes per column, once, when the column generates. Flat v1 worlds keep their plain dirt. (`SurfaceBlockAt`; the three materials are one line to swap.)
---

## Part III — Block Model

### 3.1 Identity: name-based, not position-based
`BlockID` (an enum) is a *runtime* convenience only. The actual identity written to disk is the block's **string name** (`g_blocks[id].name`). This single decision is what allows the block list to grow indefinitely during development — adding, removing, or reordering enum entries never corrupts an existing save, because loading remaps saved names onto whatever the current build's names are, substituting a safe "unknown" (air) for anything genuinely removed, with a logged warning rather than a silent misread.

### 3.2 The block registry
`blocks.h` holds one row per block type in `g_blocks[BLOCK_COUNT]`, the single source of truth every system reads — meshing, gravity, picking, the hotbar (`g_placeableList` is derived from the `placeable` flag), the save format's name table and the texture builder:
```
name         — identity on disk (3.1)
solid        — collision, raycast hits, hides neighbouring faces
foundational — never falls, always supports (Part V)
placeable    — appears on the hotbar
orientable   — stores a facing; its `front` texture goes on that side
hasData      — may carry a per-block data record
texAll / texTop / texBottom / texSide / texFront — texture names (4.3)
```
Adding a block is one enum entry plus one row. No virtual dispatch, no per-block class hierarchy — virtual calls inside the meshing and simulation inner loops would violate the "no virtual dispatch in hot paths" rule.

**Per-block state.** Every chunk stores a state byte per cell alongside the block ID (and saves it). The low 3 bits are a facing (`BlockFace`) for orientable blocks — set on placement so the front faces the player; the other 5 bits are reserved (a machine's on/off, a slab's half...) so they can be claimed without a storage or format change. It travels with a block when it falls.

**Per-block data.** A sparse map per chunk (`Chunk::data`, keyed by cell, null for the vast majority of chunks) holds variable-length records for blocks that need more than a byte — a chest's contents, a machine's buffers (the item-handler interface of Part VI will live here). A record is dropped automatically when its cell's block changes, and is saved with its chunk. Nothing writes one yet; the storage and save path exist so machines don't need a format change.

### 3.3 Prototype block roster
| Block | Foundational | Orientable | Purpose |
|---|---|---|---|
| Air | — | — | Absence of a block |
| Foundation | yes | no | Never falls; the base layer of any build |
| Stone | no | no | Terrain, subject to gravity |
| Dirt | no | no | Terrain, subject to gravity |
| Wood | no | no | Building material |
| Chest | yes | yes | Storage container (item-handler interface, Part VI) |
| Machine | yes | yes | Placeholder processing block |
| Stone slab, wood ramp, tube, stone pyramid / half pyramid / funnel / half funnel | yes (for testing) | per shape | The Prismative.cpp primitives (4.4) |
| Music block, timestream block | no | no | Light up with the music / where The Line passes (18.1) |
| Essence attractor | yes | no | Placeholder player-built node on the essence map (Part XIX) |

---

## Part IV — Rendering

### 4.1 Visual target
Blocky, saturated, clear silhouettes. Procedurally generated textures (patterns drawn in code at startup), not imported art — consistent with the "no external asset files" constraint and directly reusing the *idea* (not the code) behind cc_2_2_2.cpp's 145 generator functions.

### 4.2 The central performance decision: per-chunk merged meshing
Every exposed face of every cube-shaped block in a chunk is combined into one vertex/index buffer pair, drawn with a single `DrawIndexed` call per chunk. This converts draw cost from **O(blocks)** to **O(loaded chunks)** — the single highest-leverage decision in the whole render design, and the direct fix for the original Prismative.cpp flaw (one `UpdateSubresource` + `DrawIndexed` pair *per solid cell*, ~26,000 draw calls/frame at modest fill).

**Face culling:** a face is only emitted if the neighboring cell (in world space, across chunk boundaries where relevant) is not solid. This requires the mesher to query `World::Solid()`, not just the local chunk array, at chunk edges.

**Mesher** (`mesher.cpp`, no D3D, tested natively): each rebuild first copies the solidity of the chunk plus a one-cell shell of its 26 neighbours into a padded 18³ grid, so every culling and ambient-occlusion test is a plain array read — no per-face hash lookups across chunk borders. Vertices are packed to **8 bytes** (chunk-local position as bytes; a texture-array layer; 5-bit u/v, 2-bit AO and 3-bit face in one `uint16`), with the chunk's world origin supplied per draw from a small constant buffer; indices are **16-bit** (the worst case, a 3D checkerboard, is 49,152 vertices). That's a 2.5× smaller vertex buffer and half the index buffer compared with the previous float format. u/v reach 16 so merged (greedy) quads can later tile a texture across several blocks with no format change.

(Positions and u/v are stored in 1/8-block fixed point, so shaped blocks (4.4) fit the same format.)

**Lighting inputs, baked at mesh time:** each vertex carries its face's shade class (which gives the shader its normal and a light per-direction bias) and a per-corner **ambient occlusion** level (the classic voxel AO: the two edge neighbours and the diagonal of the open cell beside each corner); quads are split along their brighter diagonal to avoid the AO anisotropy seam. The lighting itself is a handful of per-pixel multiplies (4.9).

**Mesh rebuild policy:** only when a chunk is marked `dirty` (an edit occurred inside it, or a neighbor's edit could have exposed/hidden one of its boundary faces). Never rebuilt speculatively, never rebuilt every frame. `World::dirtyChunks` holds exactly the resident chunks whose flag is set (chunks enter and leave `World::chunks` only through `GetOrCreateChunk`/`AdoptChunk`/`TakeChunk`/`ClearChunks`, which keep the two in step), so the rebuilder looks only at chunks that need work — nothing at all while the world is static — and picks the ones **nearest the camera** first. New ground, a load or a render-distance change fills in outward from the player rather than in hash-map order.

**Per-frame cost controls, added after real play surfaced measurable stutter:**
- **Padded neighbour grid** (above) — superseded the older per-check fast path: no neighbour check pays a hash lookup at all.
- **Capped mesh rebuilds per frame** (`MAX_CHUNK_REBUILDS_PER_FRAME = 6`, render.cpp), nearest the camera first, and only for chunks whose 8 neighbouring columns are resident (2.4) — entering unexplored terrain can mark several newly-generated columns dirty in the same tick; rebuilding all of them (each a full mesh pass plus two synchronous GPU `CreateBuffer` calls) in one frame is exactly the kind of single-frame spike this cap smooths across several frames instead, mirroring Part V's per-tick work-queue philosophy.
- **View-frustum culling** (`ExtractFrustum`/`FrustumIntersectsAABB`, render.cpp) — the six view-frustum planes are extracted directly from the combined view-projection matrix each frame; a chunk whose AABB doesn't intersect it is skipped in the draw loop entirely. Bounds per-frame draw cost by what the camera can actually see rather than by how much of the world happens to be currently loaded.
- **Chunk eviction** (Section 2.4) — the companion fix to the above: without it, "currently loaded" itself only ever grows, so even a perfectly culled draw loop and a capped rebuild budget would still be iterating (if not drawing or meshing) an ever-larger set every frame. Together, loaded-set size and per-frame draw/mesh work both now track the player's current position rather than their lifetime path through the world.

### 4.3 Block textures — a texture array, authored or procedural
Every distinct block face texture is one layer of a `Texture2DArray` (64×64, full mip chain), and the mesher stamps each face with its layer from `g_blockFaceLayer[block][facing][face]` — resolved once at load, never per vertex. (This replaced a single atlas image, which is what made per-face textures, orientation and mipmaps practical: with one texture per layer there's no neighbouring tile to bleed into, so addressing can wrap and mips can't smear tiles together.) The sampler is point for magnification — crisp pixel art up close — with linear blending between mip levels so distant blocks don't shimmer. The array is an **sRGB** texture, so the shader reads linear colour and the hardware does the conversion for free; mips are averaged in linear light too (a gamma-space average darkens any contrasty texture with distance — a black/white checker would fade to 128 instead of the correct 188).

`blocktex.cpp` builds the set (pure C++, tested natively): for each (block, facing, face) it picks a texture name from the registry (3.2: front > side > all; top/bottom > all), or — if an authored `block` entry exists for that block — from that entry, which replaces the registry's mapping outright so an authored block never mixes in a placeholder face. A name resolves to authored `.vtex` art if present (scaled up by whole pixels, so it stays exactly as drawn), else a procedural placeholder of that name (`foundation`, `stone`, `dirt`, `wood`, `chest`, `chest_front`, `machine`, `machine_front`, `tube`, `music_block`, `timestream_block`, `essence_attractor`, `glass`, `crystal`), else the block's own placeholder, else a magenta checker. Hotbar icons come from the same set (the face a placed block shows the player).

**Authored art:** plain-text `.vtex` files in `assets/textures/` (a palette plus a character grid per texture, plus `block` entries mapping textures onto `all`/`top`/`bottom`/`side`/`front`), specified in `assets/textures/TEXTURE_BRIEF.md` — which doubles as the prompt handed to the art conversation, so the format the art is written in and the format the loader reads are the same document. The folder is found beside the working directory or up to three folders above the exe. A malformed entry is skipped (everything else still loads) and every problem — parse errors with file:line, unknown blocks, missing or unused textures — is written to `assets/textures/_errors.txt`, with a toast at startup saying how many; the file is deleted again once there are none.

The natural placeholders (stone, dirt, wood) are drawn as 16×16 pixel art — the chunky scale the art brief asks for — from the engine's own integer hash, never `rand()`: MSVC's `rand()` is a weak generator whose consecutive values correlate, which lined "random" specks up into diagonal streaks across the landscape on Windows builds (and never on the Linux previews). Every pattern wraps at 16, so tiles meet without a seam (tested: no step across a tile's edge larger than the steps already inside it); dirt is warm browns only, wood is staggered planks with grain running along them.

**Block library and hotbar.** The hotbar is ten slots (keys 1–9 and 0, the wheel cycles them), filled by the player from the **block library** (E, rebindable): a grid of every placeable block's icon, above the hotbar. A click on a block puts it in the selected slot and closes the library; pressing a block and dragging it onto any hotbar slot puts it there and keeps the library open for more; clicking a hotbar slot while the library is open picks which slot a click fills. The hotbar's contents are saved in settings by block name, so registry changes can't scramble them, and an unknown or no-longer-placeable name keeps that slot's default. Layout and the click-or-drag gesture are plain functions (`library.h`), tested natively; the grid scrolls by row with the wheel once blocks outgrow the window.

**Natural materials** (snow, sand, sandstone, cracked earth, clay, basalt, magma rock, log, moss) have authored art in `assets/textures/natural.vtex`, generated by `tools/natural_textures.py` from the palettes of the first art batch: tileable value noise and crack networks computed on a 16×16 torus, details scattered by a seeded hash — the same three rules the brief now asks of hand-made art (no regular dither, no details in fixed spots, lines wrap). Tested: the file loads with no errors or warnings, covers every natural block's faces, and every texture but the log's cut end wraps without a seam.

Seam rule, from a visible 1px line on every block: procedural tiles are plotted pixel-exact, each clipped to its own square (GDI+ pens once painted a strip into neighbouring tiles), and face UVs span exactly 0..1 of a layer (a half-texel inset under point sampling once drew every tile's edge texels at half width).

### 4.4 Non-cube shapes — baked into the chunk mesh
The primitives from the original Prismative.cpp prototype are back as registry shapes (`BlockShape`, `shapes.cpp`): **slab** (upper or lower half, by where on a face you click), **ramp** (rises away from the player who places it), **tube** (a quarter-block bar running out from the face you click, along any axis), **pyramid**, **half pyramid**, **funnel** (an upside-down pyramid) and **half funnel**. Each is authored once in a canonical orientation, in 1/8-block units, and rotated by the block's state byte (3.2). The mesher bakes their polygons straight into the chunk mesh like cube faces — no per-instance draw calls, which is what sank the earlier per-instance pipe blocks (one draw call per shape reproduced the exact per-object cost problem that 4.2 solved for cubes).

To make that possible the packed vertex (4.2) stores positions and texture coordinates in 1/8-block fixed point; it's still 8 bytes. A polygon lying flat on the cell boundary (a slab's bottom, a ramp's back wall, a tube's end) is hidden by a full-cube neighbour; everything else is always drawn, and only full cubes hide neighbouring faces or darken AO — a slab beside a cube leaves the cube's side visible. Sloped faces get their own shade classes (up-facing slopes slightly darker than tops, down-facing ones darker still); shapes take no AO.

**Collision** uses per-shape boxes rather than the whole cell, so a slab is half height, a tube is only as thick as it looks, and a ramp is two half steps; the player now steps up anything up to half a block (never a full block), which is what makes slabs and ramps walkable. Block picking still treats a shaped block as its full cell. The test blocks are foundational for now, so a test build doesn't collapse while you look at it, and the hotbar scrolls with the mouse wheel since the roster outgrew keys 1–9.

**Hotbar icons** (`icons.cpp`) are rendered at load from each block's real mesh by a small software rasterizer — a three-quarter view with the world shader's shading — so shapes, fronts and textures all read in the hotbar; the whole roster costs a fraction of a millisecond at startup.

### 4.5 Block picking — GPU-exact, not CPU-approximate
Two options were compared for "what block is the player looking at":
- **CPU raycast with fixed-step marching** (the original Prismative.cpp approach, `t += 0.1f`): rejected — can skip thin geometry at shallow angles, and face normals are inferred after the fact by comparing consecutive sampled cells, which is wrong at cell-corner crossings.
- **Amanatides–Woo exact voxel DDA traversal** (1987): the algorithm actually implemented. Steps exactly one voxel boundary at a time using only comparisons and one addition per step; the crossed face's normal falls directly out of which axis was stepped, not inferred. This is precise, well-understood, and cheap.
- **GPU ID-buffer readback** (discussed as a theoretically superior alternative once shapes get complex): render block-ID+face-index as color into a tiny offscreen target and read back the pixel under the crosshair. Correct for arbitrary non-cube geometry (a raycast against a ramp's *actual surface*, not its bounding cube) since it reuses the exact geometry already rasterized. **Not implemented in the prototype** — flagged as the eventual right answer once non-cube shapes exist again, but Amanatides–Woo is sufficient and simpler while every solid shape in the current roster is a full cube.

### 4.6 2D/3D split
One D3D11 device, three passes, never two graphics APIs at runtime:
1. **Sky pass** — a large box centered on the camera each frame, drawn first with depth test/write both off (reusing the UI pass's depth-disabled state) so the opaque world pass always overdraws it regardless of the box's actual size. Its view matrix drops the eye position entirely (rotation only), the standard skybox trick for making it rotate with the camera's look direction but never translate with the player's movement. Everything in it — sky gradient, sunset band, sun, moon, stars, clouds — is computed per pixel from the view direction (4.9), so it has no textures and its cost is fixed by the screen, not the scene.
2. **World pass** — perspective projection, depth test on, chunk meshes plus individually-drawn special shapes.
3. **UI pass** — its own shader/input-layout/cbuffer/blend-state/depth-state, drawn last each frame. Vertex positions are supplied already in pixel space and mapped straight to NDC in the vertex shader (`x/screenW*2-1`, `1-y/screenH*2`) — an orthographic projection in substance, without needing a matrix for it. Depth test/write off, alpha blending on (standard src-alpha/inv-src-alpha), so panels and text composite correctly over the 3D scene. Every UI vertex carries a color tint alongside its UV, so the same textured-quad pipeline draws plain glyphs, tinted panels/borders, and full-color icons.

A small font-glyph atlas (ASCII 32–126 in monospace grids, plus a solid-white strip for untextured tinted rectangles) is generated by GDI+ at load time the same way the block atlas is. Text is drawn **1:1** — one texel per screen pixel, point-sampled, snapped to whole pixels — so it stays crisp instead of being resampled from one master size; to still offer several sizes, the glyph set is baked once per size ("band", cell heights 14/18/22/28/34/44 px) and a requested scale picks the nearest band. Glyphs step by the font's own monospace advance rather than a full padded cell, so letters sit at normal text spacing and every menu label fits its row. Rectangles sample the centre of the white strip, so panels and the flat menu dim are uniform to their edges (stretching a glyph cell with a transparent rim once gave every panel a wide faded border and the dim a vignette). Hotbar icons are 32px — exactly half a tile — so point sampling stays even. The crosshair, hotbar, every menu and its sliders are all built from it. Hotbar item icons are drawn by sampling the *same* block atlas the world pass uses, rather than generating separate icon art.

GDI+ is used exclusively at load time to *generate* textures into bitmaps that get uploaded once to GPU textures; it never touches the frame loop. This was an explicit decision against mixing GDI+ and Direct3D rendering live, which would fight over the swap chain surface.

### 4.6.1 Window size, fullscreen and DPI
The backbuffer follows the window: the window is resizable and maximisable, `WM_SIZE` resizes the swap chain and depth buffer (`ResizeRenderTargets`), and the projection's aspect ratio and every UI layout read the live `g_screenW`/`g_screenH` (a minimised 0×0 window keeps the old size). The window can't be made smaller than 960×680 (below that the taller menus and the hotbar would run off-screen). F11 or Display Settings toggles **borderless fullscreen** on the window's current monitor (no exclusive mode, so alt-tab and other monitors behave normally; DXGI's own Alt+Enter exclusive fullscreen is disabled so the two can't fight); the choice is saved. The process declares per-monitor DPI awareness, so Windows never bitmap-stretches the window on a scaled display — the UI stays pixel-exact at its native size (a UI-scale option is the planned follow-up for high-DPI screens).

### 4.8 Lighting effects: sun shadows, outlines, screen-space AO
Three independently toggleable effects (Graphics Settings, saved; outlines and SSAO off by default, shadows on — since 4.9 they carry the lighting: without them the sun reaches under every overhang. Bloom, 4.10, is the fourth and is on), each built so it costs nothing while off and, while on, scales with screen pixels or with what changed — never with world size:
- **Sun shadows.** An orthographic 2048² depth map from the sun (`ShadowLightViewProj`, sky.h) over ±48–112 blocks around the player (wider with render distance), with the centre snapped to whole texels so the shadow edges don't crawl as the player moves. The sun crosses the sky in 50 minutes, so the map is re-rendered only when it's stale — the sun moved ~¼°, the player moved a quarter of the covered area, or chunk meshes changed within the area it covers (far-off chunks streaming in don't count) — and most frames just reuse it (the profiler counts re-renders). The world pixel shader tests each pixel against it with 4 taps of hardware PCF (comparison sampler, border = lit); a normal offset of just over one texel plus a slope-scaled raster bias keep surfaces from shadowing themselves. Shadowed surfaces lose the direct sun and keep the sky's ambient light (4.9), so a shadow is blue-grey at noon and fades out with the sunlight at dusk; the shadow also fades out toward the map's edge rather than ending at a line.
- **Edge outlines** and **screen-space AO** share one post pass over an off-screen copy of the scene and its depth buffer: outlines darken where the depth Laplacian spikes (zero across any flat surface however steeply it's seen, large at silhouettes and block edges); SSAO takes 12 depth samples around each pixel, comparing each to the depth the local surface plane predicts there, so flat ground at a grazing angle doesn't occlude itself. It complements the baked per-vertex AO (4.2), which already darkens block corners. With both off the scene draws straight to the backbuffer — the post path doesn't run at all.

Every shader is syntax- and type-checked off-Windows with `tools/check_shaders.py` (glslangValidator's HLSL front end). At startup a shadow or post shader that fails to compile on a given driver only disables that effect, and the world shader falls back to its pre-shadow variant (`NO_SHADOWS`) rather than failing to start — but never silently: the compiler's output goes to `shader_errors.txt` in the working directory (removed again once everything compiles), a startup toast points to it, and the Graphics menu shows the effect as UNAVAILABLE instead of a toggle that does nothing.


**Re-rendering without a hitch.** When the map goes stale (the sun moved a quarter degree, the player moved, meshes changed) the next one is drawn into a second map a quarter of the chunks per frame, then the two swap — rather than the whole depth pass landing in one frame every few seconds, a regular hitch on a modest GPU. The first map is drawn whole so shadows are present from the start.
### 4.9 Light and atmosphere — faked, but in linear light
The goal is a convincing day without paying for real light transport: no light propagation through the world, no per-block light data, no extra passes. Everything is a few per-pixel multiplies driven by one small per-frame constant buffer (`FrameCB`, filled from `ComputeAtmosphere` in sky.h, which is pure and tested), shared by the sky and world shaders through a common HLSL prelude (`g_atmosphereSrc`).
- **Linear light, filmic finish.** Textures are read as linear (sRGB view), lighting adds up in linear light, and the result goes through an ACES-style tonemap and display gamma. That's what lets the sun be genuinely bright (a sunlit face is ~3× its shaded side) without blowing out to flat white, and what keeps colours rich in shadow.
- **Sun and moon.** Direct light by the face's facing, white-gold high in the sky and orange near the horizon; moonlight is faint and blue and only exists while the moon is up at night. The sun's falloff is softened (√(N·L), a faked wrap) so a low sun still lights flat ground enough for its long shadows to read, and direct sun arrives within a minute or two of sunrise and ends exactly at sunset. Its path runs due east, straight overhead and due west (the world is on its equator: see the compass, 2.2), so noon shadows fall straight down; the stars and moon turn with it as one sky (the star field rides the sun's own angle — slow through the day, quick through the short night).
- **Hemisphere ambient.** Instead of a fixed brightness per face, faces are lit by the sky from above (blue by day, deep blue at night) and a dim warm bounce from below, blended by how much the face points up, times the baked AO. Shadows (4.8) remove only the sun, so shaded areas take on the sky's colour the way real shade does.
- **Sky.** A horizon-to-zenith gradient, a sunrise/sunset band that wraps the horizon and is strongest toward the sun, a haze around the sun, a sun disc and the moon, the star field (Part XVIII), and **clouds** — thin, wispy high-altitude streaks (cirrus) rather than heavy puffs: value noise on a high, far-off plane, stretched along the wind, domain-warped into wisps and drifting with the game clock. They're translucent — at most ~55 % opaque by day and ~25 % at night, so the stars and moon still show through — and lit right through like ice cloud: bright, glowing toward the sun, catching the sunset's colour.
- **Fog into the sky.** Distant terrain fades into exactly the sky colour behind it (the shared `SkyColor` function), from half the load radius out to its edge — so the limit of the loaded world disappears into the horizon instead of ending at a wall, and render distance sets the fog automatically.
- **Exposure.** A fixed, time-of-day exposure stands in for eye adaptation: it lifts only real night (so nights stay playable and moonlit) and leaves twilight's colour alone.
- **Glow.** Reactive blocks (blocks.h `BlockGlow`) add emitted light on top of all of this, and the world and sky shaders write "how much this pixel glows" into the scene's alpha channel for the bloom pass.

All tuning was checked with an offline CPU mirror of these shaders over a real generated world (dawn, noon, sunset, night) before it went in.

### 4.10 Glow (bloom)
Things that give off light should look like they do, so glowing blocks and the sun get a soft halo (and glowing blocks also really light their surroundings, 4.12). The world and sky shaders already know exactly what glows — a reactive block's glow level, the sun's disc and a softer ring around it, with cloud in front subtracted — and write it into the scene target's alpha. Bloom uses that mask rather than a brightness threshold, so only real light sources bloom and a sunlit white wall never smears.

The pass rides on the post pass (4.8): the scene's `rgb × alpha` is shrunk to **quarter resolution** (four bilinear taps, a 4×4 box), then blurred with a separable 9-tap Gaussian (five bilinear fetches per pass) three times, each pass twice as wide as the last — a bright core with a long, soft tail. The targets are small float textures (R11G11B10), so dim halos don't band. The post pass screen-blends the result over the image, which brightens without ever clipping to flat white. Seven quarter-resolution draws and one full-screen composite: a fixed cost set by the window size, whatever the scene holds. It's on by default (Graphics Settings → Glow) and, like the other effects, a failed shader compile only switches it off; with every effect off, the post path doesn't run at all.

### 4.11 See-through blocks (glass, crystal)
Registry blocks can be **translucent** (blocks.h). They're full cubes for collision and picking, but for rendering:
- **Culling.** The mesher's padded grid (4.2) now records what fills each cell: an opaque cube hides any face beside it and darkens AO; a translucent cube hides only faces of its *own kind* (a wall of glass has no inner walls) and never darkens AO; so stone behind glass keeps its face, and glass touching stone loses the hidden one.
- **One buffer, two ranges.** The mesher writes all opaque triangles first and the see-through ones after, and each chunk remembers where the split is. The opaque pass and the shadow map draw only the first range (so glass casts no shadow); a chunk with no glass costs nothing extra.
- **The blended pass.** After everything opaque, chunks holding glass are sorted far to near (a handful, not the world) and their second range is drawn with alpha blending, depth tested but not written, back faces culled (every cube face is wound clockwise from outside — tested — so a glass cube shows one layer, not two). The blend leaves the destination's alpha alone, so a glowing block seen through glass still blooms (4.10).
- **Faked optics.** The same world shader, told it's drawing glass, lets the world behind show through by the texture's alpha, turns toward a mirror of the sky at grazing angles (Schlick's Fresnel, reusing the sky colour function of 4.9 — the "reflection" is the sky model, not a second render of the scene) and becomes more opaque there, and adds a hard sun glint unless the glass is in shadow. No refraction, no extra render targets.
- **Art.** `.vtex` palettes take an optional alpha (`rrggbbaa`, see the texture brief); the placeholders are a mostly clear pale glass with a firmer frame, and a cloudier violet crystal with facet lines. Hotbar icons show them see-through (but never invisible).

### 4.12 Light from glowing blocks, with shadows
Bloom (4.10) makes a glowing block *look* bright, but on its own it's a screen-space halo that spills over walls — light with no shadows. So glowing blocks also light the world around them, faked cheaply with a **light grid**: a 64×64×64 cube of cells around the player (one per block, two channels), built on the CPU by `glowlight.cpp` (pure, tested natively) and uploaded as a small 3D texture.
- **Shadows by line of sight.** Each glowing block lights every open cell within 8 blocks with a smooth (1 − d/r)² falloff, but only cells it can see: a line traced from the block to the cell (the same exact voxel stepping as block picking, 4.5) stops at the first opaque cube. So walls, pillars and floors throw real shadows from it. Overlapping lights add. Glass lets light through.
- **Soft edges for free.** The world shader samples the grid once per pixel, at the centre of the open cell the face looks into, with hardware trilinear filtering — which turns one-block steps into soft light and soft shadow edges.
- **Two channels, animated for free.** Music blocks light the red channel and timestream blocks the green one; the shader scales them each frame by the music playing now and by how near The Line passes (its glow comes and goes as the line sweeps), so pulsing light costs nothing on the CPU.
- **Cost.** Rebuilt only when the player crosses into another chunk (the grid is chunk-aligned, the player always at least a chunk from its faces) or a mesh rebuild touches a chunk holding a light or within a light's reach — never per frame, and a field of hundreds of lights keeps the 128 nearest. A rebuild is a scan of the 64 chunks it covers plus a few thousand short traces per light, then a 512 KB upload. With no lights nearby the shader skips the lookup entirely.

### 4.13 Surface detail: 32-pixel materials with normal, shine and glow maps
The look the game settles on, chosen by eye from a side-by-side of 16 flat, 16, 32 and 64 pixels with maps: **32 texels per block, lit per pixel** — its own thing rather than "Minecraft with better textures", while still reading as a cube world (at 64 the surfaces started to look like photographs glued onto boxes). The texture array stores 64×64 layers, so 32-px art is scaled up by exactly 2× and stays crisp, and the density can be dialled later with no engine change. The materials are *generated* (`tools/natural_textures.py`: each material a set of continuous fields in tile units, sampled at any resolution — colours quantised to short ramps from the art batches' palettes so they stay painterly).
- **Three maps per texture.** A `.vtex` texture may follow its pixels with `height` (0–9 then a–z), `shine` and `glow` (0–9) grids. The builder turns height into a tangent-space normal map by wrapping central differences (tiles repeat, so their slopes do), softening heights drawn at 32/64 px first so gentle slopes don't light up as contour terraces (chunky 16-px art keeps crisp pixel bevels), and stores normal-xy/shine/glow in a second texture array beside the colours, with mips that average normals as vectors.
- **Normal mapping without tangents.** Every face and shape gets its surface frame in the pixel shader from screen-space derivatives of position and texture coordinates — no tangent data in the 8-byte vertex. Sun, moon, sky ambient and the glow grid's light all see the bumped normal; the face's own normal still decides whether the sun reaches it at all, so bumps never light a side turned away.
- **Shine** adds a sun glint (shadowed like the sun) and a faint sheen of sky where the map says a material is glossy — ice, water, wet flesh, pebbles, clay.
- **Glow** lights texels by themselves (lava veins, star-forge sparks, golden motes, the seedling's bud) and feeds the bloom mask; blocks whose glow should also light the world are `GLOW_EMBER` (a steady warm source in a third glow-grid channel); `GLOW_PULSE` breathes its glow map at 0.4 Hz, never below a third, far inside the flash limit.
- **Density is one number:** `tools/natural_textures.py --size` regenerates every material at 16, 32 or 64; all art in a set shares one size.
- **Cost:** one more texture read per pixel, a handful of multiplies, and a few derivative instructions; nothing on the CPU per frame.

The material set is organised in three families from the art batches: the **natural** set (stone, dirt, wood, snow, sand, sandstone, cracked earth, clay, basalt, magma, logs, moss, mossy cobble, meadow grass, water, ice, ash, coral, jungle leaves, leaf litter, peat, salt flat, pebbles, coastal sand), the **dark** set (veined flesh, flesh wound, pulsing membrane, weeping sore, corrupted flesh) and its light counterpart, the **genesis** set (genesis soil, seedling shrine, dawn light, star forge, new log). Water and ice are provisional solid see-through blocks until fluids exist.


**The September trial batch** (`assets/textures/batch_sept.vtex`, `tools/batch_textures.py`): 36 materials for the owner to sort through, made family by family against the texture briefs in docs/REVIEW_2026-09.md — **land** (loam, dark humus, gravel, coarse sand, silt, wet mud, clay bank, river cobbles, slate, granite, limestone, chalk, mossy gravel, heather, dry and frost turf), **building** (fieldstone wall, ashlar, plank floor, weathered boards, thatch, adobe and fired brick, terracotta tile, whitewash, cobble path), **industry** (steel plate, floor grate, two enamel panels, concrete, corrugated sheet) and **strange** (pulse crystal, timeworn stone, void slate, essence moss — light rather than pigment, via glow maps). A dozen parametric recipes (soil, stones, strata, speckle, bricks, planks, turf, plate, ribs, grate, straw, plaster, tiles, lattice) make each material one line of parameters; every texture tiles (the native seam test covers them). Each is a plain cube block named after its texture; rejects get removed, keepers get the name pass.
### 4.14 Plants: cards that face you
Wildflowers, glow mushrooms, ferns, brambles and reeds are `SHAPE_CARD` blocks: one upright, see-through picture that turns about the vertical to face the viewer — the old sprite trick, which gives the impression of a plant from any side for one quad. The mesher emits four vertices all at the cell's base centre with the corners in u/v and a flag in the layer's top bit; the world vertex shader spreads them into a one-block quad sideways to the eye, so every plant turns on its own with no CPU work per frame and no new vertex data. The pixel shader cuts out see-through texels, so cards draw in the opaque pass with no sorting. Plants are walk-through (not solid) but targetable: the block-picking ray hits any non-air block past the cell the eye is in, so standing in reeds doesn't make them the target of every click. They cast no shadow (a card collapses to a point in the shadow pass). Their art is generated at 32 px on transparent backgrounds; the glow mushrooms' caps glow and the block lights the world warmly (`GLOW_EMBER`). Their library and hotbar icon is simply their picture.

### 4.7 Camera and player view
**Input (walkgrid M0.14):** mouse look reads raw mouse movement (`WM_INPUT`, input.cpp), summed per frame, so Windows pointer acceleration and screen-pixel rounding don't apply; the hidden cursor is still recentred each frame so it never reaches a screen edge. If raw input can't be registered, look falls back to the cursor's offset from centre (the Voxistics method). The view is drawn between the last two simulation ticks (smooth motion, 12.y).

Standard FPS mouse-look: yaw from horizontal delta, pitch from vertical delta, pitch clamped to avoid gimbal flip (±~1.55 rad). Perspective projection, near/far planes wide enough for the load radius in use.

**Moving:** walk 4.5 blocks/s; **sprint** (hold Shift, forward only) 6.5; **crouch** (hold Ctrl) 1.8, with the player's box dropping from 1.8 to 0.9 blocks tall and the eye from 1.6 to 0.75 — so the player fits a 1-block-high, 1-block-wide gap. Letting go of crouch stands up only where there's room to; under a low roof the player stays crouched, and a player who finds themselves with no room to stand (a load, a block placed over them) crouches rather than being pushed up out of the space. **Power slide:** pressing crouch while sprinting on the ground drops into a slide — a burst to 9 blocks/s along the way the player was running, bleeding off with ground friction (none in the air) over at most ~1.4 s, at crouch height (a slide goes straight under a 1-block gap), ending in a crouch; a jump ends it early, a wall stops that axis. During the slide the eye drops further and the view **leans into the slide**, relative to where the player is looking: sideways motion rolls the camera toward that side (up to ~12°), straight ahead dips it forward, backwards tips it back, scaled by the slide's speed and eased in and out. The lean is view-only — movement always uses the level right vector. Movement state isn't saved (it re-derives in a tick). All of it is in `UpdatePlayerPhysics` (world.cpp), tested natively.

---


**Stepping up** half a block (a slab, a ramp's step) moves the body at once — collision stays exact — but not the view: the eye is lowered by the step and glides back up on its usual easing (~¼ s), so a step reads as a step, not a teleport. **After the first playtest:** sprint is 7.5 blocks/s and a slide bursts at 10; the slide forgives timing — crouch and sprint may come in either order, a crouch press stays fresh for 0.3 s and a sprint counts for 0.4 s after it ends (so a crouch just before landing, or just after letting go of sprint, still slides), one slide per press.
### 4.15 Faceted props and building pieces
Set-dressing that breaks up the cube grid cheaply: small faceted shapes baked into the chunk mesh like any other shape (4.4), one mesh reused across many materials, so the terrain stops reading as uniform cubes without new geometry per biome. Faceted on purpose (octagons, not circles) to match the pixel-sampled look, and authored on the same 1/8-block grid as every shape.

- **One mesh, three placements.** A prop is authored once, anchored on its bottom face and growing up, leaning a little toward +Z. Placing it against a face (the clicked face, `PLACE_CLICKED_AXIS`) makes that face its anchor: on a floor it's a **Swell** (a mound, bulb or knob); under a ceiling the same mesh hangs as a **Drape**; on a wall it juts out as a **Root snag** or **Ledge**, its lean turned to droop. Props on floors and ceilings are also turned a quarter-turn at a time by a hash of the cell, so a scatter never lines up.
- **The family.** Swell presets: *mound* (low, flat-topped: clumps, tussocks, clods), *bulb* (pinched base, leaning well off-centre: coral nodes, flesh bulbs, membrane sacs), *knob* (tall, pinched: root knuckles), *boulder* (wide and low, worn smooth: pebbles, drifts) and *breaker* (barely above a water surface: a rock tip, a leaf pad). **Shard** is the angular counterpart, a single broken chunk (scree; a ledge on a wall). **Ripple lip** is a thin lapping ridge along one edge of a water surface, for shorelines.
- **Dwelling and industry pieces** use the same machinery: a **beam** (a diagonal rafter; cells placed stair-wise chain into one continuous beam), **corbel** (stepped bracket), **shutter** (louvered panel), **awning** (sloped plate off a wall), **pipe** (octagonal conduit along the clicked axis), **gear** (notched disc in relief), **vent** (a stack flaring at the top), **hopper** (an open inverted frustum, for ore) and **strut** (an X-brace facing the player).
- **How they're built** (`shapes.cpp`): each shape is a few convex parts given by their corner points; an exact integer convex hull turns each into outward-wound faces, once, at startup. Placement applies an integer rotation about the cell centre (the anchor, then the quarter-turn); faces lying flat on a cell boundary are hidden by a full neighbour like any shape's; collision is one box, the bounds over every turn (a mound is low enough to step onto). The prop's own top wears the block's top texture; everything is textured by projection along its facet's dominant axis.
- **Lighting slanted facets.** Faces that aren't axis-aligned carry a slope shade class, and the pixel shader lights them by their exact flat normal (the cross product of the position's screen derivatives, turned toward the viewer) — free, exactly faceted, and it also corrected the older ramps and pyramids, which had used one fixed normal whatever their orientation.
- **Budget.** A prop is 10–30 polygons. A chunk's mesh keeps 16-bit indices; a chunk packed solid with the most detailed props could exceed 65,536 vertices, so any past that limit are left out of that chunk's mesh instead of overflowing. See-through props (ice, water) go in the blended pass.
- **Pruned after the owner's look (playtest):** the canopy-cap mushrooms (glowcap, moss canopy), the coil-stalk fronds, the tiered chimney caps (they read as stacked stone markers) and the pebble/coastal boulders and pebble breaker are gone — "none of those turned out favorable". The shard (the pointy-topped rock) stays. Machines get **bevelled blocks** instead of hard cubes (`SHAPE_BEVEL_CUBE`: every edge chamfered by an eighth — 26 flat polygons, lit by their true normals, a full block for collision): the harvester, the pulse store and the machine.
- **In the library now, not in world generation yet.** The set (35 blocks after the pruning) pairs the shapes with existing materials: moss, meadow, earth, peat, genesis and salt clumps; coral, flesh and membrane bulbs; a snow drift; stone, basalt, magma, sandstone, mossy, ore and ice shards; a water ripple, a leaf pad; log and wood beams, wood and stone corbels, a shutter, an awning; tube and lattice pipes, a machine gear, a foundation vent, an ore hopper, wood and lattice struts. Scattering them through terrain generation comes later.

### 4.16 Soft detail: distance filtering and large-scale variation
Chosen from a four-way comparison (crisp / colour bleed / variation / both): the owner picked both, then dropped the colour bleed after the first playtest — it read as a halo around things. What remains, all per-pixel with no new data:
- **Large-scale variation.** A slow drift of value (±9 % over ~24 blocks, ±4 % over ~9) and warmth (±5 % over ~31) across the world, from the pixel's world position through a small hashed value noise (a few ALU ops; walls vary with height too). It breaks up the repetition of any tiled material over distance — a meadow reads as ground, not wallpaper.
- **Distance filtering.** Pixels stay crisp up close (point sampling); from ~10 blocks out the texture read fades over ~22 blocks into a smooth 4x-anisotropic trilinear read, and surface-map bumps flatten with it, so distant ground doesn't sparkle.
- **Fog.** A gentle aerial haze builds from nearby (up to ~55 % at the loaded world's edge, exponential), and the edge itself still fades fully into the sky, so depth reads and the far terrain never ends in a hard line.

## Part V — Simulation: the falling-block system as the reusable pattern

### 5.1 Why gravity is designed this way
This is the *first* simulation system, and it establishes the template every later system (item transfer, machine ticking) should copy.

**Mechanism:**
1. An edit that removes support beneath a non-foundational block pushes that block's position onto a `fallQ` queue — it is not processed synchronously.
2. Each tick, at most `MAX_FALLS` queue entries are drained — a hard constant cap, independent of queue length.
3. Draining an entry either moves the block down one cell (re-queuing if still unsupported) or lands it permanently.

**Why this matters beyond gravity itself:** it guarantees **O(1)-bounded per-tick simulation cost regardless of how catastrophic a single edit was** — removing the foundation under an enormous structure cannot spike frame time, because the resulting cascade is smoothed across many ticks instead of resolved in one pass. Every future per-tick system (item movement through a pipe network, machine progress updates) is expected to follow this same shape: a capped-per-tick work queue, never an unbounded scan of "everything that might need attention this frame."

### 5.2 Correctness details worth preserving
- **Deferred mutation, not in-place iteration mutation.** The queue pattern avoids the exact bug found in LG2.cpp, where `SpreadGrass`/`HandleLifespans` mutated (inserted into/erased from) the same `std::vector<Block>` being iterated, and where a `Quadtree` held raw pointers into that vector that were invalidated by any subsequent insert or erase.
- **Bulk-load must bypass live gravity checks entirely** — this was an identified bug in the reviewed prototype: calling the live `World::Set()` during save-file loading runs gravity-trigger logic against a world that's only partially reconstructed, and because `unordered_map` iteration order is arbitrary, a block can appear to "lose support" simply because the block that would have supported it hasn't been placed yet. Fix: a separate `SetRaw()` path used only during load, which writes the block into its chunk directly and performs no support checks at all. Gravity re-evaluates naturally from then on as the player interacts with the loaded world.

### 5.3 Tick model
Fixed-timestep logic tick, decoupled from render/present rate via an accumulator loop (`FIXED_DT = 1/60`), generalized into the main loop rather than scattered per-feature. **At most five ticks run in one frame**: after a stall, running every missed tick at once makes the next frame slow too, and that one the next — a hitch that feeds itself; past five the backlog is let go (the world briefly runs a touch behind real time).

---

### 5.4 Scheduled block updates — gravity generalized
The falling-block queue is now one case of a general **scheduled update queue** (`ScheduleUpdate(x, y, z, kind, delayTicks)`, world.cpp): a priority queue ordered by due tick then insertion order, with a handler per `UpdateKind`. Gravity (`UPD_GRAVITY`) is the first kind; machines and anything else whose blocks change over time add a kind and a handler rather than their own queue. At most `MAX_UPDATES_PER_TICK` (64) run per tick however many are due, oldest first — Section 5.1's smoothing, now shared. An idle world has an empty queue and costs nothing: simulation cost scales with what's actually changing (Part 1.3). The profiler shows the time spent and the queue length (Part XVI).

A column with updates pending isn't evicted (a cascade must finish in its own column), and pending updates are **saved** with their remaining delays (save v6), so a save taken mid-collapse finishes collapsing after loading instead of leaving blocks floating. Long-delay updates — machine timers — will want to travel with their chunk (stored per chunk, resumed when the column returns) rather than pinning a column in memory; that's the planned extension when machines arrive.


**Second kind: covered grass dies back** (`UPD_GRASS_COVER`). Placing a block that keeps the sky off (`BlockShadesGrass`: full opaque blocks, machines, slabs — not glass, plants or thin pieces like pipes) queues a check on the first grass below it, three to four and a half game minutes out (varying cell by cell, so a roof's shadow browns unevenly). If the grass is still cut off from the sky then (`OpenToSky`: nothing shading within 64 blocks above), it turns to dirt; uncovered in the meantime, it lives. Night isn't cover, so grass never dies overnight. Cost: one scan down on a placement, one scan up when a check comes due — nothing while nothing changes. (Grass creeping back over bare dirt in the open — LG2.cpp's spreading grass — is the natural counterpart, not built yet.)

## Part VI — Not in walkgrid
Item logistics (pulse): a Voxistics feature. See `reference/voxistics/DESIGN.md`.

---

## Part VII — Save/Load System

### 7.1 Why the legacy approach was unacceptable
All four reference files persisted state via `file.write(reinterpret_cast<const char*>(&block), sizeof(Block))` — a raw struct dump. This fails three ways: (1) any struct field change silently corrupts every old save with no error; (2) no corruption detection — an interrupted write loads however far it got with no signal anything's wrong; (3) block identity is positional (enum/array order *is* the format), so adding a new block type during ongoing development reinterprets every existing save's blocks as the wrong type. A concrete bug was also found in LG2.cpp: `LoadGame` clears the quadtree and never rebuilds it, and separately, the quadtree holds pointers invalidated by `blocks.insert`/`erase` elsewhere — save/load interacting with a raw-pointer spatial index made the whole system fragile in a way that would have been very hard to diagnose from symptoms alone.

### 7.2 Format actually implemented (walkgrid v1)
The byte format lives in `worldfile.cpp` (pure C++, no OS calls — tested natively, world layer); `savegame.cpp` (game layer) applies a decoded save to live state; `gamefiles.cpp` (platform layer) does the crash-safe disk write.
```
magic (u32 "WGRD") | version (u32, 1)
player: pos.x,y,z (f32×3)  yaw,pitch (f32×2)  hotbarSelection (i32)  dayTime (f32)
generator: name (str)  version (u32)  seed (u64)                      (2.5)
blockNameCount (u32) | [ nameLen(u16) nameBytes ] × count             (3.1)
chunkCount (u32) | per chunk:
    cx, cy, cz (i32×3)  flags (u8: 1 = has state, 2 = has data)
    blocks: runs of (length u16, nameIndex u16) covering all 4096 cells
    state (if flag 1): runs of (length u16, value u8)
    data  (if flag 2): count (u16), then (cell u16, length u32, bytes)
updateCount (u32) | [ x,y,z (i32×3)  kind (u8)  delay (u32, ticks from now) ] × count   (5.4)
gameLength (u32) | bytes × gameLength   -- the game layer's own section, opaque to the engine
checksum (u32)  — FNV-1a over every byte above
```
**Only modified chunks are written** (2.4); everything else regenerates from the recorded generator. Cells run in `LocalIndex` order (x fastest, then z, then y), so the horizontal layers typical of terrain and buildings collapse into a handful of runs. An untouched world is a few hundred bytes; a modest build costs a few hundred bytes to a few KB per chunk it touched.

**The game section** lets walkgrid store its own state without the engine format naming game systems (FOUNDATIONS.md 2). It is empty today. A file longer than its sections describe is refused as corrupt.

**Voxistics formats (v2–v9) are not read.** walkgrid had no saves before v1, so the legacy loaders were removed (M0.9, D13). A Voxistics save is refused cleanly ("not a save file"), never misread.

### 7.2.1 Save location
`%USERPROFILE%\Documents\My Games\walkgrid\` — the player's own local Documents folder, never a cloud-synced one (D28). Windows' "Documents" known folder can be redirected into OneDrive by its backup feature, even for players who don't use OneDrive, so `gamefiles.cpp` asks for the profile folder (`FOLDERID_Profile`) and takes `Documents` from there. Subfolders: `Saves`, `ShaderCache`, `Screenshots`; `settings.cfg` and `perf_report.txt` sit in the folder itself. The folder is resolved fresh on every use and created if missing; if the path is blocked (a file where a folder should be) or can't be resolved, the working directory is used instead. Once, if the local folder doesn't exist yet and a `walkgrid` folder exists in the known-folder Documents (where earlier builds put it), its contents are moved across. Screenshots and the performance report show their full path when saved.

### 7.2.4 Multi-slot saves
Individual save files live in `Documents\My Games\walkgrid\Saves\slot1.sav` .. `slot5.sav` (`MAX_SAVE_SLOTS = 5`) rather than the single fixed `voxelproto.sav` this prototype originally had — the title screen's New Game / Load Game (Part XII) needs more than one world to choose between. Each slot holds one walkgrid v1 save (7.2).

A slot's occupied/empty status for the picker list (12.3) is read straight off the filesystem (`std::filesystem::exists`) rather than a stored index or catalog file, so it can never drift out of sync with what's actually on disk. Slots are auto-named "World N" by position rather than player-chosen names — building a full text-entry keyboard widget for renaming was scoped out of this pass (documented as real future work, not forgotten) in favor of shipping multiple slots that work correctly first.

**Migration:** Voxistics moved an older single save into slot 1 at start-up; walkgrid has no older saves, so that migration was removed (M0.9).

### 7.2.5 Day clock field (v4)
`SaveGame`/`LoadGame` gained one field, `g_dayTimeSeconds` (Part XIII), appended right after `hotbarIndex` — save version bumped to v4. It's world state, not a settings.cfg preference, since different saves can legitimately be at different points in their day. v2 and v3 saves (predating the day clock) still load; they default to 0.0 (dawn) rather than needing a value that was never meaningful for them.

### 7.2.2 Global settings file
Gameplay/UI preferences (mouse sensitivity, inversion, render distance, the FPS counter toggle, master/music volume, and every keybinding) originally lived inside the save file itself (v2, above). They now live in `Documents\My Games\walkgrid\settings.cfg` — the same directory as the save file, resolved through the same bulletproofed `GetSaveDirectory()` — independent of any world save. Two reasons drove the move: a title screen's Options needs to read/write these before any save is loaded or even exists, and preferences arguably belong to the *player*, not to any one world, so they should carry over between saves rather than reset per-world.

The format is plain `key=value` lines (`sensitivityX=1.000000`, `keybind.forward=87`, etc.) rather than the save file's versioned binary encoding — a handful of human-meaningful scalars a player might reasonably want to inspect or hand-edit, where "unknown keys are ignored, a missing key keeps its compiled-in default" gives forward/backward compatibility for free, with no version field needed. It's written via the same temp-file-then-rename pattern as the save file (never a direct in-place write), and re-read once at startup before anything else consults these values.

It's saved incrementally rather than only at one moment: every toggle click, every Reset to Default, every completed slider drag (once when the drag ends, not on every pixel of motion), and every committed keybind rebind writes it immediately, so a preference change survives even if the process is later killed without a clean exit.

**The game's own keys** (the hotbar, and the render distance the world layer owns) are written and read through a hook the game layer registers (`SetGameSettingsHooks`, settings.h), so the platform-layer settings file never names them. Same keys, same file as before (M0.9).

### 7.2.3 Write sequence (crash safety) — settings file
Same shape as the save file's (7.3): the whole file is one `ostringstream`-built buffer, written to `settings.cfg.tmp`, then renamed into place — never edited in-place, so a crash mid-write leaves the previous version of the file intact rather than truncated.

### 7.3 Write sequence (crash safety)
1. Serialize the entire save into an in-memory buffer.
2. Write that buffer to `slotN.sav.tmp`.
3. Only if the write completes without error: rotate the existing `slotN.sav` to `slotN.sav.bak`, then rename `.tmp` into place as `slotN.sav` (`WriteFileSafely`, gamefiles.cpp).

A crash or power loss at any point before step 3 completes leaves the previously-good save completely untouched — there is no window where the live save file is partially overwritten.

**Autosave:** every 5 minutes of actual play (paused time doesn't count, so a game left on the pause menu isn't rewritten), on Quit to Title, on Quit, and when the window is closed mid-game. With delta saves (7.2) a save is small enough to write on the main thread without a hitch; the previous file is always kept as `.bak` by the sequence above.

### 7.4 Load sequence (corruption safety)
1. Read the whole file into memory.
2. Compute FNV-1a over everything except the trailing checksum field; compare. Mismatch → abort before touching any live game state, log the reason.
3. Verify magic number and version — wrong magic or unsupported version aborts cleanly rather than attempting to interpret garbage as a world.
4. Build the saved-name → current-`BlockID` remap table. A name no longer present in the current build maps to `AIR` (with a logged warning) rather than silently reinterpreting as whatever ID happens to occupy that slot today.
5. Decode every chunk into a scratch map (never through the live `Set()` path — no gravity checks against a half-built world, 5.2); refuse the load on any structural error (a run overflowing its chunk, a bad cell index, an unknown generator).
6. Only then apply: player, day clock and generator, and hand every saved chunk to the modified-chunk store. Streaming generates the columns around the player and overlays those chunks (2.4), so a load costs the decode, however large the world.

### 7.5 Designed for later
Per-region multi-chunk files (grouping a 16×16 column of chunks behind one small offset-table header) — the natural extension once one save file becomes slow to write at scale, requiring no format redesign since v5 chunk records are already self-contained and would simply move into region-scoped files. Palette compression of chunks in memory, and writing saves on a background thread, are the other two noted extensions.

---

## Part VIII — Not in walkgrid
Reference material (the seed prototypes): a Voxistics feature. See `reference/voxistics/DESIGN.md`.

---

## Part IX — Not in walkgrid
Voxistics milestones: a Voxistics feature. See `reference/voxistics/DESIGN.md`.

---

## Part X — Audio

### 10.1 Playback backend
XAudio2 (`xaudio2.h`/`xaudio2.lib`), initialized once at startup (`InitAudio()`) after `CoInitializeEx` — the one thing in this codebase that actually requires COM initialized on the calling thread (`SHGetKnownFolderPath` manages its own COM state internally, so nothing earlier needed this). One `IXAudio2SourceVoice` plays the day-cycle music as a stream of small generated chunks (Part XIV); a second plays the world sound palette (10.4) on small buffers, rendered on its own thread. Master, Music and World Sounds are separate sliders: each channel is its own source voice under the one mastering voice. `InitAudio()` failing (no usable audio device, missing driver, etc.) is non-fatal: every audio entry point is guarded by a null check, so the game is fully playable, just silently, rather than refusing to start.

### 10.2 Procedural, not an asset
All music is synthesized in code at runtime — no audio file exists anywhere in the repo, and there's nothing to license — the same "bake it in code, never load an external asset" philosophy the block/UI textures use (8.4). An earlier 20-second looping ambient pad and a later single-curve arpeggio track both preceded the current composition; Part XIV describes what ships now.

### 10.3 Mono music, stereo world
The music is generated as a single channel: it's the room the game happens in, not a place. The world sound palette (10.4) is stereo: a block you place or break sounds from its side, softening with distance, music blocks play their clave part from where they stand, ambience spreads gently across the field, and the echo ping-pongs side to side. Placement is equal-power and held within ±60 % of full pan (nothing sits hard in one ear), with the centre at exactly the mono level. **Mono audio** (Accessibility) centres everything for players who hear on one side only.

### 10.4 The world sound palette
Short interaction, discovery, accent, texture and rare-colour sounds — 46 of them in seven families — synthesized by the music's own engine and written as parts of the composition, not effects laid over it. The full specification is **docs/SOUND_PALETTE.md**; the short version:

- **One clock, one harmony.** The score's chord, beat, section, filter cutoff and master level are pure functions of day time, so `MusicHarmonyAt(t)` (music_synth.h) tells the palette exactly what is playing at the moment a sound will be heard (`AudibleMusicTime`, read from the music chunk actually playing). Every pitch comes from the current chord's *safe set* (scale tones not a semitone from a sounding chord tone; D–G–A during a chord crossfade); every scheduled onset lands on the beat grid; thumps glide onto the current bass note.
- **Shared DNA.** The oscillators, envelopes, noise and filters are literally the track's (`synth_kit.h`, extracted from music_synth.cpp and shared), plus a formant voice for wordless vocal ad-libs and a bell. Every sound passes a lowpass that follows the track's day-long cutoff arc, and the palette as a whole a 4.2 kHz ceiling. No sound passes −21 dB on the track's own layer scale (the motif sits at −15 to −19), checked per sound, not per voice.
- **Three axes** from the ground around the player (`soundscape.h`: a fixed 1,024-block census slab per frame over a 32 × 24 × 32 box, plus movement and recent actions, all eased over seconds): **positive ↔ negative** (health vs neglect: pitch pool, contour, register, wear), **calm ↔ active** (ambient budget from 1 to 6 events per 4 bars, grid, release, echo), **organic ↔ mechanical** (soft triangle → clean pulse, timing jitter, metallic partials, filter edge). The main track leans the same way, gently (cutoff, pad and air balance, wow, pump — ±2–3 dB at most, bit-identical at the neutral point).
- **Gestures, not piles.** A tempo-synced dotted-8th echo; Sets climb the current chord's ladder and Takes descend it, so a building session plays arpeggios and a streak to the octave resolves into a cadence; onsets within 30 ms merge; repeats soften; a new note avoids a 2nd against anything ringing; event sounds suppress ambience for their length; rare colour waits for calm.
- **Where it plays.** `worldsound.cpp` wires it to the game: place, break, refused placement, hotbar, the library (open, pick, drop, close), save, footsteps on the beat (crouch every other beat, walk every beat, sprint on 8ths; soft ground scuffs and crunches, hard ground clicks and knocks quietly in key), landings, slides, sprint "hey"s, The Line passing through the player, discoveries from the census, and the per-bar ambient scheduler (wind in the grass, chirps, night shimmer, drips, ember crackle, machine rhythms and hum, music blocks as a clave part, the dark set's heartbeat and worn drone, caves, far bells, falling stars, the day's seam). Pausing fades it all in 0.3 s (pause is silence); the library and map keep their own UI sounds.
- **Cost.** A 512-sample buffer per ~11.6 ms, rendered only when something sounds or play is live; typically 2–6 of 48 voices. **The palette renders on its own thread (walkgrid M1.1).** The main thread only posts to a fixed mailbox (64 entries: cues, releases and fades in order; past 56 new cues are dropped so releases and fades always fit) and the latest state (axes, scene, listener, gait), under a brief lock. The worker applies them, keeps 3 buffers (~35 ms) queued, is woken by XAudio2 as each buffer ends, and sleeps when silent; if the queue ever runs dry mid-sound it keeps one more buffer ahead, up to 8. Rendering with all 48 voices busy measured 0.6 ms median, 1.1 ms p95 per buffer on the build machine. The voice cap: when all 48 are busy, ambience yields and anything else steals the quietest, lowest-priority voice (tested). WORLD SOUND in the F3 profiler is now the census (constant per frame) and the posting; it also shows the three axes.

---

## Part XI — Accessibility

A deliberately-scoped real slice rather than every idea discussed, plus one hard rule that applies regardless of what's implemented yet.

### 11.1 What's implemented
- **Field of view slider** (Motion/Comfort) — 45–100°, replacing what was a fixed constant. Neither a wider nor a narrower FOV is universally more comfortable for motion/vestibular sensitivity (wider can worsen edge distortion for some, narrower can worsen tunnel-vision for others), so this is a slider a player tunes in whichever direction helps them, not a binary toggle guessing a direction for them.
- **Toggle-to-move** (Input flexibility) — an Accessibility setting that changes what a WASD press *means*: instead of the action reading as "down" only while the key is physically held, a press flips a per-action latch that stays on until pressed again. Movement no longer requires holding a key down for the whole duration of walking. Implemented as a genuine input-semantics change in `IsActionDown()`, not a visual/cosmetic toggle: `WM_KEYDOWN`'s key-repeat bit (bit 30 of `lParam`) is checked so holding the key doesn't rapidly flip the latch, and the latch is cleared on focus loss (`WM_KILLFOCUS`) and on switching the mode off, so a stale toggle can never leave the player walking without input. Latching a direction releases its opposite (forward/back, left/right) — with both latched they cancelled out and the next press un-latched the wrong one, which read as inverted controls — while perpendicular latches still combine for diagonals. Mouse-button bindings toggle the same way as keys. While the mode is on, a small arrow cross in the bottom-left corner lights each latched direction, so an active latch is never invisible.
- **High-contrast UI palette** (Vision) — pushes every panel/button/slider-track fill toward the luminance extremes (near-black backgrounds, strongly saturated hover/handle colors) instead of the subtle gray-shade steps used otherwise. Text was already white-on-dark in both modes, so only fill colors branch.

All three persist in the global settings file (7.2.2) like every other preference, with their own Reset to Default, in a dedicated Accessibility submenu reachable from Pause (and, once a title screen exists, from its Options too).

### 11.2 What's deliberately not implemented yet, and why
- **A "reduce flashing" toggle.** Nothing in this prototype flashes or strobes today (no particles, no damage vignette, no lightning) — a toggle controlling zero real effects would be exactly the kind of dead control this project has already pushed back on once (the old Audio submenu's "no backend yet" placeholder, Part X). The actual commitment is the hard rule in 11.3 below, which binds *future* work whether or not a toggle exists yet. The toggle gets built alongside whatever first effect would actually need one.
- **Colour vision (built).** Pulse spin was the first meaning carried by colour, so Accessibility has a **Colour vision** setting: Typical, Red-weak, Green-weak, Blue-weak, No colour (`pulse_colours.h`). Each swaps the three pulse colours (beads, store icons) for a trio that kind of vision tells apart, and the native test checks it with the standard dichromat simulations. What it found: the typical trio already holds up fairly for the three dichromat kinds (the colours differ in brightness as well as hue), but **by brightness alone blue and red nearly merge** (0.09 apart) — the case the owner flagged — and the no-colour mode parts them (0.59). The dichromat modes are a margin of safety rather than a rescue. Spin never rests on colour alone anyway: the store's swirl, a bead's turn, the corkscrew in the air and a twisted pipe's thread all show its hand. Twisted pipes' textures keep their blue and red (their thread and rifling show the hand).
- **A UI scale slider.** Unlike the above, this is real, wanted future work — just architecturally bigger than it looks: every hit-rect (`PointInRect` calls throughout the menu click handlers, slider drag math) would need to move in lockstep with every visual size, or clicks would misalign the moment the slider left 100%. Deferred rather than shipped half-consistent.

### 11.3 Hard rule: no uncontrolled flashing or strobing, ever
Independent of whether a toggle exists to control it: no effect added to this game — weather, damage feedback, particles, screen shake, UI transitions, anything — may flash, strobe, or rapidly alternate brightness/color in a way the player doesn't control the timing of. This is a photosensitive-seizure-trigger concern, not a taste preference, and it binds every future milestone the same way the resource-discipline requirement (1.2) binds the build. Any effect that has a legitimate reason to pulse or flicker gets a player-facing toggle to disable it *before* it ships, not after.

---

## Part XII — Title Screen and Menu Structure

### 12.1 GameState vs. MenuScreen
Two separate pieces of state, not one conflated enum: `GameState` (`Title`/`InGame`) tracks whether a real game is running at all, while `MenuScreen` tracks which panel is on screen. The game starts in `GameState::Title` with an empty, unloaded `World` — no chunks generate and no physics tick (the existing "freeze simulation while any menu is open" rule, 5.3, already covers this for free, since every title-tree screen is a non-`None` `MenuScreen`) until New Game or Load Game actually hands control to the player via `EnterGameplay()`.

### 12.2 One options hub, two entry points
The six settings submenus (Look/Graphics/Display/Audio/Accessibility/Keybindings) are pure global-preference state (settings.cfg, 7.2.2) with no dependency on a loaded world, so they're reachable identically from Pause (mid-game) and from the title screen's own Options button, through one shared `MenuScreen::OptionsHub` rather than duplicating six menu entries in both places. `g_optionsReturnScreen` records which of the two opened it, so OptionsHub's Back (and Escape) return to the right place. Pause itself shrank accordingly, down to Resume / Options / Save / Load / Quit to Title / Quit.

### 12.3 New Game / Load Game slot picker
Both New Game and Load Game lead to the same slot-picker screen (`MenuScreen::SlotPicker`), distinguished only by `g_slotPickerMode`. Load Game on an empty slot is a no-op with a toast; New Game on an occupied slot needs a second click within a few seconds to confirm the overwrite (`g_confirmOverwriteSlot`/`g_confirmOverwriteTimer`) rather than silently destroying a world, without the larger scope of a full modal dialog system. A fresh New Game is saved to disk immediately (an empty-world save) so the slot stops reading as "empty" from that point on, rather than only existing once the player happens to trigger a save later.

### 12.4 Escape's hierarchy
Escape backs out exactly one level, and which level depends on which tree it's in: `SlotPicker → TitleMain`, `OptionsHub → g_optionsReturnScreen`, any settings submenu `→ OptionsHub`, and — only in the in-game tree — `Pause → (resume)` and gameplay `→ Pause`. The slot picker is shared by the title screen's Load/New Game and Pause's Load Game; `g_slotPickerReturnScreen` sends its Back row to whichever opened it (Escape from it in-game returns to Pause). `IsSettingsSubmenu()` is the single predicate both the click handlers' Back buttons and this Escape logic agree on, so the two can never disagree about which screens count as "a settings submenu" for routing purposes.

---

### 12.x Buttons and sliders you can feel
Menu buttons are drawn as mechanical keys: a raised cap on a darker body with a dark outline and a lit top edge. Hovered, the cap brightens; held down, it sinks into its body (the lit edge gives way to a shadow, the label drops with it); buttons act **on release**, like real ones, and sliding off before letting go cancels. Sliders, the map and the library still act on press, since they're dragged. Sliders are **beads on a string**: a taut thread, brighter where the bead has travelled, knotted at both ends, with a round bead (stacked slices, a lit crown and a glint) that brightens under the cursor and swells slightly while dragged. All flat rectangles in the existing UI batch: no cost. High-contrast mode keeps the same shapes at luminance extremes.

### 12.y Frame rate and vsync
Graphics settings: **Frame rate limit** (30–200 fps, steps of 10, default 60) and **Vsync** (default on). The cap applies only with vsync off (walkgrid M0.12): with vsync on, the display paces frames, and a cap that isn't a whole divisor of the refresh rate would judder. The view is drawn between the last two simulation ticks (smooth motion above 60 fps, D10); looking around is applied every frame. The cap sleeps off each frame's remaining share (1 ms timer resolution while the game runs, then a short spin). The simulation runs on a fixed step, so the cap changes only how often the world is drawn, never how fast it moves; the world sound palette keeps about two frames of audio queued, so a low cap never starves it.

## Part XIII — Day Clock

One authoritative value, `g_dayTimeSeconds` (0 to `DAY_LENGTH_SECONDS = 3600`, one in-game day = one real hour, locked in), advancing only inside the exact same gate that already freezes physics and chunk generation while any menu is open (5.3) — so it is structurally impossible for the clock to run while paused, without needing a separate check. A fresh New Game starts it at 0 (dawn); Load Game restores whatever was saved (7.2.5); it is never derived from the real-world wall clock.

This is deliberately the *only* clock anything in this system reads. Part XIV's music is built entirely around not needing a second one — see 14.4.

**The sky and world light follow it** (`ComputeSky`, sky.h — pure functions of the clock, tested natively): the sun rises due east as the music's Dawn begins (0:00), passes straight overhead (the world is on its equator; see the compass, 2.2), and sets due west at 50:00 inside Dusk, leaving ten minutes of real night. The moon runs ~140° ahead of it on the same path: it rises late in Dusk, is up through the night, and is still up in the west for the first ~11 minutes of the morning (so a new world's first sunrise has it) — and is down for the rest of the day. The sky shader blends day and night gradients, warms the sky around a low sun, and draws the sun's disc and glow; the world's brightness runs from 30 % at night (still playable) to full in the day, and the direct-sun amount drives the shadows (4.8). The star field turns about the celestial pole with the clock (normal east-to-west streaming), and The Line (Part XVIII) composites its own precession on top.

---

## Part XIV — Day-Cycle Music

### 14.1 Why chunked and clock-anchored, not baked once
The music has to stay in agreement with the day clock (and eventually lighting) for as long as the game runs, and two independently-running clocks — a baked buffer's playback position vs. the game's simulated time — can only be kept aligned approximately, by periodic correction. So the track is generated in small chunks (`MUSIC_CHUNK_SAMPLES` = 0.25 s), continuously, and every chunk is computed starting from whatever `g_dayTimeSeconds` actually is. There is only ever one clock.

### 14.2 Form: one hour, six sections, looping into itself
| Section | Day time | Tempo | Chords | Melodic material | Shared lowpass |
|---|---|---|---|---|---|
| Dawn | 0:00–8:00 | none → soft 120 BPM pulse fading in over 3 min | Dm9 → G7sus4 → Em7 → A7sus4, 120 s each, 16 s crossfades | from 4:00: D4–E4–F4–A4 as 8ths, each note blooming (2.5 s attack / 4 s release), every 8 bars (every 6 in the last 90 s) | 380 → 720 Hz over 6 min |
| Morning | 8:00–20:00 | 122 BPM | same cycle, 4 bars each | main motif D4–F4–A4–C5–A4–F4–E4–G4 on a soft pulse wave (32 % width), fading in over 20 s | 750 → 1.6 kHz over 7 min |
| Midday | 20:00–35:00 | 124 BPM | same cycle, 6 bars each | main motif (28 % width) + countermelody A4–C5–E5–G5–E5–C5–A4–G4 | 1.5 → 3.1 kHz, hold, → 2.2 kHz |
| Afternoon | 35:00–47:00 | 122 BPM | same cycle, 4 bars each | transformed motif C5–A4–F4–D4–E4–G4–A4–F4, 8th/dotted-8th, every 3 bars (6 in the last 100 s) | 2.2 → 1.0 kHz |
| Dusk | 47:00–55:00 | pulse fades out over 3.5 min | same cycle, 2 min each | 2–3-note cells of the Afternoon motif every 16–24 s | 1.0 kHz → 480 Hz |
| Night | 55:00–60:00 | none | Dm9, Em7, A7sus4 (75 s each), open D3+A3 drone, then a 50 s bloom back into Dawn's Dm9 | single pure sines (D5/A4/F4), 7 s envelopes, 25–30 s apart | ~470 Hz, opens toward 620 Hz |

Every automation curve (layer levels, cutoff, resonance, duck depth) is a keyframe table whose value at 3600 s equals its value at 0, and the final Night segment crossfades into Dawn's opening chord, so the hour closes on itself.

### 14.3 Synthesis: additive and subtractive in tandem
- **Additive** — tones built as explicit sums of sine partials: pure sine (mid/high pads, night tones, air), a *soft triangle* from the triangle series' odd partials 1/3/5 at 1/n² (low pad, bass, Dawn motif, Dusk fragments), and a sine + soft-triangle blend (countermelody). Band-limited by construction. The 3rd and 5th partials are computed from the fundamental through the exact multiple-angle identities (sin 3x = 3s − 4s³, sin 5x = 5s − 20s³ + 16s⁵), so a three-partial tone costs one sine evaluation.
- **Subtractive** — spectrally rich sources shaped by filters: PolyBLEP band-limited saw (low pad, the bass's "soft saw") and pulse (the motifs), and white noise (the textured bed and air layer). One shared resonant lowpass (RBJ biquad) carries the day-long brightness arc over the whole tonal mix; the two saw layers also get their own fixed one-pole tone filters (900 Hz pad, 400 Hz bass) so they stay soft when the shared cutoff opens past 1.5 kHz; the noise bed has its own slowly moving lowpass and the air layer a bandpass, both bypassing the shared filter.
- **Voicing** — chord tones are folded into three pad registers (low 80–160 Hz, mid 175–350 Hz, high 390–720 Hz) plus a bass tone per chord, each register a fixed bank of one oscillator per distinct frequency. A tone shared by two chords is one oscillator whose gain is the sum of both chords' weights, so common tones sustain through a change and a crossfade never adds voices.
- **Pulse** — a pre-rendered soft thump (sine gliding 104 → 52 Hz, 6 ms raised-cosine attack, windowed to exactly zero within 0.45 s, shorter than any beat) on every beat, accented on 1 and 3. From Morning on, the bass ducks sidechain-style on each beat (raised-cosine dip and recovery), with slowly varying depth.

### 14.4 Music Intensity (Accessibility, 11.1)
A linear scalar (0–1, `g_musicIntensity`) applied to the rhythmic and melodic layers only — pulse, motifs, countermelody, Dusk fragments, night tones, and duck depth. At 1.0 the arrangement plays exactly as designed, already the maximum energy this track reaches; at 0.0 only the bed (pads, bass, noise, air) remains. It is a floor control, never a ceiling-breaker: resonance is capped (Q ≤ 1.2, `kMaxQ`) and every transition is smooth at every setting.

### 14.5 Determinism: what's stateless and what isn't
Chord weights, beat position, every note event (onset, pitch, micro-timing and length variation via a hash of the event's identity), every envelope and every oscillator phase is a pure function of wrapped day time. Pad and bass phases are computed from absolute time, and every such frequency × 3600 is an integer, so phases wrap from 3600 to 0 with no jump; notes take their phase from their own onset. The only state carried between chunks (`MusicState`) is the filter histories and a resume fade-in counter. `ResetMusicState` runs on every discontinuity (new game, load, resume), and a fresh state fades in over 1.5 s, so resuming never clicks. Verified offline: rendering 10 s as one call vs. forty quarter-second chunks differs by at most 1 LSB, and the 3600 → 0 seam is smoother than the signal around it.

### 14.6 Playback: queue, pause, and the title screen
`StartMusicPlayback()` (New Game, Load, Resume, quick-load during play) re-anchors to the current `g_dayTimeSeconds`, resets state and starts the voice (the music worker's first chunk, a millisecond or two later, starts the sound); `StopMusicPlayback()` (any menu opening, focus loss, Quit to Title) stops and flushes. Pausing goes fully silent — silence reads as "in-game time stopped." The title screen is silent because nothing starts playback until a game begins. PCM lives in a fixed pool of buffers reused cyclically (one more slot than the 16-chunk lookahead, so the slot being rewritten is always older than anything XAudio2 still has queued); after a flush, no slot is rewritten until `BuffersQueued` actually reaches zero. `RefillMusicQueueIfNeeded()` adds at most one chunk per frame, and only while no menu is open. Quick-loading from a menu leaves the music stopped; Resume restarts it at the loaded time.

### 14.7 Cost and output level
**The music is synthesized on its own thread** (since the performance review): a worker keeps the 4 s lookahead full, generating each chunk outside the lock from copies of the synth state, colour and intensity, then publishing it (levels, start time, submission) in a brief locked step; starting or stopping playback bumps an epoch under the lock, so a chunk begun for the old position is discarded. The frame never pays for synthesis, not even on resume (which used to prime four chunks synchronously), and the main thread's readers take the lock only for a few reads. The history before that: the lookahead was refilled a slice per frame (2,048 samples, ~46 ms of audio) rather than a whole quarter-second chunk at once, which on a modest machine showed as a hitch four times a second in the busiest section (Midday); a chunk is submitted when full, and if the queue runs low after a stall the chunk is finished at once.
Generation cost (now off the main thread): median ~1.3 ms per 0.25 s chunk (about 190× realtime), versus ~6.8 ms average and ~30 ms worst for the previous generator, which also primed 16 chunks at once on every resume. No libm calls in the sample loop (polynomial sine, truncation-based floor, PolyBLEP); slow parameters are evaluated once per 64-sample control block and interpolated; silent layers and inactive oscillators are skipped entirely. Output scale (`kOutputScale` = 0.78) comes from a full-hour offline render at intensity 1.0: peak ~0.85 FS, loudness close to the previous track; the final clamp is only a backstop and never engages.

### 14.8 Where the spec was interpreted or overruled
- *"Continuous 8th notes, one full cycle every 2 bars"* (8 pitches) — each pitch sounds as two 8ths, which satisfies both clauses.
- *Countermelody "one cycle every 4 bars, legato 8th notes"* — one pitch per half note, legato, with a soft 8th-note re-articulation.
- *Night opens the filter to 620 Hz in its last 2 minutes, but Dawn starts at 380 Hz* — the seam wins (no sudden events): it opens to 620 Hz, then glides back to 380 Hz through the final 50 s crossfade.
- *Small gaps between sections' filter ranges* (Dawn ends at 720, Morning starts at 750; Morning holds 1.6 kHz, Midday starts at 1.5 kHz) — bridged smoothly; Midday's sweep starts from 1.6 kHz.
- *Midday "occasional C♮ for Mixolydian color"* — C♮ is already in D Dorian (Mixolydian's characteristic tone would be F♯, which would clash with the pads' F), so nothing was added. Open question for the composer.
- *Night pulse "none (or –28 dB residual)"* — none.
- *Bass register ranges* are approximate: the Em7 bass is E1 (41.2 Hz), just under Dawn's stated 45 Hz floor.
- *Resonance values* (0.20–0.33) map to Q = 0.707 + 1.2·r, capped at 1.2.
- *Tempo changes* step at section boundaries (≤ 1.6 %); every section begins on a whole bar, so the chord grid stays aligned.
- *dB values in the spec* are relative; the mix was set by measurement — melodic layers sit roughly 3–8 dB under the bed in the pulsed sections and further under it in the sparse ones.
- *Motif hand-offs* — the Morning motif fades in over 20 s at 8:00, and at 35:00 Midday's motif fades out over 15 s while Afternoon's fades in, so no line appears or vanishes at full level.

---

## Part XV — Build
**Graphics minimum (walkgrid, D20):** Direct3D 11 at feature level 11.0; every shader compiles as shader model 5.0 (`vs_5_0` / `ps_5_0`). A machine without it gets a message box at start-up instead of a silent exit.


Twenty-four source files, listed (with their layers) in `walkgrid.vcxproj` and `tools/layers.txt`, which are the list of record: `main.cpp`; layer 1 `profiler.cpp`; layer 2 `gamefiles.cpp settings.cpp`; layer 3 `world.cpp worldfile.cpp shapes.cpp`; layer 4 `render.cpp textures.cpp vtex.cpp blocktex.cpp mesher.cpp icons.cpp glowlight.cpp audio.cpp music_synth.cpp sfx_synth.cpp soundscape.cpp`; layer 5 `game.cpp input.cpp menus.cpp hud.cpp savegame.cpp worldsound.cpp`. One compiler invocation would do; the checked-in `.vcxproj`/`.vcxproj.filters` list them all for Visual Studio:

```
cl main.cpp world.cpp render.cpp audio.cpp persist.cpp game.cpp textures.cpp music_synth.cpp profiler.cpp worldfile.cpp vtex.cpp blocktex.cpp mesher.cpp shapes.cpp icons.cpp theline.cpp essence.cpp essencemap.cpp /link d3d11.lib dxgi.lib d3dcompiler.lib gdiplus.lib gdi32.lib user32.lib shell32.lib ole32.lib uuid.lib xaudio2.lib /SUBSYSTEM:WINDOWS
```

or with MinGW-w64 (used during development to compile-check this prototype on a non-Windows host, since it ships full D3D11/DXGI/D3DCompiler/GDI+/XAudio2 headers and import libraries):

```
x86_64-w64-mingw32-g++ -std=c++17 -O2 -mwindows -municode -DUNICODE -D_UNICODE \
  main.cpp world.cpp render.cpp audio.cpp persist.cpp game.cpp textures.cpp music_synth.cpp profiler.cpp worldfile.cpp vtex.cpp blocktex.cpp mesher.cpp shapes.cpp icons.cpp theline.cpp essence.cpp essencemap.cpp -o voxistics.exe \
  -ld3d11 -ldxgi -ld3dcompiler -lgdiplus -lgdi32 -luser32 -lole32 -lshell32 -luuid -lxaudio2_8 -static-libgcc -static-libstdc++
```

Each `.cpp` above owns one subsystem and includes only the headers it needs (`common.h` for shared math/block-table types; `world.h` for the simulation model; `render.h` for D3D11 state and chunk meshing; `audio.h` for XAudio2 playback; `persist.h` for settings/save-load; `game.h` for the menu state machine, input dispatch, and the UI render pass). `blocks.h` is the block registry (Part III). `worldfile.cpp` (save format), `vtex.cpp` (texture parser), `blocktex.cpp` (block texture set), `mesher.cpp` (chunk meshing), `shapes.cpp` (block shapes) and `icons.cpp` (hotbar icons) are deliberately free of Windows and D3D so the native tests (Part XVII) can build them; `textures.cpp` (GDI+ UI font atlas) and `music_synth.cpp` stay dependency-free of the rest of the project by design. `textures.cpp` exposes `extern "C"` entry points to `render.cpp` by convention; `music_synth.cpp` has its own header (`music_synth.h`) holding `MusicState` and the generator's entry points, and `audio.cpp` `static_assert`s that the music's day length matches `DAY_LENGTH_SECONDS`.

(`-lxaudio2_8` is MinGW's import-lib name for the same XAudio2 2.8 API that the Windows SDK's `xaudio2.lib` provides — a MinGW-only naming difference, same idea as `-municode` above it.)

Default controls (all fully remappable to any keyboard key or the left/right/middle mouse button via Pause → Keybindings — click a row, then press the new input; Esc cancels a rebind in progress, except on the Pause Menu row, where Esc binds Escape): WASD to move, mouse to look (click once to capture the cursor), Space to jump, Shift to sprint, Ctrl to crouch (Ctrl while sprinting: power slide, 4.7), left-click to break the targeted block, right-click to place the selected hotbar block, number keys 1–9 and 0 or the mouse wheel to select a hotbar slot, E for the block library (click a block to use it, drag it onto a slot to keep it), M for the essence map, F3 for the profiler overlay (Ctrl+F3 records a 30-second performance report to `perf_report.txt` beside the saves), F7 for The Line's debug marker, F8 to jump to the next time of day and ] / [ (or Page Up / Page Down) held to run the clock forward / backward — a whole day in 15 s, with a time readout; the one day clock drives the sky, sun, shadows, light and music, so all of them follow, and the music re-anchors to the new time on release (debug aids, like F3 and F7), F11 for fullscreen, F5 to save, F9 to load, Esc to open/close the Pause menu or back out one level from any of its submenus (Look Settings, Graphics, Display, Audio, Keybindings, each with its own Reset to Default), all clickable with the freed cursor.

## Part XVI — Frame Profiler

Part 1.3's rule (cost scales with what's on screen or changing, never with total world size) is only a rule if it can be checked, so the engine measures itself. `profiler.h/.cpp` times each system every frame with `QueryPerformanceCounter` (`ProfScope` RAII timers around terrain generation, eviction, physics, falls, music synthesis, mesh rebuilds, world draw submission, the UI pass and `Present`) and records load counters (resident chunks, chunks and triangles drawn, meshes built, dirty chunks / columns / falls waiting). A 128-frame ring buffer (~2 s) is summarised twice a second into average and worst milliseconds per system, plus frame time and **work time** (frame minus `Present`, which under vsync is mostly waiting rather than work). Worst-frame numbers matter as much as averages: a hitch is a single bad frame that an average hides.

Collection is always on (a few dozen timer reads per frame); the overlay is toggled with F3 (unless F3 is bound to an action) or Display Settings → Profiler, and persisted in settings.cfg. New systems should get a `ProfScope` and, where they have a queue, a counter — that is how a design-rule regression shows up the day it's introduced instead of in a playtest.

**Performance report (Ctrl+F3).** The overlay shows ~2 s at a time; judging a build needs longer and needs it written down. Ctrl+F3 records every frame for 30 seconds of normal play, then writes `perf_report.txt` next to the saves: the build (Debug/Release), window size and graphics settings (nothing about the machine itself is read); median, 95th, 99th percentile and worst frame and work time; the same for every system; hitch counts (work over 33, 50 and 100 ms); the five worst frames with the systems that took their time; and peak load counters. A quiet countdown shows while it records. It's how performance gets judged on the owner's machine rather than guessed in the cloud.

**GPU time.** The rows marked GPU (shadow map, world, post, UI) come from D3D11 timestamp queries bracketing each pass, in a ring of three frames read back three frames later with DONOTFLUSH, so asking never stalls the pipeline (a frame not ready, or one where the GPU's clock changed, just gives no sample). They measure only our own drawing; nothing about the machine is read. With vsync on, PRESENT includes waiting for the display, so the GPU rows are the honest measure of how much headroom the graphics have.

**Start-up timeline and the shader cache.** Start-up is timed phase by phase (`ProfBootMark`): LAUNCH (from Windows creating the process to the game's first line — loading the exe and its DLLs), WINDOW, SETTINGS, GRAPHICS DEVICE, SHADERS, TEXTURES (block art, icons, the font atlas), AUDIO and FIRST FRAME. The F3 overlay shows the total and the phases worth a glance; the Ctrl+F3 report lists them all, with how many shaders came from the cache. Kept in memory only. Everything measurable off Windows is small (the block art, icons and texture parsing take under a tenth of a second together), so the prime suspect was compiling the 13 HLSL shaders at every launch. Compiled shaders are now **cached** in `Documents\My Games\walkgrid\ShaderCache`, each file named by a hash of everything that went into it (source, entry point, profile, define, compiler version) and checked by a checksum when read. A changed shader just gets a new name, and files no shader asked for are removed after start-up. Whatever isn't cached compiles on four threads at once — a fixed number; the machine is never asked what it has. Local files only, like the saves. Worth knowing when judging start-up: Windows' own virus scan checks each freshly built exe on its first launch, which can add seconds that belong to neither the game nor its code.

## Part XVII — Tests

`tests/run.sh` builds and runs `tests/tests.cpp` with the host compiler — no Windows needed — against the platform-free modules (`world.cpp`, `worldfile.cpp`, `vtex.cpp`, `blocktex.cpp`, `mesher.cpp`, `shapes.cpp`, `icons.cpp`), with `tests/stub/` standing in for the two Windows/D3D headers they touch. It covers the `.vtex` parser (valid input and each class of error), texture assembly (placeholders, authored overrides and upscaling, orientation, warnings), the v5 save round trip (including state, data, corruption detection), legacy v4 loading (name remapping, dug-out chunks), streaming (one-ring-past-view residency, eviction keeping only modified chunks, bit-exact regeneration on return), player spawn and unstick, the mesher (culling, AO, cross-chunk faces, orientation, the 16-bit worst case), shapes (orientation, boundary culling, collision, stepping onto a slab but not a full block), the rendered icons, and the sound system (the music's harmony query and colour, every palette sound's pitch safety and level ceiling under every chord, gestures, merge, determinism, the ambient budget, pause fades, and the soundscape census; 10.4). Every change to those modules should keep it at zero failures, and new systems should add their checks here — the harnesses that used to be written and thrown away during development now live in the repo instead.

## Part XVIII — Not in walkgrid
The Line: a Voxistics feature. See `reference/voxistics/DESIGN.md`.

---

## Part XIX — Not in walkgrid
The essence network map: a Voxistics feature. See `reference/voxistics/DESIGN.md`.

---

## Part XX — Not in walkgrid
The Voxistics development pile: a Voxistics feature. See `reference/voxistics/DESIGN.md`.

---

## Part XXI — Not in walkgrid
Fliers: a Voxistics feature. See `reference/voxistics/DESIGN.md`.

---

## Part XXII — Structure: layers, threads and budgets
Summarised here; the full reasoning is in `docs/FOUNDATIONS.md`.
- **Layers** (FOUNDATIONS.md 2): 1 base, 2 platform, 3 world, 4 presentation, 5 game, 6 app (`main.cpp`). Each file's layer is listed in `tools/layers.txt`.
- **Threads** (FOUNDATIONS.md 3): the main thread owns the world, every D3D11 call and the UI. Job threads (`jobs.h`, M1.4: processor count minus two, at least 1 and at most 4, D21) build terrain (and from M1.5 meshes) from copies; results are applied on the main thread. Music and effects each have their own thread. Results are version-stamped; stale ones are dropped.
- **Budgets** (FOUNDATIONS.md 4): measured on the reference machine (GTX 1060 3GB, 16 GB), with a floor of 8 GB and a 2 GB graphics card.

## Part XXIII — The faceted world
Written as M1 lands. The agreed approach is in `docs/PROTOTYPE_OUTLINE.md` sections 3–4 and `docs/FOUNDATIONS.md` 4.1 and 4.3; still images of it are in `docs/pictures/`.

### 23.1 The surface (facetmesh.h, M1.2)
Cells stay as they are: one material per 1-block cell. The surface is built through them, in layer 3 so collision and picking (23.5) use the same shape.
- **Corners.** Each lattice corner with solid and empty cells among its eight carries one vertex: the mean of the solid/empty crossings around it, eased halfway toward its neighbouring corners (one pass; more rounds the ground off), then nudged by a seeded jitter of up to 0.16 blocks. On ground facing up the nudge is vertical; elsewhere it's along the corner's normal, with only 0.04 sideways. (Sideways jitter sheared terrace risers until their tops leaned out over their bottoms: dark, downward pockets in the first pictures.) Every corner stays inside its own cube, so facets can't fold through each other. Flat ground stays level before the jitter; a one-cell step becomes a slope.
- **Base facets.** Each face between a solid and an empty cell is a quad on its four corners, split along the fold that bulges outward, unless that fold would turn a triangle inside out (a twisted quad at a step's corner), in which case the other one. The base facets never fold (tested) and never change with distance.
- **Detail.** Near the player a quad is cut 2×2 or 4×4, but only on lumpy materials (`bump` ≥ 0.015 for 2×2, ≥ 0.04 for 4×4). Creases and borders are not cut: a cut crease becomes a curved patch (blobby), and borders are blended per pixel. The quad is first split into its two base triangles, and each is cut linearly, so cutting alone can't fold anything and facets stay flat except for the lumps. New points are pushed in or out along the smooth normal by the material's lumpiness (value noise, 1.6 lumps per block), tapering to zero at the corners and fading out where the corners' normals disagree, which keeps step edges sharp.
- **No cracks.** Every point on a shared edge is computed from that edge alone (its two corners in a fixed order), and each edge is cut as finely as the finest face using it. Faces cut coarser than an edge fan from their centre through the edge's extra points. Watertightness is tested at every mix of levels, and meshing a region whole or in eight boxes gives the same triangles.
- **What a vertex carries.** Position, smooth normal (for texture projection), the triangle's three strongest materials with per-vertex weights, openness (AO, from the 4×4×4 cells around a corner) and sky light (from the caller). Facets are lit by their true flat normal.
- **Cost.** Linear in the surface faces meshed, with a per-corner cache. In the preview world (176 × 64 × 176): 90,400 base quads, 181,000 base triangles, 402,000 with near detail around one viewpoint. Within 96 blocks of the viewer: 308,000 (budget 1.5 million).
- **Known and watched** (FORECASTS F17): about 0.08% of the surface faces away from the averaged normal at terrace corners, and a few dozen detail triangles in a scene fold slightly. They show as small dark slivers.
- **Tuning** lives in `FacetShape` (jitter, sideways jitter, smoothing, lump frequency and thresholds); the preview tool (`tools/facet_preview.sh`) renders the same spots after any change.

### 23.2 The test landscape: walkgrid-hills v1 (terrain.h, M1.4)
New worlds are `walkgrid-hills` version 1, seeded (the seed is in the save). Rolling ground from three octaves of value noise (72, 26 and 11 blocks across; nothing finer, so rounding to whole cells leaves no lone pits), 15 to 45 cells high. A broad field raises sandstone plateaus 9 cells, their edges steep enough to be cliffs. Regions of meadow grass, dry turf and moss over dirt or loam; sand and gravel in the lowlands with clay at their edges; stone and slate outcrops; stone wherever the ground drops 3 or more cells to a neighbour; snow above 45. Three cells of soil under the top, then stone (sandstone under plateaus, slate in deep pockets), on a foundation floor at y = 0. No caves.
- **Pure and pinned.** A function of (seed, x, z) only, in integer hashing and plain double arithmetic. A test pins a fingerprint of one column, so any change that alters the output fails the test and has to become version 2 (Section 2.5).
- **Flat** stays as the test ground (the native tests stand on it, and worlds made before M1.4 use it). Voxistics' sine hills are gone (D37).
- **On the job threads** (Part XXII, `jobs.h`). The main thread queues columns ring by ring as before, submits up to 4 per tick with up to 8 generating, and makes up to 4 finished columns resident per tick: a copy of their cells and the overlay of anything the player changed. A column generated for a world that has since been replaced (New Game, Load) is dropped on arrival: the streaming epoch changed. Generation measured 0.05 ms per column on the build machine; F3 shows COLUMNS GENERATING.
- **Pictures:** `docs/pictures/m1_4/` (the preview tool's `hills` mode draws the real generator's output).

### 23.3 The ground in the game (groundmesh.h, render.cpp; M1.5)
- **Built on the job threads.** Each frame, the nearest few dirty chunks (up to 8, with up to 24 building) copy their cells with a two-cell margin (20³ bytes, below the world's floor counted solid) on the main thread, and a job thread builds their facets from that copy and packs them. Up to 8 finished meshes a frame are uploaded (two `CreateBuffer` calls each). A chunk carries a version stamp from one global counter, renewed whenever it's created or dirtied; a mesh whose stamp no longer matches is dropped, since a newer build is coming. An edit dirties neighbouring chunks when it's within two cells of their border (corners read two cells out).
- **The vertex** (`GroundVertex`, 16 bytes): chunk-local position in 1/2048 block (packed in double precision, so a corner shared by two chunks packs to exactly the same world position in both: tested), two material weights, the smooth normal, openness, the triangle's three materials and sky light. 16-bit indices where they fit, 32-bit past 65,536 vertices.
- **The world shader** projects each material's textures from the world: top/bottom and two sides, weighted by the smooth normal raised to the fourth power, with the top texture on slopes up to about 50° and the side texture beyond (grass shows its soil on steep ground). Each material's texture layers come from a 4 KB constant buffer. Facets are lit by their true, flat normal from screen derivatives; the surface map's relief tilts it, fading with distance. Sun, sky and bounce ambient, shadows, glow-grid light, shine and fog are as before. Filtering is smooth everywhere (D8): trilinear and 4× anisotropic.
- **Back faces aren't drawn.** facetmesh winds every triangle outward, which D3D's default rasterizer takes as front-facing (tested against the old cube mesher, whose culling is known to work). The shadow pass draws both sides.
- **At this step** each facet shows one material (its strongest); blending and sky light come in 23.4. Walking and picking still use the cells as cubes until 23.5, so the feet can float or sink by up to half a block on slopes.
- **Not yet out:** `mesher.*`, the cube mesher, still draws the hotbar icons (icons.cpp) until those become faceted lumps in M1.9.

### 23.4 Blending and sky light (M1.6)
- **Height-based blending.** Every texture layer has a height layer beside its colour and surface layers (`BlockTextureSet::height`, one byte a texel with mips: the art's height map, or its brightness where it has none), uploaded as a third texture array. Where a triangle's vertices carry more than one material, the shader samples up to three; each material's score is its interpolated weight plus 0.6 × its height there, nudged by world noise (±0.17) so borders don't follow triangle edges, and only materials within 0.18 of the best score show, in proportion. Grass tufts stand over gravel and pebbles out of sand; the border is ragged, not a crossfade. Triangles inside one material, and ground past 64 blocks, sample one material only. Weights come from the solid cells around each corner (23.1), so a border is about a block wide. The same constants drive the preview pictures.
- **Sky light.** The world keeps each resident column's highest solid cell (`World::columnTops`, 512 bytes a column): filled when a column arrives, kept by every edit, dropped on eviction. A chunk's mesh copy takes the tops around it (10 columns of margin). Each corner looks in 8 directions up to 8 blocks for the steepest rise above it (less half a cell, so a single step barely shades); each direction gives cos² of that horizon angle, the share of sky light a surface gets from above it. The shader scales the ambient light by 0.25 + 0.75 × sky. Tested: flat ground 1.00, a 4 × 4 pit six deep 0.19 at its floor, under a two-cell roof 0.08, a cliff face 0.64. A heightmap can't see the gap under an overhang, so ground under one reads as walled in, which is how it should look. The preview pictures use a ray-marched stand-in for the same idea.

### 23.5 Walking and picking on facets (collide.h; M1.7)
The player and the crosshair meet the base facets the ground is drawn with (`FacetBaseFace`, the mesher's own fold rule), never the fine detail. Physics gathers the facets in a 7 × 7 × 7 box around the player once a tick.
- **The floor is the facet surface:** the highest facet under the footprint's centre or near its corners. Walking, the feet rise onto the ground ahead by up to 1.05 (a one-cell step, which facets make a ~45° slope) on ground no steeper than about 57° (unit normal y ≥ 0.55); a two-cell wall, ~63°, stops you. On the ground the feet follow the facets over bumps and down slopes within 0.35 a tick; beyond that, you walk off an edge and fall; falling, you land on the first ground reached (up to 72° of slope). Every change of height while on the ground moves the eye the other way first, and it eases back (80 ms), so jittered facets never jolt the view.
- **Cells still stop the body** for walls, ceilings and crawlspaces, between 0.7 above the feet (a one-cell step's cube edge stands up to ~0.66 above its slope) and 0.15 below the head (ceilings' facets hang as much). A crouched player still fits a one-cell crawlspace; a slide still carries under it.
- **This changes one Voxistics rule:** a full block's step is walkable, because on faceted ground it's a slope (D39).
- **Picking:** the ray's box of facets, nearest front-facing hit within 6 blocks (Möller–Trumbore); the cell whose face it is, and the empty cell across that face for placing.
- **Cost** (build machine, optimised): 0.06 ms a physics tick, 0.03 ms a picking ray.
- **Tested:** 16 scripted sprints over walkgrid-hills (455 blocks): never below the ground, never lifted out, never a jump of the view; a one-cell step walked up with the eye gliding; a two-cell wall stops; picking at 120 angles, every hit a solid cell with an empty neighbour across the face.
