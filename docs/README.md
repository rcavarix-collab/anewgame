# walkgrid docs: what lives where

Each document has one job. If you're looking for something, this says where it is. If you're writing something, this says where it goes.

| Document | Its one job | Updated when |
|---|---|---|
| `PROJECT_NOTES.md` | Everything in one place for a newcomer (owner, Claude or another agent): what walkgrid is, what's built, the code map, how to add things, what bit us, what's next | With every step that changes any of those |
| `AGENT_TASKS.md` | Work a helper agent can do between Claude sessions: rules, what's off limits, the task list, a report template, a log | When tasks are done or added, at each milestone |
| `agent_packs/` | One ready-to-paste text per helper task (Brief and task, facts in plain words), made by `tools/make_agent_packs.py` | Rerun the tool whenever `AGENT_TASKS.md` changes; update a task's facts when its code changes |
| `../CLAUDE.md` | How we work: the rules every session follows | Rarely; changes are logged in DECISIONS |
| `../DESIGN.md` | How each system works, as built | Every step that changes a system |
| `GOALS.md` | What we're aiming for, as checkable goals | When the owner sets or changes a goal |
| `ROADMAP.md` | The order we'll get there in: milestones and status | Every milestone, and when plans change |
| `THEORIES.md` | Ideas we believe but haven't proven, and how we'll test each | When an idea is proposed, tested or settled |
| `FORECASTS.md` | Problems we expect before they happen, their warning signs, and when to act | Reviewed at every milestone |
| `REFLECTIONS.md` | Looking back: what went well, what didn't, what we'll change | At the end of every milestone, and after anything goes wrong |
| `DECISIONS.md` | Every decision, dated, so none is reversed quietly | Whenever a decision is made |
| `SOP.md` | The gate every addition passes through | Rarely |
| `FOUNDATIONS.md` | Layers, threads, budgets and data decisions: the structure | When the structure changes (a decision) |
| `COST_LEDGER.md` | Every system's budget and measured cost | Every step that adds or changes a system |
| `SCOPE_MOSCOW.xlsx` | What's in and out of the current horizon | As work lands |
| `PROTOTYPE_OUTLINE.md` | What the first prototype is, and its test checklist | Until M1 is done |
| `M0_PLAN.md` | The step-by-step plan for M0 | Done: a record |
| `M1_PLAN.md` | The step-by-step plan for M1, with what you'll see after each step | Until M1 is done |
| `ENGINE_REVIEW.md` | What the Voxistics engine offered (September 2026) | Fixed: a snapshot |
| `SOUND_PALETTE.md` | The sound palette's rules (carried over) | When the palette changes |
| `../reference/` | Old code and docs kept only for reference (the seed files: four prototypes and two texture generators; Voxistics' design, review and scope), each removed once nothing more is wanted from it. See its README | When something is taken from it, or it's retired |

**How they connect.** A goal (GOALS) is reached through milestones (ROADMAP). Along the way, ideas are tested (THEORIES) and problems are watched for (FORECASTS). Every choice is logged (DECISIONS). Every build is measured (COST_LEDGER) and looked back on (REFLECTIONS). A reflection or a forecast that comes true usually produces a new decision or a new rule.
