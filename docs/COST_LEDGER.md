# walkgrid cost ledger

Every system, what it's allowed to cost (its budget), and what it was last measured costing on the reference machine (GTX 1060 3GB, 16 GB RAM, 1080p). The budgets come from `FOUNDATIONS.md` 4, and the rows match the F3 profiler. A system over its budget is fixed, or its overspend is accepted by the owner and logged in `DECISIONS.md`. Updated by every step that adds or changes a system (`SOP.md`).

**Measured:** figures come from the owner's F3 or Ctrl+F3 reports only. Current figures: the M0 baseline, 2026-09-25 (Release, 1280×720, render distance 3, dawn, 60.0 fps average). "Awaiting" means not measured yet.

## Per frame, main thread (budget total ≤ 5 ms)

| System | F3 row | Budget | Grows with | Measured | Notes |
|---|---|---|---|---|---|
| Terrain queueing and generation | TERRAIN | 0.8 ms | columns arriving (capped 4/tick) | 0.00 median, 0.49 worst | Moves to job threads in M1 |
| Column eviction | EVICT | 0.2 ms | columns leaving (capped 4/tick) | 0.00, 0.01 worst | |
| Player physics | PHYSICS | 0.2 ms | fixed | 0.00, 0.02 worst | |
| Scheduled updates | UPDATES | 0.3 ms | updates due (capped 64/tick) | 0.00, 0.01 worst | Nothing schedules them in walkgrid yet |
| Music synthesis | MUSIC | ~0 (own thread) | fixed | 0.00, 0.03 worst | |
| Soundscape and effects | WORLD SOUND | 0.5 ms | sounds playing (capped) | **0.54 median, 4.68 p95, 14.93 worst: over budget** | Moves to its own thread in M1 |
| Mesh rebuild and upload | MESH | 1.5 ms | dirty chunks (capped 6/frame) | 0.00 median, 2.35 worst | Building moves to job threads in M1 |
| Shadow map (when stale) | SHADOW MAP | 0.5 ms | chunks in the light's view (sliced over 4 frames) | 0.00, 0.07 worst | |
| World draw submission | WORLD DRAW | 0.8 ms | chunks in view | 0.02, 0.78 worst | |
| Post pass submission | POST | 0.1 ms | fixed | 0.01, 0.04 worst | |
| UI | UI | 0.3 ms | text on screen | 0.02, 0.13 worst | |

## Per frame, GPU (budget total ≤ 8 ms)

| Pass | F3 row | Budget | Grows with | Measured |
|---|---|---|---|---|
| Shadow map | GPU SHADOW | 1.5 ms | triangles in the light's view, when redrawn | 0.00, 0.03 worst |
| World | GPU WORLD | 5.0 ms | pixels × shader cost; triangles | 0.59 median, 0.94 worst |
| Post (SSAO, outlines, bloom) | GPU POST | 1.0 ms | screen size | 0.11, 0.20 worst |
| UI | GPU UI | 0.5 ms | text on screen | 0.00 |

## Other

| What | Budget | Measured |
|---|---|---|
| Worst single frame while walking into new ground or editing | ≤ 16.7 ms | 26.4 ms work (41.3 ms frame): **over**, from WORLD SOUND |
| Visible ground triangles (default settings) | ≤ 1.5 million | Awaiting (M1 preview counts first) |
| Process memory | ≤ 1.5 GB | Awaiting |
| GPU memory | ≤ 1 GB | Awaiting |
| Start-up to first frame | ≤ the M0 baseline | 0.99 s (cold shader cache): this is the baseline |

The per-system numbers above are the first split of the totals in FOUNDATIONS 4. The M0 baseline will show where they're wrong, and they'll be adjusted then (logged).
