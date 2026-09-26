# Round 2, E group, part 3: E17 text, E18 walking, E2 projection, E3 repetition

Delivered 2026-09-26, pasted into the chat (ChatGPT, from the packs). Checked against the code and against the papers we have read. Their text isn't stored.

| Task | Grade | In one line |
|---|---|---|
| E17 text for any language | Good sources, two conflicts | Standards-based and sound, but it recommends bundled fonts and a shaping library, both against our rules |
| E18 walking | Fair to good | Generic, and most of it we already do; keep its test course |
| E2 triplanar projection | Fair | Its account is right; its citations have wrong authors |
| E3 repetition | Fair | A fair survey, two wrong citations, behind our own plan |

## E17 text for any language: good sources, two conflicts

**Sources:** strong. Unicode's own annexes (UAX #9 bidirectional text, #14 line breaking, #29 text segmentation), the OpenType specification (GSUB and GPOS, the substitution and positioning tables), W3C's text-layout requirements.

**Where it conflicts with our rules:**
- **It recommends bundling fonts.** Ours are the player's installed system fonts (CLAUDE.md). Windows ships fonts for every script we plan for, so this isn't needed.
- **It recommends adopting a shaping library.** That's third-party code. Our route (W069) is to hand whole strings to Windows' own text shaping, which already does what it describes.

**What it missed:** UAX #11 (East Asian width), which is exactly our W068: characters that take two cells.

**Its questions answered from the code:**

| It asked | What the code does |
|---|---|
| How is the atlas keyed? | By code point: ASCII, Latin-1, plus every code point the string table uses (render.cpp, `g_uiGlyphs`) |
| What encoding are the text files? | UTF-8, converted to UTF-16 for Windows calls |
| Are translations validated? | Yes: `check_strings.py` checks every key and every slot |

**Keep:** its reading list, as the references for W068 and W069; add UAX #11. Its line-breaking notes apply when the first language without spaces between words (Chinese, Japanese, Thai) arrives.

## E18 walking: fair to good, mostly already done

**Sources:**
- Catto, GDC 2014: fine, a talk on the theory.
- Unity's documentation: acceptable as a description of common practice.
- A link to another engine's source code on GitHub: **not a theory source (D66)**. It's noted and not followed.

**What it recommends, against what world.cpp already does:**

| It recommends | Ours |
|---|---|
| A step-up limit with clearance and destination checks | `CLIMB` 1.05 (a one-cell slope, never a two-cell wall); the body is checked against cells from `BODY_SKIP` 0.7 above the feet |
| Snap to the ground going downhill, only when walking | `STICK` 0.35: the feet stay on the ground within this drop per tick |
| A walkable-slope limit, and a separate landing limit | `WALK_UP` 0.55 (about 57°) and `LAND_UP` 0.30 |
| Sample the ground under the whole footprint, not one point | `FootprintGround`: the centre and four corners, highest wins |
| Never let the player be stuck inside geometry | The entombed lift and the crouch-if-no-room rule |

**Keep:** its **test course**, a small built area to walk after any change to the terrain's shape or to movement:
- steps just under and just over the climb limit;
- drops just under and over the stick distance;
- slopes either side of the walk limit;
- a low ceiling and a one-cell crawlspace;
- a ledge and a narrow passage.

It would suit the surface-nets work (W055) most, because that changes the ground's shape under every rule above. It goes on the ideas list.

## E2 triplanar projection: fair, citations wrong

**The account is right** and matches our shader: each projection is weighted by the normal's component raised to the 4th power, then normalised (render.cpp).

**Citation errors:**
- GPU Gems 3, chapter 1 is by **Ryan Geiss** alone. It's credited to four other people (bibliography 5).
- The Azariadis & Aspragathos citation's DOI prefix belongs to another journal than the one it names. Unchecked; not used.
- "Marco Fajardo" should be **Marcos** Fajardo.

**What it missed:**
- We already skip a projection whose weight is below 0.001.
- Each projection reads three maps (colour, surface, height), so a projection skipped saves three reads, not one.

**Keep:**
- measure before changing anything (F3);
- biplanar mapping (two projections, not three) as an optional experiment only if the texture reads prove costly.

## E3 repetition: fair, behind our plan

**Covers:** texture bombing, stochastic tiling, detail textures, large-scale colour variation, Wang tiles. Cohen et al. 2003 (Wang tiles) is cited correctly.

**Wrong citations:**
- "Kopf et al. 2007, *Procedural Stochastic Textures by Tiling and Blending*": that title is **Deliot & Heitz 2019** (bibliography 2).
- "Heitz & Neyret 2012, *Stochastic Texture Filtering*": the histogram-preserving blend is **Heitz & Neyret 2018** (bibliography 10).

**What it missed:**
- **Burley 2019**, the practical version we plan to build (read in full).
- The pixel-art constraint: near the player, blending copies makes colours that aren't in the palette.

`docs/research/TEXTURE_BLENDING.md` already goes further. **Keep:** Wang tiles, as a note only. They are an alternative to random offsets per region near the player, and they need no blending, so they respect the palette. They aren't needed unless the regions show seams.

## What this round says about the helpers

- **The theory is usually right; the citations are the weak point.** Two of four got authors or years wrong. Every citation from a helper stays "?" in the bibliography until checked at the source.
- **Recommendations drift toward the common industry answer** (bundle fonts, add a library), even when the pack states our rules. Every recommendation is checked against CLAUDE.md, not only against the code.
