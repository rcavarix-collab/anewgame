# Research: tone mapping and our own look (D60)

**Sources:**
- [r] Reinhard, "Parameter Estimation for Photographic Tone Reproduction", *Journal of Graphics Tools* 7(1), 2002 (University of Utah). Read in full from a copy supplied by the owner. It's a short companion to the SIGGRAPH paper: it restates the photographic operator and automates its two parameters.
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

## From Reinhard's companion paper (read in full)

**The photographic operator, in words:**
- **Work on luminance only:** L = 0.27 R + 0.67 G + 0.06 B.
- **Find the scene's "key"** from the log-average luminance (the geometric mean). Scale the image so that average maps to a chosen key value α (0.18 is the classic middle grey). High-key scenes (a white room) and low-key ones (a dim stable) get different α.
- **Global curve:** Ld = L (1 + L / Lwhite²) / (1 + L).
  - Its slope is 1 at black, so dark detail is kept.
  - It levels off toward 1, so highlights never burn out, unless a **white point** Lwhite is set, above which values are *allowed* to go pure white: controlled burn-out, "a level of control not offered by other operators".
  - Contrast is reduced only in the light areas.
- It borrows **Ansel Adams' zone system**: eleven print zones, each doubling in intensity. Displays span about 4–5 zones; scenes can span 11 or more.
- **Parameters can be set automatically.** The key comes from how the log-average sits between the darkest and brightest 1% (a scene with most of its zones below the average is high-key). The white point comes from the scene's range in zones.

**What it means for walkgrid (our own curve, option 2):**
- Apply the curve to **luminance**, then scale the pixel's colour by the new luminance over the old. Hue and saturation are kept, so grass stays green in strong sun, where per-channel ACES shifts it.
- Our exposure is a fixed curve by time of day. The **key** idea gives a principled version: choose α per time of day (brighter key by day, lower at dusk, so dusk *reads* as dusk), from a few per-frame constants. A true log-average of each frame would be eye adaptation, a cost we don't need.
- A **white point** lets the sun's disc, snow and bright sand burn out deliberately, and only those, instead of every highlight bleaching.
- Cost: a handful of operations per pixel, the same as ACES. Judge side by side with the preview renderer.

## To read (legal copies)

- Reinhard, Stark, Shirley & Ferwerda 2002 (the SIGGRAPH paper), University of Utah technical report UUCS-02-001 (free).
