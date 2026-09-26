# Research: how people read, enjoy and find their way through a landscape

For the creative direction: the next terrain-generator version, the far-land ring, fog and colour, landmarks, and the world's sound. This is a broad research step (D67) across environmental psychology, evolutionary aesthetics, neuroscience, anthropology, human–computer interaction and soundscape ecology, and not only rendering.

**Theory only (D66).** Our understanding, in our words; no one's figures or tables are copied (D68). Most sources were confirmed through search results and abstracts **[s]**; none was read in full here. Markers as in `README.md`. Full citations: `BIBLIOGRAPHY.md` 57–76.

**Caution first.** Environmental aesthetics is a field where effects are real but often small, vary between studies and between cultures, and are sometimes contested. The strongest findings are marked strong; the rest are heuristics to test by eye in the game, not laws. Where independent sources disagree, that's said.

## 1. What makes a landscape appealing

### The Kaplans' four qualities (environmental psychology)
The Kaplans' preference framework crosses two needs, **understanding** and **exploring**, with two timescales: what can be taken in now, and what can be inferred.

| | Now (in the scene as seen) | Inferred (what the scene promises) |
|---|---|---|
| Understanding | **Coherence**: it hangs together; it reads quickly | **Legibility**: you could find your way and back |
| Exploring | **Complexity**: enough richness to look around | **Mystery**: more lies beyond, if you go on |

- **Mystery** is the classic example: a path bending out of sight, or a view partly screened. The Kaplans found it the most consistent predictor. [s]
- **But the evidence varies.** Stamps' 2004 meta-analysis (28 studies, over 6,000 participants, 1,820 scenes) found the results heterogeneous and often not reproducible. Mystery sometimes predicted preference strongly, sometimes not at all, sometimes negatively; the same was true of coherence. Mystery and complexity came out as the most influential overall. [s]
- **Our reading:** use the four as design questions to ask of a scene, not as numbers to optimise. Mystery and complexity have the best support.

### Prospect and refuge (evolutionary aesthetics)
- Appleton (1975) proposed that we enjoy landscapes whose features would have favoured survival (habitat theory).
- In particular, we like places that offer both **prospect**, a wide view to see opportunities and danger, and **refuge**, somewhere to hide or shelter.
- **The preferred spots are the edges between the two:** a sheltered place with a view out. [s]
- It's among the most cited models of landscape preference, but its evidence is mixed and it's hard to test directly [k]. Orians & Heerwagen's savanna hypothesis is a related claim: a preference for open ground with scattered trees and water [k].

### Fractal skylines (perception, with a testable figure)
- Many natural forms (coastlines, ridgelines, clouds, trees) are statistically self-similar. Their roughness can be measured as a **fractal dimension, D**: a smooth line is 1, and a line that fills the page tends toward 2.
- **Spehar, Clifford, Newell & Taylor (2003):** across natural scenes, mathematical fractals and sections of abstract paintings, people most preferred **D ≈ 1.3–1.5**, whatever the pattern's origin. [s]
- **Hagerhall, Purcell & Taylor (2004)** measured real **landscape silhouette outlines** (the skyline) and found preference related to their fractal dimension, suggesting fractal structure explains part of the known preference for natural scenes. [s]
- **Why this matters for us:** it's a figure we can **measure ourselves**. Render skylines from the facet preview tool, box-count the outline, and compare with the 1.3–1.5 band. Our terrain is sums of value noise at a few scales, so its roughness is tunable.

### Individual differences: Sevenant & Antrop 2010 (read in full [r], supplied by the owner)
*Land Use Policy* 27, 827–842.

**What they did:**
- 642 residents of Ghent, Belgium, rated 20 panoramic photographs of everyday rural and village landscapes for beauty.
- They also rated seven landscape dimensions: care, naturalness, disturbance (elements out of context), openness, complexity, urbanisation and typicality.
- Instead of assuming everyone weighs these the same, **latent class analysis** found groups of people who weigh them differently.

**What they found:**
- **People differ a lot.** For most dimensions the biggest share of variation came from the **interaction** between the person and the landscape: different people respond differently to the same scene. Five preference groups appeared, with nearly 30% in the largest.
- **Care (stewardship) and naturalness** raised beauty most strongly overall. Care mattered more than the often-assumed dominance of naturalness.
- **Disturbance** (things that don't fit their context) and urbanisation lowered it most. Disturbance was strongly tied to **low coherence**.
- **Openness, complexity and typicality had small effects.** Openness wasn't significant at all in their data, though the authors note their photos varied little in openness.
- **Attitudes predicted group membership better than demographics.** Environmental attitude, where people live, age and education mattered; gender and similar variables mostly didn't.

**Limits (theirs and ours):** one region's rural landscapes, photographs rather than presence, urban residents, linear relations only.

**What it changes for walkgrid:**
- It's **independent evidence that preference isn't universal,** agreeing with Stamps' heterogeneity (section 1). So the design rules are for most players, not all.
- **Disturbance is the thing to avoid most:** elements that don't belong to their context. A single out-of-place material or form breaks coherence. That strengthens rule 5 (districts with a consistent character).
- **Care:** signs that a place is looked after raise its appeal. In walkgrid that's the player's own work (paths, tended ground, built places), supporting rule 12 (traces make places).
- **Openness mattered less here than prospect–refuge theory suggests.** Keep vistas as rare moments (rule 7), not a constant goal.

### Arousal and complexity: a caution
Berlyne (1971) proposed an inverted U: moderate complexity and novelty please most. It shaped the field, but **later studies mostly failed to support it** [s]. We don't design to it.

### Restoration (health psychology)
- **Ulrich (1984), *Science*:** surgical patients whose window looked onto trees had shorter stays and needed fewer strong painkillers than matched patients facing a brick wall. [s]
- **The Kaplans' attention restoration theory:** natural scenes hold attention gently ("soft fascination"), which lets the effortful kind of attention rest. [s]
- **For us:** gentle, low-demand motion (clouds drifting, grass stirring, light changing) is where a calm game's restfulness comes from. It makes the sky work (steps 5–6) more than decoration.

## 2. Awe, curiosity and the unseen

### Awe
- **Keltner & Haidt (2003):** awe has two core appraisals: **perceived vastness** (something much larger than the self, physically or in scope) and a **need for accommodation** (it doesn't fit what we already understand). [s]
- Threat, beauty and the supernatural colour the experience.
- **For us:** vastness is what the far-land ring can give: a ridge where the land suddenly opens out to the horizon. It only works if it's **rare**; every hilltop can't be a revelation.

### Curiosity
- **Loewenstein (1994):** curiosity is a felt **gap in knowledge**; once the gap is noticed, not knowing is uncomfortable, and we seek to close it. [s]
- Curiosity is strongest when the gap seems small enough to close; knowing nothing about a subject raises little [k].
- **For us, together with mystery:** show **that** something is there without showing **what** it is. A silhouette on the far ring, a glint in a cave mouth, a strange colour over a rise. A distant hint gives a small gap; walking there closes it.

## 3. Finding the way

### The image of a place (urban design)
**Lynch (1960),** from interviews in three cities, found people's mental maps are built from five kinds of element:
- **paths**, what you move along;
- **edges**, boundaries you don't cross easily (shores, cliffs);
- **districts**, areas with a recognisable character;
- **nodes**, junctions and gathering points;
- **landmarks**, distinctive points seen from afar.

A place is **legible** when these are easy to recognise and fit together. [s] The elements reinforce or weaken each other: a landmark at a node is stronger than either alone.

### In virtual worlds (human–computer interaction)
**Darken & Sibert (1996)** found people have severe trouble finding their way in large virtual worlds. Without added cues, their subjects were often disoriented and struggled to complete a search. Organising a world by Lynch's principles helped. [s]

### In the brain (neuroscience)
- The 2014 Nobel Prize: **place cells** in the hippocampus fire at particular places (O'Keefe), and **grid cells** in the neighbouring entorhinal cortex give a coordinate system (M.-B. and E. Moser, 2005). Together they build an inner map. [s]
- Place cells respond to the environment's structure, not just its look [s]. Cells tuned to borders and boundaries anchor the map [k].
- **For us:** distinct, stable features (edges, landmarks, the sun's direction) are what an inner map is built on. A world whose every hill looks alike, lit by an unreadable sky, is hard to map.

### Direction from the sky
Vlachos (2010; SKY_AND_FLOW.md) found flow toward the goal reduced wrong turns. A steady wind in the clouds and the sun's arc are free, always-visible compasses.

## 4. Seeing distance

### Contrast as a depth cue (vision science)
- **O'Shea, Blackburn & Ono (1994):** far objects have less contrast (aerial perspective). A region with lower contrast against its background looks **farther**, even with no other depth cue, and even when size disagreed. [s]
- The contrast falls **toward the colour of the air**, which is the sky's colour at that hour, not a fixed white [k: painters' practice since Leonardo; physics, Koschmieder].
- **For us:**
  - fog should lower contrast toward the sky's colour at every hour (D64 already moved this way);
  - the "white in the distance" bug is exactly a failure of this cue;
  - the far ring's hills should be flat in contrast but right in hue.

## 5. Colour

### Why people like the colours they like (perception)
- **Palmer & Schloss (2010), *PNAS*:** colour preference is largely explained by how people feel about the **things associated with each colour**. The average liking of associated objects explained **80%** of the variation across colours. [s]
- Clear blues (sky, clean water) tend to be liked. Dark yellows, browns and olives tend to be disliked, because they're associated with rot, mud and waste [k: the paper's examples, to check].
- Associations vary between people and cultures [k].
- **For us:**
  - Our ground is mostly earth colours, so how they read matters. Make sand, clay and dirt **clean and warm** (sunlit sandstone, fresh earth) rather than murky olive-brown.
  - Keep healthy greens clearly green; the tone-curve question (TONE_MAPPING.md) matters here.
  - Keep the sky's blues clear.

  That's a check for step 9's 64-pixel art and the colour pass.

## 6. Sound

### Soundscape ecology
- **Pijanowski et al. (2011), *BioScience*:** a landscape's sound has three sources: **geophony** (wind, water, rain), **biophony** (living things) and **anthrophony** (people and machines). [s]
- **Krause's acoustic niche hypothesis:** in a healthy habitat, species share out the frequencies and the timing, so no voice masks another. [s]

### How people rate a soundscape
- **Axelsson, Nilsson & Berglund (2010):** people's ratings reduce to **pleasantness** (half the variance), **eventfulness** and **familiarity**.
- Soundscapes dominated by natural sounds were rated pleasant, by machines unpleasant, and by human sounds eventful. [s]

### For us
- Our world sound is geophony and a synthesized biophony. The acoustic niche idea mirrors our harmony lock: give each sound source its **own band and its own rhythm**, so none masks another (a check for `sound_demo.sh analyze`).
- A region's sound can mark it as a **district by ear**.
- A player's building (anthrophony) should make a place feel more eventful and lived in, not unpleasant.

## 7. Place and time (anthropology and geography)

- **Ingold (1993), "The temporality of the landscape":** a landscape is made by the activities that go on in it and their rhythms, the **taskscape**; it's never finished, only lived in. [s]
- **Tuan (1974, *Topophilia*; 1977, *Space and Place*):** a **space** becomes a **place** as it gathers experience and meaning; attachment to place grows with time spent there. [k]
- **For us:**
  - the day's rhythm, the weather and the player's own work are what turn walkgrid's space into places;
  - what the player builds and digs are traces that make a place theirs;
  - the world should keep them visible.

  This supports the building layer (W067) as more than a mechanic.

## 8. Design rules to try (ours, drawn from the above)

Proposals to test by eye in the game, and by measurement where possible. Each names where it lands.

1. **Skylines in the preferred roughness (measurable).** An offline check in the facet preview tool: render skylines from a few spots, box-count the outline, and compare with D ≈ 1.3–1.5. Use it to tune the next generator version's layer scales. (Terrain v2.)
2. **Prospect beside refuge.** Place the terrain's knolls and lookouts near hollows, overhangs and cave mouths, so shelter with a view happens naturally and often. (Terrain v2, with caves R6.)
3. **Mystery on purpose.** Valleys and ridges that bend, and rises that hide what's beyond. Avoid long straight valleys that show everything at once. (Terrain v2: the domain warping in round2_E_part4 is exactly this.)
4. **Show that, not what.** The far ring shows silhouettes of distant features; walking closes the gap. (Far-land ring.)
5. **Districts and edges.** Regions with a clear character each (material mix, colour, landform, sound), divided by readable edges (cliffs, ridges, later water), rather than materials salted evenly. (Terrain v2 region field; world sound.)
6. **Rare landmarks.** A few large, distinctive forms per region, visible from far off and never repeated nearby: a tall rock needle, a lone arch, a crater. Place them at nodes where valleys meet. (Terrain v2; later structures.)
7. **Rare vastness.** Now and then a ridge where the land opens to the horizon. Rarity is the point. (Far ring plus terrain.)
8. **Distance by contrast, toward the sky's colour.** Fog lowers contrast toward the sky's colour at that hour, never toward a fixed white. (Colour pass; the white-distance bug.)
9. **Clean earth colours.** Warm, clean sands and clays, clear greens, clear sky blues. (Step 9 art; tone curve.)
10. **The sky as the calm centre.** Gentle, constant motion. It's also a compass: the wind's direction steady, the sun's arc readable. (Steps 5–6.)
11. **Sound niches and districts by ear.** Each source in its own band and rhythm; regions sound different. (World sound; the analyzer.)
12. **Traces make places.** What the player does stays visible. (Building layer W067.)

## Sources

All [s] unless marked; full citations in BIBLIOGRAPHY.md 57–76. Web pages consulted 2026-09-26 through search results only; no site was fetched.
- Kaplan & Kaplan 1989 [k] (the book); Kaplan 1995 [s]
- Stamps 2004 [s]
- Sevenant & Antrop 2010 [r]
- Appleton 1975 [s]; Orians & Heerwagen 1992 [k]
- Spehar, Clifford, Newell & Taylor 2003 [s]; Hagerhall, Purcell & Taylor 2004 [s]
- Berlyne 1971 [s], with the later critique [s]
- Ulrich 1984 [s]
- Keltner & Haidt 2003 [s]
- Loewenstein 1994 [s]
- Lynch 1960 [s]
- Darken & Sibert 1996 [s]
- O'Keefe; Hafting, Fyhn, Molden, Moser & Moser 2005, via the Nobel Committee's scientific background, 2014 [s]
- O'Shea, Blackburn & Ono 1994 [s]
- Palmer & Schloss 2010 [s]
- Pijanowski et al. 2011 [s]; Krause (the niche hypothesis) [s]
- Axelsson, Nilsson & Berglund 2010 [s]
- Ingold 1993 [s]
- Tuan 1974 and 1977 [k]
