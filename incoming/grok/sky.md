=== FILE: REPORT.md ===
# Task E7: Sky colour models (M)
## Files
- sky_models.md: Theory of physical sky colour (Rayleigh/Mie), published analytic models, comparison to walkgrid’s hand-tuned gradient + twilight + solar glow, and ordered suggestions for cheap improvements.
## Done when
- every method has a source: Yes (all methods cited in Sources)
- nothing is code: Yes (words and maths only)
## Assumptions
- ASSUMPTION: walkgrid’s current sky is evaluated per-pixel or per-vertex in a simple sky dome / screen-space pass and costs a negligible fraction of the 2–3 ms world budget; no volumetric clouds or multi-scatter atmosphere are present.
- ASSUMPTION: the “hand-tuned gradient” is a fixed or time-of-day interpolated colour ramp from horizon to zenith, with an extra angular falloff for the twilight band and a soft additive disc/glow around the sun direction.
- ASSUMPTION: the sun and moon move on independent clocks and their directions are already available to the shader as normalised vectors.
## Self-check
1. Hard rules kept: Yes
2. Everything I say about the game comes from the task's facts or is labelled ASSUMPTION: Yes
3. What I'm least sure of: exact angular width and intensity curve of the existing twilight band and solar glow (not given), so suggestions stay qualitative.
## Questions for the owner or Claude
- None required for this theory document; any measured GPU cost of the current sky pass would refine the “What we might try” priorities.

=== FILE: sky_models.md ===
# Sky colour models

## Ours

Walkgrid uses a hand-tuned colour gradient running from horizon to zenith. A twilight band is added toward the sun, and a soft glow is placed around the sun itself. The sun and moon move on their own clocks; their directions are therefore available as unit vectors. No volumetric atmosphere, multi-scattering path, or precomputed lookup tables are described. The result is intentionally simple and cheap, fitting the overall graphics budget of roughly 2–3 ms on a GTX-1060-class GPU while still giving a clear sense of time of day and solar position.

ASSUMPTION: the gradient is evaluated either as a sky-dome texture or as a few arithmetic operations per pixel/vertex, the twilight band is an angular fall-off measured from the solar azimuth or elevation, and the solar glow is a soft disc or Gaussian-like term centred on the sun direction.

## The theory

### Physical basis – Rayleigh and Mie scattering

Visible sky colour is produced by the scattering of sunlight by molecules and aerosols in the atmosphere.

- **Rayleigh scattering** occurs when the scatterers (primarily N₂ and O₂ molecules) are much smaller than the wavelength of light. The scattered intensity is proportional to \(1/\lambda^4\). Consequently blue light (short wavelength) is scattered far more strongly than red light. This is the dominant reason the daytime zenith appears blue and the horizon, which views a longer path length, appears paler or whitish. The phase function for Rayleigh scatter is nearly isotropic with a mild preference for forward and backward directions:
  \[
  P_R(\theta) \propto 1 + \cos^2\theta
  \]
  where \(\theta\) is the scattering angle.

- **Mie scattering** is produced by particles comparable in size to the wavelength (aerosols, dust, water droplets). Its wavelength dependence is weaker (closer to \(1/\lambda\) or even flatter) and its phase function is strongly forward-peaked. Mie scattering therefore adds a whitish haze and the bright aureole (glow) immediately surrounding the sun or moon. When the sun is low, the long atmospheric path removes most blue light by Rayleigh scattering, leaving the reddish light that reaches the observer; the remaining Mie component still produces the solar disc glow and the twilight arch.

A full physically-based atmosphere also includes:
- extinction (Beer–Lambert law) along the view ray and the sun ray,
- single scattering (sunlight scattered once into the view ray),
- multiple scattering (light scattered several times, important for the overall brightness and the colour of the horizon at twilight),
- ground reflectance (optional albedo contribution).

Exact evaluation of the radiative-transfer equation is far too expensive for real-time use on a six-year-old GPU. Analytic or tabulated approximations are therefore employed.

### Published analytic sky models

The most widely referenced real-time analytic models are the following.

1. **Nishita et al. (1993 / 1996)**  
   Early analytic single-scattering models that integrate the Rayleigh and Mie contributions along a view ray under a simplified exponential atmosphere. They correctly reproduce the blue zenith, the reddening of the setting sun, and a basic solar aureole. Multiple scattering is either omitted or approximated by a simple additive term. The integrals are still relatively costly for a full-screen sky pass on modest hardware.

2. **Preetham, Shirley & Smits (1999)** – “A Practical Analytic Model for Daylight”  
   The classic real-time model. It starts from the Perez all-weather luminance distribution (originally developed for architectural daylighting) and extends it with analytic chromaticity formulas. Parameters are turbidity (a single scalar that controls aerosol content), solar zenith angle, and view direction. The model yields both luminance and xy chromaticity, which can be converted to RGB. It captures the overall sky gradient, the horizon brightening, and a reasonable solar aureole. Implementation cost is a few dozen arithmetic operations plus a small number of trigonometric evaluations per sample. Limitations: the original formulas were fitted to a limited range of turbidities and can produce overly saturated or greenish colours at extreme solar elevations; multiple scattering is only implicitly present.

3. **Hosek & Wilkie (2012 / 2013)**  
   A more accurate successor to Preetham. It uses a richer set of basis functions fitted to a large dataset of radiative-transfer simulations. Separate parameterisations exist for spectral radiance and for RGB. Turbidity, ground albedo, and solar elevation are the main inputs. The model produces noticeably better twilight colours, a more convincing solar disc glow, and reduced colour artefacts. Evaluation cost is comparable to Preetham (still analytic, no tables required at run time) but the coefficient tables are larger. A spectral version is available for higher fidelity when the rest of the pipeline is spectral.

4. **Bruneton & Neyret (2008)** and later precomputed atmosphere models  
   These methods precompute multiple-scattering integrals into 3-D or 4-D lookup tables (view zenith, sun zenith, view–sun angle, altitude). At run time a few texture fetches plus simple arithmetic reconstruct the in-scattered radiance. Quality is high, including realistic aerial perspective for distant terrain, but memory and the need for a precomputation step make them heavier than pure analytic models. On a GTX 1060 the texture bandwidth is acceptable if the tables are kept modest, yet the approach exceeds the “few arithmetic ops” budget of a pure gradient.

5. **Other practical approximations**  
   - Simple exponential or polynomial gradients fitted by artists (the approach closest to walkgrid’s current solution).  
   - “Skybox + sun disc” methods that combine a static or low-frequency cubemap with an additive analytic sun glow.  
   - Hybrid models that evaluate Preetham/Hosek only for the upper hemisphere and fall back to a cheaper gradient near the horizon.

All of the analytic models above treat the atmosphere as a plane-parallel or spherically symmetric medium of finite height; they do not attempt to resolve individual clouds or local pollution.

### What a cheap sky model needs

For a 60 fps title whose world already consumes 2–3 ms, a sky pass should ideally stay under a few tenths of a millisecond. That constraint implies:

- no per-pixel ray marching through the atmosphere,
- at most a handful of transcendental functions (sin, cos, exp, pow) per sample,
- preferably no dependent texture fetches (or only a very small, cache-friendly LUT),
- parameters that can be updated once per frame from the existing sun/moon clocks (solar elevation, turbidity or equivalent artistic controls).

The essential visual features that must still be present are:

- a plausible blue-to-white gradient from zenith to horizon,
- progressive reddening as the sun approaches the horizon,
- a soft, bright aureole around the sun whose colour and size change with elevation,
- a twilight band or arch that appears opposite or adjacent to the sun at low elevations,
- continuity so that the sky does not “pop” when the sun crosses the horizon.

A hand-tuned gradient plus a twilight term plus a solar glow already supplies the first, third and fourth of these features; the second (physically consistent reddening) and a more accurate aureole shape are the main gaps that an analytic model can close at modest extra cost.

## Comparison

| Method                  | GPU cost (relative) | Visual quality (day + twilight) | Implementation complexity | Notes |
|-------------------------|---------------------|---------------------------------|---------------------------|-------|
| Walkgrid hand-tuned gradient + twilight + glow | 1× (baseline)      | Good artistic control; limited physical reddening | Very low                  | Already meets budget |
| Preetham 1999           | ~2–3×               | Solid daylight; weaker twilight | Low–medium                | Classic, widely documented |
| Hosek–Wilkie 2012       | ~3–4×               | Better twilight & aureole       | Medium                    | Larger coefficient set |
| Nishita-style analytic  | ~4–6×               | Good single-scatter             | Medium                    | More transcendental ops |
| Bruneton-style LUT      | ~5–10× + bandwidth  | Highest (multi-scatter)         | High (precompute)         | Overkill for pure sky dome |
| Pure artist gradient (no analytic) | 1×             | Fully controllable but non-physical | Lowest                 | Current approach |

Costs are order-of-magnitude estimates for a full-screen or sky-dome evaluation on GTX-1060-class hardware; absolute numbers depend on shader compiler and whether the sky is drawn as a dome or as a screen-space pass.

## What we might try

Ordered by expected value for walkgrid (quality gain per added cost and risk):

1. **Keep the existing gradient and replace only the solar glow / twilight term with a Preetham-style analytic aureole.**  
   Evaluate the Preetham luminance and chromaticity solely in a narrow cone around the sun direction and blend it with the hand-tuned sky. This adds the missing physical reddening and a better aureole while leaving the bulk of the sky under artistic control. Cost remains close to the present baseline.

2. **Full Preetham evaluation for the entire sky, driven by the existing solar elevation and a single turbidity slider.**  
   Replace the gradient with the analytic formula. Add a small artistic “saturation” or “horizon lift” multiplier so the look can still be tuned. Provides consistent colour across the whole day–night cycle. Still well inside the frame budget.

3. **Hosek–Wilkie if twilight quality becomes a priority.**  
   Same interface as Preetham (turbidity + solar elevation) but improved low-sun colours. Slightly higher arithmetic cost and larger static coefficient tables; still analytic and table-free at run time.

4. **Optional low-resolution LUT for multiple scattering (Bruneton-style) behind a quality setting.**  
   Precompute offline; at run time fetch only when the user enables “high sky”. Preserves the 60 fps guarantee on the reference hardware while offering a higher-fidelity path for stronger GPUs.

5. **Artist-facing controls that map onto the physical parameters.**  
   Expose “clarity / haze” (turbidity), “horizon warmth”, and “sun disc size” rather than raw RGB stops. This keeps the player-facing experience free of numbers while giving the team a principled way to match reference photographs or concept art.

None of the above require network access, sampled audio, third-party assets, or any change to the text-file localisation system. All can be disabled or reduced by a graphics-quality setting, satisfying the six-year-old-PC constraint.

## Sources

- Rayleigh, Lord (1871). “On the light from the sky, its polarization and colour.” Philosophical Magazine. (classic derivation of the \(\lambda^{-4}\) law)
- Mie, G. (1908). “Beiträge zur Optik trüber Medien.” Annalen der Physik. (original Mie theory)
- Nishita, T., et al. (1993). “Display of the Earth taking into account atmospheric scattering.” SIGGRAPH.  
  Nishita, T., et al. (1996). “Display of clouds taking into account multiple anisotropic scattering and sky light.” SIGGRAPH.
- Preetham, A. J., Shirley, P., & Smits, B. (1999). “A Practical Analytic Model for Daylight.” SIGGRAPH. (the standard real-time reference)
- Hosek, L., & Wilkie, A. (2012). “An Analytic Model for Full Spectral Sky-Dome Radiance.” ACM Transactions on Graphics (SIGGRAPH).  
  Hosek, L., & Wilkie, A. (2013). “Adding a Solar Radiance Function to the Hosek Skylight Model.” IEEE Computer Graphics and Applications.
- Bruneton, E., & Neyret, F. (2008). “Precomputed Atmospheric Scattering.” Computer Graphics Forum (EGSR).
- Perez, R., Seals, R., & Michalsky, J. (1993). “All-weather model for sky luminance distribution.” Solar Energy. (basis of the Preetham luminance distribution)
- CIE (Commission Internationale de l’Éclairage) standards on sky luminance and chromaticity (various technical reports; used as reference data for the analytic fits).

All sources are peer-reviewed publications or standards-body documents. No code, textures, or proprietary assets have been reproduced; only the published mathematical structure and qualitative behaviour are described.

END OF DELIVERY