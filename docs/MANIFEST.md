# What ships, what stays, what goes

Every file in the repository belongs to one of three groups. Keep this log current: **every new file gets its line** (SOP checklist), and a whole folder counts as one line where everything in it shares a fate.

| Group | Meaning | Reaches a player? |
|---|---|---|
| **Ship** | In the release a player downloads: the built game and the data it loads at run time | Yes |
| **Keep** | Needed to build, check, change or understand the game: source, tools, tests, design and decision records | No, it stays in the repository |
| **Retire** | Scaffolding that's spent once its job is done: working notes, deliveries, research drafts, pictures of old builds | No, deleted when spent, with a line in `docs/DECISIONS.md` |

**Research and audits never reach a player,** because nothing in `docs/` is part of the release. They're "keep" or "retire" for the repository's sake, not the player's. A release is built from the **Ship** list alone, so the player's download is lean whatever the repository holds.

Checked 2026-09-26 against the code: what the game loads at run time is `FindAssetDirectory` (gamefiles.cpp), the texture loader (render.cpp, `LoadAuthoredTextures`, top of `assets/textures/` only, not subfolders) and the string table (strtable.cpp).

## Ship: the release

| Item | Why |
|---|---|
| `walkgrid.exe` (built in Release) | The game. Shaders, music, sound, the sky and the UI are compiled into it; there are no audio or image files (D25). |
| `assets/textures/*.vtex`, top level only (today `natural.vtex`, `batch_sept.vtex`) | The materials' art, loaded at start. |
| `assets/text/<language>.txt` (today `en.txt`; more as translations are adopted) | Every player-facing word (D26). |
| A short read-me for players (not written yet) | What the game is and where saves go, with no brand names. To write before the first release. |

**Created at run time on the player's machine, never shipped:** the settings file, saves (and their `.bak`), the shader cache, screenshots (F2) and performance reports (Ctrl+F3). All local, never sent (D23).

## Keep: the workshop

| Item | Why |
|---|---|
| `*.cpp`, `*.h` at the root | The game's source. |
| `walkgrid.sln`, `walkgrid.vcxproj`, `walkgrid.vcxproj.filters` | The build; the project file is the source list. |
| `.gitignore` | Keeps build output and local files out of the repository. |
| `tests/` | Native tests (`run.sh`, `tests.cpp`, `stub/`). |
| `tools/check_*.py`, `tools/check_*.sh`, `tools/layers.txt` | The checks every step must pass. |
| `tools/*_textures.py` (art generators), `tools/facet_preview.*`, `tools/sound_demo.*` | Make the art, the preview pictures and the offline sound checks. |
| `assets/textures/TEXTURE_BRIEF.md` | The spec for `.vtex` files. |
| `assets/textures/parked/` | Parked art (not loaded: the loader skips subfolders), kept in case a material returns. Retire when the owner says it won't. |
| `assets/text/test.txt` | The pseudo-language that `check_strings.py --pseudo` writes, used to test layout for any language. A developer aid: **never ship it**. |
| `CLAUDE.md`, `DESIGN.md` | Working rules; the design of record. |
| `docs/FOUNDATIONS.md`, `docs/SOP.md`, `docs/CONVENTIONS.md` | How we build. |
| `docs/DECISIONS.md`, `docs/COST_LEDGER.md`, `docs/SCOPE_MOSCOW.xlsx` | Decisions, costs and scope: the project's memory. |
| `docs/SOUND_PALETTE.md` | The spec the world sound implements. |
| `docs/ROADMAP.md`, `docs/GOALS.md`, `docs/PROTOTYPE_OUTLINE.md`, `docs/PROJECT_NOTES.md` | Direction and the project write-up. |
| `docs/research/` (notes, `BIBLIOGRAPHY.md`, `OVERVIEW.md`) | Why the systems are built as they are, and where each idea came from (D66–D68). Keep while the systems they explain exist; a note whose system is gone retires with it. |
| `docs/MANIFEST.md` | This log. |

## Retire: scaffolding (delete when spent)

| Item | Spent when |
|---|---|
| `test` (root, one empty line, imported with the Voxistics archive) | **Already.** Nothing refers to it. Can go in the next cleanup step. |
| `reference/` | Its own rule (CLAUDE.md, `reference/README.md`): each item goes when nothing is left to take from it. |
| `incoming/grok/` | Each delivery, once appraised and either adopted (moved into `tools/`, `tests/` or `assets/`) or declined. The appraisal in `docs/grok_reviews/` keeps the record. |
| `docs/agent_packs/` | When the helper task list is finished; the packs are generated from `AGENT_TASKS.md` and can be regenerated. |
| `docs/AGENT_TASKS.md`, `docs/grok_reviews/` | When helper work stops. Then keep only the lessons, as a short line in `DECISIONS.md` or `PROJECT_NOTES.md`. |
| `docs/PLAN_TUESDAY.md`, `docs/PLAN_LOOK.md`, `docs/M0_PLAN.md`, `docs/M1_PLAN.md` | When their steps are built and logged: the decisions and costs are in `DECISIONS.md` and `COST_LEDGER.md` by then. |
| `docs/AUDIT_*.md` | When its fixes are done, or superseded by the next audit. |
| `docs/ENGINE_REVIEW.md`, `docs/FORECASTS.md`, `docs/THEORIES.md`, `docs/REFLECTIONS.md` | When their content has been used or folded into the design. Owner's call; they may be worth keeping as history. |
| `docs/research/PAPER_IDEAS.md`, `docs/research/WANTED.md` | When acted on or dropped. |
| `docs/pictures/` (6.8 MB, the largest folder) | Each set once its step is confirmed in game and a newer set shows the same view. Keep the latest set per view as the before-and-after record. |
| `docs/README.md` | Keep while `docs/` has many files; update as they retire. |

## Rules

1. **A new file gets its line here** in the same step it's created (SOP checklist).
2. **Retiring is a step like any other:** list what goes, check nothing refers to it (search the repository), delete it, and log it in `DECISIONS.md`. Git keeps everything retrievable.
3. **A release is assembled from the Ship list only.** A packaging script that copies exactly that list (and refuses `test.txt` and `parked/`) would make this automatic. That's code for later: a `W` row when a first release is planned.
