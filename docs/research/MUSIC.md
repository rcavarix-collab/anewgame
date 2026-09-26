# Research: music theory, perception and synthesised orchestration

Held up against walkgrid's music (DESIGN.md Part XIV) and world sound (docs/SOUND_PALETTE.md). Sources span psychoacoustics, music cognition, neuroscience, orchestration and computer music.

**Theory only (D66):** our words, and no one's code, scores or tables. Rules we'd adopt are our own restatements; any numbers would come from our own measurements, never from a paper's tables (D68). Most sources were confirmed through search results and abstracts **[s]**; none was read in full here. Full citations: `BIBLIOGRAPHY.md` 91–110.

**What we have, in one paragraph.** An hour-long piece in D Dorian, keyed to the day clock, in six sections:
- one four-chord cycle all hour (Dm9, G7sus4, Em7, A7sus4);
- tempo 120–124 BPM in the day, none at night;
- a day-long lowpass arc for brightness;
- additive and subtractive voices: sine, a soft triangle, and band-limited saw and pulse;
- pads in three registers that share oscillators across chords;
- a soft kick pulse and a ducked bass;
- mono and dry.

World sounds take their pitches from safe sets against the current chord, follow the music's brightness, and share its timbres and a tempo-synced echo.

## 1. Consonance: what makes a chord smooth or rough

- **Plomp & Levelt (1965), *JASA*:** in listening tests, two tones sound rough when they're close enough to fall **within the same critical band** of the ear; beyond it, they sound smooth. [s]
  - The band is wider, relative to pitch, **at low frequencies**, so an interval that's smooth high up is rough low down.
  - In chords by Bach and Dvořák, the composers had spaced their low notes to respect this. [s]
- **Helmholtz's older idea** (beating between neighbouring partials), which they support, extends this to rich tones: two complex tones are rough when their **partials** collide. [s]
- **Grain of salt: McDermott et al. (2016), *Nature*.** The Tsimane', an Amazonian society with little exposure to Western music, rated consonant and dissonant chords **equally pleasant**. They still heard roughness as unpleasant; they just didn't dislike dissonant chords. City dwellers in the same country preferred consonance, less strongly than US listeners. **A liking for consonance is largely learned.** [s] It's real for most of our players, but not a law of nature.

**Against ours:**
- The safe-set rule (no semitone against a sounding chord tone) handles the **pitch-class** half of consonance.
- It ignores **register.** Checked in `music_synth.cpp` (`kChords`, `FoldInto`):
  - **The low pads (80–160 Hz) are already right.** They take only each chord's root and fifth (open fourths and fifths: D3 over A2, G2 and D3, E2 and B2, A2 and D3), which is exactly the wide-low spacing the theory calls for.
  - **The mid pads (175–350 Hz) take every chord tone, folded into one octave.** For Dm9 that's A3, C4, D4, E4, F4: a **semitone, E4 against F4, around 330–350 Hz**, plus whole tones either side. A semitone there sits well within one critical band, so it's the roughest spot in the track. The ninth chord's colour, voiced this closely, is paid for in muddiness.
  - **World sounds with deep bodies** follow the bass and are fine. Their upper bodies land in the mid register too.
- **Try (ours):**
  - an offline **roughness check** in `sound_demo.sh analyze`: for every voicing the track can produce, sum a roughness estimate over its pairs of partials, using our own implementation of the critical-band idea, and flag close intervals;
  - **open the mid voicing**, so colour tones sit an octave apart from the tones they clash with: the 9th (E) in the high pads only, the 3rd (F) in the mid. The same for the 7ths. The high register (390–720 Hz) already takes the upper tones, so this may only need the mid register's masks changed. Compare by ear.

## 2. Voice leading, streams and clarity

- **Huron (2001), "Tone and Voice", *Music Perception*:** the traditional rules of voice leading follow from a handful of perceptual principles, mostly **auditory stream segregation**: how the ear groups sounds into separate lines. [s] Won the Society for Music Theory's publication award.
- **The principles, in our paraphrase** [k: Huron's list, to check against the paper]:
  - lines stay distinct when **near in pitch within** a line and **apart between** lines;
  - tones that are harmonic and in the middle of the hearing range read as clear pitches;
  - lines moving **in parallel fifths or octaves fuse into one**;
  - onsets together fuse, onsets apart separate.
- **Bregman's *Auditory Scene Analysis* (1990)** is the underlying theory of streaming [k].

**Against ours:**
- The shared-oscillator pad bank, where a tone common to two chords sustains through the change, is **voice leading by construction:** pitch proximity, and no voices added in a crossfade.
- The **motif and countermelody** stay separate only while they differ in register, timbre and rhythm. At Midday they're close: A4–G5 against D4–C5, pulse against blend.
- **Try:** check that the two lines never cross, and never move in parallel octaves or fifths for long. The world sounds' "ladder" gestures should avoid doubling the motif's current pitch in unison or octave, or they'll fuse with it and blur.

## 3. Timbre: the dimensions of sound colour

- **Grey (1977), *JASA*:** listeners judged the similarity of 16 instrument tones. Three dimensions explained the results [s]:
  - how energy is spread across the spectrum (**brightness**);
  - **change within the tone over time** (spectral fluctuation);
  - high-frequency energy in the **attack**.
- **McAdams et al. (1995), *Psychological Research*:** the dimensions matched measurable descriptors: **attack time, spectral centroid** (brightness) and **spectral flux** (how much the spectrum changes). A melody is easier to pick out from a distractor the further apart their timbres are on these dimensions. [s]

**Against ours:**
- **Brightness:** handled beautifully at the scale of the day, with the lowpass arc. That's orchestration on the brightness axis.
- **Spectral flux:** low. Our voices have **fixed spectra**: sine, soft triangle, pulse at a fixed width per section, a filtered saw. Real instruments' partials rise and fall at different rates within each note, which is much of what makes them sound alive rather than electronic.
- **Attack:** one kind of shape (the raised cosine), deliberately gentle; that's right for calm.
- **Try (cheap, our own):**
  - slow, per-voice drift in partial balance, such as the soft triangle's 3rd and 5th partials breathing at their own slow rates;
  - a little pulse-width motion within a note;
  - a brightness that falls through a note's decay, as struck and plucked tones do.

  Each is a few multiplies per control block (every 64 samples), not per sample.

## 4. Orchestration: blend and contrast

- **Classic orchestration** (Rimsky-Korsakov, *Principles of Orchestration*, 1913, now public domain [k]; Adler, *The Study of Orchestration* [k]) teaches:
  - space chords like the harmonic series, wide at the bottom;
  - give the melody a timbre distinct from the accompaniment;
  - double a line at the octave for strength, and blend instruments of similar attack and brightness.
- **Timbre research agrees:** sounds **blend** when their attacks and brightness are similar, and **separate** when they differ (McAdams' later work on blend [k]).

**Against ours:**
- The pads blend well: similar soft attacks, one shared filter.
- The melodic layers stand apart by pulse timbre.
- The world sounds share the track's oscillators on purpose, so they belong. The theory warns that too much similarity also means **masking**, which is the same concern as Krause's acoustic niches (LANDSCAPE.md 6). Each world sound should sit in a register or rhythm the music isn't using at that moment.

## 5. Space: reverberation

- **Schroeder (1962), *Journal of the Audio Engineering Society*:** natural-sounding artificial reverberation from a few recirculating delays (comb filters) and all-pass filters, cheap enough for the hardware of its day. [s]
- It's the foundation of most algorithmic reverbs since. Later refinements: Moorer (1979) [k]; feedback delay networks (Jot) [k].

**Against ours:**
- The music is **mono and dry** (DESIGN.md 10.3); the world sounds have a tempo-synced ping-pong echo, but no room.
- Reverb is how the ear judges the size and material of a space. A cave that sounds like a meadow breaks immersion quietly.
- **Try:** a **world-sound reverb that follows the soundscape census.** The census already knows how enclosed the player is and what surrounds them. Short, bright reflections in stone caves; almost none on open hills; a longer, darker tail in big enclosed spaces.
- **Cost:** a few delay lines on the effects thread. The music can stay mono and dry by design; whether the music also gets a hint of space is the owner's call, since it touches D2 and 10.3.

## 6. Expectation, reward and repetition

- **Salimpoor et al. (2011), *Nature Neuroscience*:** during music that gives listeners chills, dopamine is released at the **peak**, and also **before it, in anticipation**, in a different pathway: one tied to prediction, the other to emotion. [s]
- **Huron's *Sweet Anticipation* (2006)** and **Meyer's *Emotion and Meaning in Music* (1956)** explain musical pleasure through expectation: its build-up, its fulfilment and its surprises [k].
- **Juslin & Västfjäll (2008), *Behavioral and Brain Sciences*:** music stirs emotion through several distinct mechanisms, not one [s]:
  - brain stem reflexes (sudden loud or dissonant sounds);
  - evaluative conditioning;
  - emotional contagion;
  - visual imagery;
  - **episodic memory;**
  - **musical expectancy.**

  **Rhythmic entrainment** was added in later work [k].
- **Repetition:** Margulis's *On Repeat* (2014) [k]: repetition is central to how music is enjoyed, but its effect changes with familiarity. Game-audio scholarship (Collins, *Game Sound*, 2008 [k]) notes the fatigue of loops heard for hours.
- **Tempo and mode: Husain, Thompson & Schellenberg (2002), *Music Perception*.** They varied one piece's tempo and mode: **tempo moved arousal**, and **mode moved mood**. [s]

**Against ours:**
- **Brain stem reflex:** deliberately avoided (no sudden sounds, SOUND_PALETTE 1.6). Right for a calm game.
- **Expectancy:** gentle and constant. The day has an arc, but little tension and release: one four-chord cycle all hour, with no cadence and no arrival. That's in the spirit of ambient music (Eno's aim, "as ignorable as it is interesting", in his 1978 liner notes [k]), and right as a bed, but it gives no peaks.
- **Episodic memory:** unused. A motif heard only at a certain landmark, or at sunrise, becomes a memory the music can recall later.
- **Repetition:** the same hour, every hour. Over long sessions, the hour's variety matters more than any one minute of it.
- **Tempo:** 120–124 BPM is moderately arousing. For a calm game the theory suggests slower tempos would be calmer. It's a look-and-feel choice for the owner, not a flaw.
- **Entrainment and the footsteps:** people naturally fall into step with a beat. That gives the beat-locked footsteps (round2_E_part2) a real argument, alongside the stride's. The compromise (stride quantised to the nearest subdivision within a small window) gets both.
- **Try:**
  1. **Rare arrivals.** At a few moments, harmony resolves where the world gives a moment: the sun clearing the horizon; the first sight of a landmark or vista (LANDSCAPE.md rules 6–7), as a quiet cadence or a held chord.
  2. **Places with motifs.** A region or landmark has its own short motif, heard when there, recalled faintly elsewhere.
  3. **More harmonic variety across the hour,** or across days: other modes or chord cycles per region or weather. The world sounds follow automatically, since they read the harmony from `MusicHarmonyAt`.
  4. **Tempo as a feel choice:** try slower day tempos side by side.

## 7. Synthesis methods worth knowing

- **Frequency modulation: Chowning (1973), *JAES*.** One oscillator modulating another's frequency gives rich spectra that evolve over time, controlled by one index, from two oscillators. [s] It's the cheapest route to spectral flux, bells, woods and metals.
- **Plucked strings: Karplus & Strong (1983), *Computer Music Journal* [k].** A short noise burst through a delay line with gentle filtering gives a natural pluck decaying on its own: very cheap, organic.
- **Physical modelling** (Smith, *Physical Audio Signal Processing*, CCRMA, an online book [k]): instruments as models of strings, tubes and bodies. It's the principled route to convincing plucks, blown and bowed tones, and eventually the deferred heavy music (D2).
- **Band-limited oscillators:** our PolyBLEP saw and pulse follow Välimäki and colleagues' work on anti-aliased oscillators [k].
- **All of these are synthesis,** and within D25. None needs a sample.

## 8. Generative game music

- **Plut & Pasquier (2020), *Entertainment Computing*,** survey generative music in games. It gives variety and interactivity, but systems tend to be built in isolation from one another. [s]
- Their taxonomy separates music that **adapts to game state** from music **generated fresh** [k: the details, to check].
- **Ours:** generative and deterministic, adapting **only to the clock** and to one intensity setting. The world sounds adapt to state (the soundscape axes); the music doesn't. The simplest bridge is harmony by region (6.3).

## 9. What to try (ours; owner's calls marked)

1. **Roughness check by register** (analyzer), and **open the mid pads' voicing** (the Dm9 E4–F4 semitone). Cheap; improves clarity.
2. **Spectral flux:** slow partial drift and brightness falling with each note's decay. Cheap.
3. **World-sound reverb from the soundscape census:** caves sound like caves. Moderate; effects thread.
4. **Rare arrivals:** cadences at sunrise and at first sight of landmarks. Design work; ties to LANDSCAPE.md.
5. **Motifs of place.** Design work; needs regions (terrain v2).
6. **Harmony by region or weather.** Owner's call: it changes the one-hour piece's nature.
7. **Day tempo:** slower, for calm. Owner's call.
8. **FM and plucked voices** for new colours (wood, bell, string) in both the music and the world sounds. Moderate.
9. **Footsteps:** stride quantised to the nearest subdivision, supported by entrainment research. Owner's call, already pending.

## Sources

All [s] unless marked; full citations in BIBLIOGRAPHY.md 91–110. Search results only; no site was fetched.
- Plomp & Levelt 1965
- McDermott, Schultz, Undurraga & Godoy 2016
- Huron 2001; Huron 2006 [k]
- Bregman 1990 [k]
- Grey 1977; McAdams, Winsberg, Donnadieu, De Soete & Krimphoff 1995
- Rimsky-Korsakov 1913 [k]
- Schroeder 1962
- Salimpoor, Benovoy, Larcher, Dagher & Zatorre 2011
- Meyer 1956 [k]
- Juslin & Västfjäll 2008
- Margulis 2014 [k]
- Collins 2008 [k]
- Husain, Thompson & Schellenberg 2002
- Eno 1978 [k]
- Chowning 1973
- Karplus & Strong 1983 [k]
- Smith, CCRMA [k]
- Plut & Pasquier 2020
