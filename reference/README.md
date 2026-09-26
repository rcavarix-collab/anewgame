# reference/: old material, kept only to take ideas from

Nothing in this folder is compiled, included or loaded by walkgrid. It's here so ideas can be taken from it, and each item leaves once nothing more is wanted from it.

**The rule** (CLAUDE.md, DESIGN.md 1.2):
- Take ideas from these files, never code.
- When an idea is taken, note it under that item below.
- When an item has nothing left worth taking, or the owner says so, delete it and log the removal in `docs/DECISIONS.md`. Git history keeps it if it's ever wanted again.
- This rule covers `reference/` only. The Voxistics engine code carried into walkgrid is walkgrid's code now (D1).

---

## seeds/: the owner's hand-tested prototypes and texture generators

| File | Lines | Still worth taking | Taken so far |
|---|---|---|---|
| `Prismative.cpp` (D3D11) | 1,196 | Little: its manager-class split and shape primitives are already in the engine | Shape primitives (slab, ramp, tube, pyramid, funnel), AABB collision, block picking (superseded) |
| `drillder.cpp` (GDI+ slice viewer) | 882 | Per-voxel durability (mining, machine progress); a three-slice view as a debug overlay for looking inside terrain | Integer block facing (the state byte) |
| `LG2.cpp` (2D cellular automaton) | 1,003 | Block behaviours as budgeted per-tick rules: grass spreading, tree growth, fire, water, desert creep, lifespans | Budgeted work per tick; deferred mutation (the scheduled-update queue) |
| `cc_2_2_2.cpp` (2D sandbox, 145 texture generators) | 5,759 | The texture technique library: ground, weaves, geometry, tech panels (index in `voxistics/DESIGN.md` 8.4) | Bake-once procedural textures; the natural set's techniques |
| `newpatterns.cpp` (GDI+ texture generators, added 2026-09-26) | 1,604 | 100 natural stone and soil patterns on a 32 × 32 tiling grid: granite, strata, basalt columns and cracks, mud cracks, gravel, loam, moss and root mats, crystals, lava crusts. Ideas for new materials and for re-authoring the twelve (TEXTURE_BRIEF.md) | Nothing yet |
| `bitmappattern.cpp` (GDI+ pattern viewer, added 2026-09-26) | 1,979 | 42 geometric patterns (checkerboard, plaid, herringbone, chevrons, gears, honeycomb, halftones) and a viewer: ideas for built pieces and UI, not ground | Nothing yet |

**Likely first to go:** `Prismative.cpp`, once the facet world replaces the cube shapes in M1 and nothing else is wanted from it. The owner decides.

## voxistics/: Voxistics' own documents

| File | Why it's kept | Remove when |
|---|---|---|
| `DESIGN.md` | The full Voxistics design, including the parts walkgrid dropped (pulse, The Line, essence, fliers) and Part VIII's index of the seeds | The seeds are gone and no dropped feature is being reconsidered |
| `REVIEW_2026-09.md` | The September review: its open proposals are tracked in walkgrid's docs now | Nothing in it is still open |
| `SCOPE_MOSCOW.xlsx` | Voxistics' scope, including its development pile | Nothing in its pile is wanted for walkgrid |

---

## Voxistics pieces still inside walkgrid's build

Not reference material: this is live code and data, carried over whole in M0 so nothing broke. Each is to be removed or replaced deliberately, in the milestone named. Listed here so none of it lingers by accident.

| What | Where | Plan |
|---|---|---|
| ~~The Voxistics block roster~~ | `blocks.h` | **Done (M1.9):** replaced by the twelve materials (D36, D41) |
| ~~The cube mesher~~ | `mesher.*` | **Done (M1.9):** removed; the facet mesher draws the ground |
| ~~Cube props and building shapes~~ | `shapes.*` | **Done (M1.9):** removed from the build (git history keeps them) |
| ~~Hotbar icons rendered from cube meshes~~ | `icons.*` | **Done (M1.9):** faceted lumps |
| ~~Falling ground~~ | `world.cpp` | **Done (M1.9):** switched off (D12); the update engine and its handler stay |
| ~~Grass dying back under cover~~ | `world.cpp` | **Done (M1.9):** switched off (D12); handler stays |
| Flat test ground (patchwork of grass, sand, gravel) | `world.cpp` | Kept as the test ground (D37); sine hills removed (M1.4) |
| Sounds named for Voxistics events (Timeslip, Omen, Vein, Rift closed) | `sfx_synth.*`, `worldsound.cpp` | Keep the sounds, which are good; rename or re-map them when walkgrid's events are designed |
| ~~The soundscape's block classes~~ | `soundscape.cpp` | **Done (M1.9):** retuned for the materials; its dark, genesis and mechanical axes rest at their calm defaults |
| ~~The trial-batch and flesh textures~~ | `assets/textures/*.vtex` | **Done (M1.9):** parked in `assets/textures/parked/` (natural_parked, batch_sept_parked) |
| ~~The texture brief~~ | `TEXTURE_BRIEF.md` | **Done (M1.3)** |
| Chests and machines holding data records | `world.h`, `worldfile.cpp` | The blocks went in M1.9; the data-record system (engine) stays, unused |
| The glow grid and glowing blocks | `glowlight.*`, world shader | Engine kept, inert: no walkgrid material glows |
| The scheduled-update engine (Part V) | `world.cpp` | Kept, idle: nothing in walkgrid schedules updates |
