# Fog and aerial perspective

## Ours

Distant ground fades into the sky colour over the loaded world’s last fifth. A faint exponential haze is applied on top. The fade hides the hard edge of the currently loaded terrain; the haze supplies a continuous atmospheric cue that objects farther away become less distinct and take on more of the sky’s tint.

ASSUMPTION: the fade is a linear blend of the ground’s final lit colour toward the sky buffer colour that begins at 80 % of the current load radius and reaches full sky colour at the load radius. ASSUMPTION: the exponential haze is a simple radial optical-depth term of the form exp(-d · density) with a small constant density, applied after lighting and before the edge fade.

## The theory

### Exponential fog and height fog

The classic single-scattering model for homogeneous fog treats the atmosphere as a medium with constant extinction coefficient β. The fraction of light that travels a distance d without being scattered or absorbed is the transmittance

T(d) = exp(-β d).

The in-scattered light that reaches the eye is approximated by the integral of the medium’s emission (or ambient) colour E along the ray, attenuated by the remaining transmittance. For a constant E this integral evaluates to

L = T(d) · L_surface + (1 - T(d)) · E.

This is the familiar “exponential fog” formula used since the early 1990s. Because β is constant, the optical depth is simply proportional to distance; the same formula works for both forward and reverse depth buffers.

When the medium density varies with height (ground fog, low-lying mist), β becomes a function of altitude. The most common practical form is an exponential height fall-off:

β(h) = β₀ · exp(-h / H),

where H is a scale height (typically 100–300 m for ground fog). The optical depth along a ray is then the integral of β(h(s)) ds. For a straight ray this integral has a closed form involving the difference of two exponentials, often written with the auxiliary function

F(x) = (1 - exp(-x)) / x

so that the transmittance remains analytic. The resulting “exponential height fog” appears in many real-time engines and is documented in the Real-Time Rendering literature and in GPU Gems 3.

A further refinement replaces the constant emission E with a direction-dependent sky radiance that already contains the sun’s contribution. The fog colour is then sampled from the same sky model that lights the rest of the scene, guaranteeing consistency between distant terrain and the horizon.

### Aerial perspective

Aerial perspective is the systematic change in colour and contrast of objects with increasing distance caused by scattering in the atmosphere. Rayleigh scattering (molecules) is strongly wavelength-dependent (∝ λ⁻⁴) and produces the blue of the daytime sky; Mie scattering (aerosols) is weaker in wavelength dependence and produces the white haze of dusty or humid air.

The physically based models that capture both effects solve the radiative-transfer equation under the single-scattering approximation. Two widely cited analytic solutions are:

- The Preetham model (1999) and its later improvements. It parameterises turbidity and produces a continuous sky radiance plus an in-scattering term that can be evaluated along any view ray.
- The Hosek–Wilkie model (2012) which improves the Preetham fit, especially near the horizon and for high turbidities, and supplies both sky and solar radiance.

For real-time use these models are usually pre-integrated into look-up tables or fitted polynomials so that a pixel shader can evaluate optical depth and in-scattered colour with a few texture fetches or arithmetic operations. When the atmosphere is assumed to be stratified, the optical depth between two points can be obtained from a 2-D table indexed by altitude and zenith angle (or from a closed-form expression similar to the height-fog integral).

A cheaper but still useful approximation is “distance fog with a height-modulated density and a sky-coloured in-scatter”. It does not reproduce the full Rayleigh/Mie phase functions, yet it correctly desaturates and tints distant geometry toward the horizon colour and can be evaluated in a single pass.

### Hiding the loaded world’s edge

Any streaming world has a finite loaded radius. Beyond that radius the geometry either does not exist or is a low-resolution placeholder. Visible discontinuities are avoided by one or more of the following techniques:

1. Soft distance fade – the final colour of terrain (or of its albedo before lighting) is linearly or smoothly interpolated toward the sky colour over the outermost fraction of the load radius. This is the method used by walkgrid.
2. Fog that reaches full opacity exactly at the load radius – the exponential (or height) fog density is chosen so that transmittance falls to a negligible value at the edge. The fade and the fog then reinforce each other.
3. Impostors or low-detail shells – a static or periodically updated mesh of the distant landscape is rendered behind the high-detail terrain; the transition zone is again softened by fog or alpha.
4. Clip-map or geometry-clipmap skirts – continuous geometric detail is maintained out to a larger radius, with the outermost rings deliberately fogged.
5. Skybox or skydome that matches the horizon colour of the atmospheric model, so that any remaining hard edge is camouflaged against an identical background.

In practice the combination of a soft colour fade over the last 10–20 % of the load distance plus a light exponential haze is sufficient for most outdoor games running on modest hardware; more elaborate impostor systems are reserved for titles that keep a much larger visible range.

## Comparison

| Method | Cost (GTX 1060 class) | Quality | Complexity | Notes relative to ours |
|--------|-----------------------|---------|------------|------------------------|
| Pure radial exponential fog | <0.05 ms | Low–medium (uniform haze) | Very low | Closest to our current haze; no height variation |
| Exponential height fog (analytic) | 0.05–0.1 ms | Medium–high (ground-hugging mist) | Low | Adds altitude dependence; still single-pass |
| Preetham / Hosek–Wilkie LUT | 0.1–0.3 ms | High (physically plausible colour & contrast) | Medium | Requires pre-computed tables or fit coefficients |
| Full single-scattering ray-march | >1 ms | Highest | High | Far exceeds our 2–3 ms world budget unless heavily limited |
| Soft edge fade only | negligible | Medium (hides pop-in) | Very low | Exactly our current edge treatment |
| Fade + light exponential haze | <0.1 ms | Medium | Very low | Our present combination |
| Impostor / clip-map shells | 0.2–0.5 ms + memory | High (extended range) | High | Useful later if load radius grows |

All figures assume a full-screen pass or per-pixel evaluation on a 1080p target; they scale roughly with pixel count.

## What we might try

Ordered by expected value for walkgrid (60 fps on GTX-1060-class hardware, world budget 2–3 ms):

1. Make the existing exponential haze height-dependent.  
   Replace the constant density with β(h) = β₀ exp(-h/H). The analytic optical-depth integral is inexpensive and immediately gives more realistic ground fog without extra textures. Value: high; cost: negligible.

2. Drive the in-scatter colour from the same sky model that already lights the scene.  
   Instead of a fixed fog colour, sample the horizon colour of the current sky (or a cheap analytic approximation). Guarantees consistency between distant terrain and the sky dome. Value: high; cost: one extra sky evaluation or a small LUT.

3. Tighten the edge fade so that transmittance of the haze is already near zero at the load radius.  
   The soft colour blend then only has to hide residual geometric aliasing. Value: medium; cost: zero (parameter tweak).

4. Add a simple 2-D optical-depth LUT for stratified atmosphere (altitude × zenith angle).  
   Enables a limited form of aerial perspective (blue shift, contrast loss) while staying well inside the frame budget. Value: medium; cost: 0.1–0.2 ms plus a 64×64 table.

5. Introduce a low-resolution distant terrain shell or impostor ring beyond the current load radius, still fully fogged.  
   Extends perceived range without loading extra high-detail cells. Value: medium–high if the load radius later grows; cost: extra draw call and memory, therefore optional behind a quality setting.

6. Full Preetham/Hosek–Wilkie evaluation or multi-scatter ray-march.  
   Highest fidelity but exceeds the present world budget unless heavily approximated or made optional. Value: low for the current target hardware; revisit only after the rest of the pipeline is optimised.

Items 1–3 can be implemented with a few arithmetic operations and keep the existing single-pass structure. Items 4–5 add modest resources and should be gated by a graphics-quality setting so that the six-year-old PC remains at 60 fps when they are disabled.

## Sources

- Real-Time Rendering, 4th ed., Akenine-Möller, Haines, Hoffman et al., CRC Press, 2018 – chapters on atmospheric effects and fog.
- GPU Gems 3, NVIDIA, 2007 – “Aerial Perspective” and height-fog derivations.
- Preetham, A. J., Shirley, P., Smits, B. “A Practical Analytic Model for Daylight.” SIGGRAPH 1999.
- Hosek, L., Wilkie, A. “An Analytic Model for Full Spectral Sky-Dome Radiance.” ACM TOG 2012.
- Nishita, T. et al. “Display of the Earth Taking into Account Atmospheric Scattering.” SIGGRAPH 1993 (foundational single-scattering atmosphere).
- Bruneton, E., Neyret, F. “Precomputed Atmospheric Scattering.” EGSR 2008 (LUT methods used in later real-time engines).
- IEEE and ACM peer-reviewed literature on exponential height fog integrals (standard closed-form expressions appearing in the above references).