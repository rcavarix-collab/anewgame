# Research: how real clouds move and change (for PLAN_LOOK A–C)

Purpose: make walkgrid's clouds move and change the way real ones appear to. Owner, 2026-09-26: "make the cloud speeds appear realistic"; research from reliable sources only (D66).

**Method and its limits.**
- Sources are limited to national weather services, space agencies, the WMO, and peer-reviewed or society publications (American Meteorological Society journals and glossary, NASA technical reports).
- This session's network blocks opening those pages directly. The figures below come from search results that quote them, with each source named. Before any number becomes final, it should be re-read at the source (a helper task, or the owner opening the links).
- Each figure is marked by how many independent sources support it.

## What the sources say

| Fact | Figure | Support |
|---|---|---|
| Cumulus cloud bases | about 300 m to a little over 3,000 m (1,000–10,000 ft); a representative base in the tropics is about 1,000 m | NWS cloud classification (weather.gov/lmk); AMS *J. Atmos. Oceanic Tech.* 15(6), 1998 (boundary-layer height against cloud base): 2 |
| Fair-weather cumulus | scattered, under about half the sky, little vertical growth | NWS glossary; AMS literature: 2 |
| How fast low cumulus drift | about 9 m/s in moderate trade winds, about 18 m/s in strong ones; cumulus move with the wind at or just below their base | AMS *J. Atmos. Sci.* 69(1) (Nuijens et al., "The influence of wind speed on shallow marine cumulus"): 1 (a strong source; a second would help) |
| How long a fair-weather cumulus lasts | minutes to tens of minutes; about 20 minutes for a short-lived one | peer-reviewed study of fair-weather cumulus dynamics (arXiv 1907.12980); NOAA/NWS thunderstorm life cycle (cells about 30 min, for comparison): 2, one indirect |
| Cirrus height | about 6–10 miles (roughly 8–16 km) in the upper troposphere | NASA JPL Center for Climate Sciences; NWS cloud classification: 2 |
| Winds at cirrus height | tens of m/s; jet cores 40 m/s and more, over 100 m/s in extreme winter jets | Britannica "Upper-level winds"; AMS cirrus climatology (*J. Atmos. Sci.* 58(5), 2001): 2 |
| Cirrus shape | hooked "horse's tail" streaks (cirrus uncinus): ice falling out of small generating heads and drawn out by the wind changing with height; fall streaks about 2 km deep | WMO International Cloud Atlas (Ci unc); AMS Glossary; NASA LaRC contrail-to-cirrus study (*J. Appl. Meteor. Climatol.* 2006): 3 |
| Cirrus lifetime | an hour or more for the generating heads (fall speed about 0.5 m/s over about 2 km) | NASA LaRC 2006 study: 1 |

## What that means on screen

What the eye judges is **angular speed**: how fast a cloud crosses the sky, which is its speed divided by its height.
- A cumulus at 1,200 m moving 9 m/s crosses about **0.4° a second** overhead. From zenith to horizon takes a few minutes; slower near the horizon, where perspective squeezes it.
- A cirrus at 10 km moving 30 m/s crosses about **0.17° a second**, less than half as fast, **even though its wind is three times stronger**.
- So in a real sky **low clouds appear to move faster than high ones**. Our sky should keep that.

## Why ours looked frozen (from the code)

- **Low clouds:** the plane sits 220 blocks up and drifts 2.5 blocks a second, about 0.65° a second overhead. That's real-world angular speed already. They looked still for another reason: **they never change shape**, so the eye has nothing moving inside them, and near the horizon perspective slows them further.
- **High streaks:** they move along their own length. A streak sliding along itself looks still: the motion is invisible unless something along the streak (a gap, a hook, a thickening) moves too. Their drift is also slow (about five minutes to move one streak's width).

## The design question research can't settle: real time or game time?

A walkgrid day is one real hour, 24 times faster than life.
- **Clouds moving in game time** (24× real) would sweep across the sky in about ten seconds: frantic.
- **Clouds moving in real time** match a real sky second for second, while the sun races past them.

A middle ground may read best, for example 3–4× real: lively, but still clouds. This is the owner's call; the in-game test decides.

## Proposed targets (in real seconds, to be judged in game)

| | Real sky | Proposal |
|---|---|---|
| Low clouds, angular speed overhead | about 0.4°/s (0.2–0.8) | Keep about 0.4–0.6°/s at 220 blocks: 1.5–2.5 blocks a second, as now, or a little faster if the owner picks a compression |
| Low clouds, shape change | each cloud forms and fades over about 20 min (real) | The noise itself evolves: each cloud grows, splits or fades over a few minutes on screen, so something is always changing |
| High streaks, angular speed | about 0.1–0.3°/s, slower than the low clouds | About half the low clouds' angular speed |
| High streaks, shape | hooks and fall streaks drawn out by wind shear; gaps along the streak; slow change (an hour) | Shorter wisps (about 4:1) with gaps and heads that move along the streak, so the motion shows; slow evolution |
| Direction | low and high winds differ; high winds follow the jet stream | Low clouds follow the jet stream only loosely (turned by up to about 30°), which also looks natural (wind turns with height) |

## Sources

- NWS Louisville, cloud classification: https://www.weather.gov/lmk/cloud_classification
- NWS glossary (cloud, cirrus): https://forecast.weather.gov/glossary.php?word=cloud
- AMS, *J. Atmos. Oceanic Tech.* 15(6), 1998, boundary-layer height and cloud base: https://journals.ametsoc.org/view/journals/atot/15/6/1520-0426_1998_015_1331_cblhmw_2_0_co_2.xml
- Nuijens et al., *J. Atmos. Sci.* 69(1), wind speed and shallow cumulus: https://journals.ametsoc.org/view/journals/atsc/69/1/jas-d-11-02.1.pdf
- Fair-weather cumulus dynamics (lifetime): https://arxiv.org/pdf/1907.12980
- NOAA JetStream, thunderstorm life cycle: https://www.noaa.gov/jetstream/thunderstorms/life-cycle-of-thunderstorm
- NASA JPL Center for Climate Sciences, cirrus: https://climatesciences.jpl.nasa.gov/resources/26/index.html
- Britannica, upper-level winds: https://www.britannica.com/science/climate-meteorology/Upper-level-winds
- AMS midlatitude cirrus climatology, *J. Atmos. Sci.* 58(5), 2001: https://journals.ametsoc.org/view/journals/atsc/58/5/1520-0469_2001_058_0481_amcccf_2.0.co_2.xml
- WMO International Cloud Atlas, cirrus uncinus: https://cloudatlas.wmo.int/en/species-cirrus-uncinus-ci-unc.html
- AMS Glossary, cirrus uncinus: https://glossary.ametsoc.org/wiki/cirrus-uncinus/
- NASA LaRC, contrails to cirrus (*J. Appl. Meteor. Climatol.* 2006): https://www-pm.larc.nasa.gov/sass/pub/journals/atlas_JAMC2006.pdf
