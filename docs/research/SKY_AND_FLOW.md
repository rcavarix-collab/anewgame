# Research: the sky's light, and flowing textures (for PLAN_TUESDAY steps 5–6 and the colour pass)

**Read at the source [r]:**
- Lukáš Hošek and Alexander Wilkie, "An Analytic Model for Full Spectral Sky-Dome Radiance", *ACM Transactions on Graphics* 31(4), SIGGRAPH 2012; the authors' preprint, supplied by the owner.
- Alex Vlachos (Valve), "Water Flow in Portal 2", SIGGRAPH 2010 course, *Advances in Real-Time Rendering*; the slides as published by Valve, supplied by the owner.

Theory only (D66): our understanding, in our words.

**We don't use the model's fitted coefficient tables or its reference code.** The rule is never to copy anyone's code or data. What we take is the understanding of how a clear sky's light is distributed, and why.

## 1. How a clear sky's light is shaped (Hošek & Wilkie 2012)

**Why the sky looks as it does:**
- Blue sky and red sunsets come from **Rayleigh scattering** by the air itself. It's about 16 times stronger for blue than for red light, so the path through the air matters most when the sun is low.
- **Mie scattering** from aerosols (haze, dust, droplets) sends light mostly forward. It makes a bright **aureole**, a localised glow around the sun, and grows with haze.

**The classic luminance shape (Perez et al. 1993; a published formula, CIE-standardised).** The sky's brightness at a point is a product of two factors:
- one for the view's angle from the zenith: brighter toward the horizon;
- one for its angle from the sun: a glow around the sun, plus a term for the far side.

It works well for clear skies but can't make the aureole's sharp spike.

**What Hošek & Wilkie add:**
- an anisotropic, Mie-like term that puts a **localised glow around the sun**;
- a term that **dims the zenith**, so a low sun's aureole spreads along the horizon and to the sides far more than upward, as seen in real fisheye photographs;
- separate fits per colour channel, so **sunset colours aren't a uniform orange-yellow**;
- **ground albedo**: bright ground brightens the lower sky;
- a **dark rim along the horizon at high haze**.

**One haze knob: turbidity (T).**
- It's the ratio of the atmosphere's optical thickness to that of clean air.
- The paper's own guide: T = 2 is a very clear, Arctic-like sky; 3 a clear temperate sky; 6 a warm, moist day; 10 a slightly hazy day; above 50, dense fog.
- It's valid in the model from 1 to 10.

**Its limits:**
- The sun must be above the horizon (0–90° elevation).
- **After sunset isn't modelled:** the authors list it as future work, because the model "cannot recreate the earth casting a shadow onto the atmosphere".
- It doesn't include the sun's disc itself.

**What it means for walkgrid:**
1. **Daytime colour pass (R1 item 2):**
   - shape our horizon-to-zenith gradient so a low sun's glow spreads sideways more than up;
   - brighten the lower sky over bright ground (sand, snow) a little;
   - make sunsets a gradient of colours, not one orange.

   These are per-frame constants and small shader changes: no per-pixel physics.
2. **Turbidity becomes the weather's haze knob.** One number for weather (R2/G9) can set horizon brightness, the size of the sun's glow, sunset colour and the haze term together (clear T ≈ 2–3, hazy 6–10, fog far above). That's cleaner than many unrelated sliders, and physically grounded.
3. **Twilight stays ours.** The model stops at sunset. Our twilight band and night need their own reference: S3 (sky colour through the day) and S24 (twilight at the equator), including the Earth's shadow and the pink band above it at dusk.
4. **No tables, no code (D68).** We write down what the model's data shows (the points above are that description) and tune our own curves to match it by eye against references. If more precision is wanted, extend the description from the paper's figures, never by copying its tables.

## 2. Textures that flow (Vlachos 2010)

**The problem it solved:** make a texture (water ripples, floating debris) move along a direction field across a surface without smearing, repeating or pulsing, on six-year-old hardware.

**The method, in words:**
- A low-resolution **flow map** gives a 2-D direction and speed at each point; about 4 texels per metre was enough.
- The texture's coordinates are pushed along the flow by time × speed. Pushed too far, it smears: a **distortion looks convincing only for about the first third** of the texture's size (after Max & Becker 1995).
- So **two layers run half a cycle apart**. Each distorts for a short time, then snaps back, and the two are crossfaded so each layer's snap happens while it's invisible.
- **Repetition** (the same pattern passing the same point each cycle) is fixed by giving each layer's restart its own offset.
- **Pulsing** (the whole surface breathing in step) is fixed by **offsetting the cycle's timing per pixel with a noise texture**.
- For colour textures (debris), the distortion runs from −f to +f around the rest position, rather than 0 to f, so the at-rest look is the middle of the cycle.
- Strength scales with flow speed (faster flow, stronger ripples).
- **Cost:** two extra texture reads and about 21 instructions per pixel on 2010 hardware.

**A gameplay finding:** in playtests of the game it shipped in, visible water flow toward the right path meant testers took **17% fewer wrong turns** and crossed the level faster. Flow reads as direction.

**What it means for walkgrid:**
1. **Clouds (step 5):**
   - The flow field is the wind: the jet stream's direction, gently varied across the sky by a very low-frequency noise so it isn't uniform.
   - Each cloud layer's density texture is flowed in two half-cycle-apart phases with per-pixel noise on the timing: shapes stretch, form and dissolve as they move, instead of sliding rigidly.
   - The whole field still drifts with the wind (the large motion).
   - Use the colour-map variant (−f to +f), so the average cloud is undistorted.
   - Cost: a couple of noise reads per sky pixel, sky pixels only.
2. **High streaks (step 6):** the same with a stronger along-wind stretch. The flow gives the visible motion along the streak that was missing.
3. **Wayfinding (G25, a new link):** the sky's steady wind direction could quietly help players orient, as flowing water did in the playtests. Keep the jet stream legible, not random.
4. **Later:** water (G21), if it comes, can use the same method directly.

## Sources

- [r] Hošek & Wilkie 2012, the preprint: sections 1–3, 5.1, 8 and figures 1 and 9.
- [r] Vlachos 2010, the slides, all.
- [k] Perez, Seals & Michalsky 1993, "All-weather model for sky luminance distribution" (the base formula, as described by Hošek & Wilkie).
- [k] Max & Becker 1995, "Flow visualization using moving textures" (as described by Vlachos).
