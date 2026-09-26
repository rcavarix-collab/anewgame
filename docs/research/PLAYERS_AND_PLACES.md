# Research: teaching without words, respecting players' time, and places people build

Three proposed research steps together (4, 5 and 6): how players learn a game with almost no text (D26); the ethics of engagement; and building theory for the building layer (W067). **Theory only (D66);** sources through search results **[s]**. Full citations: `BIBLIOGRAPHY.md` 138–145.

## 1. Teaching without words

### Affordances and signifiers
- **Gibson (1979), *The Ecological Approach to Visual Perception*:** an **affordance** is a possibility for action in the relation between an animal and its surroundings. A ledge affords climbing to someone tall enough; a gap affords jumping. It needn't be visible or known. [s]
- **Norman, *The Design of Everyday Things*:** took up the term in design. His revised edition (2013) added **signifiers**: affordances say what's possible, signifiers say **where and how**. [s]
- **For us:**
  - The world's shapes are its affordances. What can be climbed, what dug, and where the edge is must be readable **from the shape and material alone**, since there's no text.
  - **Signifiers for the unfamiliar:** a diggable face shows a crack or a grain when looked at; a placeable spot shows a faint outline. What the game shows today for a looked-at cell is to be checked in the running game (not confirmed from the code here).
  - Every new mechanic needs a signifier before it needs an explanation.

### Tutorials: evidence
- **Andersen, O'Rourke, Liu & Popović (2012), CHI:** eight tutorial designs in three games of different complexity, over 45,000 players. Tutorials raised play time by up to 29% in the **most complex** game, but **didn't significantly help the two simpler ones**. Mechanics that players can discover by trying don't need them. [s]
- **Caveat for us:** the study used telemetry at scale, which we'll never use (D23). Its finding is still ours to use.
- **For us:**
  - Walking, digging and placing can be discovered by trying, so **no tutorial**, only good signifiers.
  - A complex later mechanic (building layers, W067) may need a short **show-don't-tell** introduction: the world demonstrating it once, not words.

## 2. Respecting the player's time

- **Zagal, Björk & Lewis (2013),** "Dark Patterns in the Design of Games", Foundations of Digital Games: a **dark pattern** is a design that works against the player's interest for the maker's. [s] Examples:
  - **grinding:** tedious repetition that spends the player's time;
  - **appointment mechanics:** habits of daily play, often held by fear of loss;
  - monetary and social tricks.

  The paper gives questions for spotting new ones.
- **A counterpoint:** "Against Dark Game Design Patterns" (DiGRA 2020) argues the label can be applied too broadly, and that context and players' own views matter. [s]
- **For us:**
  - walkgrid has no money, no network and no telemetry (D23), which rules out most dark patterns by construction.
  - What remains to watch: **grind** (building shouldn't demand tedious gathering for its own sake), **appointments** (nothing that punishes not playing: the world waits, and the day clock stops when paused), and **fear of loss** (saves are durable, W070).
  - That's a checklist for each new mechanic's feature card, in line with the self-determination research (GAME_DESIGN.md 2): autonomy is undermined by pressure.

## 3. Places people build

- **Alexander, Ishikawa & Silverstein (1977), *A Pattern Language*, Oxford University Press:** 253 **patterns**, each a recurring problem in towns and buildings with the core of a solution. They're linked from region to room into a "language" anyone can use to make places. Examples of the kind: light on two sides of every room; a sheltered entrance; a window place to sit. [s; the example patterns, k] Much criticised and much used; its idea later shaped software design. [s]
- **Habraken's "supports"** [k]: a fixed structure that lets inhabitants shape their own spaces within it. A related idea from the same era.
- **For us, the building layer (W067):**
  - **Patterns, not blueprints.** The world can **suggest** good places through its landforms: a sheltered ledge with a view is Appleton's refuge with prospect and Alexander's window place in one. The player then builds there because it feels right.
  - **Structure plus freedom.** Habraken's split maps onto a building layer where the player sets supports (walls, floors) and fills in the rest freely.
  - **Places gather meaning through use** (Tuan, LANDSCAPE.md 7). Things built should weather, be revisited, and stay.

## 4. What to try

1. **A signifier audit:** for each action (walk, climb, dig, place, slide), what tells the player it's possible, with no words. Gaps get a signifier.
2. **No tutorial for the basics;** a show-don't-tell demonstration only for complex later mechanics.
3. **A dark-pattern check on every feature card:** grind, appointments, fear of loss (a line in the SOP's card; owner's call).
4. **The building layer:** landforms that suggest places, supports plus free fill, and building that lasts.

## Sources

All [s] unless marked; full citations in BIBLIOGRAPHY.md 138–145.
- Gibson 1979
- Norman 1988/2013
- Andersen, O'Rourke, Liu & Popović 2012
- Zagal, Björk & Lewis 2013
- "Against Dark Game Design Patterns", DiGRA 2020
- Alexander, Ishikawa & Silverstein 1977
- Habraken 1972 [k]
