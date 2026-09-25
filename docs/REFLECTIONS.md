# walkgrid reflections

Looking back, honestly: what went well, what didn't, and what we'll do differently. Written at the end of every milestone and after anything goes wrong. Newest first. Each lesson that changes how we work becomes a rule (CLAUDE.md or SOP) or a decision (DECISIONS.md), and says which.

---

## M0 on the owner's machine, 2026-09-25

**What the first Windows run showed**
- It runs: 60 fps, start-up in 0.99 s, graphics work under 1 ms a frame.
- **One system is over budget.** World sound runs at 0.54 ms typically but up to 14.9 ms, and every one of the worst frames traces to it. It renders sound effects on the main thread (forecast F11 came true).
- **Footsteps can't be heard.** They are made, but mixed about 20 dB under the music. This was carried from Voxistics, not broken by M0.
- **Two things were missed.** The title screen still said VOXISTICS: my rename search was case-sensitive. Files went into OneDrive's Documents: Windows redirects "Documents" there by default.
- **The owner expected new ground, and saw Voxistics.** Taking the old game out was M0's whole scope, and the plan said so ("What M0 can't show: anything about the faceted world"). But I never said it plainly when handing the build over: "this will look exactly like Voxistics". A hand-over has to say what the person will see, not only what changed.

**We'll do differently**
- Every hand-over opens with "what you'll see", including what hasn't changed. *(New rule in SOP section 5.)*
- Name searches are case-insensitive, and a check now fails the tests if the old name appears in any string.
- Don't hold a visible milestone behind an invisible one: the owner wanted to go straight to M1, and next time a milestone with nothing to see should be proposed as part of the next one.

## M0 so far (steps 0.1–0.13), 2026-09-25

**Went well**
- **Planning before code.** The foundations and budget work caught the triangle-count problem (F1) on paper, before any mesher existed.
- **Small steps paid off.** One step per commit, with every check after each one, meant each problem surfaced at its own step:
  - removing The Line changed a texture count;
  - removing pulse left 10 textures unused, which would have shown a start-up warning in game;
  - the game.cpp split exposed shared state.
- **The layer check pulled its weight.** It found two structural problems the plan hadn't listed (common.h naming the block list; persist spanning two layers), and it now keeps them from coming back.
- **Verification by mechanism, not by eye.** The game.cpp split was checked line by line against the original, and camera-relative rendering got a test proving the transforms agree, instead of both being assumed.

**Didn't go well**
- **My edit scripts slipped twice.** Once a script re-read a file halfway and dropped its own changes; once a brace-matcher tripped on an apostrophe in a comment. The checks caught both before anything was committed, but only because they ran every time.
- **Nothing has been seen running yet.** Everything so far is checked here, not on Windows. That's the biggest open risk in M0.

**We'll do differently**
- Keep running every check after every step, even "trivial" ones. That's what caught the slips. *(Already the rule: CLAUDE.md, How we work 4.)*
- Put the Windows check earlier where we can: ask for a build partway through a milestone when a step changes something visible.

---

## The previous attempt (before walkgrid), in the owner's words, summarised

- **What went wrong.** "Start fresh" was misread as "rewrite from scratch", and that misreading was written into CLAUDE.md as a rule. Working systems were rewritten into weaker ones: a 700-line renderer replacing a 2,250-line one, and no music, menus or settings. Effort went into new parts while what makes a game feel finished sat unused. Test counts were reported as if the game worked, without a frame ever being seen. When the owner pushed back, more new code was written instead of stopping to ask.
- **Lessons, and where they became rules:**
  - carry across, don't rewrite (CLAUDE.md, rule 3; D1);
  - approve a file-by-file plan before building (CLAUDE.md, rule 1);
  - a feature checklist per build, stating plainly what wasn't verified (CLAUDE.md, rule 4; the milestone checklists);
  - ask when a request can be read two ways (CLAUDE.md, rule 1);
  - the seed-file rule limited to the four seed files, stated plainly (CLAUDE.md; DESIGN.md 1.2).
