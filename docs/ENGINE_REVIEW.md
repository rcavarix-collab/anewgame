# Engine review: what Voxistics gives a new game

*September 2026. A review of the engine carried over from Voxistics: what each part does, how well, and what could be better. Nothing has been changed. Everything in "Improvements" is a proposal waiting on your decision.*

## How this was done, and what it can't tell you

- **Read:** every engine source file (renderer and its shaders, mesher, shapes, textures, lighting, sky, world and streaming, physics, save format, audio, UI and input, profiler), the relevant parts of `DESIGN.md`, and `docs/REVIEW_2026-09.md`. The game-specific systems were read far enough to see where they hook into the engine.
- **Ran:** the native tests (`bash tests/run.sh`): **5,068 checks, 0 failed**. These cover the simulation, mesher, textures, save format and audio synthesis. They don't cover drawing, the window, input or sound playback.
- **Could not run:** Direct3D, the game window, or audio output. `glslangValidator` isn't installed here, so the shader check wasn't run either. **Nothing below about how something looks or sounds is verified.** Visual and audio judgements come from reading the code, and each one needs your eyes and ears on a Windows build.

---

## The short version

**The engine is good, and most of it should be kept as it is.** The renderer, lighting, texture pipeline, mesher, streaming, save format, audio and profiler are careful, measured, well annotated and built for a modest PC. There's nothing here that needs a rewrite.

**Why repurposing failed last time.** The game's own content is wired straight into the engine files, so they can't be lifted cleanly. The Line, pulse logistics, fliers, the essence network and the music-reactive glow all have hooks inside `render.cpp`, `main.cpp`, `world.cpp`, `persist.cpp` and the world shader itself. Carry a file across and it drags the old game with it. Strip the game out carelessly and the engine breaks. The fix is to separate them at a short, known list of seams (section 3). It isn't to rewrite.

**What a new game will need that the engine doesn't have yet:**
1. real terrain generation (today there's a flat plain and a sine-wave hill test);
2. darkness in caves and under roofs (no sky-light or occlusion data exists);
3. a way to draw things that aren't blocks (models, creatures, items, particles);
4. smooth movement above 60 fps (no interpolation between simulation ticks);
5. raw mouse input and gamepad support.

These are the gaps, not the flaws. They're listed in section 4.

---

## 1. Inventory

Verdicts: **Keep**: reuse as it is. **Keep + improve**: reuse, with named improvements. **Untangle**: good engine code with game content woven in, to be separated. **Game**: Voxistics content, not engine.

| System | Files | Lines | Verdict |
|---|---|---|---|
| Window, frame loop, fixed 60 Hz tick | `main.cpp` | 308 | Untangle (the tick calls the Voxistics systems directly) |
| Maths, constants | `common.h` | 145 | Keep |
| D3D11 renderer: passes, shaders, shader cache, GPU timing | `render.cpp`, `render.h` | 2,420 | Untangle, then Keep + improve |
| Sky, sun, moon, stars, clouds, atmosphere colours | `sky.h` (+ sky shader in `render.cpp`) | 139 | Keep (The Line's sky effects are separable) |
| Light from glowing blocks | `glowlight.cpp/.h` | 164 | Keep + improve |
| Chunk mesher, ambient occlusion | `mesher.cpp/.h` | 271 | Keep + improve |
| Non-cube shapes, collision boxes | `shapes.cpp/.h` | 881 | Keep |
| Texture format, parser, texture array, normal/shine/glow maps | `vtex.cpp/.h`, `blocktex.cpp/.h` | 810 | Keep |
| Authored textures (93) and their generators | `assets/textures/*.vtex`, `tools/*_textures.py` | ~1,570 (tools) | Keep (library to be curated for the new game) |
| Hotbar icons (software-rendered from real meshes) | `icons.cpp/.h` | 128 | Keep |
| UI font atlas (system font via GDI+) | `textures.cpp` | 110 | Keep |
| Block registry | `blocks.h` | 475 | Untangle (engine structure; the ~140 rows are Voxistics content) |
| World storage, streaming, eviction, scheduled updates, gravity | `world.cpp/.h` | 1,198 | Keep + improve (terrain generation is a placeholder) |
| Player movement, collision, block picking | `world.cpp` | (in the above) | Keep + improve |
| Save format and disk side | `worldfile.cpp/.h`, `persist.cpp/.h` | 922 | Untangle (Line and essence fields in the format) |
| Settings file, keybindings, accessibility options | `persist.cpp` | (in the above) | Keep |
| Day-cycle music (procedural, clock-locked) | `music_synth.cpp/.h`, `synth_kit.h` | 989 | Keep (one fixed composition; see 2.8) |
| World sound palette (harmony-locked sound effects) | `sfx_synth.cpp/.h` | 2,053 | Keep |
| Soundscape census (what's around you drives the sound) | `soundscape.cpp/.h` | 291 | Keep + retune (block tables are Voxistics') |
| Audio playback (XAudio2, music thread) | `audio.cpp/.h` | 454 | Keep + improve |
| Game-to-sound glue | `worldsound.cpp/.h` | 145 | Untangle (Line cues) |
| Music-reactive level meter | `musiclevel.h` | 83 | Keep |
| Menus, HUD, settings screens, input, window messages | `game.cpp/.h` | 1,998 | Keep + improve (split it; Voxistics screens inside) |
| Block library and hotbar gesture | `library.h` | 109 | Keep if the new game has building |
| Frame profiler (F3), boot timeline, Ctrl+F3 report | `profiler.cpp/.h` | 389 | Keep |
| Native tests, shader check, MSVC check, sound demo | `tests/`, `tools/` | ~2,500 | Keep |
| The Line | `theline.cpp/.h` | 419 | Game |
| Pulse logistics | `pulse.cpp/.h`, `pulse_colours.h` | 586 | Game |
| Fliers | `fliers.cpp/.h` | 214 | Game |
| Essence network and its map | `essence*.cpp/.h` | 674 | Game |
| Seed prototypes | `Prismative.cpp`, `drillder.cpp`, `LG2.cpp`, `cc_2_2_2.cpp` | 8,830 | Reference only (not compiled) |

---

## 2. System by system

### 2.1 Frame loop (`main.cpp`)

**How it works.** A Win32 window drives a fixed 60 Hz simulation tick, decoupled from drawing. Each frame it adds the elapsed time to an accumulator and runs whole ticks: terrain queueing, physics, scheduled block updates and so on. Mouse look is read once per frame, straight from the cursor. After a stall, at most five catch-up ticks run, so a hitch can't feed itself. The frame cap sleeps, then spins for precision.

**Good.** The timing structure is right: fixed-step simulation, capped catch-up, a frozen world while paused, one day clock as the single source of time, and a boot timeline marked at every stage.

**Weaknesses.**
- **No interpolation between ticks.** The camera is placed at the player's exact position from the last tick, and that only changes 60 times a second. At 60 Hz with vsync this can't be seen. Above 60 fps, with the cap at 144 or 200 or vsync off, movement visibly stutters: some frames repeat a position and others jump. `main.cpp:263` builds the eye from `g_player` directly.
- **Vsync plus the frame cap can judder on displays that aren't 60 Hz.** This was proposed in the September review and is still open (`main.cpp:292` applies the cap even with vsync on).
- **The tick calls the Voxistics systems by name**: The Line, pulse, fliers, essence (`main.cpp:216–234`). See section 3.

### 2.2 Renderer (`render.cpp`)

**How it works.** It's a D3D11 renderer (feature level 10.0 up), and every shader is an HLSL string compiled at start-up. Compiled shaders are cached on disk, keyed by a hash of source, entry, profile and compiler version, and compiled on four threads when the cache misses. A frame runs in this order:
1. The sun shadow map, only when it has gone stale, drawn a quarter of the chunks per frame into a back buffer, then swapped.
2. The sky: a box around the camera, shaded per pixel.
3. Opaque chunks, frustum-culled per chunk.
4. Unlit pulses and fliers.
5. See-through blocks, sorted far to near per chunk.
6. An optional post pass (outlines, screen-space AO, bloom).
7. The UI pass.

Every optional effect that fails to compile on a machine switches itself off, reports why in `shader_errors.txt`, and shows as unavailable in the Graphics menu.

**Good.**
- Built for an old GPU and measured: GPU timestamps per pass show in F3 without ever stalling.
- The shadow map re-renders only when needed, and the work is spread over frames.
- It degrades gracefully when effects fail.
- It starts fast (shader cache).
- Rendering is done in linear light with sRGB textures and a filmic tonemap.
- Glow is marked explicitly (bloom reads an alpha mask rather than a brightness threshold), so a sunlit wall never smears.

**Weaknesses and improvements.**
- **Game content inside the renderer.** The world shader's constant buffer carries The Line's position and direction, the music level and eight flier "nutrient spots". The pixel shader loops over those eight spots on every pixel, and has Line-specific glow branches. The sky shader carries the Line's ghost moon and cloud lead. `DrawPulses`, `DrawFliers` and `DrawLineDebug` live in `render.cpp`. See section 3.
- **Back faces are drawn for all opaque geometry.** The opaque rasterizer state is `CULL_NONE` (`render.cpp:1666`). Cube faces are already wound consistently (the glass pass culls them), so culling back faces would roughly halve the triangles the GPU rasterizes for terrain. Shapes and plant cards need checking first: cards face the camera anyway, and shapes may not all be wound the same way. This needs a test, then a visual check on Windows.
- **Old-style swap chain.** It uses `DXGI_SWAP_EFFECT_DISCARD` with one buffer (`render.cpp:1602`). On Windows 10 and later, `FLIP_DISCARD` has lower latency and paces better with vsync off. It can be tried at creation, falling back to today's mode if it fails, without asking anything about the machine.
- **Pulses and fliers use the unlit debug pipeline**: no fog, no tonemap, no shadow. Far-off fliers won't fade into the haze. More importantly, **there is no general way to draw a non-block object** (see 4.3).
- **One constant-buffer update per chunk drawn** (`DrawChunks`, `render.cpp:803`). This is fine at today's view distances. At the largest render distance, on an old driver, it's a candidate for batching, but only if the F3 CPU "WORLD" row says so.
- **Post effects work on already-tonemapped 8-bit colour.** Screen-space AO darkens display colour, and bloom starts from an 8-bit scene. It works, but a half-float scene target would give better-behaved bloom and room for exposure changes, at the cost of more memory bandwidth. It's worth doing only if the look calls for it.
- **The September review's two cheap GPU cuts are still open**: take the smooth texture read only beyond 10 blocks, and replace three per-pixel noise evaluations with one small noise-texture read. Both keep the look.
- **Small robustness gaps**: some resource creations aren't checked. For example, the UI atlas texture creation at the end of `InitTextures` dereferences a null pointer if it fails, and the size-dependent targets aren't checked after a resize. These are rare on real hardware, but cheap to guard.

### 2.3 Lighting, sky and atmosphere (`sky.h`, shaders, `glowlight.*`)

**How it works.**
- **Sky and time.** One function of the day clock gives the sun, moon, star rotation and daylight. A second gives every light and sky colour: sun, moon, zenith, horizon, twilight band, and ambient from above and below. Both the sky shader and the world shader read the same values, so fog always matches the sky behind it. The sky is drawn per pixel with procedural stars (a hashed grid on a cube), moon discs, drifting cirrus clouds, and a sun disc bright enough to bloom.
- **Surfaces** are lit by hemisphere ambient (sky above, warm bounce below) times baked vertex AO, plus the sun (softened falloff so a low sun still lights flat ground) and the moon.
- **Sun shadows** come from a 2048² orthographic map covering up to ±112 blocks, with 2×2 hardware filtering, fading out at its edge.
- **Per-pixel normals** come from each texture's height map, with no tangent data in the vertex.
- **Shine** gives a sun glint and a faint sky reflection on glossy materials.
- **Glass** reflects the sky (Fresnel) with a sun glint.
- **Distance fog** blends into exactly the sky colour.
- **Glowing blocks** light their surroundings through a 64³ light grid around the player, rebuilt on the CPU only when the player crosses a chunk or a block near a light changes. Light is traced so walls cast shadows from it.

**Good.** This is the strongest part of the engine. It's physically sensible, all driven by a handful of per-frame constants, cheap, and coherent: one clock and one colour function feed everything. The glow grid is a smart, bounded way to get shadowed block light.

**Weaknesses and improvements.**
- **No darkness underground or indoors.** Ambient light is the same everywhere, so a cave, a room or the space under an overhang gets full sky ambient, darkened only by corner AO and SSAO. Direct sun is shadowed, but ambient isn't. Any game with caves, interiors or night-time shelter will look flat and bright inside. The standard cheap fix is a **sky-light value per vertex** (how open to the sky each cell is), computed in the mesher and multiplied into the ambient. The mesher's 8-byte vertex has no spare bits, so this means widening the vertex to 12 bytes, or giving up something (for example, spending the shade-class bits that slanted faces don't need). This is probably the single biggest visual improvement available.
- **Block light only reaches 32 blocks from the player** (the 64³ grid). A lit window or lava pool further away is dark. That's acceptable for close play and weak for vistas. A cheap distant fallback would be for the glow to show on the emitter itself (it already does) with no cast light beyond the grid.
- **Point lights for moving things don't exist.** Only blocks emit light. A creature, projectile or held torch can't light anything.
- **The glow grid's three channels are Voxistics' own**: music, timestream (The Line) and ember. They're generic in structure, but hard-wired to those meanings in the shader.
- **Sun shadows cover about ±112 blocks with one map.** That's sharp enough near the player. Past the map, terrain is unshadowed; the fade is there, so it shouldn't show as a line. Cascades would be the upgrade, but only if a new game needs long shadow distance.

### 2.4 Textures and materials (`vtex.*`, `blocktex.*`, `assets/textures`, `tools/`)

**How it works.**
- **The file format.** Textures are plain-text `.vtex` files: a palette plus a grid of palette keys, 16, 32 or 64 pixels across. Optional height, shine and glow maps sit alongside. `block` entries map textures to faces (all, top, bottom, side, front).
- **Loading.** At start-up, every `.vtex` file is parsed, scaled up to 64² by whole pixels, turned into mipmaps, and packed into one texture array: a colour array (sRGB) and a matching surface array (normal, shine, glow). Blocks with no authored art fall back to procedural placeholders. Problems go to `_errors.txt` and a toast; they never crash.
- **The textures themselves.** There are 93 authored materials in three files (natural, industry, the September trial batch). Python generators build them from continuous tileable fields, so a material can be regenerated at any size and tweaked by one parameter.
- **Up close and far away.** Pixels stay crisp up close (point sampling) and fade into anisotropic filtering with distance. Normal maps flatten with distance too, and a slow world-scale colour drift breaks up tiling.

**Good.**
- Text-based and diff-able.
- Authorable by hand, by script or by an art conversation (`TEXTURE_BRIEF.md` is the spec).
- Deterministic.
- Tolerant of bad input.
- The distance handling is exactly what pixel textures on an old GPU need.

**Weaknesses and improvements.**
- **The library is Voxistics'**, including the flesh set, the pulse industry set and "void slate". The September review's four-family direction (land, industry, strange, threat) was for Voxistics. **A new game needs its own curated list**, drawn from these 93 and the generators, not a wholesale copy of the block roster.
- **All layers are stored at 64² whatever the art's size.** 16² and 32² art is upscaled on load, so memory is 4 to 16 times what it needs to be. That's fine at 93 materials (about 2 MB with mips). It only matters if the library grows into the hundreds.
- **A contact-sheet tool** (every texture on a block, a slab and a pipe, in daylight and dusk, on one page) was proposed in September and would make curation far faster. It's still open.

### 2.5 Meshing and shapes (`mesher.*`, `shapes.*`, `icons.*`)

**How it works.**
- **Reading the neighbourhood.** Each 16³ chunk is meshed on the main thread from a padded 18³ copy of itself and its 26 neighbours, so every culling and AO test is a plain array read.
- **Cubes.** Cube faces hidden by opaque neighbours are dropped. Glass hides only its own kind.
- **Corner AO.** Each visible face gets classic four-corner AO, and the quad is split along its brighter diagonal to avoid the well-known AO seam.
- **Shapes.** Non-cube shapes are authored once in 1/8-block units and rotated by the block's state. There are 29: slabs, ramps, pyramids, faceted props, beams, pipes that join their neighbours, bevelled cubes and more.
- **Plant cards.** These are four vertices at a base point, spread in the vertex shader to face the camera.
- **Packing and budget.** Vertices are packed into 8 bytes. At most 6 chunk rebuilds run per frame, nearest first, and only once all neighbouring columns exist.
- **Icons.** Hotbar icons are rendered once at load by a tiny software rasterizer from the real meshes.

**Good.**
- It's fast and compact, with no per-frame allocation.
- The AO is done properly.
- Shapes are data rather than code paths.
- It's tested natively.

**Weaknesses and improvements.**
- **No greedy meshing** (merging same-texture faces into larger quads). The vertex format already reserves room for it (u and v run to about 32 blocks). It would cut vertex and triangle counts several times over for flat terrain. The trade-off is that merged quads can't carry per-corner AO unless the merge respects AO, which is the usual approach. Worth it if the GPU "WORLD" row or chunk counts become the limit.
- **Meshing on the main thread.** It's budgeted, so it doesn't stutter today. A worker thread is the next step if walking into new ground ever hitches. That was noted in September too.
- **Block IDs are 8-bit.** About 140 of 254 are used, and the mesher reserves 255. That's plenty for a curated new roster. It's a hard ceiling only if the new game needs hundreds of block types.
- **Picking is per cell, not per shape.** The crosshair targets a slab's whole cell, including its empty half. `DESIGN.md` 4.5 already names the fix: an ID-buffer readback, or a ray test against the shape's collision boxes (cheaper, and enough).

### 2.6 World, streaming, simulation and saves (`world.*`, `worldfile.*`, `persist.*`)

**How it works.**
- **Storage.** Chunks live in a hash map. A column (16 chunks, y 0 to 255) is generated when it enters the load radius plus one ring, at most 4 per tick, nearest ring first. Columns are evicted 2 rings beyond that, also 4 per tick.
- **Only changes are kept.** Unmodified chunks are simply dropped on eviction: the generator rebuilds them bit for bit. Modified chunks are kept, and they're all the save contains. A save records its generator's name and version, so old worlds keep regenerating the same way.
- **Scheduled updates.** A bounded queue of "update this cell in N ticks" handles gravity and grass die-back, at most 64 per tick. An idle world costs nothing.
- **Saves are crash-safe.** Each is written to a temp file, then rotated with a backup. A checksum, a version check, and a block remap by name are all applied before anything touches live state. Nine format versions load.
- **Player movement** has AABB collision (shape-aware), half-block step-up, crouch, sprint and a power slide with a camera lean.
- **Picking** uses an exact voxel ray walk.

**Good.**
- This is textbook, with the reasoning written down.
- Costs scale with what's near and what changed, never with world size.
- The save format is small and robust.
- The generator-version rule is exactly right for a game that regenerates untouched ground.

**Weaknesses and improvements.**
- **Terrain generation is a placeholder.** New worlds are dead flat at y = 12, with a noise patchwork of grass, sand and pebbles (`DefaultNewWorldGen` is marked TEMPORARY). The only alternative is a 2004-style sine-wave hill. There are no caves, water, trees, biomes or seeded shape. **Whatever the new game is, this is the biggest missing piece.** The structure (pure function of seed and coordinates, versioned, per-column, capped per tick) is ready for a real generator. It's the content that's missing.
- **Generation runs on the main thread**, at 4 columns per tick. That's fine for a trivial generator. A real one (3D noise, caves, features) will cost far more per column and should move to a worker thread, which fits the existing queue design.
- **No water or fluids.** Shallow water is a textured block.
- **Movement is instant.** There's no acceleration, momentum (outside the slide) or air control. It's fine for a builder. A game with more physical movement would want velocity-based horizontal motion.
- **Autosave writes on the main thread** (September proposal, still open). It's small today because saves only hold changes.
- **Game fields in the save format.** The Line's history and essence discoveries are version-7, 8 and 9 fields, and `GenerateColumn` notifies the pulse system when a chunk arrives (`world.cpp:394`). See section 3.

### 2.7 Menus, HUD, settings and input (`game.cpp`, `persist.cpp`)

**How it works.**
- **The UI.** Everything is drawn in immediate mode each frame, in pixel space, through one small UI pipeline: rectangles and text from a font atlas baked at start-up in six sizes from the system's monospace font.
- **Screens.** There's a title screen with 5 save slots, a pause menu, and an options hub with Look, Graphics, Display, Audio, Keybindings and Accessibility screens.
- **Controls.** Buttons act on release and feel pressed. Sliders drag. Every change is saved at once to `settings.cfg`.
- **Accessibility.** There's toggle-to-move, high-contrast menus, mono audio, colour-vision modes, FOV, music intensity and sensitivity per axis.
- **Input.** Win32 key and mouse messages, with rebindable actions.

**Good.**
- The settings and accessibility coverage is unusually complete for a prototype.
- Settings persist robustly.
- Buttons feel good.
- Fullscreen is borderless.
- The window is DPI-aware.

**Weaknesses and improvements.**
- **`game.cpp` does seven jobs in ~2,000 lines**: input, menus, HUD, overlays, the Voxistics store and map screens, the tutorial, and save flows. The September proposal to split it into `input.cpp`, `menus.cpp`, `hud.cpp` and the game screens is the natural way to separate engine UI from Voxistics UI.
- **Menu layouts are hand-placed per screen.** Adding a screen means writing its layout and click handling by hand. That's fine for a handful; a small shared layout helper (rows, sliders, toggles, back button) would make new screens cheap.
- **Mouse look recentres the real cursor every frame** (`main.cpp:180–194`) instead of reading raw mouse input. That gives lower precision and can pick up Windows pointer acceleration. It also fights with high-polling mice and remote-desktop tools. `WM_INPUT` raw mouse input is the standard fix.
- **No gamepad support.**
- **Text is ASCII only and set in capitals.** That's consistent with "English only, minimal text" (CLAUDE.md), but worth knowing if that changes.

### 2.8 Audio (`music_synth.*`, `sfx_synth.*`, `synth_kit.h`, `soundscape.*`, `audio.*`, `worldsound.*`)

**How it works.**
- **Music** is synthesized, not recorded. It's a one-hour composition in six sections (dawn to night) locked to the day clock, rendered in quarter-second chunks on its own thread, 4 seconds ahead. Because it's a pure function of the clock, it can't drift out of sync. Pausing is silence.
- **Sound effects** come from the world sound palette: about 50 sounds in five tiers (interaction, event, accent, texture, rare). They're synthesized with the same primitives as the music. Every pitch comes from the chord playing at that moment, and scheduled sounds land on the beat. Footsteps fall on the beat.
- **The soundscape** is a rolling census of blocks around the player. It drives three slow axes (positive/negative, calm/active, organic/mechanical) that colour both the sound effects and the music, and it notices discoveries.
- **Playback** uses XAudio2 with two voices: music, and effects in stereo with placement and distance.
- **Testing.** The whole palette renders offline, deterministically, so `tools/sound_demo.sh` can produce WAVs and check them for clashes without a sound card.

**Good.** Rare and distinctive. The entire soundtrack and sound design costs no disk space, never repeats a sample, and is always in key with itself. It's tested offline.

**Weaknesses and improvements.**
- **It's one composition.** The music is Voxistics' hour: its chord set, tempo and sections. `audio.cpp` statically asserts that the day is exactly 3,600 seconds. A new game with a different day length, mood, or a need for combat or tension music needs either a new composition in the same engine, or the per-day variation the September review proposed (still open). The engine (synth kit, chunking, clock anchoring, harmony queries) carries over. The score is content.
- **Sound effects are rendered on the main thread**, into roughly 35–45 ms of queued buffers, topped up once per frame. A frame longer than that (a hitch, a save, a window drag) can starve the effects voice, which is heard as a gap or click. The music already has a worker thread; the effects voice should get the same treatment, or fill from XAudio2's buffer-end callback.
- **Soundscape tables are Voxistics' blocks**: which blocks count as natural, mechanical, dark or "genesis". Structurally generic, but the content needs retuning for a new roster.
- **The effect names carry Voxistics' meaning** (timeslip, rift closed, vein, omen), and `worldsound.cpp` triggers cues for The Line. The synthesis is reusable; the mapping of events to sounds is game design.

### 2.9 Profiler, start-up and tools (`profiler.*`, `tests/`, `tools/`)

**How it works.**
- **F3** shows CPU time per system, GPU time per pass, and load counters: chunks resident and drawn, triangles, meshes built, shadow renders, queues waiting.
- **Ctrl+F3** writes a report, and a boot timeline marks every start-up stage.
- **Checks.** Native tests cover the pure-C++ modules. `check_shaders.py` validates every shader variant. `check_msvc.sh` catches what Visual Studio's SDL checks would reject.

**Good.** This is what made "lagless on a modest machine" enforceable rather than hoped for. Keep all of it.

**Weaknesses.** None of it can see a rendered frame. A small **screenshot capture** would close the biggest verification gap: a key, or a command-line flag, that saves the backbuffer to a PNG in the save folder. You could then send me frames from your machine, and I could compare looks between builds instead of guessing. That fits the privacy rule, because it only acts when asked and writes a local file.

---

## 3. Where the game is woven into the engine

Separating these is what makes the engine carry over cleanly. Each one is small; the point is to do all of them before building anything new.

| Seam | Where | What it does now | Proposed separation |
|---|---|---|---|
| Simulation tick | `main.cpp:216–234` | Calls pulse, fliers, The Line and essence by name | The tick calls one `GameTick(dt)` that the game supplies |
| World shader constants | `render.h:42`, `render.cpp:1182–1197`, world shader | The Line's position and direction, the music level, 8 flier spots, looped per pixel | Keep a small, generic "game effects" constant block, or drop it; a new game fills its own |
| World shader glow kinds | `blocks.h` `BlockGlow`, world shader, `glowlight.*` | Music, timestream (Line), ember, pulse | Generic kinds (steady, pulsing, driven-by-value), with the game deciding what drives them |
| Sky shader | `render.cpp:1122–1152` | The Line's sky lead, ghost moon, star wobble | Optional sky offsets supplied by the game (zero by default) |
| Extra draws | `render.cpp:1210–1212`, `1905–2104` | Line debug marker, pulses, fliers | A general "draw these objects" path (see 4.3) that the game fills |
| Chunk arrival | `world.cpp:394` | Tells the pulse system a chunk came back | A chunk-arrived callback the game registers |
| Save format | `worldfile.*`, `persist.cpp:324, 420–423` | Line history and essence discoveries in the file | A generic "game section" blob in the save, owned by the game |
| Block registry | `blocks.h` | Engine fields plus ~140 Voxistics blocks in one enum | Keep the structure; the new game writes its own roster |
| Sound glue | `worldsound.cpp`, `soundscape.cpp` | Line cues, Voxistics block classes | The game supplies its event-to-sound mapping and block classes |
| UI screens | `game.cpp` | Map, store and library screens mixed with engine menus | Split as in 2.7; the game owns its screens |

When this is done, the Voxistics-only files (`theline.*`, `pulse.*`, `pulse_colours.h`, `fliers.*`, `essence*.*`) can be removed from the build. The engine should then compile, run and look the same, minus those features. That's a checkable milestone: the before and after frames should match. It's also the answer to last time. We carry the engine intact, and only the Voxistics layer comes off.

---

## 4. Gaps a new game will almost certainly need

In rough order of how likely a new game is to need each one, whatever it turns out to be:

1. **A real terrain generator** (2.6). Seeded height, caves, water level, surface materials, features. It fits the existing versioned-generator structure, and moves generation to a worker thread.
2. **Sky light in the mesh** (2.3). This gives dark caves and interiors. It needs a wider vertex.
3. **A general object renderer.** Lit, fogged, shadow-casting meshes that aren't blocks: creatures, items, tools, anything that moves. It needs instancing for many copies. It would replace the debug-pipeline pulses and fliers.
4. **Interpolated rendering** (2.1). Draw between the last two ticks, so movement is smooth above 60 fps.
5. **Raw mouse input and gamepad** (2.7).
6. **Particles.** Dust, sparks, splashes, drawn cheaply and bloom-marked.
7. **Dynamic lights for moving things** (2.3). Even a few per frame.
8. **Effects audio off the main thread** (2.8).
9. **A new score, or per-day variation** (2.8). Which one depends on the game's mood.

None of these replaces an existing system. Each one adds to it.

---

## 5. Still open from the September review

- Frame cap ignored while vsync is on (fixes judder on displays that aren't 60 Hz).
- Autosave written on a background thread.
- The two cheap world-shader GPU cuts.
- Splitting `game.cpp`.
- Moving shader strings to a `shaders.h`.
- Per-day music variation, and breathing room in the music.
- The texture contact-sheet tool.

---

## 6. Suggested order

1. **Agree the inventory.** That's this document: which parts are engine, which are Voxistics.
2. **Separate the seams** (section 3), with no change in behaviour, checked by frames from your machine before and after. Add the screenshot capture first, so there's something to compare.
3. **Decide the new game's core**, then choose from section 4 only what it needs, plus the few cheap fixes from section 2 that it benefits from.

Nothing in this document has been built.
