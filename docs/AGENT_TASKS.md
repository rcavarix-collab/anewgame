# Tasks for a helper agent (Grok, fast mode)

Work another agent can do on walkgrid between Claude sessions. Written for a **chat assistant in fast mode**: it can read what's pasted to it and write text, but it can't open the repository, compile, run scripts, render pictures or play the game. Claude reviews, applies and checks everything at the next session.

**Copy and paste only, no attachments.** Every task has a ready-made pack in `docs/agent_packs/`: the Brief, the task and the excerpts of code and docs it needs, as text in one to four parts, each short enough for one message on a free plan. The agent answers in text too.

So every task here:
- is small enough for one conversation and a few pastes;
- has its pack, holding only the excerpts it needs;
- names exactly what to hand back, in what form;
- says what "done" means;
- ends with a fixed self-review.

Nothing the agent writes goes into the game until Claude has checked it.

---

## For the owner: how to run a task

1. Start a fresh chat for each task (fast mode keeps a short memory).
2. On GitHub, branch `claude/epic-rubin-6pctr0`, open `docs/agent_packs/README.md`: it lists each task's files in order. Free chat plans cap a message's length, so most tasks come in 2–4 parts of at most 12,000 characters (`R1-1.md`, `R1-2.md`, ...); a short task is one file (`T2.md`). For each part in turn: open it, press the **Copy raw file** button (two overlapping squares, top right of the file), paste it into the chat and send. The agent answers "got part 1 of 3" and so on, and starts the task when it gets the last part. If a paste is refused or cut short, tell Claude: the part size is one number in the tool.
3. If the answer stops part way, type `continue`. Repeat until it writes `END OF DELIVERY`.
4. Copy the whole answer (the copy button under it; for several parts, copy each in turn). On GitHub, branch `agent/grok` (create it from `claude/epic-rubin-6pctr0` the first time), choose **Add file, Create new file**, name it `incoming/grok/<task>.md` (for example `incoming/grok/R1.md`), paste, and commit. Several parts go one after another in the same file. Or simply paste it to Claude next session.
5. Add a line to the Log at the bottom of this file, or tell Claude which tasks you ran.

Don't paste the agent's code into the game yourself. Claude applies it next session and runs the checks, so a mistake can't break your build.

**Keeping packs fresh (Claude).** The packs are made by `python3 tools/make_agent_packs.py` from this file and the current code. Claude reruns it at every milestone, whenever it appends a task, and before the owner's away days, and commits the result. Each pack says the date it was made. A pack made before code changed may have stale excerpts: Claude's apply step (below) rejects an edit whose Find text no longer matches, so nothing breaks, the task is just redone.

---

## The Brief (paste this first, every time)

> You're helping build **walkgrid**, a first-person game on a faceted world (angular ground over a grid of one-block cells), written in C++17 with Direct3D 11 for Windows. The owner is Ryan; the lead developer is another AI (Claude), who will review and apply everything you produce at its next session. You can't compile or run anything, so precision matters more than speed.
>
> **Hard rules, never broken:**
> - no network code of any kind;
> - no recorded or sampled audio (sound is synthesized only);
> - no brand, product or company names, or anyone's art, music or text, in anything the player sees;
> - every word the player sees comes from `assets/text/en.txt` through `Str("key")` or `StrF("key", {...})`, never written into code;
> - no reading the player's machine;
> - never copy another game's look.
>
> **How to hand work back.** Everything goes in your answer, as text: the owner copies it into one file, and Claude splits it. So:
> - Start with `REPORT.md` (template below), then each other file you produce.
> - Put each file under a heading line `=== FILE: incoming/grok/<task number>-<name>/<file name> ===`, then its content as one complete fenced block.
> - If you're running out of room, stop at the end of a file and write `CONTINUED: say "continue"`. When the owner says continue, carry on with the next file; never repeat or restart one.
> - End your last part with the line `END OF DELIVERY`.
> - **Code changes to existing files are never whole rewritten files.** Write them as numbered edits in a file called `EDITS.md`, each in exactly this form:
>
>   ````
>   ### Edit 1: <one-line purpose>
>   File: <path, e.g. hud.cpp>
>   Find (copied exactly from the file I gave you; at least 3 whole lines; appears only once in the file):
>   ```cpp
>   <the lines, character for character, including indentation>
>   ```
>   Replace with:
>   ```cpp
>   <the new lines>
>   ```
>   ````
>
>   Copy the Find text from the excerpt you were given; never retype it from memory. It can only come from an excerpt in this message. Change nothing outside the edits: no reformatting, no renaming, no "while I'm here" fixes. Keep the file's style: its naming, its comment density, a header comment on any new file saying what it is.
> - **New files** (a new test function, a script, a document) are complete files.
> - If something the task needs isn't in what you were given, don't guess: list it under "Needed but not given" in the report and stop that part.
> - The code and docs below are **excerpts**: a file may hold more than you see. Say "not in my excerpt" rather than assuming something is missing from the game.
>
> **Code rules:**
> - C++17 only.
> - Use only functions, types and constants you can see in the excerpts you were given. List any you assumed exist.
> - Never use these words as names: `small`, `near`, `far`, `interface`, `hyper`, `pascal`, `cdecl` (Windows macros).
> - Never use `sprintf`, `strcpy`, `fopen`, `getenv`, `localtime` or similar unsafe C functions: use `snprintf` and `std::` equivalents.
> - Comment lines inside shader string code must not end in `;`.
>
> **Before you answer, review your own work once, completely**, against the task's "Done when" list and the checklist in the report template, and fix what you find. Then fill in the report honestly: "not sure" is a useful answer; a confident wrong one costs a day.
>
> **REPORT.md template:**
> ```
> # Task <number>: <name>
> ## What I produced
> <each file, one line: path, new or edits, what it's for>
> ## Done when (from the task)
> <each item: Yes / Partly / No, with where in my files>
> ## Self-review
> 1. I re-read every file I produced start to finish after writing it: Yes/No
> 2. Every Find block is copied exactly and is unique in its file: Yes/No/Not applicable
> 3. Every function, type, constant and string key I use appears in the excerpts I was given (list any that don't): ...
> 4. Hard rules and code rules above all kept: Yes/No (say which, if No)
> 5. Every claim I make about the existing code quotes it or names the file and function: Yes/No
> 6. What I changed during my review pass: ...
> 7. What I could not check (I can't compile or run anything): ...
> 8. Confidence, per file: high / medium / low, and why
> ## Needed but not given
> ## Questions for the owner or Claude
> ```

---

## The tasks

Each task lists: **Pack has** (the excerpts its pack carries; nothing to attach), **Do**, **Hand back** (exact paths), **Done when**, and any task-specific checks. Sizes: **S** fits one reply; **M** may need a follow-up "continue". The groups are ordered by usefulness, and within each group the first tasks matter most.

### Research notes (documents only: safest, and most useful for the next Claude session)

Each note should run 800–2,000 words. Use short sections. For every option, give:
- what it is;
- where it costs (when a chunk is built, or every frame; name the system from COST_LEDGER);
- what the player would see;
- which files it would touch;
- risks.

End with a recommendation and the open questions for the owner. No code beyond short illustrative snippets.

**R1. Cheap realistic lighting (M)**
- Pack has: `docs/PROJECT_NOTES.md`, `DESIGN.md` 4.8, 4.9, 23.3, 23.4 and 23.5a; `ComputeAtmosphere` from `sky.h`; the world shader's lighting lines from `render.cpp`; `docs/COST_LEDGER.md`.
- Do: the owner wants realistic light at no frame cost, not heavy post-processing. The problem today is a "golf ball" look, a dimpled self-shadowing on the lumpy ground. Weigh:
  - lighting baked into the ground when a chunk is built (each ground vertex already carries "openness" and "sky" values: see groundmesh and 23.4);
  - whether screen-space AO should go;
  - how small lumps and sun shadows interact at a low sun;
  - sky and sun colour models;
  - aerial perspective;
  - anything else cheap.
- Hand back: `incoming/grok/R1-lighting/lighting.md`, `REPORT.md`.
- Done when: at least 5 options are costed as above, one is recommended, and the golf-ball cause is discussed with the evidence it rests on.

**R2. Weather (M)**
- Pack has: `docs/PROJECT_NOTES.md` section 1; `DESIGN.md` Part XIII; `sky.h`; `docs/SOUND_PALETTE.md` sections 1–3.
- Do: design a weather system with cheap tricks only (nothing volumetric: owner). The knobs that exist:
  - cloud cover per layer (`CIRRUS_COVER`, `CUMULUS_COVER`; `fClouds` in the shaders);
  - the jet stream (`JetStreamAngle`);
  - fog distance, light colour and exposure (`ComputeAtmosphere`);
  - the sound palette's axes and ambient scene.

  Propose:
  - a small weather state (its fields);
  - how it moves between kinds (transitions over game time);
  - 4–6 kinds as data (a table of knob values);
  - rain or snow as screen-space tricks;
  - their sound through the synth;
  - how a new kind is added without code changes.
- Hand back: `incoming/grok/R2-weather/weather.md`, `REPORT.md`.
- Done when: every knob it uses exists in the given files (cited), the kinds are a table, and adding a kind is shown step by step.

**R3. Terracing, a nicer way (S)**
- Pack has: `facetmesh.h`, the function `CornerPos` from `facetmesh.cpp`, `terrain.h`, `docs/DECISIONS.md` rows D35, D39, D55, D62.
- Do: gentle slopes show one-cell steps as terraces. The smoothing pass (`FacetShape::terrace`, off) softens player edits too. Evaluate:
  - marking edited cells (each cell has a spare state byte);
  - smoothing driven by the terrain's height function rather than cells;
  - a terrain v3 that shapes slopes differently;
  - one more idea of its own.
- Hand back: `incoming/grok/R3-terracing/terracing.md`, `REPORT.md`.
- Done when: each option covers the chunk seams (neighbouring chunks must compute identical corners), collision (it uses the same corners) and saved worlds.

**R4. Our own look (S)**
- Pack has: `docs/PROJECT_NOTES.md` section 1, `DESIGN.md` 23.1 and 23.5a, `assets/textures/TEXTURE_BRIEF.md`. Screenshots are optional: if the owner adds one or two, the agent uses them; without them it works from the words.
- Do: walkgrid currently resembles Valheim's low-texel realism in terrain and light; the owner wants it distinct (D60). Propose concrete levers: palette, facet emphasis and edges, light colour through the day, sky character, texture style. Describe them in words and from our own screenshots, never by pointing at another game's assets. Say what makes each ours.
- Hand back: `incoming/grok/R4-look/look.md`, `REPORT.md`.
- Done when: 5–8 levers, each with what changes, where (file or system) and its cost; a short "what walkgrid should never look like" list.

**R5. Graphics presets (S)**
- Pack has: `settings.h`, `DESIGN.md` 4.8 and 23.6, `docs/COST_LEDGER.md`, `docs/FOUNDATIONS.md` section 4.
- Do: propose LOW / MEDIUM / HIGH presets as a table of the existing settings (render distance, fine detail, sun shadows, SSAO, bloom, outlines, frame limit): one for the floor machine (8 GB RAM, 2 GB card), one for the reference (GTX 1060), one above. Use only settings that exist in `settings.h`.
- Hand back: `incoming/grok/R5-presets/presets.md`, `REPORT.md`.
- Done when: every setting named exists (cited), and each choice gives its reason from the cost ledger.

**R6. Caves and terrain v3 (M)**
- Pack has: `terrain.h`, `terrain.cpp`, `DESIGN.md` 23.2 and 23.7, `docs/FOUNDATIONS.md` sections 3–5.
- Do: how caves and overhangs could be generated in a future terrain version:
  - noise approaches;
  - cost per column (it runs on a job thread);
  - how the hidden-chunk walk (23.7) benefits;
  - what the versioning rules require (old versions never change).
- Hand back: `incoming/grok/R6-caves/caves.md`, `REPORT.md`.
- Done when: 2–3 approaches are compared, and the versioning and seam requirements are addressed explicitly.

**R7. M2 options (S)**
- Pack has: `docs/GOALS.md`, `docs/ROADMAP.md`, `docs/PROJECT_NOTES.md` section 9, and the scope sheet's rows as text (made from `docs/SCOPE_MOSCOW.xlsx`).
- Do: propose 3–5 options for milestone M2. For each: what the player would get, the rough size, what it depends on, and what it risks.
- Hand back: `incoming/grok/R7-m2/m2_options.md`, `REPORT.md`.
- Done when: the options are distinct, each ties to a goal or scope row by its ID, and nothing contradicts a logged decision.

### Text

**T1. A second language (M): owner picks the language**
- Pack has: `assets/text/en.txt`, `strtable.h` (for the format rules). The owner types the language under the pack when pasting (for example `Language: French`).
- Do: translate every text. Keep every key, and every `{0}`/`{1}` slot exactly. Keep words few and plain, capitals where the language has them, `\n` where English has it. Leave `window.title` and `title.name` as the game's name.
- Hand back: `incoming/grok/T1-<language code>/<language code>.txt` (e.g. `fr.txt`), `REPORT.md`.
- Done when: the file has exactly the same keys as `en.txt`, in the same order, and slots match line by line.
- Extra self-check (put it in the report): the key count in each file, and every line whose slots differ (should be none).

**T2. English proofread (S)**
- Pack has: `assets/text/en.txt`, the "Minimal text" rule from `CLAUDE.md`.
- Do: find wording that is inconsistent (the same thing named two ways), longer than needed, unclear, or that shows a number where a feel would do. Don't edit the file.
- Hand back: `incoming/grok/T2-proofread/proofread.md` (a table: key, current text, suggested text, reason), `REPORT.md`.
- Done when: every suggestion keeps the key and its slots, and each has a reason.

### Tests (C++, for code with no graphics; Claude compiles and runs them)

Test style: tests live in `tests/tests.cpp` as `static void TestSomething()` functions, called from `main()` at the bottom. They use `CHECK(condition)` and print a short summary with `printf`. Every test pack carries the first 60 lines of `tests/tests.cpp` (the includes and the `CHECK` macro) and its `main`, so the agent copies the style.

**X1. Sky edge cases (S)**
- Pack has: `sky.h`, the two test excerpts.
- Do: a new `static void TestSkyEdges()`. Check:
  - `DiscCover`: symmetric in a sense you can justify; 0 when apart; continuous as discs slide past (check small steps change it by small amounts);
  - `moonLit`: near 0 at some time in days 0–7 and near 1 at another;
  - `JetStreamAngle`: continuous across whole-day boundaries;
  - `ComputeAtmosphere` during a total solar eclipse (find one by scanning `ComputeSky` over days 0–400): direct sunlight close to 0.
- Hand back: `incoming/grok/X1-sky-tests/TestSkyEdges.cpp` (just the function), `EDITS.md` (one edit adding the call to `main`), `REPORT.md`.
- Done when: every function and field used exists in `sky.h` (cited), and every loop is bounded (under a second of work in total).

**X2. String table edge cases (S)**
- Pack has: `strtable.h`, `strtable.cpp`, the two test excerpts.
- Do: `static void TestStringsEdges()`. Cover:
  - a key defined twice (the later wins);
  - a line without `=`;
  - a very long text;
  - `{9}` with fewer arguments (stays literal);
  - invalid UTF-8 mid-line (`DecodeUtf8` gives U+FFFD and moves on);
  - `WideToUtf8(Utf8ToWide(s)) == s` for a few strings with accents.

  Restore the table with `SetStrings({})` at the end.
- Hand back: `incoming/grok/X2-string-tests/TestStringsEdges.cpp`, `EDITS.md`, `REPORT.md`.
- Done when: as for X1.

**X3. Chunk openings shapes (S)**
- Pack has: `facetmesh.h` (the `FacetOpenings` and `FacetPairBit` parts are at the end), the two test excerpts, and the existing `TestHiddenChunks` function (it shows how a test grid is built).
- Do: `static void TestOpeningsShapes()` with an L-shaped tunnel, a vertical shaft, a sealed pocket in the middle, and a thin wall splitting the cube. Assert the expected face pairs.
- Hand back: `incoming/grok/X3-openings-tests/TestOpeningsShapes.cpp`, `EDITS.md`, `REPORT.md`.
- Done when: each shape's expected pairs are explained in a comment (which faces see which, and why).

### Small tools (Python, standalone; they never change game files)

**P1. Save file reader, `tools/dump_save.py` (S)**
- Pack has: `worldfile.h`, `worldfile.cpp` (the layout is in its header comment; the decode function is exact), `savegame.cpp` (the `DAY1` tag).
- Do: a read-only script. `python3 tools/dump_save.py slot1.sav` prints:
  - version, player position, time of day, day count;
  - generator name, version and seed;
  - the block names table;
  - chunk count, and edited cells per material;
  - whether the checksum matches.

  Standard library only. It never writes.
- Hand back: `incoming/grok/P1-dump-save/dump_save.py`, `REPORT.md`.
- Done when: every field is read in the same order and width as `DecodeSave` reads it, and the report cites the lines matched.

**P2. Docs checker, `tools/check_docs.py` (S)**
- Pack has: the top of `tools/check_strings.py` (as a style example), and the list of the repository's files.
- Do: a script that exits 1 and prints each problem when:
  - a `D<number>` mentioned in any `.md`, `.cpp` or `.h` file has no row in `docs/DECISIONS.md`;
  - a path in backticks in `DESIGN.md`, `docs/PROJECT_NOTES.md` or `docs/AGENT_TASKS.md` doesn't exist.

  Standard library only.
- Hand back: `incoming/grok/P2-check-docs/check_docs.py`, `REPORT.md`.
- Done when: it runs from any folder (paths relative to the script, as `check_strings.py` does), and it skips `reference/` and `incoming/`.

### Small features (edits to game code; Claude applies and tests them)

**F1. Budgets in the F3 overlay (M)**
- Pack has: the F3 debug block from `hud.cpp`, `profiler.h`, `docs/FOUNDATIONS.md` section 4 and `docs/COST_LEDGER.md` (for the budgets).
- Do: in the F3 overlay (the block between `// D26: debug text` and `// D26: end` in `hud.cpp`), add each row's budget beside AVG and WORST, and mark rows whose AVG is over budget with `!`. Budgets come from a small table in `hud.cpp`, one per `ProfSection`, taken from the ledger. Rows without a budget show `-`. It's debug text, so English is fine; keep it inside the markers.
- Hand back: `incoming/grok/F1-f3-budgets/EDITS.md`, `REPORT.md`.
- Done when: the `ProfSection` names match `profiler.h` exactly (cited), the table has one entry per section, and the change is only inside the debug block (plus the table just above it).

**F2. Key-binding conflicts (S)**
- Pack has: from `hud.cpp`, the `drawRowButton` helper and the Keybindings screen (it draws around `g_rebindingAction`); the Keybindings part of `game_internal.h`; `settings.h`.
- Do: in the Keybindings menu, draw a row whose input is also bound to another action in a warning colour (and that other row too). Colour only: no new words.
- Hand back: `incoming/grok/F2-key-conflicts/EDITS.md`, `REPORT.md`.
- Done when: it uses `g_keyBindings` and `ACT_COUNT` as declared in `settings.h`, costs nothing outside the Keybindings screen, and leaves every other screen untouched.

### Reviews (read and report; never fix)

For each: read the excerpts and list suspected bugs, edge cases and risks, most serious first. Each item gives the file, function and line text (quoted), what could go wrong, and a concrete scenario. Say how sure you are. Findings only: no fixes.

**V1. The sky clocks (S).** Pack has: `sky.h`. Hand back: `incoming/grok/V1-review-sky/review.md`, `REPORT.md`.
**V2. The shadow crossfade (S).** Pack has: from `render.cpp`, the shadow state around `ShadowFade`, the function `UpdateShadowMap`, and the world shader's `ShadowLit` and where it's used. Hand back: `incoming/grok/V2-review-shadows/review.md`, `REPORT.md`.
**V3. Terrain v2 (S).** Pack has: `terrain.h`, `terrain.cpp`. Focus: v1's output must be unchanged byte for byte by the v2 code paths. Hand back: `incoming/grok/V3-review-terrain/review.md`, `REPORT.md`.
**V4. Saving the day count and the game folder (S).** Pack has: `savegame.cpp`, `gamefiles.h`, `gamefiles.cpp`. Hand back: `incoming/grok/V4-review-files/review.md`, `REPORT.md`.

---

## Off limits, even when asked

Threads (`jobs.*`, audio threads), the renderer's structure (passes, resources), the save format (`worldfile.*`), changing an existing terrain version's output, sound recipes and music (`sfx_synth.cpp`, `music_synth.cpp`), editing or reversing anything in `docs/DECISIONS.md`, and anything in `reference/`.

## What Claude does with a delivery

At the next session, for each `incoming/grok/<task>.md` (or pasted answer):
1. Split it into its files by the `=== FILE:` lines. Read `REPORT.md`, especially "Needed but not given" and the confidence.
2. Apply `EDITS.md` mechanically: every Find must match exactly once, or the edit is rejected.
3. Run all the checks.
4. Review against the task's "Done when".
5. Merge into the main branch with the owner's OK, or send it back with notes.

Research notes feed the next plan the owner approves.

## Log

| Date | Task | Result (delivered / reviewed / merged / sent back) |
|---|---|---|
| | | |
