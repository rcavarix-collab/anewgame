# Prototype outline (in progress)

A working record of what's been decided for the first prototype and what's still open. Nothing is built until you approve the finished outline. The engine review is `docs/ENGINE_REVIEW.md`.

## Decided

| # | Decision | Notes |
|---|---|---|
| D1 | The Voxistics engine carries across intact. Only the Voxistics game layer comes off (ENGINE_REVIEW.md section 3). | Checked by matching frames before and after. |
| D2 | Music: death metal instead of electronic, following the action rather than the clock. | Synthesized or recorded: open (Q4). |
| D3 | Reactive sound effects that don't interfere with the music. | Frequency and timing slots, ducking, a voice cap with priority tiers, and effects rendered off the main thread. |
| D4 | Textures and lighting carried over. Add sky light, so caves and interiors are dark. | |
| D5 | World: **option A**. A faceted surface over the existing grid, not cubes (Minecraft) and not smooth blobs (Astroneer). Smaller triangles where needed. | Storage, streaming and saves stay; the mesher and collision are replaced. |
| D6 | Lots of action without lag: every action system has a fixed per-tick budget, shown in F3. | Numbers to be set (Q5). |
| D7 | Natural material transitions (grass into dirt, and so on) instead of a quilt of triangles. | See "Material blending" below. |

## Material blending (D7): proposed approach

The quilt comes from each triangle having one material. The fix is to give **materials to the corners, not the triangles**, and blend across the surface:

1. **Weights per corner.** Each mesh vertex takes the materials of the grid cells around it: up to three per triangle, with weights. Inside a single material, a triangle uses the plain one-texture path, so only the triangles on a border pay for blending.
2. **Blend by height, not by fading.** A straight crossfade looks muddy. Each material already has a height map (from the Voxistics surface maps), so at a border the taller detail wins: grass tufts poke through over dirt, and pebbles sit proud of sand. The edge comes out ragged and natural, and it's crisp rather than blurred.
3. **Borders that don't follow the triangles.** A world-space noise nudges the weights, so borders wander instead of tracing triangle edges. The shader already has this noise.
4. **Texture projection for any angle.** Facets aren't aligned to the axes, so textures are projected from the world (two projections blended by the surface angle), not from per-face coordinates.
5. **Cost control.** At most three materials per triangle. Triangles away from a border use the single-texture path. Distant ground drops to the dominant material.
6. **Transitions over time (optional).** Because weights live on the vertices, grass creeping onto dirt, or ground scorched by action, is a weight change and a rebuild of that one piece. There's no per-frame cost.

Vertex size grows from 8 bytes to about 16–20 (materials, weights, normal). That's expected and affordable.

**Checking it without Windows:** a small native tool that renders still images of the faceted mesh with this blending, on the CPU, so you can judge the look before it goes into the engine. Then the same scene on your machine, to confirm the real shader matches.

## Open questions

- **Q1. The action.** What is it: enemies, destruction, projectiles? First person or not?
- **Q2. The first test.** What you do in it, in a sentence or two.
- **Q3. Texture style.** Pixel-crisp (as in Voxistics) or smoothly filtered? Height blending works with both, but they look different.
- **Q4. The music.** Synthesized in code (the riskiest part: judged by WAV clips first) or recorded stems you provide?
- **Q5. Scale.** Roughly how much action at once: tens, hundreds or thousands of moving things? This sets the budgets.
