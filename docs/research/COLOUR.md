# Research: colour science and colour harmony, for the palette

Proposed research step 7, for step 9's 64-pixel art and the colour pass. **Theory only (D66);** sources through search results **[s]**. Full citations: `BIBLIOGRAPHY.md` 146–149.

## 1. Measuring colour as people see it
- **CIE 1976 L\*a\*b\* (CIELAB),** now the standard ISO/CIE 11664-4:
  - a colour space built so that **equal distances look like roughly equal differences**;
  - it gives lightness (L\*), chroma and hue;
  - it defines colour difference as a distance. [s]
- **CIEDE2000** (ISO/CIE 11664-6) corrects that distance for how differences are perceived at different lightness, chroma and hue. [s]
- **For us:**
  - Our palettes are chosen in RGB, which isn't perceptually even: two steps of the same size in RGB can look very different.
  - The texture generators (`tools/*_textures.py`) can **build each material's palette in CIELAB:** even lightness steps, controlled chroma, and hue kept in one family. Then convert to sRGB.
  - The standards are published definitions, so implementing their formulas from the standard is fine. Check the text of the standard itself, and don't copy anyone's code.

## 2. Colours that go together
- **Schloss & Palmer (2011), *Attention, Perception, & Psychophysics*:** earlier studies disagreed on whether harmony comes from similar or contrasting hues. Separating three judgements resolved it [s]:
  - **preference** for a pair;
  - **harmony** of a pair;
  - **preference** for a figure colour on its ground.

  Both **preference and harmony rise with hue similarity.** Preference also depends on liking the colours themselves and on **lightness contrast**, and harmony follows perceived similarity closely.
- **With Palmer & Schloss 2010** (LANDSCAPE.md 5; colours are liked for what they're associated with):
  - **Within a material:** a tight hue family with a wide lightness range reads as harmonious and still has depth.
  - **Between neighbouring materials:** similar hues with different lightness (sand beside sandstone) harmonise. Contrast of lightness, not of hue, separates figure from ground: a path from grass, a landmark from the hill.
  - **Across the whole land:** a few hue families, each tied to something liked (clean water, healthy plants, warm clean earth, clear sky).

## 3. With accessibility
- Lightness contrast is also what survives colour-vision deficiency (ACCESSIBILITY.md 2). **The same rule serves both:** separate things by lightness and pattern; use hue for harmony and mood.

## 4. What to try (step 9 and the colour pass)
1. **Palettes built in CIELAB** in the texture generators: even lightness steps, controlled chroma, one hue family per material.
2. **A palette sheet check** (offline): lightness contrast between each pair of neighbouring materials, plus the colour-vision simulation.
3. **A land palette:** a few hue families tied to liked associations, and no murky olive-browns.

## Sources

All [s]; full citations in BIBLIOGRAPHY.md 146–149.
- ISO/CIE 11664-4 (CIELAB)
- ISO/CIE 11664-6 (CIEDE2000)
- Schloss & Palmer 2011
- Palmer & Schloss 2010 (bibliography 72)
