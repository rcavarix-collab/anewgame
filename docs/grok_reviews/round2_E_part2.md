# Round 2, E group, part 2: E14 procedural audio, E15 saves, E16 frame pacing

Delivered 2026-09-26, pasted into the chat (by ChatGPT, from the format). Checked against the code. The owner is placing the files in `incoming/grok/` as usual.

**Grade: good for all three.**
- Each kept to theory, labelled every assumption, and asked the right questions where the pack's facts were thin.
- E15 found a **real gap** in our saves.
- E14 raised a real **design question** about our footsteps.

## E15 saves: good, and it found a gap

**Sources:** strong. SQLite's own documentation (file format, atomic commit, application file format), RFC 1951 (DEFLATE), RFC 8878 (Zstandard), NIST FIPS 180-4, CouchDB's overview. Standards and primary documentation throughout.

**Its questions answered from the code:**

| It asked | What the code does |
|---|---|
| Are the generator's version and settings saved? | Yes: generator name, version and seed (worldfile.h). |
| Does the save flush contents and the folder entry to disk? | **No.** `WriteFileSafely` (gamefiles.cpp) writes the `.tmp` through a normal file stream and renames it; nothing forces the bytes onto the disk first. Safe against the game crashing, **not against a power cut or system crash**: the rename can reach the disk before the data, leaving an empty or partial file under the save's name. |
| Does loading check the backup too? | **No.** `LoadGame` (savegame.cpp) reads only the main file. If that's missing or damaged, it gives up, even though a good `.bak` sits beside it. There's also a moment in the swap (after the old save becomes `.bak`, before the new one is renamed in) when only the `.bak` exists. |
| Accidental damage, or tampering too? | Accidental only: a whole-file FNV-1a checksum. Tampering doesn't matter for a single-player game. |
| Records validated before use? | Yes: `DecodeSave` checks every length and count before reading (worldfile.cpp). |

**What to fix (small, planned as W070):**
1. Force the new file's contents to disk before swapping it in: flush the file, then a replace that's written through.
2. If the main save is missing or fails its checksum, **load the `.bak`**, and say so quietly.
3. A test that simulates each interruption point.

Also noted, not needed now: compressing each chunk's runs further (deflate) only if saves grow large; per-chunk checksums only if damage needs locating.

## E14 procedural audio: good, and a design question

**Sources:** real and relevant:
- Roads, *The Computer Music Tutorial* (MIT Press, 1996);
- Menzies, EURASIP (dated 2011 in the delivery; the DOI says 2010);
- Turchet 2016 (*Applied Acoustics*) and the DAFx-10 paper;
- the 2011 TVCG footstep paper (its author order needs checking);
- Verron & Drettakis (AES 2012);
- the modal impact-synthesis paper (I3D 2011; the author order likely Lloyd, Raghuvanshi, Govindaraju);
- Böttcher (2013).

**It didn't know our three axes** (organic/mechanical, negative/positive, calm/active; SOUND_PALETTE 3). The pack didn't state them: Claude's gap, not its error.

**The design question:**
- It recommends footsteps **driven by each step's contact**, following the stride, with contact strength shaping the sound.
- Ours are **played on the music's beat grid** while the player walks (worldsound.cpp: "footsteps on the beat"): a deliberate musical choice from the engine we carried over.
- The owner reported, before footsteps became audible, that they couldn't hear them "reacting to the player". A beat-locked step can't match the stride, so this may be why they still feel detached.
- **Owner's call:** keep them on the beat, move to the stride, or both (steps land on the stride but are quantised to the nearest musical subdivision within a small window).

**Worth keeping:**
- **Modal synthesis for digging and placing:** a few resonances with decay times per material family, excited by an impact. Stone rings, earth thuds. It fits our synth (additive partials with envelopes), and tuning the resonances to the harmony keeps it musical.
- **Weather as a population of small sound events** (Verron & Drettakis): a noise bed plus scattered short impacts, their density and brightness driven by the weather state. That's the rain design for G11.
- **Fast events, slow world:** event sounds respond at once; only the soundscape moves slowly. We already do this; worth keeping explicit.

## E16 frame pacing: good, little new

**Sources:**
- Fiedler's "Fix your timestep" (a practitioner article, widely cited);
- Nystrom, *Game Programming Patterns* (a book, free online);
- Android's and Apple's frame-pacing documentation (vendor documentation);
- a *Game Developer* magazine issue from 2000 (the attribution to Hecker & Simpson needs checking).

**Its questions answered from the code:**
- **Stalls:** the frame's time is clamped to 0.25 s, at most 5 catch-up ticks run per frame, and any backlog left is dropped (main.cpp). That's the textbook guard against the "spiral of death".
- **Cap and vsync:** the cap applies only with vsync off (DESIGN 12.y), so they never fight.
- **Interpolation:** drawing only; the simulation never reads the interpolated state. Menus and teleports draw the latest state.

**Keep:** its diagnostic order (first, did the tick run on time; second, was the drawn state interpolated right; third, was the frame presented on time) and "look at the spread of frame times, not the average". Our Ctrl+F3 report already gives the median, p95, p99 and the worst frames.

## Changes to plans

- **W070 (new, proposed): durable saves.** Flush before the swap; fall back to the `.bak`; test the interruption points. Small; a candidate for Tuesday's batch 1.
- **Footsteps: stride or beat?** A decision for the owner (it would be D69 once decided).
- **Modal dig and place sounds:** an idea for the sound palette, after M2's direction is set.
