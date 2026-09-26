# walkgrid: naming and annotation conventions

The standard for every file we write or touch, from 2026-09-26. It writes down what the code already does in almost every file (measured in `docs/AUDIT_2026-09-26.md`), so following it means matching the code around you, not changing course. It extends FOUNDATIONS.md §6 and the "Organisation and careful annotation" priority in CLAUDE.md.

**Existing code isn't renamed to fit.** Renaming working code is churn and risk (FOUNDATIONS 1.3). Where the code has an established exception, it's recorded here as allowed. Files are brought up to this standard when a step touches them anyway, or in a dedicated notes-only pass the owner approves.

---

## 1. Names

### C++
| What | Style | Examples |
|---|---|---|
| Functions and methods | PascalCase | `GroundHeight`, `UpdatePlayerPhysics`, `JobsSubmit` |
| Types (structs, classes, enums) | PascalCase | `FacetTri`, `ChunkCoord`, `SoundPalette` |
| Plain `enum` values | ALL_CAPS, with the enum's prefix | `JOB_TERRAIN`, `PROF_MESH`, `MAT_STONE` |
| `enum class` values | PascalCase | `MenuScreen::Pause`, `DecodeResult::BadChecksum` |
| Globals (file scope, `static` or `extern`) | `g_` + camelCase | `g_loadRadius`, `g_chunkVersionCounter` |
| Constants | ALL_CAPS | `CHUNK_SIZE`, `WALK_UP`, `SAVE_VERSION` |
| Locals, parameters, struct members | camelCase | `meshVersion`, `seenFrame`, `minUp` |
| Namespaces | lowercase | `synth` |

**Allowed exceptions, recorded rather than renamed:**
- **Synthesis files** (`music_synth.cpp`, `sfx_synth.cpp`, `synth_kit.h`) name their tuning constants `kName` (`kOutputScale`, `kMasterDb`). New constants in those files follow them; everywhere else uses ALL_CAPS.
- **Large constant data** that reads as a table or source text (shader source strings, the block registry, key bindings) keeps the `g_` prefix: `g_shaderSrc`, `g_blocks`.
- **`wWinMain`:** Windows requires the name.
- **Five older `k`-prefixed tables** outside the synth files (`kProcedural`, `kU`/`kV`, `kHandlers`, `kPatchSoft`, `kTimePresets`): leave them as they are; new tables use ALL_CAPS.

**Names say what, in the game's words.** Use walkgrid's own vocabulary (facet, cell, chunk, column, material, band), never a removed feature's (pulse, essence, The Line). When a feature is removed or renamed, search for its words and update comments too.

### Files
- **Source files:** lowercase, words run together, with one system per file (`facetmesh.cpp`, `groundmesh.h`, `worldsound.cpp`). A few files use an underscore (`music_synth.cpp`, `game_internal.h`); leave them, but new files run words together unless that makes the name hard to read.
- A `.cpp` and its `.h` share a name. Every source file has its line in `tools/layers.txt` and the project file.
- **Tools** (`tools/`): lowercase with underscores (`check_layers.py`, `natural_textures.py`), following Python's own convention.
- **Docs:** `docs/NAME.md` in capitals (`DECISIONS.md`, `COST_LEDGER.md`); research notes in `docs/research/`.

### Other names
- **String-table keys** (D26): lowercase, dotted by screen or area (`pause.resume`, `library.rows`).
- **Decisions** are `Dnn` (`docs/DECISIONS.md`), scope rows `Wnnn` (`docs/SCOPE_MOSCOW.xlsx`), Grok tasks by their group letter and number (`E15`).
- **Python:** functions and variables in snake_case, constants in ALL_CAPS (PEP 8).

---

## 2. Annotation

### The file header
Every file opens with a header saying:
1. **What it is:** the system, in a sentence or two;
2. **What it costs:** per frame, per call or per change, or "nothing per frame";
3. **Its layer:** 1 base, 2 platform, 3 world, 4 presentation, 5 game, 6 app;
4. **Its design section:** a `DESIGN.md` section (or `docs/…` for specs such as SOUND_PALETTE).

Template (from `jobs.h` and `terrain.h`, the models to copy):

```
// facetmesh.h
//
// What it is, in plain words: the system, and what it does for the game.
// Anything a caller must know (threads, ownership, invariants).
//
// Layer 3 (world). Cost: linear in the surface faces meshed; nothing per
// frame. DESIGN.md Part XXIII.
```

**Pairs:**
- The **`.h` carries the full header**, since it's the interface other files read.
- The **`.cpp` opens with a pointer** (`// facetmesh.cpp -- see facetmesh.h.`), plus anything specific to the implementation: its structure, its tricks, what it deliberately doesn't do.
- **A `.cpp` without a `.h`** (`main.cpp`, `game.cpp`, `tests/tests.cpp`, the tools) carries the full header itself.

**Python and shell tools:** a module docstring or top comment covering the same points. "Layer" doesn't apply; say instead what the tool reads and writes, and whether it's part of the checks.

### Lines that need a note
- **Say what a non-obvious line guards against,** not what it does (FOUNDATIONS §6). For example: "the head may meet a ceiling cell this far: ceilings' facets hang as much lower or higher".
- **Every tuning constant says its unit and meaning where it's declared.** The walking constants in `world.cpp` are the model: `CLIMB` 1.05, "the most the feet rise onto the ground ahead: a one-cell slope, never a two-cell wall".
- **Magic numbers from outside get a name or a note:**
  - a hash's constants ("FNV-1a 64-bit prime and offset basis");
  - a format's layout ("run-length pairs: count, then value");
  - a spec section ("SOUND_PALETTE 5.3").
- **Numbers from research are ours** (D68). The note says where the theory is (`docs/research/…`), never that a value came from a paper's table.

### References in comments
- **Point to things that exist:** `DESIGN.md` sections, `Dnn`, `Wnnn`, `docs/…` files, other source files by name.
- **When a file, section or feature is renamed or removed,** search the code for references and update them in the same step.
- **Provenance notes are welcome** ("moved from Voxistics' persist.cpp in M0.9, unchanged in behaviour"), but must name today's files correctly.

### What stays out of code comments
- **`TODO`, `FIXME` and `XXX`.** There are none today; open work goes to the scope sheet as a `W` row, and the comment, if any, names the row.
- **Brand, product and company names.**
- **Player-facing text.** It goes through the string table; a literal that isn't player text is marked `// D26: not player text`, which `check_strings.py` understands.

### The docs are part of done (SOP)
A step that changes a file:
- brings that file's header up to this standard;
- checks its references;
- updates `DESIGN.md` where the design changed.

The SOP's check line covers it: every new or changed file has its header.

---

## 3. Checks

**Today:**
- `check_layers.py` checks layers and `layers.txt`;
- `check_strings.py` checks player text and its keys.

**Proposed** (`docs/AUDIT_2026-09-26.md`; code, so it waits for the go-ahead): `tools/check_notes.py`, run with the other checks. It would check:
- that every `.h` and every stand-alone `.cpp` header names its cost, layer and design section;
- that every `DESIGN.md` section referenced from code exists (FOUNDATIONS §6 already promised this check);
- that no comment names a removed file (`persist.cpp`, `mesher.cpp`, `shapes.cpp`) or a removed feature (from a short list kept in the script);
- that there are no `TODO`, `FIXME` or `XXX` markers.
