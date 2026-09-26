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
> - any research uses reliable sources only (science agencies, standards bodies, peer-reviewed work, recognised references; never forums), with every source listed; it's for theory and explanation, never for copying anyone's code, art or text.
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

## The tasks (round 2, from 2026-09-26)

Round 1 (R1–R7, T1–T2, P1, X1–X2) is done and appraised: `docs/grok_reviews/`. Round 2 feeds Tuesday's plan (`docs/PLAN_TUESDAY.md`) and the directions after it. The groups are ordered by usefulness; within a group, the first tasks matter most.

Each task has: **Facts** (in the group's notes and the task), **Do**, **Hand back**, **Done when**. Sizes: **S** fits one reply; **M** may need a "continue".

What round 1 taught (every pack carries its group's note):
- Grok is strong at comparing options on fixed points, laying out designs, translating, and small code written to an exact spec.
- It's weak wherever it has to guess how our code works: it fills gaps with plausible inventions.
- So these tasks give it the facts, and ask for designs, specs, research and standalone tools, never changes to our code.

### A. Art: a style brief for each material at 64 pixels (feeds Tuesday step 9)

Facts for every A task:
- walkgrid's ground is faceted: angular triangles over a grid of one-block cells. Material textures are projected from the world onto the facets, 64 × 64 pixels per block (D65, up from 32), repeating every block, crisp near the player (each texel a sharp square) and smoothed further away.
- Each material has a **top** texture (ground up to about 80° steep) and a **side** texture (near-vertical faces); most use one for both. Each texture also has a **height** map (36 levels, low to high). Where two materials meet, the taller texel wins, so tall features (tufts, pebbles) poke through their neighbours. Tall features that are long and thin (ripples, strata lines) showed up as thin streaks inside other materials: avoid tall thin features.
- The look should be pixel art with a clear **mark language** per material (its own shapes: blades, chips, bands...), distinct at a glance, readable at 64, never photographic, never another game's look. Our signature is the faceted ground; textures serve it.
- The owner wants more life and less repetition: variation within the texture, no obvious grid when a block repeats, colours that stay readable at dawn, noon, dusk and under moonlight.
- The 12 materials:
  - meadow grass;
  - dry turf;
  - moss;
  - dirt;
  - loam;
  - clay;
  - sand;
  - gravel;
  - stone;
  - slate: currently far too dark and blue, nearly black;
  - sandstone: a top and a layered side;
  - snow.
- Textures are drawn by Python generators (value noise, Voronoi cells, scatter, colour ramps), not by hand, so a brief must be something a generator can follow.

Hand back for each A task: `<material>.md`, with these parts:
- **Palette:** 8–16 hex colours with a role each (base, shadow, highlight, accent).
- **Marks:** the shapes, their sizes in pixels at 64, how many per block, and how they're spread.
- **Top vs side,** where they differ.
- **Height:** what's tall and what's low, and why (for blending).
- **Variation:** how two neighbouring blocks should differ so the grid doesn't show.
- **Must never look like:** a short list.
- **Generator recipe:** in plain words, the steps a noise and scatter generator would follow.

Done when: every hex colour is listed, every size is in pixels at 64, and nothing depends on a tool we don't have.

**A1. Meadow grass (S)**
- Do: the brief. It's the most common material; it should read as lively, cool-green grass with short angled blades, and never lime or plastic.
- Hand back: `meadow_grass.md`.
- Done when: as for the group.

**A2. Sand (S)**
- Do: the brief. Warm and fine. Ripples must be low in the height map (they caused orange streaks in stone), and the grain can be tall and dotty.
- Hand back: `sand.md`.
- Done when: as for the group.

**A3. Stone (S)**
- Do: the brief. Cool grey rock with facets and cracks that suit the angular ground.
- Hand back: `stone.md`.
- Done when: as for the group.

**A4. Slate (S)**
- Do: the brief. Dark, layered, splitting rock that is still clearly readable in full light: lift it well above near-black, less saturated blue.
- Hand back: `slate.md`.
- Done when: as for the group, plus a note on how its value compares with stone's.

**A5. Sandstone, top and layered side (S)**
- Do: the brief. The side shows horizontal strata. Keep the strata low in the height map, and design the layer thicknesses so the cliff doesn't look striped like a barcode.
- Hand back: `sandstone.md`.
- Done when: as for the group.

**A6. Dirt (S)**
- Do: the brief. It shows on step sides under grass and wherever the ground is dug.
- Hand back: `dirt.md`.
- Done when: as for the group.

**A7. Loam (S)**
- Do: the brief. It must be clearly different from dirt at a glance: darker, richer, crumblier.
- Hand back: `loam.md`.
- Done when: as for the group.

**A8. Clay (S)**
- Do: the brief. Found at the edges of sand and gravel lowlands. Smooth, dense, cracked when dry.
- Hand back: `clay.md`.
- Done when: as for the group.

**A9. Gravel (S)**
- Do: the brief. Loose stones, whose tall pebbles win at borders. Avoid a regular cobble pattern.
- Hand back: `gravel.md`.
- Done when: as for the group.

**A10. Dry turf (S)**
- Do: the brief. Dusty, ochre, sparse grass; it must be distinct from meadow grass and from sand.
- Hand back: `dry_turf.md`.
- Done when: as for the group.

**A11. Moss (S)**
- Do: the brief. Soft, dense, deep green, with a cushion texture unlike grass blades.
- Hand back: `moss.md`.
- Done when: as for the group.

**A12. Snow (S)**
- Do: the brief. Found above a certain height. It must not blow out to plain white in sun: subtle cool shading and sparkle points.
- Hand back: `snow.md`.
- Done when: as for the group.

**A13. The palette as a whole (M)**
- Do: put the 12 materials' base colours side by side. Check that neighbours differ enough in value or hue. The likely neighbours:
  - grass beside dirt, dry turf and moss;
  - sand beside gravel and clay;
  - stone beside slate;
  - sandstone beside grass.

  Propose one coherent world palette.
- Hand back: `world_palette.md`: a table of material, base hex, shadow hex, highlight hex; and the neighbour pairs with a contrast note each.
- Done when: every neighbour pair is covered, and any pair that's too close gets a proposed fix.

**A14. Readable at every hour (S)**
- Facts: light runs orange at a low sun, white-gold high, blue and dim under the moon; shade is sky-lit and bluish.
- Do: for each material, say what could go wrong at dawn, noon, dusk and night (for example "slate vanishes at night"), and a palette fix.
- Hand back: `hours.md`.
- Done when: all 12 materials at all four times.

**A15. Height maps and borders (S)**
- Do: a table of every likely neighbour pair (as in A13): which material should win at the border, which features poke through, and how tall each material's features should be to get that.
- Hand back: `borders.md`.
- Done when: every pair has a winner, a reason, and no tall thin features.

### S. Research from reliable sources (D66)

Facts for every S task:
- Research standard (owner, D66): reliable sources only: national weather and science agencies, standards bodies, peer-reviewed or society publications, recognised references. Never forums, social sites or AI-written pages.
- Like a college paper: more than one independent source where possible, figures that can be tested, and every source listed with a link. Mark anything you couldn't read at the source.
- Be a considerate visitor: few requests, no bulk fetching; a site that refuses access is left alone.
- The findings feed a game that fakes nature cheaply, so end each note with **"What this means for a game":** the few numbers or rules worth copying.

Hand back for each S task: `<topic>.md` with sections Findings (each with its sources), Numbers (a table), What this means for a game, and Sources (a list with links).

Done when: every number has at least one listed source, the main ones two, and nothing comes from a forum.

**S1. Check our cloud figures (M)**
- Facts: our note claims:
  - cumulus bases about 300–3,000 m, typically about 1,000–1,500 m;
  - low cumulus drift about 9 m/s (moderate wind) to 18 m/s (strong);
  - a fair-weather cumulus lives about 20 minutes;
  - cirrus sits about 8–16 km up, in winds of tens of m/s (jet cores 40 m/s and more);
  - cirrus uncinus fall streaks are about 2 km deep;
  - the generating heads live an hour or more.
- Do: confirm or correct each figure, at the source.
- Hand back: `clouds_check.md`.
- Done when: every figure is marked confirmed, corrected (with the new figure) or unconfirmed.

**S2. How a cumulus grows and fades (S)**
- Do: the stages of a fair-weather cumulus's life as seen from the ground (shape, edges, brightness), how neighbouring clouds merge or split, and how fast the edges change.
- Hand back: `cumulus_life.md`.
- Done when: stages with timings, and what the edges look like at each.

**S3. Sky colour through the day (M)**
- Do: explain the colours from sunrise to night:
  - zenith against horizon;
  - the glow around the sun;
  - the twilight colours;
  - the anti-twilight arch and the Earth's shadow rising opposite the sun at dusk;
  - why the horizon is paler.

  Give colour values or colour temperatures where sources give them.
- Hand back: `sky_colour.md`.
- Done when: each time of day has its colours and the reason.

**S4. Sunlight colour and brightness by sun height (S)**
- Do: how direct sunlight's colour (temperature) and strength change with the sun's height above the horizon, and how the share of sky light rises as the sun drops.
- Hand back: `sunlight.md`.
- Done when: a table by sun height (0°, 5°, 10°, 20°, 45°, 90°).

**S5. Overcast light (S)**
- Do: how much light an overcast sky gives compared with a clear one; why shadows vanish; how colour shifts. Include thin overcast against thick.
- Hand back: `overcast.md`.
- Done when: ratios with sources.

**S6. Wet ground (S)**
- Do: how much darker and glossier soil, sand, rock and grass look when wet, why, and how long they take to dry.
- Hand back: `wet_ground.md`.
- Done when: darkening figures (albedo or reflectance) per surface.

**S7. Haze and visibility (S)**
- Do: how far one sees in clear air, haze, mist, fog and rain; how distance turns land blue or grey (aerial perspective); how height changes it.
- Hand back: `visibility.md`.
- Done when: a table of conditions against visibility distance.

**S8. What rain looks like (S)**
- Do: drop sizes and fall speeds; how rain looks near and far; streaks; how it lightens distant land; puddles and splashes.
- Hand back: `rain_look.md`.
- Done when: fall speeds with sources.

**S9. What falling and lying snow looks like (S)**
- Do: flake fall speeds and drift; how snowfall shortens visibility; how snow on the ground looks in sun and shade.
- Hand back: `snow_look.md`.
- Done when: fall speeds and visibility with sources.

**S10. Sand ripples and dunes (S)**
- Do: ripple spacing and height, and how they line up with the wind.
- Hand back: `sand_ripples.md`.
- Done when: sizes in centimetres with sources.

**S11. Mesas and sandstone strata (S)**
- Do: how mesas and buttes form; strata thickness and colours; how the cliffs erode; what grows at the cliff foot and on top.
- Hand back: `mesas.md`.
- Done when: layer thickness ranges and colour descriptions with sources.

**S12. Grass and meadow colour at a distance (S)**
- Do: how grassland colour varies with season, dryness, distance and light; why fields look patchy from afar.
- Hand back: `grass_colour.md`.
- Done when: the causes of patchiness, with sources.

**S13. Night light (S)**
- Facts: our darkest night is 30% of daylight in the game's brightness terms (a stand-in, not physics).
- Do: real light levels (lux) for full sun, overcast, sunset, twilight stages, full moon, quarter moon, starlight; how human night vision changes colour and detail.
- Hand back: `night_light.md`.
- Done when: a lux table and the colour-vision facts, with sources.

**S14. Solar and lunar eclipses as seen (S)**
- Do: how dark a total solar eclipse gets and how fast; the colours of the sky and horizon at totality; what a partial eclipse looks like; the colours of the Moon in a lunar eclipse.
- Hand back: `eclipses.md`.
- Done when: darkness figures and colours, with sources.

**S15. Sound of wind (S)**
- Do: what makes wind audible (turbulence, whistling in edges, rustle of grass); how it changes with speed; how far it carries.
- Hand back: `wind_sound.md`.
- Done when: the mechanisms and rough frequency ranges, with sources.

**S16. Footstep acoustics (S)**
- Do: what distinguishes steps on grass, sand, gravel, rock, snow and mud (frequency content, duration, crunch). Only for synthesising them: describe, never provide recordings.
- Hand back: `footsteps.md`.
- Done when: each surface's character, with sources.

**S17. Natural soundscapes by time of day (S)**
- Do: how an open landscape sounds at dawn, midday, dusk and night, and in rain; loudness levels in dB where sources give them.
- Hand back: `soundscape_day.md`.
- Done when: each time of day with its sound sources and levels.

**S18. How caves form, and what they look like (S)**
- Do: the kinds of caves (dissolution, lava tubes, sea caves, talus), their shapes, sizes and entrances; which fit sandstone and slate country.
- Hand back: `caves_real.md`.
- Done when: each kind with typical dimensions, with sources.

### G. Game design research (documents only)

Facts for every G task:
- walkgrid today:
  - a first-person walk over faceted ground: dig and place ground cells of 12 materials;
  - hills and sandstone mesas;
  - a real sun, moon and stars with eclipses; two cloud layers; gliding shadows;
  - synthesised music following the day, and synthesised footsteps;
  - saves; translatable text; no inventory, crafting, plants, weather or caves yet.
- Rules:
  - few words on screen, and no numbers where a feel will do;
  - all sound synthesised;
  - nothing from another game's look;
  - must run at 60 fps on a six-year-old PC, with every heavy effect having a setting;
  - no network use.
- Built blocks (for buildings) are a separate, future layer (owner's idea, parked). The ground can't be built with the way buildings will be.

Hand back for each G task: `<topic>.md`, 800–2,000 words. Options with, for each: what the player does and sees, why it's fun, rough cost (when the world is built, or every frame), risks. End with a recommendation and open questions.

Done when: each option covers those points.

**G1. Built blocks: where they meet the ground (M)**
- Facts: the ground's corners are eased and jittered, so a cube placed on it would float at some corners and sink at others.
- Do: compare at least:
  - the ground flattening itself under a built block;
  - a skirt on the block reaching down;
  - a footing or foundation piece;
  - one idea of your own.
- Hand back: `build_meet.md`.
- Done when: each option says what happens when the ground under a block is later dug away.

**G2. Built blocks: one grid or two (M)**
- Do: built blocks on the same one-block cells as the ground, or on a finer grid (half or quarter blocks)? Weigh freedom against complexity, saving and collision.
- Hand back: `build_grid.md`.
- Done when: both are weighed on the same points.

**G3. Built blocks: the look (M)**
- Do: how built pieces could look handmade and ours (bevelled edges, chamfers, visible joints), clearly not plain cubes and not another game's style. Include a starter set of 8–12 pieces (wall, floor, beam, step, post...).
- Hand back: `build_look.md`.
- Done when: every piece is described in shape and size.

**G4. Built blocks: what they're made of (S)**
- Do: which building materials (worked stone, timber, clay brick, thatch...) fit a world of these 12 ground materials, and how one might get them. Suggestions only: nothing is decided about gathering.
- Hand back: `build_materials.md`.
- Done when: each material says where it would come from.

**G5. Built blocks: walking, climbing and stepping (S)**
- Facts: the player can step up about half a block without jumping.
- Do: how stairs, slopes, ladders and steps between ground and built pieces should feel.
- Hand back: `build_movement.md`.
- Done when: each case says what happens.

**G6. Built blocks: what building is for (M)**
- Do: reasons to build, beyond the pleasure of building (shelter from weather at night? views? keeping things?), without meters or numbers.
- Hand back: `build_purpose.md`.
- Done when: at least five reasons, each fitting the rules.

**G7. Plants: which kinds suit our world (M)**
- Facts: plants can be flat cards that turn to face the viewer (one quad each) or small faceted shapes.
- Do: 10–15 plant kinds matched to the 12 materials and to height, slope and sun.
- Hand back: `plants_kinds.md`.
- Done when: each plant names its ground, where it grows and its look.

**G8. Plants: growth without a simulation (M)**
- Facts: a plant's stage can be a pure function of its age (the time it was planted against the day count), so nothing ticks.
- Do: growth stages, spreading rules (deterministic from the world seed), and what the player can do with plants.
- Hand back: `plants_growth.md`.
- Done when: stages and rules can be written as a table.

**G9. Weather: the kinds, corrected (M)**
- Facts, corrections to the first design:
  - overcast removes almost all direct sun and its shadows, while sky light stays;
  - fog for weather comes from the haze term, not from moving the loaded world's edge;
  - exposure is left to the day clock;
  - the world is on its equator with no seasons, so snow needs a height rule;
  - weather has to be saved and should be repeatable from the world's seed.
- The knobs:
  - high streak cover 0–1;
  - low cloud cover 0–1;
  - sun strength;
  - sky colours (top, horizon);
  - ambient light from above and below;
  - haze scale and strength;
  - wind (the jet stream) direction.
- Do: 5–7 kinds as a table of those knobs, with transitions (durations, next-kind weights).
- Hand back: `weather_kinds.md`.
- Done when: every kind uses only the knobs above, and overcast has no hard shadows.

**G10. Weather: rain and snow on screen, cheaply (M)**
- Facts: there's a post-processing pass with scene depth already; precipitation must live inside it (no new passes), costing nothing when dry.
- Do: how to fake rain streaks, splashes near the ground, snowflakes and wet-looking ground with per-pixel tricks.
- Hand back: `weather_screen.md`.
- Done when: each effect says what it reads (depth, wind, time) and roughly what it costs per pixel.

**G11. Weather: sounds as synth recipes (M)**
- Facts: sound comes from a synthesiser: oscillators, noise, filters, envelopes, echo. Every pitch comes from the music's harmony (safe pitch sets). Three slow axes shape every sound: organic↔mechanical, negative↔positive, calm↔active.
- Do: recipes, in plain synthesis terms, for a rain bed, drips, heavy rain, distant rumble, wind gusts and snow hush, and how each moves with the axes.
- Hand back: `weather_sounds.md`.
- Done when: every recipe is only noise, oscillators, filters and envelopes, with any pitch taken from the harmony.

**G12. Caves: a generator outline (M)**
- Facts: terrain is made column by column (16 × 16 blocks, full height) on background threads, about 0.05 ms per column today. A caves version would be a new, frozen terrain version. Proposed: sparse "worm" tunnels plus coarse 3-D density (sampled every 4 blocks and interpolated) for chambers.
- Do: a step-by-step outline:
  - how worms are seeded on a coarse grid;
  - how each column finds every worm that can reach it;
  - radii, how deep they run, where they open to the surface;
  - how the density field makes chambers;
  - how to keep thin walls from appearing.
- Hand back: `caves_outline.md`.
- Done when: every step is deterministic from (seed, x, y, z), and the reach bound is stated.

**G13. A reason to stay: loops compared (M)**
- Do: compare 5 play loops (for example build a home, tend plants, explore caves, chase eclipses and weather, collect materials), each fitting the rules.
- Hand back: `loops.md`.
- Done when: each loop has a first ten minutes and a tenth hour.

**G14. Discovery without words (S)**
- Do: ways the world can reward exploring without text or numbers: views, sounds, rare sights, changes in music.
- Hand back: `discovery.md`.
- Done when: at least ten ideas, each with a cost note.

**G15. The first ten minutes (S)**
- Do: how a new player learns to walk, dig, place and choose materials with almost no words.
- Hand back: `first_minutes.md`.
- Done when: every action has a way it's discovered.

**G16. Night play (S)**
- Facts: nights are ten real minutes of each hour-long day; moonlight depends on the moon's phase.
- Do: what's worth doing at night; how dark it should get; how to light a place without breaking the look.
- Hand back: `night_play.md`.
- Done when: at least five ideas.

**G17. Accessibility check of the settings (S)**
- Facts, the settings:
  - look sensitivity and inversion;
  - vsync and frame cap;
  - sun shadows, outlines, crevice shading, glow;
  - FPS counter, profiler, fullscreen;
  - master, music, world and footstep volumes;
  - field of view;
  - toggle-to-move, high-contrast interface, music intensity, mono audio;
  - rebindable keys.
- Do: what's missing, judged against published game accessibility guidelines (cite them), in order of value.
- Hand back: `accessibility.md`.
- Done when: each suggestion cites a guideline.

**G18. A screen with almost nothing on it (S)**
- Facts: the screen shows a small crosshair, a hotbar of 10 materials with the selected one's name, and short messages.
- Do: ideas for showing the time of day, direction, height and what's selected without numbers or clutter.
- Hand back: `hud_ideas.md`.
- Done when: each idea says what it replaces.

**G19. Sky events worth waiting for (S)**
- Facts: the moon goes round in 8 days, eclipses happen, the jet stream swings every few days.
- Do: rare sky events the game could show cheaply (halos, sun dogs, crepuscular rays through gaps in clouds, meteor showers, the green flash), with how rare each should be and how to fake it.
- Hand back: `sky_events.md`.
- Done when: each event has a cheap method and a rarity.

**G20. Sound of place (S)**
- Facts: the world sound already knows open sky, enclosed and deep spaces, and nearby water and plants.
- Do: how the ambient sound should change on a mesa top, in a valley, at a cliff foot, underground and in a built room.
- Hand back: `sound_of_place.md`.
- Done when: each place with its sound changes, all synthesisable.

### K. Data specs (text tables the game could load)

Facts for every K task:
- Tables are plain text, one row per line, `key = value` pairs or columns separated by `|`, with `#` comments.
- Names are lowercase with underscores.
- Nothing player-visible goes in them: words come from the text table by key.

Hand back for each K task: the table file, and `REPORT.md` explaining each column.

Done when: every column is explained, and every row is complete.

**K1. Weather kinds table (S)**
- Do: G9's kinds (or your own reading of the facts there) as a loadable table:
  - one row per kind;
  - columns: the knobs, the duration range and the next-kind weights;
  - a height rule for snow.
- Hand back: `weather.txt`.
- Done when: the weights for each kind sum to a positive number, and no kind can get stuck forever.

**K2. Plant table (S)**
- Do: G7's plants as a table:
  - which ground, height range, slope limit and sun need;
  - growth stage durations in game days;
  - spread chance.
- Hand back: `plants.txt`.
- Done when: every plant has every column.

**K3. Built piece catalogue (S)**
- Do: G3's pieces as a table:
  - name;
  - size in blocks (fractions allowed);
  - how it rests on the ground (G1);
  - which materials it can be made of (G4).
- Hand back: `pieces.txt`.
- Done when: every piece has every column.

### P. Standalone tools (Python 3.9+, standard library only)

Facts for every P task:
- Tools live in `tools/`, run from any folder, and find the repository relative to their own file.
- Output is plain text; any image is PNG.
- They never change game files unless the task says so.
- On bad input they print a clear message, never a stack trace.
- They start with a comment saying what the tool is and how to run it.

Hand back for each P task: the script.

Done when: every rule above holds, and the task's own points.

**P2. `vtex_to_png.py`: texture sheets as pictures (M)**
- Facts, the `.vtex` text format:
  - Lines starting `#` are comments; indentation is optional.
  - A texture is:
    - `texture <name>`;
    - `size <n>` (8, 16, 32 or 64);
    - `palette`, followed by lines of one key character and 6 hex digits (8 with alpha);
    - `pixels`, followed by exactly n rows of n keys;
    - optionally `height` (n rows of n characters, `0`–`9` then `a`–`z`, 36 levels), `shine` (`0`–`9`) and `glow` (`0`–`9`);
    - `end`.
  - `block <name>` … `end` entries map textures to faces; skip them.
  - A file holds any number of textures.
- Do: read one `.vtex` and write one PNG. Each texture is a row: colour, then height as grey, then shine and glow if present, each scaled up 4× (nearest), 8 pixels apart. PNG writing uses `zlib` and `struct` only.
- Hand back: `vtex_to_png.py`.
- Done when: a texture with no height still works, and a bad row is reported with its line number.

**P3. `vtex_lint.py`: check texture files (S)**
- Facts: as P2. Rules to check:
  - size is one of 8, 16, 32, 64;
  - exactly `size` rows of exactly `size` characters in every map;
  - every pixel key is in its palette;
  - palette keys are unique within a texture and aren't space or `#`;
  - height characters are only `0`–`9` and `a`–`z`;
  - every texture ends with `end`.
- Do: check one or more files; print `file:line: problem` for each; exit 1 if any.
- Hand back: `vtex_lint.py`.
- Done when: each rule has a test case in a comment at the bottom showing a bad input and its message.

**P4. `tile_seams.py`: do textures tile? (S)**
- Facts: as P2. A texture repeats every block, so its left edge meets its right edge, and top meets bottom.
- Do: for each texture, compare the colour difference across the wrap edges with the average difference between neighbouring pixels inside. Report textures whose seams are more than 1.5× the inside, with which edge.
- Hand back: `tile_seams.py`.
- Done when: the measure is explained in a comment.

**P5. `texture_values.py`: brightness and saturation report (S)**
- Facts: as P2.
- Do: for each texture:
  - mean brightness (relative luminance from sRGB);
  - mean saturation;
  - the darkest and lightest 5%;
  - flag any texture whose mean brightness is below 0.12 or saturation above 0.6.
- Hand back: `texture_values.py`.
- Done when: the luminance formula is stated in a comment.

**P6. `perf_compare.py`: compare two performance reports (S)**
- Facts, the report format (plain text):
  - header lines;
  - a table headed `ms  median  p95  p99  worst  average`, one row per system, the name in capitals possibly with spaces, then five numbers;
  - a `peak load  max` table of name-and-number rows.
- Do: `perf_compare.py old.txt new.txt` prints each system's median and p95 old against new, with the change in percent, and marks a row `!` when median or p95 got more than 10% worse and more than 0.05 ms.
- Hand back: `perf_compare.py`.
- Done when: rows only in one report are listed separately, and names with spaces parse.

**P7. `save_diff.py`: compare two saves (M)**
- Facts: the save layout (little-endian):
  - `u32` magic, the bytes `WGRD`; `u32` version;
  - player: `f32` x, y, z, yaw, pitch; `i32` hotbar slot; `f32` time of day;
  - generator: `str` name, `u32` version, `u64` seed;
  - `u32` name count, then `str` block names;
  - `u32` chunk count, then per chunk:
    - `i32` cx, cy, cz; `u8` flags (1 state, 2 data);
    - blocks as runs of (`u16` length, `u16` name index) covering 4096 cells;
    - if flag 1, state as runs of (`u16` length, `u8` value) covering 4096 cells;
    - if flag 2, `u16` count, then (`u16` cell, `u32` length, bytes) each;
  - `u32` update count, then per update: `i32` x, y, z; `u8` kind; `u32` delay;
  - `u32` game-section length, then its bytes;
  - `u32` FNV-1a checksum of everything before it.
  - `str` is a `u16` length then UTF-8 bytes. Cells run x fastest, then z, then y (index = (y × 16 + z) × 16 + x).
- Do: `save_diff.py a.sav b.sav` lists chunks only in one save, and for chunks in both, each cell whose block differs, as world coordinates (chunk × 16 + local) and old → new names. Block indexes map through each file's own name table.
- Hand back: `save_diff.py`.
- Done when: different name tables in the two files are handled.

**P8. `text_lengths.py`: translations that won't fit (S)**
- Facts: text files are `key = text`, `#` comments, UTF-8; `{0}` slots; `\n` line breaks. English is `assets/text/en.txt`; others are `<code>.txt` beside it.
- Do: for each language, list keys whose text is more than 35% longer than English (counting characters, slots as 3), longest first; and keys whose Latin letters aren't all capitals.
- Hand back: `text_lengths.py`.
- Done when: `window.title` and `font` are skipped.

**P9. `cloud_motion.py`: angular speed calculator (S)**
- Facts: a cloud at height h moving at speed v, seen at elevation angle e above the horizon, crosses the sky at about (v / h) × sin²(e) radians per second, for motion along the view direction; across the view it's (v / h) × sin(e).
- Do: a calculator that prints a table of degrees per second and seconds-to-cross-10° for given h, v and elevations 10°, 30°, 60°, 90°, both directions, plus the same for a game cloud plane at a given height in blocks (1 block = 1 m) and a time compression factor.
- Hand back: `cloud_motion.py`.
- Done when: the formulas are derived in a comment.

### X. Tests to an exact spec (C++17; Claude compiles and runs them)

Facts for every X task:
- A test is a function `static void TestSomething()` that calls `CHECK(condition)` (counts a check, prints the line if it fails).
- `<cmath>`, `<cstdio>`, `<vector>`, `<string>`, `<algorithm>`, `<unordered_map>`, `<cstdint>` and `<filesystem>` are included.
- Keep each test under a second.
- Beware C++ traps: a hex escape swallows following hex letters (`"\x80cd"` is one escape: write `"\x80" "cd"`), and floats need tolerances.
- Tolerances must come from the numbers given: a value that changes over time can differ between two moments, so compare continuity with a margin suited to the gap.

Hand back for each X task: the one function.

Done when: it uses only the names given, and every loop is bounded.

**X3. Sky basics (S)**
- Facts (header `sky.h`):
  - `SkyState ComputeSky(float dayTime, uint32_t day)`: dayTime in seconds 0–3600; the sun rises at 0 and sets at 3000.
  - Fields: `Vec3 sunDir, moonDir` (unit vectors); `float daylight` (0.30 at night up to 1); `float sunLight` (0..1); `float starsVisible` (0..1); `float moonLit` (0..1, 8-day cycle).
  - `Vec3` has `x, y, z` and `Dot(a, b)`.
- Do: `TestSkyBasics()`:
  - sun and moon directions have length 1 (±0.001) at 24 times across 3 days;
  - the sun is above the horizon (y > 0) at 1500 and below at 3300;
  - daylight stays within [0.30, 1];
  - starsVisible is 0 at 1500;
  - moonLit repeats after 8 days (same time of day, ±0.02).
- Hand back: `TestSkyBasics.cpp`.
- Done when: as for the group.

**X4. Chunk face pairs (S)**
- Facts (header `facetmesh.h`):
  - `int FacetPairBit(int a, int b)` numbers the 15 unordered pairs of cube faces 0–5 as bits 0–14, the same for (a, b) and (b, a);
  - `bool FacetFacesSee(uint16_t openings, int a, int b)` is true when a == b or that pair's bit is set;
  - `FACET_ALL_OPEN` is `0x7FFF`.
- Do: `TestFacePairs()`:
  - all 15 pairs map to distinct bits 0–14;
  - symmetry;
  - FacetFacesSee(0, a, a) is true and FacetFacesSee(0, a, b) false for a ≠ b;
  - FACET_ALL_OPEN sees every pair.
- Hand back: `TestFacePairs.cpp`.
- Done when: as for the group.

**X5. Game folder choice (S)**
- Facts (header `gamefiles.h`): `std::filesystem::path ChooseGameFolder(const std::filesystem::path& documents, const std::filesystem::path& savedGames)`:
  - documents not inside OneDrive → documents / "My Games" / "walkgrid";
  - documents inside a folder named OneDrive (any path component equal to "OneDrive", any case) → savedGames / "walkgrid";
  - documents empty → savedGames / "walkgrid";
  - both empty, or OneDrive with no savedGames → an empty path.
- Do: `TestGameFolderChoice()`, with at least 8 cases including mixed case, a OneDrive-like name that isn't one ("OneDriveBackup"), and forward and back slashes.
- Hand back: `TestGameFolderChoice.cpp`.
- Done when: every case's expectation follows the rules stated. Mark as ASSUMPTION anything the rules don't settle, such as whether "OneDriveBackup" counts.

**X6. Text table, more (S)**
- Facts (header `strtable.h`):
  - `void SetStrings(const std::unordered_map<std::string, std::string>&)`;
  - `const std::string& Str(const char* key)`: the key itself when missing;
  - `std::string StrF(const char* key, std::initializer_list<std::string> args)`: fills `{0}`…`{9}`; slots may appear in any order and more than once;
  - `bool HasStr(const char* key)`;
  - `std::vector<uint32_t> StringCodepoints()`: every code point the loaded texts use, sorted, no repeats, not counting the `font` entry's text.
- Do: `TestStringsMore()`:
  - slots in reverse order;
  - a repeated slot;
  - HasStr on present and missing keys;
  - codepoints sorted and unique for a table with accents;
  - the `font` entry ignored.

  End with `SetStrings({})`.
- Hand back: `TestStringsMore.cpp`.
- Done when: as for the group.

**X7. Terrain heights (S)**
- Facts (header `terrain.h`): `int HillsHeight(uint64_t seed, int wx, int wz)` returns the top solid cell's y for a world column; it's pure (same inputs, same output) and stays within 1..70.
- Do: `TestHillsHeights()`:
  - the same inputs twice give the same height, for 1,000 columns;
  - every height is within 1..70;
  - two different seeds give different heights somewhere in a 32 × 32 area;
  - neighbouring columns differ by at most 20.

  Mark the 20 as an ASSUMPTION and explain it.
- Hand back: `TestHillsHeights.cpp`.
- Done when: as for the group.

**X8. Disc overlap properties (S)**
- Facts (header `sky.h`): `float DiscCover(float r1, float r2, float d)`: the fraction of disc 1 covered by disc 2, centres d apart; 0 when d ≥ r1 + r2. Covered area is DiscCover × π × r1².
- Do: `TestDiscCoverProperties()`:
  - it never increases as d grows;
  - its result is always within [0, 1];
  - the covered area is the same whichever disc is "1" (r1² × DiscCover(r1, r2, d) ≈ r2² × DiscCover(r2, r1, d), relative tolerance 1e-3);
  - equal discs at d = 0 give 1.
- Hand back: `TestDiscCoverProperties.cpp`.
- Done when: as for the group.

### T. Translations (carry the player's words)

Facts for every T task:
- The pack carries `en.txt`: one `key = text` per line; `#` starts a comment; `{0}`, `{1}` are slots filled by the game; `\n` is a line break.
- Keep every key, in the same order, and every slot exactly. Words few and plain; capitals where the language has them.
- Leave `window.title`, `title.name` and `font` as they are.
- Use the everyday words a game in that language would use; say when you're unsure.

Hand back for each T task: the language file.

Done when: the same keys as `en.txt`, in the same order, slots matching line by line, and the key count in the report.

**T3. Italian, `it.txt` (M)**
- Do: translate. Hand back: `it.txt`. Done when: as for the group.

**T4. Brazilian Portuguese, `pt-BR.txt` (M)**
- Do: translate. Hand back: `pt-BR.txt`. Done when: as for the group.

**T5. Polish, `pl.txt` (M)**
- Do: translate. Hand back: `pl.txt`. Done when: as for the group.

**T6. Dutch, `nl.txt` (M)**
- Do: translate. Hand back: `nl.txt`. Done when: as for the group.

**T7. Swedish, `sv.txt` (M)**
- Do: translate. Hand back: `sv.txt`. Done when: as for the group.

**T8. Danish, `da.txt` (M)**
- Do: translate. Hand back: `da.txt`. Done when: as for the group.

**T9. Norwegian Bokmål, `nb.txt` (M)**
- Do: translate. Hand back: `nb.txt`. Done when: as for the group.

**T10. Finnish, `fi.txt` (M)**
- Do: translate. Hand back: `fi.txt`. Done when: as for the group.

**T11. Czech, `cs.txt` (M)**
- Do: translate. Hand back: `cs.txt`. Done when: as for the group.

**T12. German, second pass, part 1 of 2 (S)**
- Facts: the pack carries the first half of the keys, in English and in the first German translation. Known problems (fix them if they're in your half): `HAUTLAUTSTÄRKE` should be `HAUPTLAUTSTÄRKE`; `SPIELE WIE GEWOHNT` should be `SPIEL WIE GEWOHNT`.
- Do: review every line in your half as a native speaker would; fix it; list each change with a reason.
- Hand back: `de_part1.txt` (your half, `key = text` lines, same order) and `changes.md`.
- Done when: the same keys as the English half, in order, with slots matching.

**T13. German, second pass, part 2 of 2 (S)**
- Facts: the pack carries the second half of the keys, in English and in the first German translation. Known problems (fix them if they're in your half): `HAUTLAUTSTÄRKE` should be `HAUPTLAUTSTÄRKE`; `SPIELE WIE GEWOHNT` should be `SPIEL WIE GEWOHNT`.
- Do: review every line in your half as a native speaker would; fix it; list each change with a reason.
- Hand back: `de_part2.txt` (your half, `key = text` lines, same order) and `changes.md`.
- Done when: the same keys as the English half, in order, with slots matching.

**T14. Mexican Spanish, second pass, part 1 of 2 (S)**
- Facts: the pack carries the first half of the keys, in English and in the first Mexican Spanish translation. Known problems (fix them if they're in your half): `MARGAS` should be `MARGA`; `RESTAURAR PREDETERMINADO` is too long for a button.
- Do: review every line in your half as a native speaker would; fix it; list each change with a reason.
- Hand back: `es-MX_part1.txt` (your half, `key = text` lines, same order) and `changes.md`.
- Done when: the same keys as the English half, in order, with slots matching.

**T15. Mexican Spanish, second pass, part 2 of 2 (S)**
- Facts: the pack carries the second half of the keys, in English and in the first Mexican Spanish translation. Known problems (fix them if they're in your half): `MARGAS` should be `MARGA`; `RESTAURAR PREDETERMINADO` is too long for a button.
- Do: review every line in your half as a native speaker would; fix it; list each change with a reason.
- Hand back: `es-MX_part2.txt` (your half, `key = text` lines, same order) and `changes.md`.
- Done when: the same keys as the English half, in order, with slots matching.

**T16. French, second pass, part 1 of 2 (S)**
- Facts: the pack carries the first half of the keys, in English and in the first French translation. Known problems (fix them if they're in your half): `CLIQUEZ UNE LIGNE` should be `CLIQUEZ SUR UNE LIGNE`; `QUITTER VERS TITRE` is awkward.
- Do: review every line in your half as a native speaker would; fix it; list each change with a reason.
- Hand back: `fr_part1.txt` (your half, `key = text` lines, same order) and `changes.md`.
- Done when: the same keys as the English half, in order, with slots matching.

**T17. French, second pass, part 2 of 2 (S)**
- Facts: the pack carries the second half of the keys, in English and in the first French translation. Known problems (fix them if they're in your half): `CLIQUEZ UNE LIGNE` should be `CLIQUEZ SUR UNE LIGNE`; `QUITTER VERS TITRE` is awkward.
- Do: review every line in your half as a native speaker would; fix it; list each change with a reason.
- Hand back: `fr_part2.txt` (your half, `key = text` lines, same order) and `changes.md`.
- Done when: the same keys as the English half, in order, with slots matching.

### H. Writing

Facts for every H task: few words; plain; no numbers where a feel will do; everything the player reads goes into the text table by key (lowercase, dotted, like `hint.dig`).

**H1. Wordless-first hints (S)**
- Do: at most eight hints for a first game (moving, looking, digging, placing, choosing materials, the materials library, saving, the menu), each at most five words. Propose a key for each and when it would show.
- Hand back: `hints.txt` as `key = text` lines, and `REPORT.md`.
- Done when: every hint is five words or fewer.

**H2. Glossary of the project's words (S)**
- Facts, the words:
  - facet, cell, chunk, column;
  - material, top and side texture, height map;
  - openness, sky view;
  - detail bands, fine detail;
  - jet stream, cirrus, cumulus;
  - day clock, day count, eclipse;
  - string table, pack, round.
- Do: a one-line plain definition of each, for a new helper reading the project.
- Hand back: `glossary.md`.
- Done when: every word is defined in one line, with no invented facts (mark any guess).

**H3. Settings names, plainer (S)**
- Facts: the settings list is in G17.
- Do: for each setting, the plainest short name a player understands without knowing graphics jargon (for example crevice shading, not screen-space ambient occlusion). At most three words each.
- Hand back: `setting_names.md`: current name, suggested name, reason.
- Done when: every setting is covered.

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
| 2026-09-26 | T2 | Delivered (8 of 126 texts looked at). Appraised: fair; five edits kept, waiting for the go-ahead. `docs/grok_reviews/T2.md` |
| 2026-09-26 | T1 | Delivered three times (German, Mexican Spanish, French). Appraised: all keys and slots right; a few word fixes. `docs/grok_reviews/code_and_text.md` |
| 2026-09-26 | P1 | Delivered. Appraised: good; ran correctly on a real save. |
| 2026-09-26 | X2 | Delivered. Appraised: good; one C++ escape bug. |
| 2026-09-26 | X1 | Delivered. Appraised: fair; two checks wrong, one from the task's own wording. |
