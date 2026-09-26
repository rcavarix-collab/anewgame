// terrain.cpp -- walkgrid-hills v1; see terrain.h (DESIGN.md 23.2).
//
// Layer 3. Everything here is a pure function of the seed and the
// coordinates: 64-bit integer hashing into double-precision value noise
// (no transcendental calls, no fused operations), so every build and
// every machine regenerates the same ground bit for bit.

#include "terrain.h"
#include "common.h"
#include <algorithm>
#include <cmath>

namespace {

uint64_t Mix(uint64_t h) {
    h ^= h >> 33; h *= 0xFF51AFD7ED558CCDull; h ^= h >> 33; h *= 0xC4CEB9FE1A85EC53ull; h ^= h >> 33;
    return h;
}
uint64_t Salt(uint64_t seed, uint64_t k) { return Mix(seed ^ (k * 0x9E3779B97F4A7C15ull)); }
double Lattice(int64_t x, int64_t z, uint64_t seed) {
    uint64_t h = Mix(seed ^ ((uint64_t)x * 0x9E3779B97F4A7C15ull) ^ ((uint64_t)z * 0xC2B2AE3D27D4EB4Full));
    return (double)(h >> 11) * (1.0 / 9007199254740992.0);
}
// Smooth value noise, 0..1.
double Noise(double x, double z, uint64_t seed) {
    double fx = std::floor(x), fz = std::floor(z);
    int64_t ix = (int64_t)fx, iz = (int64_t)fz;
    double u = x - fx, v = z - fz;
    u = u * u * (3 - 2 * u); v = v * v * (3 - 2 * v);
    double a = Lattice(ix, iz, seed), b = Lattice(ix + 1, iz, seed);
    double c = Lattice(ix, iz + 1, seed), d = Lattice(ix + 1, iz + 1, seed);
    double top = a + (b - a) * u, bot = c + (d - c) * u;
    return top + (bot - top) * v;
}
double Smooth(double e0, double e1, double x) {
    double t = (x - e0) / (e1 - e0);
    t = t < 0 ? 0 : t > 1 ? 1 : t;
    return t * t * (3 - 2 * t);
}

// Salts, one per field (changing any is a new version).
enum { S_BROAD = 1, S_MID, S_SMALL, S_PLATEAU, S_ROCK, S_REGION, S_PATCH, S_PATCH2, S_SLATE };

// How far into a plateau (0 outside, 1 on top): a narrow smoothstep of a
// broad field, so plateaus are wide and their edges are cliffs.
double Plateau(uint64_t seed, int wx, int wz) {
    return Smooth(0.60, 0.635, Noise(wx / 90.0, wz / 90.0, Salt(seed, S_PLATEAU)));
}
double Rock(uint64_t seed, int wx, int wz) { return Noise(wx / 15.0, wz / 15.0, Salt(seed, S_ROCK)); }

// The top material and what lies under it, for a column of height h
// whose steepest drop to a neighbour is `slope` cells.
struct Layers { uint8_t top, under, body; bool plateau; };
Layers LayersAt(uint64_t seed, int wx, int wz, int h, int slope, int version) {
    Layers L;
    double p = Plateau(seed, wx, wz), rock = Rock(seed, wx, wz);
    double region = Noise(wx / 120.0, wz / 120.0, Salt(seed, S_REGION));
    double patch = Noise(wx / 9.0, wz / 9.0, Salt(seed, S_PATCH));
    double patch2 = Noise(wx / 7.0, wz / 7.0, Salt(seed, S_PATCH2));
    L.plateau = p > 0.5;
    L.body = (uint8_t)(L.plateau ? BLOCK_SANDSTONE : BLOCK_STONE);
    uint8_t grass = (uint8_t)(region < 0.40 ? BLOCK_MEADOW_GRASS : region < 0.66 ? BLOCK_DRY_TURF : BLOCK_MOSS);
    uint8_t soil = (uint8_t)(grass == BLOCK_MOSS ? BLOCK_LOAM : BLOCK_DIRT);
    if (h >= 45) { L.top = BLOCK_SNOW; L.under = L.plateau ? BLOCK_SANDSTONE : BLOCK_STONE; }
    // v2: the foot of a mesa's cliff (raised by it, steep, not yet plateau)
    // is sandstone like the cliff above, where v1 bared stone in a rhythm.
    else if (version >= 2 && !L.plateau && p > 0.08 && slope >= 3) { L.top = BLOCK_SANDSTONE; L.under = BLOCK_SANDSTONE; L.body = BLOCK_SANDSTONE; }
    else if (L.plateau) {
        if (slope >= 3) { L.top = BLOCK_SANDSTONE; L.under = BLOCK_SANDSTONE; }  // the cliff's rim
        else { L.top = patch > 0.74 ? soil : grass; L.under = soil; }
    }
    else if (rock > 0.74) { L.top = rock > 0.80 ? BLOCK_SLATE : BLOCK_STONE; L.under = L.top; }
    else if (slope >= 3) { L.top = BLOCK_STONE; L.under = BLOCK_STONE; }          // steep ground shows rock
    else if (h <= 20) { L.top = BLOCK_SAND; L.under = BLOCK_SAND; }
    else if (h <= 21) { L.top = patch > 0.5 ? BLOCK_GRAVEL : BLOCK_SAND; L.under = BLOCK_SAND; }
    else if (h <= 23 && patch2 > 0.62) { L.top = BLOCK_CLAY; L.under = BLOCK_CLAY; }
    else { L.top = patch > 0.72 ? soil : patch2 > 0.80 ? (uint8_t)BLOCK_GRAVEL : grass; L.under = soil; }
    return L;
}

} // namespace

int HillsHeight(uint64_t seed, int wx, int wz) {
    double n1 = Noise(wx / 72.0, wz / 72.0, Salt(seed, S_BROAD));
    double n2 = Noise(wx / 26.0, wz / 26.0, Salt(seed, S_MID));
    double n3 = Noise(wx / 11.0, wz / 11.0, Salt(seed, S_SMALL));
    double h = 27.0 + 18.0 * (n1 - 0.5) + 6.0 * (n2 - 0.5) + 2.2 * (n3 - 0.5);
    double p = Plateau(seed, wx, wz);
    h += 9.0 * p;
    double rock = Rock(seed, wx, wz);
    if (rock > 0.74 && p < 0.5) h += (rock - 0.74) * 22.0;   // outcrops, up to ~5 cells proud
    int ih = (int)std::floor(h);
    return std::max(4, std::min(Y_MAX - 8, ih));
}

void HillsColumn(uint64_t seed, int cx, int cz, TerrainColumn& out, int version) {
    const int N = CHUNK_SIZE;
    int bx = cx * N, bz = cz * N;
    // Heights one cell beyond the column, for the slope.
    int hg[N + 2][N + 2];
    for (int z = 0; z < N + 2; z++)
        for (int x = 0; x < N + 2; x++) hg[z][x] = HillsHeight(seed, bx + x - 1, bz + z - 1);
    int maxH = 0;
    for (int z = 1; z <= N; z++) for (int x = 1; x <= N; x++) maxH = std::max(maxH, hg[z][x]);
    out.chunks = maxH / N + 1;
    out.present.assign(out.chunks, false);
    out.cells.assign((size_t)out.chunks * CHUNK_CELLS, 0);
    for (int lz = 0; lz < N; lz++)
        for (int lx = 0; lx < N; lx++) {
            int h = hg[lz + 1][lx + 1];
            int slope = 0;
            slope = std::max(slope, h - hg[lz + 1][lx]);
            slope = std::max(slope, h - hg[lz + 1][lx + 2]);
            slope = std::max(slope, h - hg[lz][lx + 1]);
            slope = std::max(slope, h - hg[lz + 2][lx + 1]);
            int wx = bx + lx, wz = bz + lz;
            Layers L = LayersAt(seed, wx, wz, h, slope, version);
            // v2: grass wraps down over the side of a step, as turf does,
            // instead of baring the soil under it on every 2-cell rise.
            bool grassy = L.top == BLOCK_MEADOW_GRASS || L.top == BLOCK_DRY_TURF || L.top == BLOCK_MOSS;
            // Counting diagonal neighbours too, and one cell deeper than the
            // bared side: the lower ground's surface corners touch the cell
            // under the step's edge, and the mesher blends in every cell
            // around a corner -- a soil cell there put a dirt triangle at
            // every step corner, evenly spaced across the slope.
            int wrap = 1; // cells from the top that show the top material (flat ground: just the top)
            if (version >= 2 && grassy) {
                int drop = slope;
                for (int dz = -1; dz <= 1; dz += 2)
                    for (int dx = -1; dx <= 1; dx += 2) drop = std::max(drop, h - hg[lz + 1 + dz][lx + 1 + dx]);
                wrap = drop > 0 ? drop + 1 : 1;
            }
            // Slate lies in the deep rock, in broad pockets.
            bool slateDeep = Noise(wx / 23.0, wz / 23.0, Salt(seed, S_SLATE)) > 0.55;
            for (int y = 0; y <= h; y++) {
                uint8_t m;
                int depth = h - y;
                if (y == 0) m = BLOCK_FOUNDATION;
                else if (depth < wrap) m = L.top;
                else if (depth <= 3) m = L.under;
                else if ((L.plateau || L.body == BLOCK_SANDSTONE) && y > h - 12) m = BLOCK_SANDSTONE; // v1: body is sandstone only on the plateau
                else m = (uint8_t)((slateDeep && y < 14) ? (int)BLOCK_SLATE : L.body == BLOCK_SANDSTONE ? (int)BLOCK_STONE : (int)L.body);
                int cy = y / N;
                out.present[cy] = true;
                out.cells[(size_t)cy * CHUNK_CELLS + (size_t)((y % N) * N + lz) * N + lx] = m;
            }
        }
}
