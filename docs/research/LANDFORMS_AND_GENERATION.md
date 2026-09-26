# Research: how real land takes its shape, and how to measure a generator

Two proposed research steps together, because both serve the next terrain-generator version (terrain v2):
1. what geomorphology says makes land look like land;
2. what procedural-generation research says about measuring a generator's variety.

**Theory only (D66);** our words. First written from search results **[s]**. Since then the owner supplied legal copies of three sources, now **read at the source [r]** (section 4):
- Smith & Whitehead 2010, in full;
- Shaker, Togelius & Nelson's landscape chapter, in full;
- Huggett's geomorphology textbook, only chapter 1 of the publisher's preview.

Full citations: `BIBLIOGRAPHY.md` 123–128 and 151–153.

## 1. Landforms (geomorphology)

### Slopes have limits
- **The angle of repose:** loose, dry material can't stand steeper than about **33–37°** before it slumps. The typical range is 25–40° by material, lower when wet. [s]
- **Talus (scree):** rock falling from a cliff collects at its foot as a slope at about that angle, and keeps re-settling to it as more arrives. [s] (Carson 1977, *Earth Surface Processes*, measured angles of repose and of talus slopes [s].)
- **Cliffs exist only where rock is cohesive.** Sand, earth and gravel can't make them.

**For us:**
- Our ground is angular by design, but **its steepness should follow its material.** Sand and gravel slopes ease at around 35°. Earth can be a little steeper. Only stone and sandstone should make cliffs.
- **Under every cliff, a skirt of scree at the angle of repose.** That's one of the most recognisable signatures of real land, and cheap to generate: a talus band below steep stone.
- It fits the facet world: a one-cell step is 45°, just past repose, so a sand slope should rise in steps of one cell up over two across.

### Water organises land
- Rivers form **branching networks.**
  - **Horton and Strahler's ordering:** each junction of two streams of equal order makes a stream one order higher, and there are many small streams for each large one. [s]
  - **Hack's law:** a main stream's length grows as roughly the 0.6 power of its basin's area, so basins lengthen as they grow. [s]
  - **Drainage density** (channel length per area) sets how finely a landscape is cut: many small valleys, or few broad ones. [s]
- **For us:**
  - Value noise makes hills with no drainage: hollows that would be lakes, and valleys that go nowhere. Real land is almost all **valleys that join and lead downhill.**
  - A generator doesn't need to simulate erosion to look right. It can **carve a branching valley network** (trees of valleys whose depth grows with order) into the noise. That also makes **paths** and **edges** in Lynch's sense (LANDSCAPE.md 3).
  - Carving valleys needs regional information (where the water goes), not only local. That's a real design problem for a streamed world: nobody knows where water goes without seeing the whole slope. Two routes:
    - valleys as a **fixed function of position** (a noise-defined network, checked for consistency);
    - a coarse **precomputed drainage map per large region**, computed once when the region is first needed and saved.

    Both are deterministic.

### Hard rock caps soft
- **Mesas, buttes and escarpments** come from **differential erosion:** a hard caprock over softer layers protects what's beneath it. The softer rock erodes around the edges, the cap collapses as scree, and a plateau shrinks through mesa, butte and spire to nothing. [s]
- **For us:**
  - Our plateaus (terrain.cpp `Plateau`) and layered sandstone are halfway there. Real flat tops need a **hard layer on top**, **cliff edges below it**, scree skirts, and occasional **isolated buttes and spires** left standing.
  - Those are exactly the rare, far-visible **landmarks** LANDSCAPE.md rule 6 asks for.
  - The cycle suggests their proportions: a few big mesas, more buttes, a handful of spires.

### Design rules from landforms
1. Steepness by material, with a cap near the angle of repose for loose ground.
2. A scree skirt under every stone cliff.
3. Branching valleys that lead downhill, with depth and width growing with order.
4. Caprock plateaus that break down into mesas, buttes and spires as landmarks.
5. Few closed hollows, unless they're meant as lakes (later water).

## 2. Measuring a generator

### Expressive range
- **Smith & Whitehead (2010),** at the workshop on procedural content generation in games, introduced **expressive range analysis** [s]:
  - generate many samples;
  - score each on a few meaningful metrics (they used **linearity** and **leniency** for platform levels);
  - plot the scores as a **2-D histogram**.

  The picture shows what the generator tends to make, what it can't make, and its hidden biases.
- **Shaker, Togelius & Nelson (2016),** *Procedural Content Generation in Games*, Springer: the field's first textbook. It includes landscapes (fractals, noise, agents) and evaluating generators. [s] The authors have distributed free copies [k: to check].

### For terrain v2
- **Metrics of our own,** each measurable offline from `TerrainHeight` and the cell generator, with no game running:
  - **roughness:** the skyline's fractal dimension (LANDSCAPE.md rule 1);
  - **relief:** the height range per region;
  - **openness:** the share of spots with a long view (prospect);
  - **shelter:** the share of spots near overhangs or hollows (refuge);
  - **landmark spacing:** the typical distance to the nearest landmark;
  - **drainage density** (once valleys exist);
  - **material mix per region.**
- **The picture:** generate a few hundred regions from different seeds, score each, and plot pairs of metrics as histograms, before and after a change. That's how "terrain v2 is more varied" becomes a measurement instead of a feeling, and how a bias shows: every region the same relief, no vistas ever.
- **Cost:** a native tool, like `facet_preview`; nothing in the game.

## 3. What to try

1. **Landform rules 1–5** as terrain v2's design goals, with LANDSCAPE.md's rules.
2. **An expressive-range tool** (`tools/terrain_range`), with the metrics above, run before and after each generator change.
3. **Decide the drainage approach:** a fixed function of position, or per-region precomputation. A design question for the terrain v2 plan.

## 4. Read at the source (supplied by the owner)

### Smith & Whitehead 2010, the whole paper [r]
- **Why variety needs measuring.** A generator can make thousands of levels quickly and still be useless if many are effectively identical. Judge it by **the range and style of what it can make**, not by count or speed.
- **The method, four steps:**
  1. **choose metrics** that describe **global, emergent qualities** of the output, from the player's point of view. They should **not** simply be the generator's own input parameters, so that you can later see how changing those inputs moves the output;
  2. **generate a large sample** (they used 10,000 levels per run);
  3. **plot the space** as 2-D histograms of one metric against another (hexagonal bins, brighter for more samples);
  4. **compare the plots** as parameters change.
- **Their metrics,** for 2-D platform levels:
  - **linearity:** how well a straight line fits the level's profile;
  - **leniency:** how forgiving it is, from a score per component.
- **The lesson that matters most for us.** The plots exposed a strong **bias toward linear levels** that came from a small implementation detail: a slight extra chance of repeating the last component, added late to fix something else. In their words, they would never have realised the change had such far-reaching effects without the analysis.

  **Terrain v2 will have details like that, and this is how we'd catch them.**
- **Its limits, in the authors' words:**
  - their metrics were aesthetic, and not sufficient on their own;
  - better metrics, and perhaps a model of player behaviour, are future work;
  - expressive is not the same as creative.

**For our tool (section 2):**
- Metrics are emergent properties of the land (relief, roughness, openness, shelter, landmark spacing, drainage), never the generator's own knobs.
- Samples in the thousands: cheap offline, since `TerrainHeight` needs no chunks.
- Hexagon-binned pairs, before and after each change.

### Shaker, Togelius & Nelson 2016, chapter 4, "Fractals, noise and agents with applications to landscapes" [r]
- **Interpolated (value) noise:**
  - random heights on a coarse lattice, filled in between;
  - **bilinear** filling gives straight slopes and sharp peaks and hollows: "a child's drawing of mountains", with points where collision can snag;
  - the **cubic S-curve** they give, s(x) = 3x² − 2x³, makes slopes that start gently, steepen and round off.

  **That's exactly our terrain's fade (terrain.cpp `Noise`),** so our base method is the textbook one.
- **Gradient noise** (Perlin's) generates slopes rather than heights, so peaks and valleys emerge from the slopes. It **avoids the rectangular grid effects** of lattice value noise. That's the book's confirmation of the axis-aligned patterns noted in round2_E_part4, and of the remedies there: turning each layer's lattice, or gradient noise in a new generator version.
- **Fractal terrain:** real land varies at every scale. Summing layers with amplitude falling as frequency rises (1/f) approximates fractional Brownian motion. **Diamond-square** is a cheap classic. Our named layers at 72, 26 and 11 blocks are a hand-tuned version of the same idea.
- **The chapter's key criticism:** fractal and noise methods are **hard to control**. A designer can only set global, unintuitive parameters.
- **Two ways to get control:**
  - **Agents** (Doran & Parberry 2010, as described there): separate software agents build a coastline, smooth, lay beaches, raise ridges and hills, and finally dig **rivers**. A river agent starts at the coast, walks uphill to a ridge, then digs its way back down, widening as it goes.
  - **Search:** evolve terrain against measurable goals, such as a minimum smooth, walkable area balanced against obstacle edge length.
- **For us:**
  - The river agent is one concrete answer to the drainage question (section 1): carve valleys by walking from low to high and digging down. It needs to see a whole region, which supports the **per-region precomputed drainage map**.
  - Search is a model for later: tune the generator's parameters against the expressive-range metrics, offline, and ship the tuned constants. Nothing runs at load.

### Huggett, *Fundamentals of Geomorphology*, 4th edition (2017), chapter 1 and front matter [r]
The publisher's preview holds chapter 1 and the contents. The chapters we need most, **8 (Hillslopes)** and **9 (Fluvial landscapes)**, aren't in it; they stay on the wanted list.

What chapter 1 gives:
- **Form has three sides:**
  - **constitution:** what the ground is made of;
  - **configuration:** its size and shape;
  - **mass flow:** how fast material moves through it.

  Processes are driven from inside the Earth (uplift, volcanoes, faults) and from outside (weathering, water, ice, wind). Form and process shape each other.
- **Landforms come in scales,** each with its typical area and lifespan:
  - microscale: pools, ripples, small fault scarps;
  - mesoscale: meanders, dunes, small volcanoes;
  - macroscale: floodplains, sand seas, block-faulted terrain;
  - megascale: major drainage basins, mountain ranges.

  (Described, not copied: D68.) **For us:** walkgrid's playable area spans the micro and lower meso scales. Mountain ranges and great basins can appear only as the far ring's silhouettes.
- **Davis's "geographical cycle"** (superseded as a theory, but a clear picture of landscape "ages"):
  - **youth:** few streams, V-shaped valleys, poorly drained ground between them, waterfalls and rapids;
  - **maturity:** a fully integrated drainage network, **relief at its greatest**, hillslopes dominating;
  - **old age:** broad, gentle valleys, wide floodplains, low relief.

  **A strong idea for terrain v2:** regions of different "ages", each internally consistent. That's variety with a logic behind it, and it gives districts (LANDSCAPE.md rule 5) a physical basis.
- **Slope form follows process.** The book's example is a numerical model (Kirkby's) of a scarp at the edge of a plateau:
  - where **soil creep** dominates, the slope rounds into a smooth, broad profile;
  - where **surface wash** dominates, it keeps a steeper upper face over a long, gentle lower slope.

  **For us:** hilltops that are **convex** and footslopes that are **concave** are the signatures of real hillslopes. Value noise makes symmetric bumps, neither. A shaping curve applied to height (convex near crests, concave near valley floors) would be a cheap step toward real profiles.
- **Geodiversity:** the variety of rocks, soils, landforms and processes, valued the way biodiversity is. A useful word for what terrain v2 is after.

### Revised design rules (adding to section 1)
6. **Regions with a landscape "age"** (youthful, mature, old), each consistent within itself.
7. **Convex crests, concave footslopes:** shape height profiles, not only sum noise.
8. **Measure every generator change** with the expressive-range tool, since small details cause large biases (Smith & Whitehead's own finding).

## Sources

All [s]; full citations in BIBLIOGRAPHY.md 123–128.
- Angle of repose and talus: Carson 1977; Britannica, "Talus"
- Horton 1945 [k]; Strahler 1957 [k]; Hack 1957 [k] (through search summaries)
- Mesas and buttes: differential erosion (educational summaries; a textbook is wanted)
- Smith & Whitehead 2010 [r]
- Shaker, Togelius & Nelson 2016, chapter 4 [r]
- Huggett 2017, chapter 1 [r]
- Doran & Parberry 2010 and Kirkby 1971 [k: as described in the two books]
