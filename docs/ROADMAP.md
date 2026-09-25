# walkgrid roadmap

The order we get to the goals (`GOALS.md`) in. Each milestone ends with the owner's check on Windows, a reflection (`REFLECTIONS.md`) and a forecast review (`FORECASTS.md`). A milestone is only started once the one before it is done.

## Now

**M0: separate the engine from Voxistics.** *In progress (2026-09-25).* Plan: `M0_PLAN.md`.
- Done: screenshot key, check scripts, docs, The Line / pulse / fliers / essence off, generic glow, fresh save format, persist split, rename, game.cpp split, smooth motion, camera-relative rendering.
- Left: raw mouse input, back-face culling (only if a test proves it's safe).
- Then: the owner's Windows build, baseline screenshots compared, and a Ctrl+F3 report as the performance baseline.

## Next

**M1: the first functional test.** Scope: `PROTOTYPE_OUTLINE.md`. Suggested order, so the riskiest ideas are proven first (see `THEORIES.md`):
1. The CPU preview tool: still images and triangle counts of faceted ground. This settles T1–T3 before the engine changes.
2. The test landscape generator (versioned).
3. The facet mesher with detail bands, on the job threads.
4. Collision and picking on facets.
5. Material blending and sky light in the world shader.
6. Starting materials, and their sounds.
7. Effects audio on its own thread.
8. Hidden-chunk skipping and heading-aware loading.

## Later (Won't for this horizon; parked, not rejected)

In no fixed order until the owner decides:
- action systems (enemies, projectiles, particles) with budgets;
- a real terrain generator with caves;
- third person and a player model;
- death metal music;
- see-through materials;
- plants that face you;
- falling ground;
- grass die-back;
- gamepad support.
