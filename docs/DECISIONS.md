# walkgrid decision log

One line per decision: what was decided, when, by whom, and why. A decision is only reversed by a new entry that says so. Nobody quietly overrides an old one; this log exists because that happened last time.

| # | Date | Decision | By | Why / where |
|---|---|---|---|---|
| D1 | 2026-09-25 | The Voxistics engine is carried across intact; only the Voxistics game layer comes off. No rewriting of working systems from memory. | Owner | The previous attempt rewrote working systems into weaker ones. ENGINE_REVIEW.md |
| D2 | 2026-09-25 | Music stays as Voxistics' procedural day music for now; death metal is deferred. | Owner | Real instruments are hard to emulate convincingly |
| D3 | 2026-09-25 | Reactive sound effects must not interfere with the music (harmony-locked palette, voice cap, effects off the main thread). | Owner | PROTOTYPE_OUTLINE.md D3 |
| D4 | 2026-09-25 | Textures and lighting carried over; sky light added so hollows go dark. | Owner (sky light: proposed) | ENGINE_REVIEW.md 2.3 |
| D5 | 2026-09-25 | World is a faceted surface over the existing cell grid (option A), not cubes and not smooth blobs. | Owner | PROTOTYPE_OUTLINE.md 3 |
| D6 | 2026-09-25 | Smaller ground triangles, without hurting performance: subdivide only where it shows. | Owner | FOUNDATIONS.md 4.1 |
| D7 | 2026-09-25 | Natural material transitions by height-based blending. | Owner | PROTOTYPE_OUTLINE.md 4 |
| D8 | 2026-09-25 | Smooth-filtered textures, not pixel-crisp. | Owner | |
| D9 | 2026-09-25 | First person only for the prototype; no player model. Third person later, if wanted. | Owner | |
| D10 | 2026-09-25 | 60 fps is the floor; higher where the machine allows, with no jitter (interpolated rendering). | Owner | |
| D11 | 2026-09-25 | Working name: walkgrid. | Owner | |
| D12 | 2026-09-25 | Parked until after the first test: falling ground, grass die-back, facing plants. | Owner | Not needed yet |
| D13 | 2026-09-25 | Foundations approved: working rules, layer rule, thread model, budgets, data decisions (fresh save format v1, 1-block cells, versioned generator, camera-relative rendering). | Owner | FOUNDATIONS.md |
| D14 | 2026-09-25 | Scope is tracked with a fresh MoSCoW sheet (`docs/SCOPE_MOSCOW.xlsx`); only the owner sets Must. | Owner | |
| D15 | 2026-09-25 | Reference machine: GTX 1060 3GB, 16 GB RAM (about six years old). Floor: 8 GB RAM, 2 GB graphics card. The game never reads the machine; these are for our budgets only. | Owner | FOUNDATIONS.md 4 |
| D16 | 2026-09-25 | Render only what can be seen; preload what may be seen soon. | Owner | FOUNDATIONS.md 4.3 |
| D17 | 2026-09-25 | M0 plan approved; work starts. Screenshot key is F2 (F12 breaks into the Visual Studio debugger). | Owner (F2: proposed) | M0_PLAN.md |
