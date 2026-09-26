# walkgrid research library

Our own research: theory and explanation from reliable sources (D66), a regular step in how we work (D67). Grok's research deliveries live in `incoming/grok/`, and Claude's appraisals of them in `docs/grok_reviews/`. This folder is ours.

**Papers aren't stored here:** some are marked not for redistribution. `BIBLIOGRAPHY.md` says where each legal copy lives. Markers in the notes:
- **[r]** read in full here;
- **[s]** confirmed by search (abstract);
- **[k]** from knowledge, to check;
- **[p]** a practitioner source (not peer-reviewed; used only where the claim can be checked independently);
- **[m]** first-principles maths.

| Note | What it's for | Key sources |
|---|---|---|
| [OVERVIEW.md](OVERVIEW.md) | Every system checked against the published theory; refreshed each milestone (D67) | many |
| [CLOUDS.md](CLOUDS.md) | How real clouds move and change; targets for ours | NWS, NASA, WMO, AMS |
| [SKY_AND_FLOW.md](SKY_AND_FLOW.md) | Daylight's shape and colour; flowing textures for the clouds | Hošek & Wilkie 2012 [r], Vlachos 2010 [r] |
| [TEXTURE_BLENDING.md](TEXTURE_BLENDING.md) | Material edges and hiding repetition | Schuster et al. 2020 [r], Burley 2019 [r], Heitz & Neyret 2018 [s] |
| [SURFACE_NETS.md](SURFACE_NETS.md) | Terracing, and a seam-safe answer | Gibson 1998 [r] |
| [VERTEX_DARKENING.md](VERTEX_DARKENING.md) | Triangle patches from per-corner darkening (the noon patches) | first principles [m], practice [p] |
| [DITHERING.md](DITHERING.md) | Blue-noise borders for the crisp mosaic | Ulichney 1993 [r] |
| [PIXEL_ART.md](PIXEL_ART.md) | Palettes and marks for the 64-pixel materials | Gerstner et al. 2012 [s] |
| [TONE_MAPPING.md](TONE_MAPPING.md) | Our tone curve and our own look | Reinhard 2002 (JGT) [r], Reinhard et al. 2002 [s] |
| [LANDSCAPE.md](LANDSCAPE.md) | How people enjoy, read and find their way through a landscape; twelve design rules for terrain, far land, colour and sound | Kaplan, Appleton, Spehar et al., Lynch, Keltner & Haidt, Palmer & Schloss, Pijanowski et al. [s] |
| [GAME_DESIGN.md](GAME_DESIGN.md) | Why people play; walkgrid held against the theory; how to see our own blind spots | Hunicke et al., Ryan et al., Sweetser & Wyeth, Pinelle et al. [s] |
| [MUSIC.md](MUSIC.md) | Consonance by register, voice leading, timbre, reverb, expectation; nine things to try in our music and world sound | Plomp & Levelt, Huron, Grey, McAdams et al., Schroeder, Salimpoor et al., Chowning [s] |
| [SOUND_DESIGN.md](SOUND_DESIGN.md) | World sounds as events: materials by decay, modal and granular synthesis, footsteps, distance, sync, what sounds tell the player | Gaver, Klatzky et al., van den Doel et al., Cook, Turchet et al., Zahorik et al., Chion, Jørgensen, Schafer [s] |
| [PAPER_IDEAS.md](PAPER_IDEAS.md) | Papers we might write from the project (background) | none yet |

## Research steps proposed next (2026-09-26; the owner chooses)

Ranked by how directly each serves the plans in hand:
1. **Landforms (geomorphology):** how real land takes its shape (drainage, erosion, talus, the angle of repose, plateaus and mesas), from agencies and textbooks. Feeds terrain v2 directly, so the land reads as plausible.
2. **Procedural generation theory:** expressive-range analysis (measuring what a generator can produce and how varied it is) and the academic text on procedural content. It turns "is terrain v2 more varied?" into a measurement.
3. **Accessibility:** colour-vision deficiency (we tell materials apart by colour), photosensitivity guidance, simulator sickness in first person (field of view, the slide's lean and roll), and hearing (sound cues need visual equivalents). Standards bodies and peer-reviewed work.
4. **Teaching without words (D26):** affordances and signifiers, and onboarding through play. How a player learns walkgrid with almost no text.
5. **Ethics of engagement:** dark patterns in game design and compulsion loops, to keep walkgrid respectful of players' time. Fits our privacy stance.
6. **Building and places (W067):** pattern languages in architecture, and how people make places their own. For the building layer.
7. **Colour science for the palette:** perceptual colour spaces and harmony, for step 9's art and the colour pass.
