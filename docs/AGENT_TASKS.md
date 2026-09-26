# Tasks for a helper agent (Grok, fast mode)

Work another agent can do on walkgrid between Claude sessions. Written for a **chat assistant in fast mode** that sees only what's pasted to it.

**No files, only what the task needs** (owner, 2026-09-26: "we can let grok see chunks of code, but I'm not going to give it piles of files"):
- Each task carries the facts it needs, written out in plain words by Claude: names of the knobs, what they do, their ranges and limits.
- Where a format matters (how a test is written, a function's signature, a file layout), the task includes a few lines showing it. Never a whole file.
- The one exception is T1 and T2, which carry the player's own words (`assets/text/en.txt`), because those words are what the task works on.

**Each task is one paste**, a file in `docs/agent_packs/` (for example `docs/agent_packs/R1.md`). It holds the Brief and the task. Its name is at the top and the bottom, so tasks can't be mixed up. The agent answers in text.

Nothing the agent writes goes into the game until Claude has checked it against the real code.

---

## For the owner: how to run a task

1. Start a **new private chat** for every task (Grok's private or temporary chat, so it doesn't draw on its memory of earlier chats). Never reuse a chat that had another task in it: the agent mixes them up. Its first R1 run, in a fresh chat, quoted task R2's inputs, which were never in the R1 pack: it made them up or drew them from its own memory.
2. On GitHub, branch `claude/epic-rubin-6pctr0`, open `docs/agent_packs/<task>.md` (the list is `docs/agent_packs/README.md`). Press **Copy raw file** (two overlapping squares, top right of the file), paste it into the chat and send.
3. If its answer stops part way, type `continue`, until it writes `END OF DELIVERY`.
4. Copy the whole answer (the copy button under it; for several parts, each in turn). On GitHub, branch `agent/grok` (create it from `claude/epic-rubin-6pctr0` the first time), choose **Add file, Create new file**, name it `incoming/grok/<task>.md` (for example `incoming/grok/R1.md`), paste and commit. Or paste it to Claude next session.
5. Add a line to the Log at the bottom of this file, or tell Claude which tasks you ran.

If the agent refuses, stalls, or says it's missing files, reply: `Everything you need is in my first message. Where you need a detail that isn't there, make a sensible assumption, label it ASSUMPTION, and carry on.` If it still won't, skip the task and tell Claude.

Don't paste the agent's code into the game yourself. Claude applies it next session and runs the checks, so a mistake can't break your build.

**Keeping packs fresh (Claude).** `python3 tools/make_agent_packs.py` makes the packs from this file. Claude reruns it whenever this file changes and commits the result. When the code changes, Claude updates the facts in the affected tasks.

---

## The Brief (goes at the top of every pack)

> You're helping build **walkgrid**, a first-person game for Windows (C++17, Direct3D 11). The world is faceted: angular ground built over a grid of one-block cells, lit by a sun, moon and sky that move on their own clocks. The owner is Ryan; the lead developer is another AI (Claude), who checks and applies everything you write at its next session.
>
> **This message is complete, and it's about one task only: the one named at the top.** You won't get any files. The task tells you the facts you need about the game, with a few lines of code where a format matters. Trust them.
>
> **Never stop or refuse for lack of information.** Where you'd need a detail that isn't here, pick the most sensible assumption, write it in your text as `ASSUMPTION: ...`, and carry on. Claude checks every assumption against the real code, so a labelled guess is useful and an unlabelled one is harmful.
>
> **Hard rules, never broken:**
> - no network use of any kind;
> - no recorded or sampled audio (all sound is synthesized);
> - no brand, product or company names, and nobody else's art, music, text or characters;
> - never copy another game's look;
> - everything must run well on a six-year-old PC (GTX 1060 class) at 60 frames a second; heavy effects need a setting to turn them off;
> - the player sees few words, and every word the player sees comes from a text file keyed by name, so the game can be translated;
> - no numbers shown to the player where a feel or a band will do.
>
> **How to answer.** Everything goes in your reply, as text. The owner copies it into one file and Claude splits it:
> 1. First, a short report (template below), under the line `=== FILE: REPORT.md ===`.
> 2. Then each file the task asks for, under its own line `=== FILE: <name the task gives> ===`, with the content right after it (code in one fenced block).
> 3. If you run out of room, stop at the end of a file and write `CONTINUED: say "continue"`. When told to continue, go on with the next file; never repeat or restart one.
> 4. End with the line `END OF DELIVERY`.
>
> **Before you answer, check your work once against the task's "Done when" list** and fix what you find.
>
> **Report template:**
> ```
> # Task <ID>: <name>
> ## Files
> <each file: name, one line on what it is>
> ## Done when
> <each item from the task: Yes / Partly / No, and where>
> ## Assumptions
> <every ASSUMPTION you made, one line each, or "none">
> ## Self-check
> 1. Hard rules kept: Yes/No
> 2. Everything I say about the game comes from the task's facts or is labelled ASSUMPTION: Yes/No
> 3. What I'm least sure of: ...
> ## Questions for the owner or Claude
> ```

---

## The tasks

Each task has: **Facts** (what's true in the game today, for the agent to build on), **Do**, **Hand back**, **Done when**. The groups are ordered by usefulness.

### Research notes (documents only)

Each note runs 800–2,000 words in short sections. For every option, give:
- what it is;
- when it costs: when a piece of ground is built (once, on a background thread) or every frame;
- what the player would see;
- what could go wrong.

End with a recommendation and the open questions for the owner. Short illustrative snippets only; no full code.

**R1. Cheap realistic lighting (M)**
- Facts:
  - The ground is a mesh of flat triangles (facets), built for each 16×16×16 chunk of cells on a background thread whenever the chunk changes.
  - Each ground corner stores two baked values:
    - "openness": how many of the 64 cells around it are solid;
    - "sky view": how much sky it sees, from the steepest rise within 8 blocks in 8 directions.
  - The ground shader lights each pixel as ambient + direct:
    - Ambient is sky light from above (blue by day) blended with a dim warm bounce from below by how much the surface faces up, plus a fill light from the sun: 8% of the sun from the whole sky, more on faces turned toward the sun, and a warm bounce onto faces tilting sideways or down.
    - The ambient is multiplied by openness and by (0.25 + 0.75 × sky view).
    - Direct light is the sun colour × √(facing) × shadow, with a moon term at night.
    - The facet's lighting normal is 55% of the way from its flat normal toward the smooth one, so neighbouring facets differ less.
  - Sun shadows come from a 2048² shadow map over up to ±112 blocks, softened by 9 filtered samples. When the sun moves, the map is redrawn over four frames and crossfaded, so shadows glide.
  - Low clouds cast soft moving shadows.
  - Colour is lit in linear light and finished with a filmic tone curve.
  - Distant ground fades into the sky colour.
  - There is an optional screen-space ambient occlusion (SSAO) pass, now off by default: on the lumpy ground it made a dimpled "golf ball" look.
  - Per-frame graphics budget: 5 ms for the world, 1 ms for post effects; today the world takes about 1–3 ms.
  - The owner wants realistic light at no frame cost, and dislikes heavy post-processed looks.
- Do: propose at least 5 cheap ways to make the light more convincing. Consider:
  - more baked at build time (for example bent normals, or a longer horizon);
  - better sky and sun colours through the day;
  - aerial perspective;
  - how small lumps and low sun interact;
  - anything else cheap.
- Hand back: `lighting.md`.
- Done when:
  - at least 5 options, each with when it costs and what it looks like;
  - one recommended option, and a first step Claude could take.

**R2. Weather (M)**
- Facts, the knobs that exist today:
  - two cloud layers:
    - thin high streaks, cover 0..1 (default 0.55), stretched along the wind;
    - soft low clouds, cover 0..1 (default 0.28) at 220 blocks up, which cast soft shadows on the ground;
    - both are drawn in the sky shader from a few per-frame numbers: no extra passes;
  - a high wind, the "jet stream": a direction that wanders over a few game hours and, every few game days, swings up to about 60°; the clouds drift with it;
  - per-frame light values:
    - sun colour and strength;
    - sky colour at the top and at the horizon;
    - ambient light from above and from below;
    - exposure;
    - fog start and end distance (fog fades distant ground into the sky colour);
  - the day clock: one game day is one real hour; the sun is up for 50 minutes of it; the moon and stars keep their own clocks and eclipses happen;
  - sound: everything is synthesized. A world-sound system reads the surroundings into three slow-moving axes (organic ↔ mechanical, negative ↔ positive, calm ↔ active) and an ambient scene (plants, water, open sky, enclosed or deep). Every sound's character follows the axes, and all pitches come from the music's harmony. The music is one hour long, following the day in six sections.
  - The owner rules out anything costly such as volumetric clouds.
- Do: design a weather system:
  - a small weather state (its fields);
  - how it moves between kinds over game time;
  - 4–6 kinds as a data table of knob values;
  - rain and snow as cheap screen-space tricks;
  - their sounds as synth ideas that fit the three axes;
  - how a new kind is added as data, without code changes.
- Hand back: `weather.md`.
- Done when:
  - the kinds are a table using only the knobs above (anything new is marked ASSUMPTION: new knob);
  - adding a kind is shown step by step.

**R3. Terracing, a nicer way (S)**
- Facts:
  - The ground is cells one block wide. The mesher places each corner where the cells around it meet, eases it halfway toward a smooth surface, and jitters it a little from a seed.
  - On gentle slopes, one-cell steps show as terraces.
  - A smoothing pass exists but is off: it also softens the player's own one-cell digging and building, which must stay crisp.
  - Neighbouring chunks must compute exactly the same corner positions, or cracks open.
  - Walking and collision use the same corners as drawing.
  - Each cell has a spare state byte.
  - Terrain comes from a pure height function of (seed, x, z). Each version of it is frozen forever, so old worlds never change; a new shape needs a new version.
- Do: evaluate:
  - marking edited cells (so smoothing skips them);
  - smoothing driven by the terrain's height function instead of by cells;
  - a new terrain version that shapes gentle slopes differently;
  - one idea of your own.
- Hand back: `terracing.md`.
- Done when: each option addresses seams between chunks, collision, and existing saved worlds.

**R4. Our own look (S)**
- Facts:
  - The ground is faceted (angular triangles over a cell grid), with 32-pixel textures projected from the world, crisp near the player.
  - Materials: meadow grass, dry turf, moss, dirt, loam, clay, sand, gravel, stone, slate, layered sandstone, snow.
  - Plateaus with sandstone cliffs, rolling hills.
  - The sky has a real sun, moon and stars, thin high cloud streaks and soft low clouds.
  - The owner feels it currently resembles another well-known game's low-texel realism, and wants walkgrid to be distinctly its own. The faceted ground is our signature.
- Do: propose concrete levers:
  - palette;
  - facet emphasis and edges;
  - light colour through the day;
  - sky character;
  - texture style.

  Describe each in words. Never point at another game's assets.
- Hand back: `look.md`.
- Done when:
  - 5–8 levers, each with what changes, where (ground, sky, textures, light) and its rough cost;
  - a short "walkgrid should never look like" list.

**R5. Graphics presets (S)**
- Facts, the graphics settings that exist:
  - render distance, in chunks of 16 blocks;
  - fine detail, levels 0–4 (4, the default, is fine detail everywhere; lower levels keep it closer to the player);
  - sun shadows on/off;
  - SSAO on/off (off by default);
  - bloom on/off;
  - outlines on/off;
  - frame-rate cap 30–200;
  - vsync on/off;
  - field of view.

  Budgets per frame:
  - main thread 16.7 ms at the 60 fps floor;
  - graphics card: world 5 ms, shadows 1.5 ms, post effects 1 ms.

  Measured on the reference PC (GTX 1060 3 GB) at 1080p, render distance 3: world 3.1 ms, shadows 0.2 ms, post effects 0.7 ms. The floor machine is 8 GB RAM and a 2 GB graphics card.
- Do: propose LOW / MEDIUM / HIGH presets as a table of these settings: one for the floor machine, one for the reference, one above. Include a short rule for which preset to pick the first time the game runs, based only on video memory.
- Hand back: `presets.md`.
- Done when: only the settings above are used, and every choice gives its reason.

**R6. Caves (M)**
- Facts:
  - Terrain today is a height map: a pure function of (seed, x, z), made column by column (16 × 16 blocks, full height) on a background thread, at about 0.05 ms a column.
  - Each version of the terrain is frozen forever (old worlds never change), so caves mean a new version.
  - The renderer skips chunks the camera can't see into: it walks from the camera's chunk through the openings between chunks, and a chunk that is solid all through blocks the walk.
  - The ground mesh is built per 16³ chunk.
- Do: compare 2–3 ways to generate caves and overhangs. Cover:
  - the noise approach;
  - cost per column;
  - how caves interact with the chunk-skipping walk;
  - what versioning requires.
- Hand back: `caves.md`.
- Done when: the approaches are compared on the same points, with a recommendation.

**R7. Next milestone options (S)**
- Facts:
  - Built (milestone M1, the prototype):
    - faceted ground with 12 materials;
    - walking, digging and building on facets;
    - terrain with hills and sandstone mesas;
    - sun, moon, stars and eclipses;
    - two cloud layers and a changing jet stream;
    - shadows that glide;
    - synthesized music following the day, and footsteps per material;
    - saves;
    - text ready for translation;
    - a profiler.
  - Not yet: anything to do, own, make or find beyond placing blocks; weather; caves; props and building pieces (designed, not built); plants.
- Do: propose 3–5 options for milestone M2. For each:
  - what the player gets;
  - rough size;
  - what it depends on;
  - what it risks.

  Keep them distinct.
- Hand back: `m2_options.md`.
- Done when: each option says why it makes the game more fun to play, not only bigger.

### Text (these carry the player's words, `en.txt`)

**T1. A second language (M): the owner picks it**
- Facts: the pack carries `en.txt`, one `key = text` per line; `#` starts a comment; `{0}` and `{1}` are slots filled by the game; `\n` is a line break. The owner writes the language under the pack when pasting (for example `Language: French`).
- Do:
  - Translate every text.
  - Keep every key, in the same order, and every slot exactly.
  - Keep words few and plain, capitals where the language has them, and `\n` where English has it.
  - Leave `window.title` and `title.name` as the game's name.
- Hand back: `<language code>.txt` (for example `fr.txt`).
- Done when: the same keys as `en.txt`, in the same order, and slots matching line by line. Give the key count in the report.

**T2. English proofread (S)**
- Facts: the pack carries `en.txt` (format as in T1). The rule: show, don't tell; few, plain words; no numbers where a feel will do.
- Do: find wording that is inconsistent (the same thing named two ways), longer than needed, unclear, or showing a number where a feel would do. Don't rewrite the file.
- Hand back: `proofread.md`, a table: key, current text, suggested text, reason.
- Done when: every suggestion keeps the key and its slots, and has a reason.

### Code from a spec (Claude compiles, tests and wires it in)

**P1. Save file reader, `dump_save.py` (S)**
- Facts, the save file layout (little-endian):
  - `u32` magic, the bytes `W G R D` in file order; `u32` version (1);
  - player: `f32` x, y, z, yaw, pitch; `i32` hotbar slot; `f32` time of day (seconds, 0–3600);
  - generator: `str` name, `u32` version, `u64` seed;
  - `u32` name count, then that many `str` block names;
  - `u32` chunk count, then per chunk:
    - `i32` cx, cy, cz; `u8` flags (1 = has state, 2 = has data);
    - blocks: runs of (`u16` length, `u16` name index) covering all 4096 cells;
    - if flag 1: state as runs of (`u16` length, `u8` value) covering 4096 cells;
    - if flag 2: `u16` count, then per entry (`u16` cell, `u32` length, bytes);
  - `u32` update count, then per update: `i32` x, y, z; `u8` kind; `u32` delay;
  - `u32` game-section length, then its bytes. When it starts with the 4 bytes `DAY1`, the next `u32` is the day count.
  - Last: `u32` FNV-1a checksum (32-bit: start 2166136261, for each byte xor then multiply by 16777619) of every byte before it.
  - `str` is a `u16` length then that many UTF-8 bytes. Cells run x fastest, then z, then y.
- Do: a read-only Python 3 script, standard library only. `python3 dump_save.py slot1.sav` prints:
  - the version, player position and time of day, and the day count;
  - the generator's name, version and seed;
  - the block names;
  - the chunk count, and how many cells of each block name the stored chunks hold;
  - whether the checksum matches.

  A short or damaged file prints a clear message instead of a stack trace. It never writes anything.
- Hand back: `dump_save.py`.
- Done when: fields are read in exactly the order and sizes above, and every read checks there are enough bytes left.

**X1. Sky tests (S)**
- Facts, the C++ you may call (header `sky.h`; everything is inline, with no side effects):
  - `float DiscCover(float r1, float r2, float d)`: the fraction of disc 1 (radius r1) covered by disc 2 (radius r2) with centres d apart (radians on the sky); 0 when apart.
  - `SkyState ComputeSky(float dayTime, uint32_t day)`: dayTime in seconds, 0–3600. Fields:
    - `Vec3 sunDir`, `moonDir`;
    - `float moonLit` (0 new .. 1 full);
    - `float solarEclipse`, `lunarUmbra`, `lunarPenumbra` (0..1);
    - `float daylight`, `sunLight`, `starsVisible`.
  - `Atmosphere ComputeAtmosphere(const SkyState&)`: fields `Vec3 sunColor, moonColor, zenith, horizon, ambientUp, ambientDown` (`Vec3` has `x, y, z` floats) and `float exposure`.
  - `float JetStreamAngle(double T)`: T in days (day + dayTime / 3600); radians.
  - Constants: `SUN_DISC_RADIUS` 0.0283, `MOON_DISC_RADIUS` 0.0332.
  - The test style:
    ```cpp
    static void TestSomething() {
        CHECK(condition);   // counts a check; prints the line if it fails
    }
    ```
    `<cmath>`, `<cstdio>`, `<vector>` and `<algorithm>` are included.
- Do: `static void TestSkyEdges()`, which checks:
  - DiscCover is 0 apart, 1 for a small disc inside a big one, and changes by small amounts for small steps of d;
  - moonLit is near 0 at some time in days 0–7 and near 1 at another;
  - JetStreamAngle is continuous across whole-day boundaries;
  - during a total solar eclipse (find one by scanning days 0–400 in steps small enough, say 30 s, only near noon), `sunColor` is close to black.

  Keep the whole test under a second.
- Hand back: `TestSkyEdges.cpp` (just the function).
- Done when: it uses only the names above, and every loop is bounded.

**X2. Text table tests (S)**
- Facts, the C++ you may call (header `strtable.h`):
  - `void ParseStrings(const std::string& utf8, const std::string& fileName, std::unordered_map<std::string, std::string>& out, std::vector<std::string>& errors)`: parses `key = text` lines; a later key replaces an earlier one; `#` starts a comment; a bad line adds a message to `errors`.
  - `void SetStrings(const std::unordered_map<std::string, std::string>& table)`: replaces the loaded table.
  - `const std::string& Str(const char* key)`: the text, or the key itself if missing.
  - `std::string StrF(const char* key, std::initializer_list<std::string> args)`: fills `{0}`, `{1}`...; a slot with no argument stays as written.
  - `uint32_t DecodeUtf8(const std::string& s, size_t& i)`: reads one code point at `i` and advances; invalid bytes give 0xFFFD and advance by one.
  - `std::wstring Utf8ToWide(const std::string&)`, `std::string WideToUtf8(const std::wstring&)`.
  - Test style as in X1 (`CHECK(condition)` inside `static void TestSomething()`).
- Do: `static void TestStringsEdges()`, which covers:
  - a key defined twice;
  - a line without `=`;
  - a 10,000-character text;
  - `{9}` with fewer arguments;
  - invalid UTF-8 mid-line;
  - that WideToUtf8(Utf8ToWide(s)) gives back s for three strings with accents.

  End with `SetStrings({})`.
- Hand back: `TestStringsEdges.cpp`.
- Done when: it uses only the names above.

---

## Off limits, even when asked

Anything needing whole source files; threads; the renderer's structure; the save format itself; changing an existing terrain version's output; sound recipes and music; editing or reversing anything in `docs/DECISIONS.md`; anything in `reference/`.

## What Claude does with a delivery

At the next session, for each `incoming/grok/<task>.md` (or pasted answer):
1. Split it into its files by the `=== FILE:` lines. Read the report, especially the assumptions.
2. Check every fact and assumption against the real code.
3. For code: put it in place, wire it in (a test's call in `main`, a tool in `tools/`), and run all the checks.
4. Research notes feed the next plan the owner approves.
5. Merge with the owner's OK, or send it back with notes.

## Log

| Date | Task | Result (delivered / reviewed / merged / sent back) |
|---|---|---|
| 2026-09-26 | R1 | Stopped. The owner pasted R1's three parts correctly, as a first chat; the agent still quoted R2's inputs, which no R1 pack ever contained (checked in every commit), and the Brief then told it to stop when something was missing. Rewritten: one self-contained paste per task, facts in plain words, assume and label instead of stopping, private chats. |
| 2026-09-26 | R1 | Delivered (second attempt). Appraised: fair. Keep a longer sky-view reach, a colour-curve tuning pass, height-aware haze for later; three options dropped (already exist, or rest on things that don't). `docs/grok_reviews/R1.md` |
| 2026-09-26 | R2 | Delivered (sections 1–2 missing from the paste). Appraised: good skeleton, wrong details (fog knob, overcast light, sound not free, no saving). `docs/grok_reviews/R2.md` |
| 2026-09-26 | R3 | Delivered. Appraised: fair; keep the edited-cell flag, drop the rest. `docs/grok_reviews/R3.md` |
| 2026-09-26 | R4 | Delivered. Appraised: weak to fair; mostly existing or against the owner's direction; keep the texture mark language and per-texel mosaic edges. `docs/grok_reviews/R4.md` |
| 2026-09-26 | R5 | Delivered (the first-run rule missing from the paste). Appraised: fair; the reference preset must equal the defaults, no SSAO. `docs/grok_reviews/R5.md` |
| 2026-09-26 | R6 | Delivered. Appraised: good, the best so far; keep worms and coarse-lattice density. `docs/grok_reviews/R6.md` |
| 2026-09-26 | R7 | Delivered. Appraised: fair; invented a props list and missed the building layer (W067). `docs/grok_reviews/R7.md` |
