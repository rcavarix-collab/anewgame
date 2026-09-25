// icons.cpp -- see icons.h.
//
// Each material's icon is a small faceted lump of it (M1.9): a few cells
// meshed by facetmesh.h exactly as the ground is, drawn by a tiny software
// rasterizer from a three-quarter view, textured the way the world shader
// does it (the top texture on ground up to ~50 degrees, the side beyond).

#include "icons.h"
#include "facetmesh.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

void RenderBlockIcons(BlockTextureSet& set) {
    const int N = BLOCK_TEX_SIZE, SS = 2, R = N * SS; // render at 2x, box down to 1x
    const float cy = cosf(0.785398f), sy = sinf(0.785398f); // 45 degrees around
    const float cp = cosf(0.5236f), sp = sinf(0.5236f);     // 30 degrees down
    const float scale = R * 0.2f;
    const Vec3 light = Normalize({ -0.4f, 1.0f, -0.6f });

    // The lump: a low, wide mound (so its top, the material's face, shows
    // most), in a 9^3 box of air.
    const int G = 9;
    std::vector<uint8_t> cells((size_t)G * G * G);
    std::vector<float> depth((size_t)R * R);
    std::vector<float> rgba((size_t)R * R * 4);

    for (int id = 1; id < BLOCK_COUNT; id++) {
        FacetMaterial mats[256];
        mats[id].solid = true;
        std::fill(cells.begin(), cells.end(), 0);
        for (int y = 3; y <= 4; y++)
            for (int z = 1; z <= 7; z++)
                for (int x = 1; x <= 7; x++) {
                    int r = std::abs(x - 4) + std::abs(z - 4);
                    if ((y == 3 && r <= 4 && std::abs(x - 4) <= 2 && std::abs(z - 4) <= 2) || (y == 4 && r <= 1))
                        cells[((size_t)y * G + z) * G + x] = (uint8_t)id;
                }
        FacetGrid g; g.nx = g.ny = g.nz = G; g.cells = cells.data(); g.mats = mats;
        FacetBuildParams p; p.bx1 = p.by1 = p.bz1 = G;
        FacetMesh m; FacetBuild(g, p, m);

        std::fill(depth.begin(), depth.end(), 1e9f);
        std::fill(rgba.begin(), rgba.end(), 0.0f);
        auto project = [&](Vec3 q, float& X, float& Y, float& Z) {
            float x = q.x - 4.5f, y = q.y - 4.0f, z = q.z - 4.5f;
            float rx = x * cy - z * sy, rz = x * sy + z * cy; // smaller rz = nearer the camera
            X = R * 0.5f + rx * scale;
            Y = R * 0.5f - (y * cp + rz * sp) * scale;
            Z = rz * cp - y * sp;
        };
        const uint8_t* texTop = set.mips[0].data() + (size_t)set.faceLayer[id][FACE_POS_Z][FACE_POS_Y] * N * N * 4;
        const uint8_t* texSide = set.mips[0].data() + (size_t)set.faceLayer[id][FACE_POS_Z][FACE_POS_X] * N * N * 4;
        for (size_t i = 0; i + 2 < m.idx.size(); i += 3) {
            Vec3 P[3] = { m.verts[m.idx[i]].pos, m.verts[m.idx[i + 1]].pos, m.verts[m.idx[i + 2]].pos };
            Vec3 n = Normalize(Cross(P[1] - P[0], P[2] - P[0]));
            float X[3], Y[3], Z[3];
            for (int k = 0; k < 3; k++) project(P[k], X[k], Y[k], Z[k]);
            float den = (Y[1] - Y[2]) * (X[0] - X[2]) + (X[2] - X[1]) * (Y[0] - Y[2]);
            if (fabsf(den) < 1e-6f) continue;
            float shade = 0.55f + 0.45f * std::max(0.0f, Dot(n, light));
            // Top texture on gentle ground, side texture on steep: as the world shader.
            const uint8_t* tex = n.y > 0.55f ? texTop : texSide;
            int x0 = std::max(0, (int)std::min({ X[0], X[1], X[2] })), x1 = std::min(R - 1, (int)std::max({ X[0], X[1], X[2] }) + 1);
            int y0 = std::max(0, (int)std::min({ Y[0], Y[1], Y[2] })), y1 = std::min(R - 1, (int)std::max({ Y[0], Y[1], Y[2] }) + 1);
            for (int py = y0; py <= y1; py++)
                for (int px = x0; px <= x1; px++) {
                    float fx = px + 0.5f, fy = py + 0.5f;
                    float a = ((Y[1] - Y[2]) * (fx - X[2]) + (X[2] - X[1]) * (fy - Y[2])) / den;
                    float b = ((Y[2] - Y[0]) * (fx - X[2]) + (X[0] - X[2]) * (fy - Y[2])) / den;
                    float c = 1.0f - a - b;
                    if (a < 0 || b < 0 || c < 0) continue;
                    float z = a * Z[0] + b * Z[1] + c * Z[2];
                    float& d = depth[(size_t)py * R + px];
                    if (z >= d) continue;
                    d = z;
                    Vec3 q = P[0] * a + P[1] * b + P[2] * c;
                    // World-projected, as the ground is: the facing axis picks the plane.
                    float u, v;
                    if (fabsf(n.y) > 0.55f) { u = q.x; v = q.z; }
                    else if (fabsf(n.x) > fabsf(n.z)) { u = q.z; v = -q.y; }
                    else { u = q.x; v = -q.y; }
                    int tx = (((int)floorf(u * N)) % N + N) % N, ty = (((int)floorf(v * N)) % N + N) % N;
                    const uint8_t* t = tex + ((size_t)ty * N + tx) * 4;
                    float* o = &rgba[((size_t)py * R + px) * 4];
                    o[0] = t[0] * shade; o[1] = t[1] * shade; o[2] = t[2] * shade; o[3] = 255.0f;
                }
        }
        // 2x2 box down into the icon cell (soft silhouette edges).
        for (int y = 0; y < N; y++)
            for (int x = 0; x < N; x++) {
                float acc[4] = { 0, 0, 0, 0 };
                for (int sy2 = 0; sy2 < SS; sy2++)
                    for (int sx2 = 0; sx2 < SS; sx2++) {
                        const float* s = &rgba[((size_t)(y * SS + sy2) * R + (x * SS + sx2)) * 4];
                        for (int c = 0; c < 3; c++) acc[c] += s[c] * (s[3] / 255.0f);
                        acc[3] += s[3];
                    }
                uint8_t* o = set.icons.data() + ((size_t)y * set.iconsW + (size_t)id * N + x) * 4;
                float alpha = acc[3] / (SS * SS);
                float cov = alpha / 255.0f;
                for (int c = 0; c < 3; c++) o[c] = (uint8_t)(cov > 0 ? std::min(255.0f, acc[c] / (SS * SS) / cov) : 0.0f);
                o[3] = (uint8_t)std::min(255.0f, alpha);
            }
    }
}
