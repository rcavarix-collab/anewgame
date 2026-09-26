# Research: how real land takes its shape, and how to measure a generator

Two proposed research steps together, because both serve the next terrain-generator version (terrain v2):
1. what geomorphology says makes land look like land;
2. what procedural-generation research says about measuring a generator's variety.

**Theory only (D66);** our words. Sources confirmed through search results **[s]**. The university sites holding authors' copies were blocked by this environment's network, not by the sites, so nothing was read in full. Full citations: `BIBLIOGRAPHY.md` 123–128. A proper geomorphology textbook is on the wish list at the end.

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

## Sources

All [s]; full citations in BIBLIOGRAPHY.md 123–128.
- Angle of repose and talus: Carson 1977; Britannica, "Talus"
- Horton 1945 [k]; Strahler 1957 [k]; Hack 1957 [k] (through search summaries)
- Mesas and buttes: differential erosion (educational summaries; a textbook is wanted)
- Smith & Whitehead 2010
- Shaker, Togelius & Nelson 2016
