# walkgrid against the published theory: an evaluative overview

The owner asked (2026-09-26) for our whole approach to be checked against how others have solved the same problems, what options exist, and where we could do better. **Theory and explanation only (D66): methods described in words, never anyone's code.** This is Claude's overview. Grok's E-series tasks (E1–E18) deepen each topic, and every finding is checked before anything is built.

**Sources.**
- **[r]** sources were read in full (a legal copy supplied by the owner).
- **[s]** sources were confirmed to exist by search this session: title, authors, venue.
- **[k]** sources are cited from Claude's knowledge and still need checking at the source.
- The network here blocks most publishers, so no paper was read in full; the summaries of the [s] sources come from their abstracts as returned by search.

## Verdict in one paragraph

walkgrid's foundations match well-established practice:
- per-chunk meshing on worker threads with version stamps;
- a fixed-step simulation drawn between ticks;
- world-space (triplanar) texturing;
- height-based material blending;
- a single cascaded-style shadow map;
- baked occlusion instead of screen-space;
- procedural audio;
- delta saves with a checksum and a crash-safe write.

Nothing needs replacing. The published work points to six improvements, four of them already on Tuesday's plan. In order of value:
1. **Stochastic tiling** to hide repetition (plan E).
2. **Clouds animated by flow and warping, not sliding** (plan A–C).
3. **Relaxed surface nets** as a principled answer to terracing.
4. **Bent-normal ambient** from our existing bake.
5. **An analytic sky model**, used as a colour reference.
6. **Far terrain from a coarse ring** (clipmap-style) to see beyond the loaded world.

## System by system

| System | Ours | What the theory offers | Verdict |
|---|---|---|---|
| Surface from cells | Facet mesher: corners where cells meet, eased halfway toward a smooth surface, jittered, lumps where calm | Marching cubes [s] (Lorensen & Cline 1987) makes surfaces from sampled density; dual contouring [s] (Ju et al. 2002) keeps sharp features from Hermite data; **surface nets [s] (Gibson 1998) relax a net of nodes, each constrained to stay inside its own cell, which removes terracing** | Ours is a relative of surface nets: one easing step, no relaxation. **Relaxation constrained to the cell answers terracing (D62) in a principled way:** gentle slopes smooth out, the surface never leaves its cells (so picking and collision stay honest), and edited cells can be pinned (R3's flag). A strong lead for W055. |
| Texture projection | Three axis projections weighted by the smooth normal to the 4th power | Triplanar mapping [s] (Geiss, *GPU Gems 3* ch. 1, 2007), the same idea | Standard and sound. Biplanar variants (two reads instead of three) [k] could save texture reads if the world pass ever gets tight. |
| Repetition | Every block repeats its texture; one slow world-wide colour drift | Texture bombing [s] (Glanville, *GPU Gems* ch. 20, 2004): scatter shifted copies on a grid of cells; **histogram-preserving stochastic tiling [s] (Heitz & Neyret 2018, HPG best paper): blend three randomly offset copies on a triangle grid while keeping the texture's contrast; they report about 20× faster than comparable procedural noise**; tile sets (Wang tiles) [k] | **Plan E should follow Heitz & Neyret's idea:** it's made for exactly this and cheap (three reads per projection). A plain average of three copies washes contrast out; their contrast-keeping blend is the point. Our crisp near-texels need care: the random offset should snap to whole texels near the player. |
| Material blending | Height-based: weights lifted by each texture's height map, tallest wins in a narrow band, world noise | **Max-blending [r] (Schuster et al. 2020): effective height = weight × height, tallest wins, a small grace band blended**; splat maps; dithered transitions | Ours is the right family but **additive** (weight + height), which lets a low-weight material's peaks poke through far from its ground: the orange lines. The published rule is **multiplicative**; adopt it in plan D, with the per-texel decision near the player. See `TEXTURE_BLENDING.md`. |
| Sun shadows | One 2048² map over ±48–112 blocks, redrawn over four frames, crossfaded, 9-tap filter, normal offset | Cascaded / parallel-split shadow maps [s] (Zhang et al. 2006; Dimitrov, NVIDIA 2007): several maps at growing distances; soft filtering (PCF [k], PCSS [k], variance [k] and exponential maps [k]) | A single map suits our short view. **If far terrain arrives, a second cascade** (a coarse map for distance) is the standard next step. Our crossfade is an unusual, sensible answer to a moving sun at low cost. |
| Ambient and occlusion | Baked per corner: openness (64 cells) and an 8-direction sky view; SSAO optional, off | Ground-truth AO and **bent normals [s] (Jimenez et al. 2016, Activision)**: the average open direction steers ambient toward where the sky really is; horizon-based methods [k] | Our sky-view bake already walks horizons in 8 directions, so the **bent direction comes almost free at bake time**. Storing it needs vertex room we don't have (16 bytes, full): a packed 2-byte direction would make the 16-byte vertex 18 bytes (20 in practice, for alignment). Worth measuring after plan G shows what the noon patches are. |
| Sky colour | Hand-tuned gradient, twilight band, sun glow | Analytic sky models [s] (Hosek & Wilkie 2012, which improves on Preetham 1999 [k] at sunsets and in haze); precomputed atmospheric scattering [k] (Bruneton & Neyret 2008) | We don't need a physical model per pixel. **Use Hosek–Wilkie as a reference:** sample its colours offline at our sun heights and fit our gradient to them (a tuning pass, R1's item 2). Cheap and physically grounded. |
| Clouds | Two noise layers on planes, drifting with the jet stream | Flow maps [s] (Vlachos, "Water flow in Portal 2", SIGGRAPH 2010): each point advected along a flow field, with two phases crossfaded so the texture never smears; domain warping and noise evolution [k]; volumetric clouds [k] (excluded: cost, owner) | **The flow-map idea is exactly what the clouds lack:** shapes that move *through* rather than slide. Two noise phases advected along the wind and crossfaded, plus slow evolution of the noise itself, give forming and fading clouds for a few more operations per sky pixel. Plan A. |
| Fog and distance | Edge fade over the last fifth, faint exponential haze | Exponential height fog and aerial perspective [k] (Hoffman & Preetham 2002; Wenzel, SIGGRAPH 2006 course) | Height-aware haze (R1 item 3) is the textbook form; add it once we see further. |
| Seeing far | Detail bands per chunk; world ends at render distance | Geometry clipmaps [s] (Losasso & Hoppe 2004): nested grids centred on the viewer, refilled as it moves; geomipmapping [k]; chunked LOD [k] | **Our terrain height is a pure function of (x, z), which is exactly what clipmaps want:** a coarse height ring beyond the loaded world, drawn cheaply and fogged, would end the "world stops at 48 blocks" problem. Edits don't matter at that distance. The biggest single look upgrade still unplanned; a candidate after Tuesday. |
| Visibility | Chunk face-connectivity, walk from the camera, frustum culling | Cave-culling flood fills [k] (a practitioner method), portals [k] (Luebke & Georges 1995), hierarchical Z [k] (Greene et al. 1993) | Ours is the established method for block worlds. Nothing to change. |
| Threads | Fixed pool; work on copies; versioned results applied on the main thread | Job systems, work stealing [k] (Blumofe & Leiserson 1999), fibres [k] (Gyrling, GDC 2015) | Ours is the simple, safe end of the range, right for our load. Work stealing matters only at many cores. |
| Frame pacing | Fixed step, interpolated drawing, vsync or cap | Fixed-timestep loops with interpolation [k] (widely cited practitioner article, Fiedler 2004) | Textbook. Nothing to change. |
| Procedural audio | Additive and subtractive synthesis, harmony-locked, three axes | Procedural audio as process, not recordings [s] (Farnell, *Designing Sound*, MIT Press 2010); physically informed models [k] (Cook 2002) | We're squarely in this tradition. **Farnell's models for wind, rain, footsteps and fire are the theory base for weather sound (G11)** and better footsteps. |
| Saves | Delta chunks, run-length encoding, FNV-1a checksum, temp–backup–swap write | Atomic commit and journalling [k] (SQLite documentation), region files [k], compression [k] | Sound. If saves grow large: general compression (deflate) over runs, and a stronger checksum [k] (CRC-32). No urgency. |
| Text | Key table per language, glyph atlas from used characters, one cell per character | Unicode text layout [k]: bidirectional algorithm (Unicode UAX #9), shaping engines, East Asian width (UAX #11) | **UAX #11 defines which characters are wide:** the basis for W068's double-width cells. Shaping (W069) stays parked; the route, if ever, is to let Windows lay out whole strings. |
| Walking | Box against the facet floor, half-block step-up | Character controllers [k] (Ericson, *Real-Time Collision Detection*, 2004) | Standard. G5 and the building layer will test it. |

## What this adds to the plan

| Idea | Where it goes | Status |
|---|---|---|
| Stochastic tiling, texel-snapped near the player | Plan E, step 8 | Tuesday |
| Clouds advected with two crossfaded phases (flow-map idea) plus noise evolution | Plan A–C, steps 5–6 | Tuesday |
| Hosek–Wilkie as a colour reference for our sky and light | R1's tuning pass | Batch 4 or after |
| Constrained surface-net relaxation for terracing, edited cells pinned | W055 | A new lead for the parked row; needs a plan |
| Bent-normal ambient from the existing sky-view bake | After plan G | Measure first |
| Far terrain as a coarse clipmap-style ring from the height function | New | Proposed as its own feature after Tuesday |
| A second shadow cascade | With far terrain | Later |
| East Asian width by UAX #11 | W068 | Planned |

## Sources

Read in full [r]:
- Schuster, Trettner, Schmitz & Kobbelt, "A three-level approach to texture mapping and synthesis on 3D surfaces", PACM CGIT 3(1), 2020: https://doi.org/10.1145/3384542 (notes: `TEXTURE_BLENDING.md`)

Found by search this session [s]:
- Lorensen & Cline, "Marching cubes", SIGGRAPH 1987: https://dl.acm.org/doi/10.1145/37402.37422
- Ju, Losasso, Schaefer & Warren, "Dual contouring of Hermite data", ACM TOG 21(3), 2002: https://dl.acm.org/doi/10.1145/566654.566586
- Gibson, "Constrained elastic surface nets", 1998: https://www.semanticscholar.org/paper/Constrained-Elastic-Surface-Nets:-Generating-Smooth-Gibson/3949fce7abebcd89db0db70d286ce9d24a2d5bea
- Geiss, "Generating complex procedural terrains using the GPU", *GPU Gems 3* ch. 1, 2007: https://developer.nvidia.com/gpugems/gpugems3/part-i-geometry/chapter-1-generating-complex-procedural-terrains-using-gpu
- Glanville, "Texture bombing", *GPU Gems* ch. 20, 2004: https://developer.nvidia.com/gpugems/gpugems/part-iii-materials/chapter-20-texture-bombing
- Heitz & Neyret, "High-performance by-example noise using a histogram-preserving blending operator", PACM CGIT, 2018: https://dl.acm.org/doi/10.1145/3233304 (and https://inria.hal.science/hal-01824773/)
- Zhang et al., "Parallel-split shadow maps for large-scale virtual environments", 2006; *GPU Gems 3* ch. 10: https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-10-parallel-split-shadow-maps-programmable-gpus
- Dimitrov, "Cascaded shadow maps", NVIDIA, 2007: https://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
- Jimenez et al., "Practical real-time strategies for accurate indirect occlusion", Activision, 2016: https://research.activision.com/publications/archives/practical-real-time-strategies-for-accurate-indirect-occlusion
- Hosek & Wilkie, "An analytic model for full spectral sky-dome radiance", ACM TOG 31(4), 2012: https://cgg.mff.cuni.cz/projects/SkylightModelling/HosekWilkie_SkylightModel_SIGGRAPH2012_Preprint_lowres.pdf
- Losasso & Hoppe, "Geometry clipmaps: terrain rendering using nested regular grids", SIGGRAPH 2004: https://hhoppe.com/proj/geomclipmap/
- Vlachos, "Water flow in Portal 2", SIGGRAPH 2010 course: https://advances.realtimerendering.com/s2010/index.html
- Farnell, *Designing Sound*, MIT Press, 2010: https://mitpress.mit.edu/9780262014410/designing-sound/

From knowledge, to be checked at the source [k] (Grok's E tasks ask for this):
- Preetham, Shirley & Smits, "A practical analytic model for daylight", SIGGRAPH 1999.
- Bruneton & Neyret, "Precomputed atmospheric scattering", 2008.
- Hoffman & Preetham, "Rendering outdoor light scattering in real time", 2002.
- Wenzel, "Real-time atmospheric effects in games", SIGGRAPH 2006 course.
- Luebke & Georges, "Portals and mirrors", 1995.
- Greene, Kass & Miller, "Hierarchical Z-buffer visibility", 1993.
- Blumofe & Leiserson, "Scheduling multithreaded computations by work stealing", 1999.
- Gyrling, "Parallelizing the Naughty Dog engine using fibers", GDC 2015.
- Fiedler, "Fix your timestep", 2004 (a practitioner article, widely cited).
- Cook, *Real Sound Synthesis for Interactive Applications*, 2002.
- Ericson, *Real-Time Collision Detection*, 2004.
- Unicode Standard Annex #9 (bidirectional algorithm) and #11 (East Asian width).
- SQLite documentation, "Atomic commit".
