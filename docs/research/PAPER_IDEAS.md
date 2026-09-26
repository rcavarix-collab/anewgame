# Could we write one? Candidate papers from building walkgrid

A background idea from the owner (2026-09-26): if we wrote a paper like the ones we've been reading, what might be worth documenting? Not part of the game; something we can do alongside it.

**Status: ideas only, nothing claimed.** Nothing here is known to be novel. Before anything is called new, it needs a search of the published work, the same as any research step (D66). The habit worth starting now is **keeping the evidence** as we go: dated notes, before-and-after pictures, measurements, and the decision log. A paper written later is only as good as the record kept now.

## Candidates, strongest evidence first

### 1. Appraising AI research assistants: a case study (dropped by the owner, 2026-09-26)
- **Dropped:** an AI checking another AI's work has no independent judge ("police policing the police"); findings wouldn't be credible without a human-scored comparison. Kept here only as a record. The per-review tallies below aren't needed for it.
- **What:** every delivery from Grok and ChatGPT has been checked against the code and the sources, and the results written down (`docs/grok_reviews/`). That's an unusual record: dated, per task, with the errors categorised.
- **What we could measure:**
  - citations with wrong authors, years or venues;
  - claims about our code that were invented;
  - recommendations that broke a stated rule, such as bundled fonts or third-party code (E17);
  - what proved useful: E15 found a real gap in our saves (W070).
- **Why it might matter:** people are starting to use these tools for literature work, and measured error rates from a real project are scarce.
- **To start now:** give each review a small tally (claims made, checked, true, false, unverifiable). We're most of the way there already.
- **Where it could go:** a human–computer interaction or software-engineering venue, as an experience report.

### 2. Anti-repetition that keeps a pixel-art palette
- **What:** near the player, published methods (bombing, histogram-preserving blending, Burley 2019) mix copies into colours that aren't in the art. Our plan picks one offset copy per texel-aligned region, with no blending, and switches to Burley's blend only at a distance (TEXTURE_BLENDING.md).
- **What we could measure:**
  - colours outside the palette (zero, by construction, near the player);
  - visible repetition, compared side by side;
  - the cost in F3 on the reference machine.
- **Novelty unknown:** needs a literature search on palette-constrained texture synthesis first.
- **Where it could go:** the *Journal of Computer Graphics Techniques*, which publishes practical methods like this and is open access.

### 3. Walking on faceted ground over a cell grid
- **What:** the ground is drawn and walked as angular facets over cells (D39), while cells still stop the body, with the thresholds tuned so one-cell steps become slopes and two-cell walls stay walls.
- **What it adds:** a middle path between cube worlds and smooth isosurfaces (marching cubes, surface nets).
- **What we could document:** the facet rule, including which way each square is split; the walking rules; and the terracing problem with its fix (W055).
- **Novelty unknown:** related work to check includes surface nets, dual contouring and smooth voxel terrain.

### 4. World sound locked to the music's harmony
- **What:** every sound effect is synthesised (D25), with pitches drawn from sets that agree with the music playing (SOUND_PALETTE.md), and a tool checks it (`sound_demo.sh analyze`).
- **What we could measure:** clashes with the music, before and after the lock. Players' preference would need a listening study, which is a bigger undertaking.
- **Novelty unknown:** adaptive-music research is large and needs searching.

### 5. Smaller notes, if any of them turn out well
- Cloud motion calibrated to real angular speeds (CLOUDS.md).
- Cheap sky-lit fill light and fog as per-pixel constants (D63, D64), with measured cost.
- A game private by construction: no network, only the operating-system limits it must respect (D21, D23). This could be a short position piece.

## How we'd do it properly
1. **Search first.** A research step on the topic: what's published, and what isn't.
2. **Write the claim before measuring.** What we expect, and what result would prove us wrong.
3. **Measure on the owner's machine,** with the F3 and Ctrl+F3 reports and fixed camera spots, as already done for the fill light and fog pictures.
4. **Credit everything,** in the bibliography's style. Our code and art stay ours; the paper describes the method.
5. **Honest limits:** one machine, one game, and no player study unless we run one.

## First small habit (costs nothing)
Date every before-and-after picture folder, and keep F3 reports with the change they measured. That serves candidates 2 to 5.
