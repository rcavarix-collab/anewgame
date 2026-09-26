# Research: tone mapping and our own look (D60)

**Sources:**
- [s] Reinhard, Stark, Shirley & Ferwerda, "Photographic tone reproduction for digital images", *ACM TOG* (SIGGRAPH) 2002, with a free technical report at the University of Utah (UUCS-02-001).
- [p] Narkowicz, "ACES filmic tone mapping curve", 2016: the fit our shader uses (a practitioner post). Its basis, the Academy Color Encoding System, is an industry standard [k].

## What the theory says

- **Tone mapping** maps the scene's wide range of brightness to the screen's narrow one.
- **Reinhard et al. drew on photographic practice** (Ansel Adams' zone system: dodging and burning) for a simple global curve and a local operator.
- **Filmic curves** (ACES-style) apply an S-shaped response. They roll off highlights softly and deepen shadows, a "film" look. They work on each colour channel separately, which **shifts saturated colours toward white or yellow as they brighten**: an intentional look, but a strong one.

## Our case (from the code)

- `ToDisplay` in render.cpp applies an exposure, then the ACES fit (Narkowicz), then display gamma, per channel.
- Filmic ACES is **what many engines use by default**. Keeping it unchanged means our light shares a look with many other games. That's relevant to D60, "our own look".

## Options to explore (a look decision for the owner, not a fix)

1. **Keep ACES** and push character elsewhere (palette, sky). Safe.
2. **A tone curve of our own:** designed from photographic principles (shoulder, toe, a mild S), applied to **luminance** rather than per channel, so bright colours keep their hue (grass stays green in strong sun, sand stays sand). Distinctive and cheap: a few operations.
3. **A small per-time-of-day grade** (a warm or cool lift in the shadows, pulled from the sky model) as part of the curve. Cheap, per-frame constants.

**How to judge:** the preview renderer can render the same scenes through each curve side by side, for the owner to choose.

## To read (legal copies)

- Reinhard et al. 2002, the University of Utah technical report (free).
