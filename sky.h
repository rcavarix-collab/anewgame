// sky.h
//
// The day/night model (DESIGN.md Part XIII): where the sun and moon are,
// how bright the day is, and how far the star field has turned -- all
// pure functions of the one day clock, so the sky, world lighting and
// shadows can never disagree with each other or with the music (whose
// sections they line up with: sunrise as Dawn begins, sunset inside
// Dusk, ten minutes of real night). Header-only, no D3D; tested natively.

#pragma once

#include "world.h" // DAY_LENGTH_SECONDS

struct SkyState {
    Vec3 sunDir;      // unit vector toward the sun (below the horizon at night)
    Vec3 moonDir;     // unit vector toward the moon
    float moonLit;    // 0 new .. 1 full: how much of the moon's face the sun lights
    float solarEclipse; // 0..1: how much of the sun's disc the moon covers
    float lunarUmbra; // 0..1: how much of the moon's disc is in the world's full shadow
    float lunarPenumbra; // 0..1: ... in its outer, partial shadow
    float daylight;   // world light multiplier: NIGHT_LIGHT at night .. 1 in full day
    float sunLight;   // 0..1: how much direct sun there is (drives shadows; 0 once set)
    float starsVisible; // 0..1 star field opacity
    float starAngle;  // radians the star field has turned about the pole (normal E-W streaming)
};

static const float SUNRISE_SECONDS = 0.0f;    // Dawn begins
static const float SUNSET_SECONDS = 3000.0f;  // 50:00, inside Dusk (47-55)
static const float NIGHT_LIGHT = 0.30f;       // darkest the world gets (still playable)

// The sky's clocks (D57; DESIGN.md Part XIII). The sun keeps the day; the
// moon and stars run on their own, compressed but in real proportion:
//  - the stars gain one turn a year on the sun (about 1 degree a day), as
//    real stars rise a little earlier each night;
//  - the moon goes round in 8 days (new, waxing, full, waning), drifting
//    east against the sun, so it rises later each day;
//  - its path is tilted 10 degrees to the sun's, and where the two cross
//    (the nodes) turns once in 34.7 days, so eclipses come in seasons:
//    only a new or full moon near a crossing lines up. On average an
//    eclipse every 16 days (lunar about every 25, solar about every 47),
//    some of them total (simulated over 560 days).
// The discs' sizes (radians) are what the sky shader draws.
static const float MOON_MONTH_DAYS = 8.0f;
static const float MOON_TILT = 0.1745f;          // 10 degrees
static const float MOON_NODE_DAYS = 34.7f;
static const float STAR_YEAR_DAYS = 365.0f;
static const float SUN_DISC_RADIUS = 0.0283f;    // 1.62 degrees: the sun's bright core
static const float MOON_DISC_RADIUS = 0.0332f;   // 1.9 degrees: a little larger than the sun, so it can cover it whole
static const float UMBRA_RADIUS = 0.0454f;       // the world's full shadow at the moon's distance
static const float PENUMBRA_RADIUS = 0.0785f;    // its partial shadow

// How much of a disc of radius r1 a disc of radius r2 covers, their
// centres d apart (small angles: flat geometry), 0..1.
static inline float DiscCover(float r1, float r2, float d) {
    if (d >= r1 + r2) return 0.0f;
    if (d <= fabsf(r1 - r2)) return r2 >= r1 ? 1.0f : (r2 * r2) / (r1 * r1);
    float a1 = acosf((d * d + r1 * r1 - r2 * r2) / (2 * d * r1)), a2 = acosf((d * d + r2 * r2 - r1 * r1) / (2 * d * r2));
    float lens = r1 * r1 * (a1 - 0.5f * sinf(2 * a1)) + r2 * r2 * (a2 - 0.5f * sinf(2 * a2));
    return lens / (3.14159265f * r1 * r1);
}
// The high wind that carries the clouds (D58): a jet stream blowing from
// west to east most of the time, its direction wandering over hours and
// now and then swinging well off course (up to ~60 degrees) before
// settling back. Returns the angle it blows toward, radians from east
// (+x) toward north (+z), from `T` days since the world began. Smooth in
// T; the clouds' drift is the sum of this wind over time (render.cpp), so
// a change of direction never jumps them.
static inline float SkyWiggle(double x, uint32_t salt) {
    // Smooth value noise in one dimension, -1..1.
    double f = floor(x); float u = (float)(x - f);
    auto h = [&](int64_t i) { uint64_t z = (uint64_t)i * 0x9E3779B97F4A7C15ull ^ ((uint64_t)salt << 32); z ^= z >> 31; z *= 0xBF58476D1CE4E5B9ull; z ^= z >> 29; return (float)((z >> 11) * (1.0 / 9007199254740992.0)) * 2.0f - 1.0f; };
    float a = h((int64_t)f), b = h((int64_t)f + 1);
    u = u * u * (3 - 2 * u);
    return a + (b - a) * u;
}
static inline float JetStreamAngle(double T) {
    float wander = 0.35f * SkyWiggle(T * 3.0, 1);                      // over a few game hours
    float swing = SkyWiggle(T / 2.5, 2);                               // every few game days...
    swing = swing * swing * swing * 1.05f;                             // ...rarely far: up to ~60 degrees
    return wander + swing;
}

static inline float SkyAngle(Vec3 a, Vec3 b) { float c = Dot(a, b); return acosf(c > 1 ? 1 : c < -1 ? -1 : c); }

static inline float SkySmooth(float e0, float e1, float x) {
    float t = (x - e0) / (e1 - e0);
    t = t < 0 ? 0 : (t > 1 ? 1 : t);
    return t * t * (3 - 2 * t);
}

// `day`: whole days since the world began (saved with it); the moon and
// stars need it, the sun doesn't.
static inline SkyState ComputeSky(float dayTime, uint32_t day = 0) {
    const float PI = 3.14159265f;
    float t = fmodf(dayTime, DAY_LENGTH_SECONDS);
    if (t < 0) t += DAY_LENGTH_SECONDS;
    // Sun angle along its arc: 0 at sunrise (due east), pi at sunset
    // (due west), carrying on below the horizon through the night.
    float a = t < SUNSET_SECONDS ? PI * (t - SUNRISE_SECONDS) / (SUNSET_SECONDS - SUNRISE_SECONDS)
                                 : PI + PI * (t - SUNSET_SECONDS) / (DAY_LENGTH_SECONDS - SUNSET_SECONDS);
    SkyState s;
    // As at the equator (owner's call: the sky reads more plainly): the sun
    // rises due east, passes straight overhead and sets due west, its whole
    // path in one vertical plane. Noon shadows fall straight down.
    s.sunDir = Normalize(kEast * cosf(a) + kUp * sinf(a));
    // Days since the world began, continuously (a double: exact for ages).
    double T = (double)day + (double)dayTime / DAY_LENGTH_SECONDS; // unwrapped: 3600 s into day 0 is day 1
    // The moon's angle ahead of the sun along the sky. A new world starts
    // ~140 degrees (a waning moon still up in the west at the first
    // sunrise); it falls back 45 degrees a day: waning, new, waxing, full.
    float E = (float)(2.45 - 2.0 * PI * fmod(T / MOON_MONTH_DAYS, 1.0));
    float node = (float)(0.9 + 2.0 * PI * fmod(T / MOON_NODE_DAYS, 1.0));
    float beta = MOON_TILT * sinf(E - node);     // north or south of the sun's path
    float m = a + E;
    s.moonDir = Normalize(kEast * (cosf(m) * cosf(beta)) + kUp * (sinf(m) * cosf(beta)) + kNorth * sinf(beta));
    s.moonLit = 0.5f * (1.0f - Dot(s.sunDir, s.moonDir));
    // Eclipses, from the discs themselves: the moon over the sun; the
    // moon in the world's shadow, which points away from the sun.
    s.solarEclipse = DiscCover(SUN_DISC_RADIUS, MOON_DISC_RADIUS, SkyAngle(s.sunDir, s.moonDir));
    Vec3 antiSun = { -s.sunDir.x, -s.sunDir.y, -s.sunDir.z };
    float dShadow = SkyAngle(antiSun, s.moonDir);
    s.lunarUmbra = DiscCover(MOON_DISC_RADIUS, UMBRA_RADIUS, dShadow);
    s.lunarPenumbra = DiscCover(MOON_DISC_RADIUS, PENUMBRA_RADIUS, dShadow);
    // A solar eclipse barely dims the day until the sun is nearly gone,
    // then the day falls toward night in the last moments of cover.
    // The land's light follows the eclipse: direct sun as the share of
    // the disc still showing (so shadows fade with it), and the whole
    // day's light falling toward night as the last of the sun goes.
    float dim = 1.0f - 0.93f * SkySmooth(0.30f, 1.0f, s.solarEclipse);
    float up = SkySmooth(-0.12f, 0.25f, s.sunDir.y) * dim;
    s.daylight = NIGHT_LIGHT + (1.0f - NIGHT_LIGHT) * up;
    // Direct sun arrives within a minute or two of sunrise (low, orange,
    // long shadows) rather than after the sun has climbed ~9 degrees. It
    // ends exactly at the horizon: below it the sun moves 5x faster (the
    // night is short), which would turn the last of the fade into a snap.
    s.sunLight = SkySmooth(0.0f, 0.10f, s.sunDir.y) * (1.0f - s.solarEclipse);
    s.starsVisible = 1.0f - SkySmooth(-0.20f, 0.05f, s.sunDir.y) * dim;
    // The stars turn with the sky (the sun's angle: slow through the day,
    // quick through the short night) and gain a turn a year on the sun.
    s.starAngle = a + (float)(2.0 * PI * fmod(T / STAR_YEAR_DAYS, 1.0));
    return s;
}

// The sun's orthographic view-projection for the shadow map: centred on
// `eye`, covering +-`extent` blocks across and +-`depthHalf` along the
// light, with the centre snapped to whole texels of a `mapSize` map so
// the texel grid doesn't crawl (shimmer) as the player moves.
static inline Mat4 ShadowLightViewProj(Vec3 eye, Vec3 sun, float extent, float depthHalf, int mapSize) {
    Vec3 up = fabsf(sun.y) > 0.99f ? Vec3{ 0, 0, 1 } : Vec3{ 0, 1, 0 };
    Mat4 view = MatLookToLH({ 0, 0, 0 }, { -sun.x, -sun.y, -sun.z }, up);
    float texel = 2.0f * extent / mapSize;
    float lx = eye.x * view.m[0][0] + eye.y * view.m[1][0] + eye.z * view.m[2][0];
    float ly = eye.x * view.m[0][1] + eye.y * view.m[1][1] + eye.z * view.m[2][1];
    float lz = eye.x * view.m[0][2] + eye.y * view.m[1][2] + eye.z * view.m[2][2];
    lx = floorf(lx / texel) * texel;
    ly = floorf(ly / texel) * texel;
    return MatMul(MatMul(view, MatTranslation(-lx, -ly, -(lz - depthHalf))), MatOrthoLH(2 * extent, 2 * extent, 0.0f, 2 * depthHalf));
}

// Rotation by `angle` about unit `axis` (Rodrigues), for column vectors:
// v' = m * v.
static inline void AxisAngleMatrix(Vec3 axis, float angle, float m[3][3]) {
    float c = cosf(angle), s = sinf(angle), k = 1 - c;
    float x = axis.x, y = axis.y, z = axis.z;
    m[0][0] = c + x * x * k;     m[0][1] = x * y * k - z * s; m[0][2] = x * z * k + y * s;
    m[1][0] = y * x * k + z * s; m[1][1] = c + y * y * k;     m[1][2] = y * z * k - x * s;
    m[2][0] = z * x * k - y * s; m[2][1] = z * y * k + x * s; m[2][2] = c + z * z * k;
}

// The celestial pole: perpendicular to the sun's path (ComputeSky), so the
// stars turn about the same axis, in the same sense, as the sun does --
// the normal east-to-west streaming. At the equator it lies on the
// northern horizon: every star rises straight up out of the east.
static inline Vec3 CelestialPole() { return kNorth; }

// ---- Atmosphere: the colours of light at this time of day (Part XIII) ----
// All linear-light RGB (the shaders tonemap and convert to sRGB at the
// end). One function feeds the sky shader, the world shader's lighting
// and fog, and the CPU preview used to tune them -- so the fog always
// matches the sky behind it and nothing drifts apart.
struct Atmosphere {
    Vec3 sunColor;     // direct sunlight reaching the ground (0 at night)
    Vec3 moonColor;    // direct moonlight (0 by day / when set)
    Vec3 zenith;       // clear sky straight up
    Vec3 horizon;      // clear sky at the horizon
    Vec3 twilight;     // colour of the band around a low sun
    Vec3 ambientUp;    // light from the sky onto up-facing surfaces
    Vec3 ambientDown;  // bounce light onto down-facing surfaces
    float exposure;    // a fake eye adaptation: brighter at night
    float twilightAmount; // 0..1 how strong the sunset/sunrise band is
};

static inline Vec3 SkyLerp(Vec3 a, Vec3 b, float t) { return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t }; }
static inline Vec3 SkyScale(Vec3 a, float s) { return { a.x * s, a.y * s, a.z * s }; }

static inline Atmosphere ComputeAtmosphere(const SkyState& s) {
    Atmosphere a;
    float day = (s.daylight - NIGHT_LIGHT) / (1.0f - NIGHT_LIGHT);   // 0 night .. 1 day
    float high = SkySmooth(0.0f, 0.45f, s.sunDir.y);                 // sun well up
    // Sunlight: white-gold high in the sky, orange as it nears the horizon.
    Vec3 sunLow = { 1.00f, 0.50f, 0.22f }, sunHigh = { 1.00f, 0.95f, 0.86f };
    a.sunColor = SkyScale(SkyLerp(sunLow, sunHigh, high), 2.3f * s.sunLight);
    // Moonlight: faint and blue, only while the moon is up and the sun isn't.
    float moonUp = SkySmooth(-0.02f, 0.15f, s.moonDir.y) * (1.0f - day);
    // As bright as the lit share of its face (a full moon lights the land,
    // a new one doesn't), dimmed by the world's shadow in a lunar eclipse.
    float moonBright = (0.08f + 0.92f * s.moonLit) * (1.0f - 0.92f * s.lunarUmbra) * (1.0f - 0.35f * s.lunarPenumbra);
    a.moonColor = SkyScale({ 0.55f, 0.65f, 1.0f }, 0.28f * moonUp * moonBright);
    a.zenith = SkyLerp({ 0.004f, 0.006f, 0.018f }, { 0.10f, 0.28f, 0.78f }, day);
    a.horizon = SkyLerp({ 0.012f, 0.016f, 0.035f }, { 0.55f, 0.68f, 0.90f }, day);
    a.twilightAmount = (1.0f - high) * SkySmooth(-0.18f, 0.02f, s.sunDir.y) * (1.0f - SkySmooth(0.25f, 0.45f, s.sunDir.y));
    a.twilight = { 1.00f, 0.36f, 0.11f };
    // Ambient: the sky's own light from above, a warm dim bounce from below.
    a.ambientUp = SkyLerp({ 0.035f, 0.045f, 0.09f }, { 0.42f, 0.50f, 0.64f }, day);
    a.ambientDown = SkyLerp({ 0.012f, 0.012f, 0.02f }, { 0.20f, 0.17f, 0.13f }, day);
    a.exposure = 1.0f + 1.6f * (1.0f - day) * (1.0f - day); // twilight keeps its colour; only real night is lifted
    return a;
}
