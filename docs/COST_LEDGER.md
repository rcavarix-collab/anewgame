# walkgrid cost ledger

Every system, what it's allowed to cost (its budget), and what it was last measured costing on the reference machine (GTX 1060 3GB, 16 GB RAM, 1080p). The budgets come from `FOUNDATIONS.md` 4, and the rows match the F3 profiler. A system over its budget is fixed, or its overspend is accepted by the owner and logged in `DECISIONS.md`. Updated by every step that adds or changes a system (`SOP.md`).

**Measured:** figures come from the owner's F3 or Ctrl+F3 reports only. "Awaiting" means no report yet: the M0 baseline fills this column.

## Per frame, main thread (budget total ≤ 5 ms)

| System | F3 row | Budget | Grows with | Measured | Notes |
|---|---|---|---|---|---|
| Terrain queueing and generation | TERRAIN | 0.8 ms | columns arriving (capped 4/tick) | Awaiting | Moves to job threads in M1 |
| Column eviction | EVICT | 0.2 ms | columns leaving (capped 4/tick) | Awaiting | |
| Player physics | PHYSICS | 0.2 ms | fixed | Awaiting | |
| Scheduled updates | UPDATES | 0.3 ms | updates due (capped 64/tick) | Awaiting | Nothing schedules them in walkgrid yet |
| Music synthesis | MUSIC | ~0 (own thread) | fixed | Awaiting | |
| Soundscape and effects | WORLD SOUND | 0.5 ms | sounds playing (capped) | Awaiting | Moves to its own thread in M1 |
| Mesh rebuild and upload | MESH | 1.5 ms | dirty chunks (capped 6/frame) | Awaiting | Building moves to job threads in M1 |
| Shadow map (when stale) | SHADOW MAP | 0.5 ms | chunks in the light's view (sliced over 4 frames) | Awaiting | |
| World draw submission | WORLD DRAW | 0.8 ms | chunks in view | Awaiting | |
| Post pass submission | POST | 0.1 ms | fixed | Awaiting | |
| UI | UI | 0.3 ms | text on screen | Awaiting | |

## Per frame, GPU (budget total ≤ 8 ms)

| Pass | F3 row | Budget | Grows with | Measured |
|---|---|---|---|---|
| Shadow map | GPU SHADOW | 1.5 ms | triangles in the light's view, when redrawn | Awaiting |
| World | GPU WORLD | 5.0 ms | pixels × shader cost; triangles | Awaiting |
| Post (SSAO, outlines, bloom) | GPU POST | 1.0 ms | screen size | Awaiting |
| UI | GPU UI | 0.5 ms | text on screen | Awaiting |

## Other

| What | Budget | Measured |
|---|---|---|
| Worst single frame while walking into new ground or editing | ≤ 16.7 ms | Awaiting |
| Visible ground triangles (default settings) | ≤ 1.5 million | Awaiting (M1 preview counts first) |
| Process memory | ≤ 1.5 GB | Awaiting |
| GPU memory | ≤ 1 GB | Awaiting |
| Start-up to first frame | ≤ the M0 baseline | Awaiting |

The per-system numbers above are the first split of the totals in FOUNDATIONS 4. The M0 baseline will show where they're wrong, and they'll be adjusted then (logged).
