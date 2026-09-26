# Research: sound design, the world's sounds and how they're heard

The owner's aim: sound design, meaning the world's effects (footsteps, digging, placing, landing, ambience) and how the ear reads them, alongside MUSIC.md. Sources: ecological acoustics, psychoacoustics, physically based synthesis, film sound theory, game sound studies and virtual reality research.

Held up against `sfx_synth.cpp`, `soundscape.cpp` and `docs/SOUND_PALETTE.md`.

**Theory only (D66):** our words, and no one's code or patches; every sound stays synthesised (D25). Most sources were confirmed through search results and abstracts **[s]**; none was read in full here. Full citations: `BIBLIOGRAPHY.md` 111–122.

## 1. We hear events, not sounds

- **Gaver (1993), "What in the world do we hear?", *Ecological Psychology*:** in everyday listening we hear **events and their sources** (something hitting, scraping, rolling, dripping), not pitch and timbre as a musician does. [s]
- Sound-making events divide by what vibrates: **solids, liquids and gases**. Within each come a few basic interactions:
  - for solids: impacts, scraping, rolling, deformation;
  - for liquids: drips, pours, splashes;
  - for gases: wind, whooshes, explosions.

  More complex sounds are these in **temporal patterns** (a bounce, a run of footsteps), **compounds** or **hybrids**. [s]
- **For us, a design grammar.** Every world sound should answer three questions:
  1. What material is it?
  2. What interaction: impact, scrape, crumble, roll?
  3. What pattern in time?

  A dig is scrape plus crumble; placing is an impact; a landing is an impact with a body behind it; footsteps are impacts in a gait pattern. Sounds built this way read as events in the world, which the harmony lock (SOUND_PALETTE 1) can then colour musically.

## 2. How the ear tells materials apart

**Klatzky, Pai & Krotkov (2000), *Presence*.** Read in full [r]: the authors' preprint, supplied by the owner.

**The physics they build on:**
- A struck object's sound is a sum of decaying sinusoids, its **modes**.
- Each mode's frequency depends on the object's stiffness, density, shape and how it's held.
- Its **decay** depends mostly on the material's internal friction (damping), which is **shape-invariant**: the same for a small or large piece of the same material.
- In the model they use, the time for a mode to fall by a factor e is **inversely proportional to its frequency**, so **higher partials die sooner than lower ones**. One number per material (a decay parameter) sets how fast.

**What they did:** four experiments with synthesised struck-bar sounds, varying pitch and that decay parameter.
- Listeners rated how likely two sounds were to come from the same material.
- As a control, one group was asked about the bar's length instead.
- Finally, listeners sorted sounds into rubber, wood, glass and steel.

**What they found:**
- **Decay is the main cue to material,** about **twice as strong as pitch**, with the two acting independently. Together they explained around four-fifths of the similarity judgements.
- **It's the rate of decay itself,** not total energy or how long the sound lasts: holding energy roughly constant didn't change the result.
- **The control confirmed it's about material:** asked about length, listeners leaned on decay much less.
- **Categories fall in order of decay:** rubber shortest, then wood, glass, and steel longest, matching measured damping of those materials. **Glass** differs from **steel** mainly by being **higher-pitched**: people judge categories by pitch and decay together. (Their tables of fitted values aren't copied, D68; the order and the roles are what matter.)
- **Frequency-dependent decay is essential.** In an informal test, the same modes with **one decay rate for all frequencies didn't sound like a single uniform material.**
- **For games:** a one-parameter material model is enough to tell materials apart and to sort them into categories, which matters because audio gets a tiny share of the processor (they cite about 3% in games of the time).

**Against ours:**
- `Hardness()` sets how footsteps and landings sound per material. **For granular ground that's right:** sand, earth and snow crunch longer as grains settle, a different physics from a struck solid (Cook, section 3).
- **For impacts on solids, decay is the main cue,** and SOUND_PALETTE 1.6 caps metallic partials to die within about 60 ms. That squeezes out the cue that tells stone from glass from metal.
- **Two changes, both within D25 and the harmony lock:**
  1. **Frequency-dependent decay in every struck voice:** each partial's decay time inversely proportional to its frequency, with one decay number per material. It's cheap, it's what makes a voice sound like one object, and it helps even inside the 60 ms cap.
  2. **Longer decay for harder materials** (stone, crystal, glass, metal), on a **safe pitch from the current chord**, and quiet. **Owner's call,** since it relaxes 1.6's cap for impacts on solids.

## 3. Synthesising contact sounds

- **Modal synthesis: van den Doel, Kry & Pai (2001), "FoleyAutomatic", SIGGRAPH.** A struck object's sound is modelled as a small set of **modes**: resonant frequencies, each with its own decay and gain. The modes are driven by contact forces computed at **audio rate**, faster than the frame rate, so continuous contact (scraping, rolling) sounds smooth and physical in real time. [s]
- **Cook (2002), *Real Sound Synthesis for Interactive Applications*,** and his physically informed stochastic models [s, k for details]:
  - **granular sounds** (gravel, sand, crumbling, rain) as many tiny random impacts whose rate and colour follow the physics: the **number** of grains, their **size** and the **energy** of the event;
  - walking sounds analysed and resynthesised with parameters for the walker's gait.
- **Farnell (2010), *Designing Sound*, MIT Press:** sound as a **process, not data**. Each effect is built from first principles: analyse the physics, model it, then synthesise it. [s] (Already cited in the round-2 appraisals, E14.)
- **Against ours:** our palette already builds from bodies, subs and "grain" layers per material. Two cheap upgrades:
  1. **Modes per material:** two or three resonances with their own decays, pitched to safe tones, for placing and landing.
  2. **Grain counts from the event:** a dig on sand sets many small grains, a dig on stone fewer and bigger chunks, and a harder swing more energy.

  Both stay synthesis-only and harmony-locked.

## 4. Footsteps

- **Nordahl, Serafin & Turchet (2010), IEEE Virtual Reality:** a system synthesising footsteps on different surfaces in real time, driven by the walker's own steps. [s]
- **Turchet, Serafin & Cesari (2013), *ACM Transactions on Applied Perception*:** interactively synthesised footstep sounds simulating different terrains **changed how people actually walked**, affecting their pace. [s: the direction and size of the effect, to check]
- **For us:**
  - Footsteps aren't decoration: they shape how movement **feels**.
  - A step sound that doesn't match the stride can make the body feel disconnected.
  - That supports the pending decision (round2_E_part2; MUSIC.md 6): **stride timing quantised to the nearest beat subdivision within a small window.** It keeps entrainment to the music and the link to the body.
  - Each ground material should also change the step's pattern, not just its colour: sand shuffles, stone clicks, grass swishes.

## 5. Distance and space

- **Zahorik, Brungart & Bronkhorst (2005), *Acta Acustica united with Acustica*:** people judge a sound's distance from several cues [s]:
  - loudness;
  - the **direct-to-reverberant ratio:** far sounds have more room sound relative to direct sound;
  - **high frequencies lost over distance** [k: the air-absorption cue, to check in the paper];
  - familiarity with the source.

  Perceived distance grows more slowly than real distance, so far sounds seem nearer than they are. [s]
- **Against ours:** distance changes **only the gain**, gently (`b.dist = 1/(1 + (d − 3)/8)`), with no loss of brightness and no room sound. So a far sound is just a quieter near one.
- **Try (cheap):** darken with distance (a lowpass cutoff that falls with d). Together with the census-driven reverb (MUSIC.md 5), the reverb's share rises with distance. Distance then reads as distance.

## 6. Sound and picture together

- **Chion, *Audio-Vision* (1990; English 1994) [s]:**
  - **Synchresis:** a sound and an image that happen together fuse into one event, even when the sound isn't the "real" one.
  - That's the licence for every synthesised effect: the **timing** sells it more than the likeness.
- **For us:**
  - The sync must be tight. Sounds placed on the beat (tier 2, "next beat") trade sync for musicality, and past a small delay synchresis weakens.
  - Keep **interaction sounds immediate** (tier 1 already does), and delay only the colour layers to the beat.

## 7. What sounds do for the player

- **Jørgensen, *Left in the Dark* (2008), and her game-sound studies** [s]: players with the sound off lost information about off-screen events, reacted more slowly and learned more slowly. Sound **informs** (what happened, where, how well), **draws attention**, **guides**, and supports **learning**.
- **Schafer, *The Soundscape* (1977) [s, k for terms]:**
  - **keynotes:** the background everyone hears but no one listens to (wind, the hum of a place);
  - **signals:** foreground sounds meant to be noticed;
  - **soundmarks:** sounds unique to a place, like landmarks for the ear.
- **For us:**
  - **Informing:** a dig should sound different when it's about to break through, and a placed cell different when it's unsupported. These are feedback for the competence gap (GAME_DESIGN.md 3).
  - **Keynotes by region:** each district has its own background (LANDSCAPE.md rule 5).
  - **Soundmarks:** a landmark has a sound of its own, heard before it's seen: water in a gorge, wind in a rock arch.
  - **Accessibility:** anything sound tells the player, the screen should also show, gently, for players who can't hear it (see the proposed accessibility step).

## 8. What to try (ours; owner's calls marked)

1. **The event grammar** (material × interaction × pattern) as the checklist for every world sound. Free.
2. **Distance darkening,** and a reverb share that grows with distance. Cheap.
3. **Modes per material for impacts,** pitched to safe tones, with each partial's decay inversely proportional to its frequency (free), and a longer overall decay for harder materials. **Owner's call:** the longer decay relaxes SOUND_PALETTE 1.6's 60 ms cap.
4. **Grain counts from the event** for digs and crumbles. Cheap.
5. **Footsteps on the stride, quantised to the beat,** with patterns per surface. Owner's call, pending.
6. **Informative sounds:** break-through and unsupported cues. Design work.
7. **Keynotes and soundmarks** by region and landmark; needs terrain v2's regions.

## Sources

All [s] unless marked; full citations in BIBLIOGRAPHY.md 111–122. Search results only; no site was fetched.
- Gaver 1993
- Klatzky, Pai & Krotkov 2000 [r]
- van den Doel, Kry & Pai 2001
- Cook 2002; Cook 1997 (PhISM)
- Farnell 2010 (bibliography 4)
- Nordahl, Serafin & Turchet 2010
- Turchet, Serafin & Cesari 2013
- Zahorik, Brungart & Bronkhorst 2005
- Chion 1990/1994
- Jørgensen 2008
- Schafer 1977
