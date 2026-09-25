# reference/: old material, kept only to take ideas from

Nothing in this folder is compiled, included or loaded by walkgrid. It's here so ideas can be taken from it, and each item leaves once nothing more is wanted from it.

**The rule** (CLAUDE.md, DESIGN.md 1.2):
- Take ideas from these files, never code.
- When an idea is taken, note it under that item below.
- When an item has nothing left worth taking, or the owner says so, delete it and log the removal in `docs/DECISIONS.md`. Git history keeps it if it's ever wanted again.
- This rule covers `reference/` only. The Voxistics engine code carried into walkgrid is walkgrid's code now (D1).

---

## seeds/: the owner's four hand-tested prototypes

| File | Lines | Still worth taking | Taken so far |
|---|---|---|---|
| `Prismative.cpp` (D3D11) | 1,196 | Little: its manager-class split and shape primitives are already in the engine | Shape primitives (slab, ramp, tube, pyramid, funnel), AABB collision, block picking (superseded) |
| `drillder.cpp` (GDI+ slice viewer) | 882 | Per-voxel durability (mining, machine progress); a three-slice view as a debug overlay for looking inside terrain | Integer block facing (the state byte) |
| `LG2.cpp` (2D cellular automaton) | 1,003 | Block behaviours as budgeted per-tick rules: grass spreading, tree growth, fire, water, desert creep, lifespans | Budgeted work per tick; deferred mutation (the scheduled-update queue) |
| `cc_2_2_2.cpp` (2D sandbox, 145 texture generators) | 5,759 | The texture technique library: ground, weaves, geometry, tech panels (index in `voxistics/DESIGN.md` 8.4) | Bake-once procedural textures; the natural set's techniques |

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
| The Voxistics block roster (~130 blocks: flesh, genesis, custodian, trial batch and more) | `blocks.h`, `blocktex.cpp` placeholder tiles | M1: replaced by walkgrid's starting materials |
| The cube mesher | `mesher.*` | M1: replaced by the facet mesher |
| Cube props and building shapes | `shapes.*` | M1: parked (D12's spirit; nothing faceted uses them) |
| Hotbar icons rendered from cube meshes | `icons.*` | M1: render faceted lumps |
| **Falling ground** (gravity on placed blocks) | `world.cpp` (`MaybeQueueFall` in `LiveEdit`) | **Still active today.** Parked for the first test (D12): switch off in M1 |
| **Grass dying back under cover** | `world.cpp` (`UPD_GRASS_COVER`) | **Still active today.** Parked for the first test (D12): switch off in M1 |
| Test world generators (flat patchwork, sine hills) | `world.cpp` | M1: replaced by walkgrid's versioned test landscape |
| Sounds named for Voxistics events (Timeslip, Omen, Vein, Rift closed) | `sfx_synth.*`, `worldsound.cpp` | Keep the sounds, which are good; rename or re-map them when walkgrid's events are designed |
| The soundscape's block classes (natural, dark, genesis, mechanical) | `soundscape.cpp` | M1: retuned for walkgrid's materials |
| The trial-batch and flesh textures | `assets/textures/*.vtex` | M1: the owner picks the starting set; the rest are parked |
| The texture brief (describes cubes and Minecraft-scale blocks) | `assets/textures/TEXTURE_BRIEF.md` | M1: rewritten for faceted ground and smooth filtering |
| Chests and machines holding data records | `blocks.h`, `world.h` | Decide in M1: keep the data-record system (engine), drop the blocks |
