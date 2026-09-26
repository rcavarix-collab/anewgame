# Research: game design theory, and walkgrid held up against it

The owner asked for this with a grain of salt: every creator has trouble seeing the flaws in their own design. So this note does two things:
1. It sets out the published theory on why people play and what makes play satisfying.
2. It says plainly where walkgrid meets that theory and where it doesn't yet.

It closes with what the research says about finding flaws you can't see yourself.

**Theory only (D66).** Our words; nothing copied. Most sources were confirmed through search results and abstracts **[s]**; none was read in full here. Full citations: `BIBLIOGRAPHY.md` 77–90.

**Grain of salt, applied to the theory too.** Game design theory is young:
- Much of it is frameworks and heuristics, not experiments.
- The experimental part rests mostly on players' own reports, and on correlations.
- Each item below says how strong its footing is.

## 1. Frameworks for thinking about a design

### MDA: mechanics, dynamics, aesthetics
**Hunicke, LeBlanc & Zubek (2004)**, from the Game Design and Tuning Workshop at GDC and presented at an AAAI workshop, split a game into three layers [s]:
- **mechanics:** the rules and systems;
- **dynamics:** how those behave in play;
- **aesthetics:** the feelings play produces.

The designer works from mechanics outward; the player meets the game from aesthetics inward. The paper names eight kinds of fun [k: from the paper, to check]: sensation, fantasy, narrative, challenge, fellowship, discovery, expression, and submission (play as a pastime).

**Footing:** a framework, not a finding, but the most cited paper in game design. Later authors criticise it for reducing "aesthetics" to a list and for leaving out the player's context [k].

**Use:** name which of the eight walkgrid is for, then check every feature against them.

### Caillois: the forms of play
**Caillois (1958; English 1961)** sorts play into four kinds [s]:
- **agon:** competition;
- **alea:** chance;
- **mimicry:** make-believe;
- **ilinx:** vertigo, the pleasure of disorienting motion.

Each kind runs along a spectrum from **paidia** (free, improvised play) to **ludus** (play structured by rules and goals).

**Footing:** sociology and philosophy; a classic lens, not a measurement.

**Use:** a sandbox like walkgrid sits toward paidia. The power slide and the long view from a ridge are ilinx. It's a vocabulary for what kind of play each feature feeds.

### Games as spaces for stories
- **Jenkins (2004), "Game Design as Narrative Architecture":** designers are less storytellers than **narrative architects**. Games are **spatial stories**, **evocative spaces** whose places carry meaning, with stories **enacted** by the player, **embedded** in the world, or **emergent** from play. [s]
- **Footing:** criticism and theory.
- **Use:** walkgrid has no written story, and needs none. Its stories are embedded in the landscape (LANDSCAPE.md: landmarks, traces, mystery) and emergent from what the player builds.

## 2. Why people play (the empirical part)

### Competence, autonomy, relatedness
**Ryan, Rigby & Przybylski (2006), *Motivation and Emotion*,** in four studies [s]:
- Games are enjoyed, and leave players feeling better, to the degree they satisfy three needs from self-determination theory:
  - **competence:** feeling effective and growing;
  - **autonomy:** acting by one's own choice;
  - **relatedness:** connection with others.
- Intuitive controls supported competence and autonomy, and both went with a sense of presence.

**Przybylski, Rigby & Ryan (2010), *Review of General Psychology*,** set this out as a general model [s].

**Footing:** the strongest empirical line here, peer-reviewed and replicated across games. It's still mostly players' own reports.

### Why people play online games
- **Yee (2006), *CyberPsychology & Behavior*,** by factor analysis of players' answers [s], found ten motivations in three groups:
  - **achievement:** progress, mechanics, competition;
  - **social:** socialising, relationships, teamwork;
  - **immersion:** discovery, role-play, customisation, escapism.
- These are components one player mixes, not exclusive "types" [k]. That's a correction to the older, unvalidated Bartle player types (1996) [k].
- **Use:** walkgrid is immersion-first (discovery, customisation, escape).

### Flow
- **Csikszentmihalyi (1990)** described **flow**: absorbed, effortless concentration when challenge matches skill, goals are clear and feedback is immediate. [k]
- **Sweetser & Wyeth (2005), *Computers in Entertainment*,** turned flow into **GameFlow**, a checklist for evaluating enjoyment with eight elements [s]:
  1. concentration;
  2. challenge;
  3. player skills;
  4. control;
  5. clear goals;
  6. feedback;
  7. immersion;
  8. social interaction.
- **Footing:** a synthesis of heuristics, checked against expert reviews of games [k]; useful as a checklist, not a law.

## 3. walkgrid against the theory: honest gaps

What the design says (DESIGN.md 1.1): you walk the faceted world, dig and build it one cell at a time, and hear it in step with the music. "What the game becomes beyond the first prototype is still open."

| Theory asks | Where walkgrid stands |
|---|---|
| **Which aesthetics?** (MDA) | Never named. Sensation, discovery, expression and submission (a calm pastime) fit what's built. Challenge, fellowship and narrative are absent. **Naming the target is the owner's call**, and it would let every future feature be judged against it. |
| **Autonomy** | Strong: go anywhere, change anything. |
| **Competence** | **Weak.** Nothing to get better at, nothing that pushes back, and little feedback that effort mattered. Without some growth, autonomy alone can feel aimless. This is likely the biggest gap. |
| **Relatedness** | Absent: single player, no characters. It can come without other players, through companions, creatures or traces of others in the world, or it can be consciously left out. |
| **Clear goals** (GameFlow) | None stated. A sandbox can let players set their own, but it usually offers prompts: something to find, make or reach. |
| **Feedback** | Good at the level of senses (footsteps, harmony-locked sounds, light), thin at the level of results. |
| **Immersion** | The strongest suit: the look, the synthesised sound, the day, no text in the way (D26). |
| **Discovery** | The landscape research (LANDSCAPE.md) is exactly this; the terrain today is fairly uniform, so there's little to find. |
| **Ilinx** | The power slide is a start; slopes, drops and the far view could carry more. |

**None of this is a flaw in what's been built.** M1 was about the world, and it's working. It's a list of what the theory says a player will look for next, and it lines up with R7's "a reason to stay" question and the building layer (W067).

## 4. Seeing flaws in your own design

### The designer's blind spot
- The **curse of knowledge:** once you know something, it's hard to imagine not knowing it. First described in economics (Camerer, Loewenstein & Weber 1989) [k] and a staple of usability work.
- A designer knows where everything is, what every control does and what they meant. A new player knows none of it.

### Heuristic evaluation
- **Pinelle, Wong & Stach (2008), CHI:** from reviews of 108 games across six genres, they drew **twelve common classes of usability problems** and a matching set of heuristics for inspecting a game's prototype. [s]
- Nielsen's work on heuristic evaluation found that one evaluator catches only a minority of problems; several independent evaluators together catch most [k].
- **Use:** an inspection against the heuristics is cheap. Claude can do one on the design, but it's no substitute for players.

### Playtesting methods
**Games User Research** (Drachen, Mirza-Babaei & Nacke, eds., Oxford University Press, 2018) collects the field's methods [s]:
- watching players;
- **think-aloud** (the player says what they're thinking as they play);
- heuristic evaluation;
- **RITE** (rapid iterative testing and evaluation: fix between testers).

Small usability tests with a handful of people find most of the big problems [k: Nielsen & Landauer 1993].

**Fullerton's *Game Design Workshop*** argues for "playcentric" design: playtest from the first prototype, and keep the player's experience as the goal throughout [k].

### What this means for us
- **The owner is the designer and, so far, the only tester.** That's exactly the case the blind-spot research warns about. Claude can't test either: it can't play the game, only read about it.
- **A few outside testers,** people who've never seen walkgrid, watched while thinking aloud with no help given, would show what neither of us can see. It fits our privacy rules exactly: observation in the room, no telemetry (D23).
- **A one-page protocol** would help:
  - what to watch for, without leading the tester;
  - what to ask afterwards (the three needs; what they wanted to do and couldn't);
  - a note-taking sheet.

## 5. Questions for the owner (to think about, not to answer now)

1. **Which of the eight aesthetics is walkgrid for?** Two or three primary ones are usually enough.
2. **Where does competence come from?** Mastery of movement, of building, of understanding the world, or of surviving it? Or is walkgrid deliberately a calm pastime, with competence left light?
3. **Relatedness:** others in the world (creatures, companions, traces), or consciously solitary?
4. **When should outside playtesters first see it?**

## Sources

All [s] unless marked; full citations in BIBLIOGRAPHY.md 77–90. Search results only; no site was fetched.
- Hunicke, LeBlanc & Zubek 2004
- Caillois 1958/1961
- Jenkins 2004
- Ryan, Rigby & Przybylski 2006; Przybylski, Rigby & Ryan 2010
- Yee 2006
- Csikszentmihalyi 1990 [k]
- Sweetser & Wyeth 2005
- Bartle 1996 [k]
- Camerer, Loewenstein & Weber 1989 [k]
- Pinelle, Wong & Stach 2008
- Nielsen & Landauer 1993 [k]
- Drachen, Mirza-Babaei & Nacke 2018
- Fullerton [k]
