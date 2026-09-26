# Tasks for a helper agent

A list of useful work another agent can do on walkgrid while Claude isn't available (first use: late September 2026, several days until the owner's next Claude session). Everything here is chosen to be safe to do without close supervision:
- it adds substance to what's built without changing the engine's structure;
- or it's research that feeds the next Claude session;
- or it checks and tidies what's there.

Heavy or risky work (threads, the renderer's core, the save format, the terrain generator's output, sound recipes) is deliberately left out; see "Off limits" below.

**Read first, in this order:** `docs/PROJECT_NOTES.md` (what walkgrid is, the code map, plug-in points, pitfalls), `CLAUDE.md` (the rules; they bind every agent), then the `DESIGN.md` section for whatever the task touches.

---

## How to work

1. **One task at a time, on its own branch:** `agent/<short-name>` (for example `agent/translation-fr`), branched from `claude/epic-rubin-6pctr0`. Never commit to that branch directly: the owner or Claude reviews and merges.
2. **Ask the owner before starting** anything marked *(owner's OK)*. Research tasks and checks need no OK, since they produce documents, not changes to the game.
3. **Run the checks** if you can run code (Linux, with `g++`, `mingw-w64`, `glslang-tools`, Python 3 with Pillow and openpyxl):
   - `bash tests/run.sh` (native tests, plus the MSVC, layer and string checks);
   - `python3 tools/check_shaders.py` (13 variants, all ok; the count must stay 13);
   - `sh tools/check_msvc.sh`;
   - `sh tools/check_mingw.sh`;
   - `python3 tools/check_layers.py`.

   All must pass before you hand anything over. If you can't run code, say so plainly in the report, and hand over the change as files or a patch, marked "unverified".
4. **Match the house style:** every source file opens with a header saying what it is, its cost, its layer and its DESIGN.md section. Comments explain what a non-obvious line guards against. Match the surrounding code, and write plain words in docs and comments.
5. **Docs are part of done:** if a task changes behaviour, update the `DESIGN.md` section, add a `docs/DECISIONS.md` row marked "Agent (proposed)" if a choice was made, and a `docs/COST_LEDGER.md` line if it costs anything per frame.
6. **Report every task** with this template, as a file `docs/agent_reports/<date>-<task>.md` on the branch:
   ```
   Task: <name from this list>
   What changed: <files, one line each: new / changed>
   What you'll see: <in the game, or "nothing: tests/docs only">
   Checked: <which checks ran and passed; or "could not run code">
   Not checked: <what only the owner's Windows build can show>
   Questions for the owner or Claude: <if any>
   ```

## Off limits (leave these for a Claude session)

- **Threading:** `jobs.*`, the audio threads in `audio.cpp`, and anything that shares data between threads.
- **The renderer's structure:** passes, shadow maps, resource creation, the world shader's lighting model. Small, local shader changes only when a task below says so.
- **The save format** (`worldfile.*`), and saving and loading in general.
- **The terrain generator's output** (`terrain.cpp`): changing an existing version corrupts saved worlds (see PROJECT_NOTES 8).
- **The sound palette's recipes** (`sfx_synth.cpp`), the music (`music_synth.cpp`) and the harmony rules.
- **Any `docs/DECISIONS.md` entry:** never edit or reverse one; add a new row marked proposed.
- **Anything in `reference/`:** ideas only, never compiled.
- **Network use of any kind, telemetry, reading the player's machine, recorded audio, and brand or product names.** These are hard rules (PROJECT_NOTES 1).

---

## The list

Sizes: **S** is an hour or two, **M** is half a day, **L** is a day or more. The groups are in rough order of usefulness, and within each group the first items matter most.

### A. Checks and tidying (no OK needed)

1. **Docs drift audit (M).** Read `DESIGN.md` Parts XIII and XXIII and section 4.6–4.9 against the code they describe (`sky.h`, `render.cpp` shaders, `facetmesh.*`, `groundmesh.*`, `terrain.*`, `collide.*`). List every place where the text and the code disagree (numbers, behaviour, file names). Deliverable: a report with each mismatch and the suggested wording. Don't rewrite DESIGN.md wholesale.
2. **Header audit (S).** Check that every source file in `tools/layers.txt` opens with the standard header (what, cost, layer, DESIGN section), and that `walkgrid.vcxproj` compiles exactly the `.cpp` files that `layers.txt` lists. Deliverable: the list of gaps, and fixed headers (comments only) on a branch.
3. **A docs checker, `tools/check_docs.py` (M).** A new script that fails when:
   - a `D<n>` referenced in any doc or code comment doesn't exist in DECISIONS.md;
   - a file named in PROJECT_NOTES or DESIGN doesn't exist;
   - a string key used in code is missing from `assets/text/en.txt` (`check_strings.py` already does this; reuse, don't duplicate).

   Add it to `tests/run.sh` only with the owner's OK.
4. **Old names in comments (S).** Search comments for leftovers from Voxistics that no longer describe walkgrid (cubes, the old block roster, The Line, essence, removed shapes). List them with a suggested replacement; comments only.
5. **Theories and forecasts from real data (S).** The owner's two Ctrl+F3 reports are summarised in `docs/COST_LEDGER.md` and PROJECT_NOTES 4. Propose status updates for `docs/THEORIES.md` (T4, T5 especially) and `docs/FORECASTS.md` (F1, F4, F5, F7, F11, F19) based on them, as a report. The owner decides.
6. **Scope sheet tidy (S).** Suggest status changes for rows that the work has overtaken. For example, W032 (the release performance check) happened on 2026-09-26. Edit only with openpyxl, keeping the formatting, and only rows the owner OKs.

### B. Tests for pure code (no OK needed; no game changes)

The native tests (`tests/tests.cpp`, ~740 checks) run without Windows. Each of these adds checks for code that has few today. Keep each test's printout short and its checks meaningful: never test that a constant equals itself.

7. **Sky at the edges (S):** `sky.h`.
   - `ComputeAtmosphere` during a total solar eclipse: sun colour near zero, stars visible.
   - `moonLit` exactly 0 and 1 at new and full moon.
   - `DiscCover` symmetric cases and continuity as the discs slide past each other.
   - `JetStreamAngle` continuity across day boundaries.
8. **Save round trip with the day count (S):** encode a world with a day count (the `DAY1` tag in the game's section, `savegame.cpp`) and decode it; a section without the tag gives day 0. The tag parsing may need moving into a pure helper first: that's a small refactor, so propose it in the report.
9. **String table edge cases (S):** `strtable.cpp`.
   - A key defined twice (the later wins).
   - Very long lines.
   - Every `{n}` slot beyond the arguments stays literal.
   - Invalid UTF-8 in the middle of a line.
   - `test.txt` round-trips through `--pseudo`.
10. **Terrain v2 invariants over many seeds (M):** for seeds 1–50, v2 surface heights equal v1's, and the tops differ only where v1 had bare stone or slate at a mesa's foot (the test does this for seed 7 only). Print a one-line summary.
11. **Settings clamps (S):** every value read from a hand-edited `settings.cfg` is clamped (`settings.cpp`); test garbage, negative and huge values. The function reads a real file, so propose a pure `ParseSettings(text)` split in the report first.
12. **Facet openings (S):** more shapes for `FacetOpenings` (an L-shaped tunnel, a vertical shaft, a sealed pocket) with the expected face pairs.

### C. Tools (no OK needed for new tools; OK before wiring into the checks)

13. **Save file dumper, `tools/dump_save.py` (M).** Reads a `slotN.sav` (format in the header of `worldfile.cpp`) and prints the header, generator, day count, number of chunks and edited cells. It reads only and never writes; it's for debugging a save the owner sends.
14. **Before/after picture helper (S).** Wrap `tools/facet_preview.sh` in a script that renders the same view twice with different options (for example `SOFTEN 0` against `0.55`) and saves a stacked comparison JPEG, as was done by hand for `docs/pictures/soft_look/`.
15. **Sound clip sets (S).** Extend `tools/sound_demo.cpp` with a `palette OUTDIR` mode: every sound solo, once, at each of the four chord times, for the owner to listen through. Use the existing sounds only, and don't change recipes.

### D. Content through the plug-in points *(owner's OK)*

16. **A second language (M).** The owner picks the language. Copy `assets/text/en.txt` to `<lang>.txt`, translate every text, keep every `{n}` slot, keep words few and plain, and all in capitals where the language has them. Check with `python3 tools/check_strings.py`. If the language needs letters Consolas lacks, name a suitable installed Windows font in the `font` key and say so.
17. **Texture candidates (L).** From the ideas in `reference/seeds/newpatterns.cpp` (100 stone and soil patterns; ideas only, re-authored, never copied as code), make 8–12 candidate `.vtex` textures in the style of `tools/natural_textures.py`, following `assets/textures/TEXTURE_BRIEF.md`. Render contact sheets with `sh tools/facet_preview.sh OUTDIR sheets`. Deliverable: the sheets and the files, not wired into any material; the owner picks.
18. **Material pairing sheets for v2 (S).** Re-render `docs/pictures/m1_3/pairs.jpg` and the hills pictures with the current build's look (the soft look, v2 terrain, fog), into a new `docs/pictures/<date>/` folder, so the review pictures are current.

### E. Small features *(owner's OK first; each touches few files)*

19. **Budgets in F3 (S–M, scope W011).** Show each profiler row's budget beside its measured time in the F3 overlay (budgets in `docs/FOUNDATIONS.md` 4 and `docs/COST_LEDGER.md`), and mark rows over budget. It's debug text, so keep it inside the `// D26: debug text` markers in `hud.cpp`.
20. **Key-binding conflicts (S).** In the Keybindings menu, rebinding an input already used by another action should show both rows marked. No new words if possible (a colour or a mark); otherwise the words go in `en.txt`.
21. **Screenshot without the interface (S).** A second key, or holding Shift with F2, takes the screenshot without the HUD and F3 overlay. Follow how F2 works in `game.cpp` and `render.cpp` (`ReadBackbuffer`), and don't add a pass.
22. **Graphics presets, proposal first (S, then M).** Propose LOW / MEDIUM / HIGH presets mapping the existing settings (render distance, fine detail, sun shadows, SSAO, bloom, outlines) for the 2 GB-card floor and the owner's reference machine. Build them only after the owner approves the mapping.

### F. Research and design notes (no OK needed; documents only)

Write each as `docs/research/<topic>.md`: what the options are, what each costs (at build time vs per frame), what it would look like, which files it would touch, and a recommendation. No code. These are what the next Claude session builds from.

23. **Cheap realistic lighting.** The owner wants realistic light at no frame cost, not heavy post-processing. Present problem: a "golf ball" look (dimpled self-shadowing on lumpy ground). Options to weigh:
    - light baked into the ground at chunk build time (per-corner openness and sky view already exist in the mesher);
    - better analytic sky and sun colour;
    - aerial perspective;
    - whether screen-space AO should go;
    - how the fine lumps interact with sun shadows at a low sun.
24. **Weather.** Kinds of weather and how they'd be orchestrated, with cheap tricks only (nothing volumetric, owner). The knobs that already exist:
    - cloud cover per layer (`CIRRUS_COVER`, `CUMULUS_COVER`, and `fClouds` in the shaders);
    - the jet stream (`JetStreamAngle`);
    - fog distance, light colour and exposure (`ComputeAtmosphere`);
    - the sound palette's axes and ambient scene.

    Propose a small weather state that sets them with transitions, and a way for new kinds to plug in as data. Include rain or snow as screen-space tricks, and their sound (synth only).
25. **Terracing, a nicer way.** Gentle slopes show one-cell steps as terraces. A smoothing pass exists (`FacetShape::terrace`, off, D62) but softens player edits too. Evaluate:
    - marking edited cells (the per-cell state byte);
    - shaping at terrain generation (a v3);
    - smoothing driven by the generator's height function instead of the cells;
    - anything else, with trade-offs.
26. **Our own look (D60).** walkgrid resembles Valheim's low-texel realism in terrain and light; the owner wants it distinct. Describe, in words and by pointing at our own preview pictures (never at another game's assets), concrete levers:
    - palette, facet emphasis and edge treatment;
    - sky character, light colour through the day;
    - texture style.

    Say what makes each ours. Proposals only.
27. **Caves and terrain v3.** How caves and overhangs could be generated for a future terrain version:
    - noise approaches;
    - the cost per column on a job thread;
    - what the visibility walk (hidden chunks) gains underground;
    - what the save and version rules require.
28. **M2 candidates.** From `docs/GOALS.md`, `docs/ROADMAP.md`, the scope sheet and the play-test notes, propose what M2 could be (3–5 options, each with its value and size), for the owner to choose from.

### G. Review (no OK needed)

29. **Code review of the post-play-test work.** Read the commits from `git log` since "Game folder back in the Documents" (D45) through the latest, and list suspected bugs, edge cases and risks with file and line, most serious first. Don't fix them; a report only. Pay attention to:
    - the shadow tween (three maps, crossfade timing);
    - the moon and eclipse maths;
    - cloud drift over long play;
    - the day count across saves and scrubbing;
    - the new footstep ground lookup;
    - terrain v2.
30. **Performance guesses to check on the owner's machine.** From the code, list what might cost more than expected at render distance 8 and 1080p with fine detail everywhere, and exactly what to look at in F3 or a Ctrl+F3 report to confirm or rule each out.

---

## When you finish a task

- Push the branch, write the report (template above), and add a one-line entry to the log below. Include the date, task number, branch and a one-line result.
- Don't start a task that depends on an unanswered question. Pick another.

## Log

| Date | Task | Branch | Result |
|---|---|---|---|
| | | | |
