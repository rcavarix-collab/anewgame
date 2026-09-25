# walkgrid forecasts

Problems we expect before they happen: why we expect each one, the warning signs to watch for, when to act, and what we'll do. The point is to deal with them early, while they're cheap. Reviewed at every milestone. A forecast that comes true moves to `REFLECTIONS.md` with what we learned. One that stops being possible is marked "retired" with the reason.

**Likelihood / cost if ignored:** Low, Medium, High.

---

## Rendering and the world

### F1. The fine mesh's triangle count gets away from us
- **Likelihood:** High. **Cost if ignored:** High.
- **Why.** Small triangles multiply fast: a cliff or a dug-out hollow has far more surface than flat ground (FOUNDATIONS 4.1).
- **Warning signs.** Triangle count in F3 above 1.5 million; GPU world row above 5 ms; any chunk hitting its vertex cap.
- **Act.** From M1 step 1: the preview tool measures before the mesher is built.
- **Plan.** Subdivide only where it shows (T2); a hard per-chunk cap; shadows from the coarse mesh.

### F2. Cracks where detail levels meet
- **Likelihood:** High. **Cost if ignored:** Medium.
- **Why.** Every level-of-detail system has them, as edges of different resolution meet.
- **Warning signs.** Flickering pinholes along band edges, especially at grazing angles.
- **Act.** In the mesher's design, not after.
- **Plan.** Stitch band edges; a native test that walks every band border looking for gaps.

### F3. The player snags on facet edges
- **Likelihood:** Medium. **Cost if ignored:** High (feel).
- **Why.** A capsule sliding over triangles catches on the edges between them. It's a known problem in every triangle-collision game.
- **Warning signs.** Stops or hops when walking diagonally across slopes.
- **Act.** M1 step 4.
- **Plan.** Collide against the coarse facets only; smooth normals across shared edges; scripted walk tests over varied ground.

### F4. Rebuild storms when walking, on top of edits
- **Likelihood:** Medium. **Cost if ignored:** Medium.
- **Why.** Moving shifts every detail band, so many chunks want rebuilding at once.
- **Warning signs.** "Dirty waiting" climbing in F3 while walking; edits appearing late.
- **Act.** M1 step 3.
- **Plan.** Hysteresis on band edges; edits jump the queue; a cap on rebuilds per frame.

### F5. Material blending is expensive on the GPU
- **Likelihood:** Medium. **Cost if ignored:** Medium.
- **Why.** Up to 12 texture reads per pixel on border triangles.
- **Warning signs.** GPU world row rises with how many borders are on screen.
- **Act.** M1 step 5.
- **Plan.** Single-material path for interior triangles; dominant material only past a distance; the September review's two cheap cuts.

### F6. Chunk vertex data outgrows memory at long view distances
- **Likelihood:** Low. **Cost if ignored:** Medium.
- **Why.** The vertex grows from 8 to about 20 bytes, and there are more vertices near the player.
- **Warning signs.** Process or GPU memory near the budget at render distance 8.
- **Act.** When M1's mesher lands; the ledger records it.
- **Plan.** Coarse far chunks; free CPU copies once uploaded, except near the player (collision).

## Simulation and structure

### F7. The main thread fills up as systems arrive
- **Likelihood:** High (once action arrives). **Cost if ignored:** High.
- **Why.** Every system starts small and grows. This is the Minecraft trap.
- **Warning signs.** The main-thread CPU row creeping up milestone after milestone.
- **Act.** Every milestone (SOP section 4).
- **Plan.** The cost ledger; per-system caps; subtract before adding.

### F8. Game code creeps back into the engine
- **Likelihood:** Medium. **Cost if ignored:** High.
- **Why.** It's always faster in the moment to reach across.
- **Warning signs.** A layer-check failure, or a "temporary" exception.
- **Act.** Always; the layer check runs with every test run.
- **Plan.** Hooks, not includes; no exceptions to the layer rule without a logged decision.

### F9. Save format churn
- **Likelihood:** Medium. **Cost if ignored:** Medium.
- **Why.** Every feature wants to store something, and each change risks old saves.
- **Warning signs.** Several version bumps in a milestone.
- **Act.** When the first thing uses the game section.
- **Plan.** A tagged, length-prefixed record per system inside the game section, so a new record never breaks old saves and unknown records are skipped.

### F10. Precision far from the start
- **Likelihood:** Low. **Cost if ignored:** Low.
- **Why.** Positions are 32-bit floats.
- **Warning signs.** Jitter or physics oddities past about 100,000 blocks out.
- **Status.** Rendering is handled (M0.13). Physics and picking still use floats: revisit if the world is meant to be crossed that far.

## Sound

### F11. The effects voice starves during a hitch
- **Likelihood:** Medium. **Cost if ignored:** Medium.
- **Why.** Today it's topped up once per frame from the main thread, with only about 40 ms queued.
- **Warning signs.** Clicks or gaps in effects during saves, loading or window drags.
- **Act.** M1 step 7.
- **Plan.** Its own thread, like the music.

### F12. The music wears thin
- **Likelihood:** High. **Cost if ignored:** Low now, Medium later.
- **Why.** It's the same hour every hour (September review).
- **Warning signs.** It stops being noticed, or starts being turned off.
- **Act.** After M1, or when the new score is decided.
- **Plan.** Seeded per-day variation, or the new score.

## Process

### F15. Carried-over behaviour we didn't choose stays switched on
- **Likelihood:** High. **Cost if ignored:** Medium.
- **Why.** M0 carried Voxistics over whole, so everything it did still happens. Falling ground and grass die-back are live today, although parked for the first test (D12).
- **Warning signs.** Anything happening in play that isn't in the outline.
- **Act.** M1: the list in `reference/README.md` is worked through item by item.
- **Plan.** Every carried-over behaviour is either in the M1 scope or switched off, and the list is checked at the end of M1.

### F13. Scope creep inside a milestone
- **Likelihood:** High. **Cost if ignored:** High.
- **Why.** Good ideas arrive mid-work.
- **Warning signs.** Steps that didn't exist in the plan.
- **Act.** Always.
- **Plan.** New ideas go to the scope sheet as Could or Won't and wait (SOP section 1).

### F14. The docs drift from the code
- **Likelihood:** Medium. **Cost if ignored:** High.
- **Why.** Updating docs is easy to skip.
- **Warning signs.** A section describing something that isn't there.
- **Act.** Every step.
- **Plan.** Docs are part of done (SOP section 5); a check script flags code references to design sections that don't exist.
