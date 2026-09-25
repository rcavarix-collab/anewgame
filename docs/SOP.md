# walkgrid: standard operating procedure for adding anything

**Why this exists.** Games slow down and break when features pile up without rules: each one adds a little cost that runs all the time, touches systems it shouldn't, and never leaves. This procedure is the gate everything passes through, however small. It builds on `CLAUDE.md` (how we work) and `docs/FOUNDATIONS.md` (layers, threads, budgets).

**Status: draft for the owner's approval.** Once approved, `CLAUDE.md` points here and it's binding.

---

## 1. Before anything is built: the feature card

Every addition gets a short card in the plan for its step. No card, no code.

| Question | The answer must say |
|---|---|
| **What does the player get?** | One sentence, from the player's side. "It's cool" or "it might be useful" isn't an answer. |
| **Scope row** | Its row in `docs/SCOPE_MOSCOW.xlsx`. Only the owner sets Must; new ideas enter as Could or Won't. |
| **Layer and files** | Which layer it lives in, and every file it touches: kept, changed or new. |
| **What it costs** | Per frame and per tick when idle, and when busy. Memory. What its cost grows with. It must grow with what's on screen or what changed, never with world size or play time. |
| **Which budget pays** | Which line in FOUNDATIONS.md 4 it comes out of, and how much of it. If nothing has room, something else gives, or the feature waits. |
| **How it's switched off** | A setting, a quality level, or at least one clearly marked place in the code. Heavy effects always get a setting (the 2 GB-card floor). |
| **How it's checked** | Which tests here, and what the owner looks for on Windows. |
| **How it leaves** | If it doesn't earn its place: which files and lines go, and whether saves are affected. |

## 2. Hard rules

1. **Nothing always-on without a budget.** A feature that costs anything every frame or tick while nothing is happening needs the owner's explicit OK and a budget line. Work happens on change, not on a timer, wherever possible.
2. **One system per file, one layer per file.** The layer check (`tools/check_layers.py`) must pass. The engine never names the game.
3. **Every loop has a cap.** Anything that can grow (entities, particles, rebuilds, sounds, lights) has a hard per-tick or per-frame limit, and says what happens past it (queue, drop the farthest, drop to a coarser level). Never unbounded.
4. **No per-frame allocation** in hot paths. Buffers are reused.
5. **Saves are versioned on purpose.** A feature that stores something uses the game section, and a change to what's stored is a format decision logged in `DECISIONS.md`.
6. **Start-up stays fast.** Anything new at launch shows in the boot timeline, and is cached, deferred or done once.
7. **Photosensitivity, privacy and "nothing anyone owns"** apply to every feature (CLAUDE.md).

## 3. After it's built: the measurement

- **Before and after, in numbers.** The step's report gives the F3 or Ctrl+F3 figures for the affected rows. Mine come from what can be measured here (tests, triangle and vertex counts, offline audio). The frame-time rows come from yours.
- **A cost ledger.** `docs/COST_LEDGER.md` lists every system, its budget, and its last measured cost on the reference machine. It's updated whenever a system is added or changed, so the total is never a surprise.
- **No step lands with a system over its budget** unless the owner accepts it, and that acceptance is logged.

## 4. At every milestone: subtract before adding

- **Review the ledger.** Anything costing more than it gives, or that nobody uses, is proposed for parking or removal. That's the same move as taking The Line and pulse off in M0.
- **Parked means out of the build**, not switched off in place. Its code stays in the repository history; its art goes to a `parked` folder (as `assets/textures/parked/` does now).
- **Settle the performance baseline.** A Ctrl+F3 report on the reference machine is kept per milestone, and the next milestone is compared with it.
- **Look back and look ahead.** Write the milestone's entry in `REFLECTIONS.md`, review every open item in `FORECASTS.md` (did any warning sign appear?), update theory statuses in `THEORIES.md`, and bring `ROADMAP.md` up to date.

## 5. Documentation is part of done

A step isn't finished until all of these are true:
- every new or changed file has its header (what it is, what it costs, its layer, its DESIGN.md section);
- `DESIGN.md` describes the system as built, not as planned;
- `DECISIONS.md` has any decision the step made;
- the scope sheet's Status is updated;
- the ledger has the step's costs;
- any new idea we're relying on but haven't proven is in `THEORIES.md`, and any problem we can see coming is in `FORECASTS.md`.

Code and docs that disagree count as a bug.
