// tests/tests.cpp
//
// Native regression tests for the engine's platform-free modules: the
// .vtex parser, block texture assembly, the save format (v5 round trip
// and legacy v4), column streaming/eviction, player spawn/unstick, and
// the chunk mesher. Built and run by tests/run.sh with the host
// compiler; D3D and Windows are replaced by tests/stub/.

#include "../world.h"
#include "../worldfile.h"
#include "../vtex.h"
#include "../blocktex.h"
#include "../glowlight.h"
#include "../musiclevel.h"
#include "../library.h"
#include "../icons.h"
#include "../sky.h"
#include "../music_synth.h"
#include "../sfx_synth.h"
#include "../soundscape.h"
#include "../facetmesh.h"
#include "../terrain.h"
#include "../jobs.h"
#include "../groundmesh.h"
#include "../collide.h"
#include "../strtable.h"
static const uint64_t HILLS_V1_FINGERPRINT = 0xb337671eeedafb98ull; // walkgrid-hills v1, seed 1, column (0, 0): re-pinned in M1.9 when the registry was renumbered (same materials, same places; saves store names)
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <map>
#include <chrono>
#include <unordered_map>
#include <sstream>
#include <fstream>

static int g_failures = 0, g_checks = 0;
// How far faceted flat ground sits from its cells' tops: the corners'
// seeded jitter (facetmesh.h FacetShape: 0.16) and a little.
static const float FACET_Y = 0.2f;
// Texture layers the registry names (M1.9): foundation plus the materials' tops and sides.
static const int LAYERS_M19 = 14; // foundation + 13 named: 12 tops, the sandstone strata, soils shared
#define CHECK(cond) do { g_checks++; if (!(cond)) { g_failures++; printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); } } while (0)

static void ResetWorldState(World& w) {
    w = World();
    ResetColumnStreaming();
    ClearScheduledUpdates();
}

static void Stream(World& w, float px, float pz, int ticks) {
    for (int t = 0; t < ticks; t++) {
        int cx = FloorDiv16((int)floorf(px)), cz = FloorDiv16((int)floorf(pz));
        EnsureChunksLoaded(cx, cz);
        ProcessColumnGeneration(w);
        ProcessColumnEviction(w);
    }
}

static bool ChunksEqual(const Chunk& a, const Chunk& b) {
    if (memcmp(a.blocks, b.blocks, sizeof(a.blocks)) || memcmp(a.state, b.state, sizeof(a.state))) return false;
    size_t na = a.data ? a.data->size() : 0, nb = b.data ? b.data->size() : 0;
    if (na != nb) return false;
    if (na) for (auto& kv : *a.data) { auto it = b.data->find(kv.first); if (it == b.data->end() || it->second != kv.second) return false; }
    return true;
}

// ---------------------------------------------------------------------

static void TestVtex() {
    printf("vtex parser\n");
    const char* good =
        "# comment\n"
        "texture stone   # trailing comment\n"
        "size 8\n"
        "palette\n"
        "  a 7c7c82\n"
        "  b #6a6a70   # a leading # on the hex is tolerated\n"
        "  # 000000\n"            // a comment line, not a key
        "pixels\n"
        "  aaaabbbb\n  abababab\n  aaaaaaaa\n  bbbbbbbb\n  aaaabbbb\n  abababab\n  aaaaaaaa\n  bbbbbbbb\n"
        "end\n"
        "block chest\n  all stone\n  front stone\nend\n";
    VtexSet s;
    ParseVtex(good, "good.vtex", s);
    CHECK(s.errors.empty());
    CHECK(s.textures.size() == 1);
    if (!s.textures.empty()) {
        CHECK(s.textures[0].size == 8);
        CHECK(s.textures[0].rgb[0] == 0x7c7c82);
        CHECK(s.textures[0].rgb[4] == 0x6a6a70);
    }
    CHECK(s.blocks.size() == 1 && s.blocks[0].front == "stone");
    for (auto& e : s.errors) printf("    %s\n", e.c_str());

    // Alpha (see-through blocks): "rrggbbaa", stored as 255 - alpha in the
    // top byte so plain 6-digit colours are unchanged; a 7-digit colour is
    // an error.
    VtexSet al;
    ParseVtex("texture pane\nsize 8\npalette\n g 80c0ff40\n f #a0b0c0ff # opaque\npixels\n"
              " gggggggg\n gggggggg\n gggggggg\n gggggggg\n ffffffff\n ffffffff\n ffffffff\n ffffffff\nend\n"
              "block sand\n all pane\nend\n", "alpha.vtex", al);
    CHECK(al.errors.empty() && al.textures.size() == 1);
    if (!al.textures.empty()) CHECK(al.textures[0].rgb[0] == 0xBF80C0FFu && al.textures[0].rgb[63] == 0x00A0B0C0u);
    BlockTextureSet at; BuildBlockTextures(al, at);
    const uint8_t* ap = at.mips[0].data() + (size_t)at.faceLayer[BLOCK_SAND][0][0] * BLOCK_TEX_SIZE * BLOCK_TEX_SIZE * 4;
    CHECK(ap[0] == 0xFF && ap[1] == 0xC0 && ap[2] == 0x80 && ap[3] == 0x40);            // BGRA, alpha kept
    CHECK(ap[((size_t)(BLOCK_TEX_SIZE - 1) * BLOCK_TEX_SIZE) * 4 + 3] == 255);           // bottom rows opaque
    VtexSet al7; ParseVtex("texture q\nsize 8\npalette\n g 80c0ff4\npixels\nend\n", "seven.vtex", al7);
    CHECK(!al7.errors.empty());

    // Surface maps: height (0-9a-z), shine and glow (0-9) after the pixels.
    {
        std::string rows8 = ""; for (int i = 0; i < 8; i++) rows8 += " aaaaaaaa\n";
        std::string hrows = ""; for (int i = 0; i < 8; i++) hrows += " 0123456z\n";
        std::string srows = ""; for (int i = 0; i < 8; i++) srows += " 00000009\n";
        VtexSet m;
        ParseVtex("texture ramp\nsize 8\npalette\n a 808080\npixels\n" + rows8 + "height\n" + hrows + "shine\n" + srows + "glow\n" + srows + "end\n", "maps.vtex", m);
        CHECK(m.errors.empty() && m.textures.size() == 1);
        if (!m.textures.empty()) {
            const VtexTexture& t = m.textures[0];
            CHECK(t.height.size() == 64 && fabsf(t.height[0]) < 1e-6f && fabsf(t.height[7] - 1.0f) < 1e-6f && fabsf(t.height[3] - 3.0f / 35.0f) < 1e-6f);
            CHECK(t.shine.size() == 64 && t.shine[7] == 1.0f && t.shine[0] == 0.0f && t.glow[7] == 1.0f);
        }
        VtexSet e;
        std::string badH = ""; for (int i = 0; i < 8; i++) badH += " 0000000!\n";
        ParseVtex("texture b1\nsize 8\npalette\n a 808080\npixels\n" + rows8 + "height\n" + badH + "end\n", "badh.vtex", e);           // bad digit
        ParseVtex("texture b2\nsize 8\npalette\n a 808080\npixels\n" + rows8 + "glow\n 0000000\nend\n", "short.vtex", e);            // short row
        ParseVtex("texture b3\nsize 8\npalette\n a 808080\npixels\n" + rows8 + "shine\n" + srows + "shine\n" + srows + "end\n", "twice.vtex", e); // twice
        ParseVtex("texture b4\nsize 8\npalette\n a 808080\npixels\n" + rows8 + "glow\n 00000000\nend\n", "few.vtex", e);            // too few rows
        CHECK(e.textures.empty() && e.errors.size() == 4);
        for (auto& x : e.errors) printf("    (expected) %s\n", x.c_str());
    }

    VtexSet bad;
    ParseVtex("texture a\nsize 8\npalette\n x 000000\npixels\n xxxxxxx\nend\n", "short.vtex", bad);  // 7-char row
    ParseVtex("texture b\nsize 8\npalette\n x 000000\npixels\n xxxxxxxy\nend\n", "key.vtex", bad);   // unknown key
    ParseVtex("texture c\nsize 8\npalette\n x 000000\npixels\n xxxxxxxx\nend\n", "rows.vtex", bad);  // 1 of 8 rows
    ParseVtex("texture d\nsize 12\nend\n", "size.vtex", bad);                                        // bad size
    ParseVtex("block chest\n  lid stone\nend\n", "face.vtex", bad);                                  // unknown face
    ParseVtex("texture e\nsize 8\npalette\n", "noend.vtex", bad);                                     // no end
    CHECK(bad.textures.empty());
    CHECK(bad.errors.size() == 6);
    for (auto& e : bad.errors) printf("    (expected) %s\n", e.c_str());
}

static void TestBlockTextures() {
    printf("block textures\n");
    VtexSet none;
    BlockTextureSet t;
    BuildBlockTextures(none, t);
    CHECK(t.warnings.empty());
    printf("    %d texture layers\n", t.layerCount);
    CHECK(t.layerCount == LAYERS_M19);

    // The natural materials' art in the repo loads cleanly and covers
    // every natural block (no magenta fallback), with seamless wrap.
    {
        FILE* fp = fopen("../assets/textures/natural.vtex", "rb");
        CHECK(fp != nullptr);
        if (fp) {
            std::string text; char buf[4096]; size_t got;
            while ((got = fread(buf, 1, sizeof buf, fp)) > 0) text.append(buf, got);
            fclose(fp);
            VtexSet nat; ParseVtex(text, "natural.vtex", nat);
            CHECK(nat.errors.empty() && nat.textures.size() == 9 && nat.blocks.empty()); // M1.9: the picks only; the registry names their faces
            for (auto& tx : nat.textures) CHECK(tx.size == 32 && !tx.height.empty()); // one density for everything (32), all with relief
            // The parked pulse set (not loaded by the game) still parses cleanly.
            FILE* fi = fopen("../assets/textures/parked/industry.vtex", "rb");
            CHECK(fi != nullptr);
            if (fi) {
                std::string itext;
                while ((got = fread(buf, 1, sizeof buf, fi)) > 0) itext.append(buf, got);
                fclose(fi);
                VtexSet parked; ParseVtex(itext, "industry.vtex", parked);
                CHECK(parked.errors.empty() && parked.textures.size() == 10);
            }
            // ...and the September trial batch (batch_sept.vtex): textures named after their blocks.
            FILE* fb = fopen("../assets/textures/batch_sept.vtex", "rb");
            CHECK(fb != nullptr);
            if (fb) {
                std::string btext;
                while ((got = fread(buf, 1, sizeof buf, fb)) > 0) btext.append(buf, got);
                fclose(fb);
                size_t before = nat.textures.size();
                ParseVtex(btext, "batch_sept.vtex", nat);
                CHECK(nat.errors.empty() && nat.textures.size() == before + 4);
            }
            BlockTextureSet nt; BuildBlockTextures(nat, nt);
            CHECK(nt.warnings.empty());
            for (auto& w : nt.warnings) printf("    %s\n", w.c_str());
            // Every material wears authored art (M1.9: the loaded .vtex files
            // hold exactly the materials' textures; the rest are parked).
            for (int id = BLOCK_MEADOW_GRASS; id < BLOCK_COUNT; id++)
                for (int f = 0; f < FACE_COUNT; f++) {
                    const std::string& name = nt.layerNames[nt.faceLayer[id][FACE_POS_Z][f]];
                    bool art = false; for (auto& tx : nat.textures) if (tx.name == name) art = true;
                    CHECK(art);
                    if (!art) printf("    %s: %s\n", g_blocks[id].name, name.c_str());
                }
            // Every natural texture except the log's cut end tiles: each row's
            // and column's wrap step is no bigger than the steps inside it.
            for (auto& tx : nat.textures) {
                if (tx.name == "log_top") continue;
                auto d = [&](uint32_t a, uint32_t b) { return abs((int)(a >> 16 & 255) - (int)(b >> 16 & 255)) + abs((int)(a >> 8 & 255) - (int)(b >> 8 & 255)) + abs((int)(a & 255) - (int)(b & 255)); };
                const int N = tx.size;
                double maxC = 0, maxR = 0;
                for (int b = 1; b < N; b++) {
                    double c = 0, r = 0;
                    for (int i = 0; i < N; i++) { c += d(tx.rgb[i * N + b - 1], tx.rgb[i * N + b]); r += d(tx.rgb[(b - 1) * N + i], tx.rgb[b * N + i]); }
                    maxC = std::max(maxC, c / N); maxR = std::max(maxR, r / N);
                }
                double wc = 0, wr = 0;
                for (int i = 0; i < N; i++) { wc += d(tx.rgb[i * N + N - 1], tx.rgb[i * N]); wr += d(tx.rgb[(N - 1) * N + i], tx.rgb[i]); }
                wc /= N; wr /= N;
                bool ok = wc <= maxC * 1.05 + 1 && wr <= maxR * 1.05 + 1;
                if (!ok) printf("    seam in %s: across %.1f (inner max %.1f), down %.1f (inner max %.1f)\n", tx.name.c_str(), wc, maxC, wr, maxR);
                CHECK(ok);
            }
        }
    }
    CHECK(t.mipCount == 7);
    CHECK(t.faceLayer[BLOCK_MEADOW_GRASS][FACE_POS_Z][FACE_POS_Y] != t.faceLayer[BLOCK_MEADOW_GRASS][FACE_POS_Z][FACE_POS_X]); // grass on top, soil on the side
    CHECK(t.faceLayer[BLOCK_MEADOW_GRASS][FACE_POS_Z][FACE_POS_X] == t.faceLayer[BLOCK_DIRT][FACE_POS_Z][FACE_POS_X]);
    CHECK(t.faceLayer[BLOCK_STONE][FACE_POS_Z][FACE_POS_Y] == t.faceLayer[BLOCK_STONE][FACE_NEG_X][FACE_NEG_Z]);
    CHECK(t.mips.back().size() == (size_t)t.layerCount * 4); // 1x1 per layer

    // Authored art overrides a block's whole mapping and is upscaled.
    VtexSet art;
    ParseVtex("texture my_stone\nsize 8\npalette\n r ff0000\n g 00ff00\npixels\n"
              " rrrrrrrr\n gggggggg\n rrrrrrrr\n gggggggg\n rrrrrrrr\n gggggggg\n rrrrrrrr\n gggggggg\nend\n"
              "block stone\n all my_stone\nend\n"
              "block dirt\n all nope\nend\n"
              "block unobtainium\n all my_stone\nend\n", "art.vtex", art);
    CHECK(art.errors.empty());
    BlockTextureSet a;
    BuildBlockTextures(art, a);
    uint16_t L = a.faceLayer[BLOCK_STONE][FACE_POS_Z][FACE_POS_Y];
    CHECK(a.layerNames[L] == "my_stone");
    const uint8_t* px = a.mips[0].data() + (size_t)L * BLOCK_TEX_SIZE * BLOCK_TEX_SIZE * 4;
    CHECK(px[2] == 255 && px[1] == 0);                                  // row 0: red (BGRA)
    CHECK(px[(size_t)7 * BLOCK_TEX_SIZE * 4 + 1] == 0);                 // row 7 still red (8x upscale)
    CHECK(px[(size_t)8 * BLOCK_TEX_SIZE * 4 + 1] == 255);               // row 8: green
    CHECK(a.warnings.size() == 2); // dirt -> missing texture; unknown block
    for (auto& w : a.warnings) printf("    (expected) %s\n", w.c_str());

    // The natural placeholders tile seamlessly: the step across the wrap
    // edge (last column to first, last row to first) is no bigger than the
    // biggest step already inside the tile between logical pixels. And
    // dirt is brown: never greener than it is red.
    for (BlockID id : { BLOCK_STONE, BLOCK_DIRT }) {
        const uint8_t* L = t.mips[0].data() + (size_t)t.faceLayer[id][FACE_POS_Z][FACE_POS_Z] * BLOCK_TEX_SIZE * BLOCK_TEX_SIZE * 4;
        const int S = BLOCK_TEX_SIZE, k = S / 16;
        auto px = [&](int x, int y) { return L + ((size_t)y * S + x) * 4; };
        auto diff = [&](const uint8_t* a, const uint8_t* b) { return abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2]); };
        auto colStep = [&](int x0, int x1) { double d = 0; for (int y = 0; y < S; y++) d += diff(px(x0, y), px(x1, y)); return d / S; };
        auto rowStep = [&](int y0, int y1) { double d = 0; for (int x = 0; x < S; x++) d += diff(px(x, y0), px(x, y1)); return d / S; };
        double maxCol = 0, maxRow = 0;
        for (int b = k; b < S; b += k) { maxCol = std::max(maxCol, colStep(b - 1, b)); maxRow = std::max(maxRow, rowStep(b - 1, b)); }
        double wrapX = colStep(S - 1, 0), wrapY = rowStep(S - 1, 0);
        printf("    %s: biggest inner step %.1f / %.1f, across the wrap %.1f / %.1f\n", g_blocks[id].name, maxCol, maxRow, wrapX, wrapY);
        CHECK(wrapX <= maxCol * 1.05 + 1 && wrapY <= maxRow * 1.05 + 1);
        if (id == BLOCK_DIRT) { bool green = false; for (int i = 0; i < S * S; i++) if (L[i * 4 + 1] > L[i * 4 + 2]) green = true; CHECK(!green); }
    }

    // Surface layers: flat and matte without maps; a height ramp rising
    // along u tilts the normal back toward -u; shine and glow carry over;
    // the smallest mip of a flat surface stays flat.
    {
        const size_t S = BLOCK_TEX_SIZE;
        CHECK(t.surface.size() == (size_t)t.mipCount && t.surface[0].size() == t.mips[0].size());
        const uint8_t* flat = t.surface[0].data() + (size_t)t.faceLayer[BLOCK_STONE][FACE_POS_Z][FACE_POS_Y] * S * S * 4;
        CHECK(flat[0] == 128 && flat[1] == 128 && flat[2] == 0 && flat[3] == 0);
        std::string rows = "", h = "", g = "";
        for (int i = 0; i < 16; i++) { rows += " aaaaaaaaaaaaaaaa\n"; h += " 0011223344556677\n"; g += " 0000000000000009\n"; }
        VtexSet ramp;
        ParseVtex("texture rampy\nsize 16\npalette\n a 808080\npixels\n" + rows + "height\n" + h + "glow\n" + g + "end\nblock stone\n all rampy\nend\n", "rampy.vtex", ramp);
        CHECK(ramp.errors.empty());
        BlockTextureSet rt; BuildBlockTextures(ramp, rt);
        const uint8_t* L = rt.surface[0].data() + (size_t)rt.faceLayer[BLOCK_STONE][FACE_POS_Z][FACE_POS_Y] * S * S * 4;
        const uint8_t* mid = L + (20 * S + 8) * 4;   // on a step between two height levels
        CHECK(mid[0] < 126 && mid[1] >= 127 && mid[1] <= 129); // leans toward -u, not along v
        CHECK(L[(20 * S + 63) * 4 + 3] == 255 && L[(20 * S + 10) * 4 + 3] == 0);  // glow only in the last column
        const uint8_t* top = rt.surface.back().data() + (size_t)rt.faceLayer[BLOCK_DIRT][FACE_POS_Z][FACE_POS_Y] * 4;
        CHECK(top[0] == 128 && top[1] == 128);
    }

    // Mips average in linear light: a black/white checker fades to the
    // sRGB code of 50% linear (188), not to gamma-space 128; flat colour
    // survives every level unchanged.
    VtexSet checker;
    ParseVtex("texture chk\nsize 8\npalette\n k 000000\n w ffffff\n r ff0000\npixels\n"
              " kwkwkwkw\n wkwkwkwk\n kwkwkwkw\n wkwkwkwk\n kwkwkwkw\n wkwkwkwk\n kwkwkwkw\n wkwkwkwk\nend\n"
              "texture flat\nsize 8\npalette\n r 804020\npixels\n"
              " rrrrrrrr\n rrrrrrrr\n rrrrrrrr\n rrrrrrrr\n rrrrrrrr\n rrrrrrrr\n rrrrrrrr\n rrrrrrrr\nend\n"
              "block stone\n all chk\nend\nblock dirt\n all flat\nend\n", "chk.vtex", checker);
    CHECK(checker.errors.empty());
    BlockTextureSet c;
    BuildBlockTextures(checker, c);
    uint16_t LC = c.faceLayer[BLOCK_STONE][FACE_POS_Z][FACE_POS_Y], LF = c.faceLayer[BLOCK_DIRT][FACE_POS_Z][FACE_POS_Y];
    int m = 4, sz = BLOCK_TEX_SIZE >> m; // one texel = 2x2 source pixels
    const uint8_t* cm = c.mips[m].data() + (size_t)LC * sz * sz * 4;
    CHECK(cm[0] == 188 && cm[1] == 188 && cm[2] == 188 && cm[3] == 255);
    const uint8_t* fm = c.mips.back().data() + (size_t)LF * 4;
    CHECK(fm[0] == 0x20 && fm[1] == 0x40 && fm[2] == 0x80 && fm[3] == 255);
}

// The save checksum (worldfile.cpp keeps its own private), for hand-built files.
static uint32_t Fnv1a(const uint8_t* data, size_t len) {
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < len; i++) { h ^= data[i]; h *= 16777619u; }
    return h;
}

static void TestSaveRoundTrip() {
    printf("save format (walkgrid v1) round trip\n");
    World w; ResetWorldState(w);
    g_loadRadius = 2;
    g_worldGen.type = GEN_FLAT; g_worldGen.version = 1; g_worldGen.seed = 0x1234567890ABCDEFull;
    Stream(w, 8, 8, 20);
    size_t generated = w.chunks.size();
    CHECK(generated > 0);
    for (auto& kv : w.chunks) CHECK(!kv.second->modified);

    // Edits with state and data in two chunks.
    w.Set(3, 12, 3, BLOCK_AIR);
    w.Set(5, 13, 5, BLOCK_CLAY, 5); // the (reserved) state byte round-trips too
    Chunk* c = w.FindChunk({ 0, 0, 0 });
    c->data = std::make_unique<std::unordered_map<uint16_t, std::vector<uint8_t>>>();
    (*c->data)[(uint16_t)Chunk::LocalIndex(5, 13, 5)] = { 1, 2, 3, 250 };
    w.Set(20, 40, 20, BLOCK_SANDSTONE); // a chunk the generator never made (cy = 2)

    Player p; p.x = 1.5f; p.y = 13; p.z = 2.5f; p.yaw = 0.7f; p.hotbarIndex = 3;
    std::vector<uint8_t> buf;
    std::vector<PendingUpdate> pend = { { 7, 20, 7, UPD_GRAVITY, 3 }, { -1, 5, 9, UPD_GRAVITY, 0 } };
    std::vector<uint8_t> game = { 9, 0, 255, 42 }; // the game layer's section: opaque bytes
    EncodeSave(p, 1234.5f, g_worldGen, w, g_evictedChunks, pend, game, buf);
    printf("    %zu generated chunks, 2 modified -> %zu bytes\n", generated, buf.size());
    CHECK(buf.size() < 3000);

    SaveData d;
    CHECK(DecodeSave(buf.data(), buf.size(), d) == DecodeResult::Ok);
    CHECK(d.version == SAVE_VERSION);
    CHECK(d.player.x == p.x && d.player.yaw == p.yaw && d.player.hotbarIndex == 3);
    CHECK(d.dayTime == 1234.5f);
    CHECK(d.gen.type == GEN_FLAT && d.gen.seed == g_worldGen.seed);
    CHECK(d.chunks.size() == 2);
    CHECK(d.updates.size() == 2 && d.updates[0].x == 7 && d.updates[0].delay == 3 && d.updates[1].y == 5);
    CHECK(d.game == game);
    for (auto& kv : d.chunks) {
        Chunk* orig = w.FindChunk(kv.first);
        CHECK(orig && orig->modified && ChunksEqual(*orig, *kv.second));
    }

    // Corruption is caught, not loaded.
    std::vector<uint8_t> broken = buf; broken[buf.size() / 2] ^= 0x40;
    SaveData d2; CHECK(DecodeSave(broken.data(), broken.size(), d2) == DecodeResult::BadChecksum);
    SaveData d3; CHECK(DecodeSave(buf.data(), 20, d3) != DecodeResult::Ok);

    // Extra bytes the sections don't account for mean damage: refused.
    {
        std::vector<uint8_t> longer(buf.begin(), buf.end() - 4);
        longer.push_back(0);
        uint32_t sum = Fnv1a(longer.data(), longer.size());
        for (int i = 0; i < 4; i++) longer.push_back((uint8_t)(sum >> (8 * i)));
        SaveData d4; CHECK(DecodeSave(longer.data(), longer.size(), d4) == DecodeResult::Corrupt);
    }
    // A Voxistics save (magic "VXLG") is refused cleanly, never misread.
    {
        std::vector<uint8_t> vox;
        auto u32 = [&](uint32_t v) { for (int i = 0; i < 4; i++) vox.push_back((uint8_t)(v >> (8 * i))); };
        u32(('G' << 24) | ('L' << 16) | ('X' << 8) | 'V'); u32(9); u32(0);
        u32(Fnv1a(vox.data(), vox.size()));
        SaveData d5; CHECK(DecodeSave(vox.data(), vox.size(), d5) == DecodeResult::BadMagic);
    }
}

static void TestStreaming() {
    printf("streaming, eviction, regeneration\n");
    World w; ResetWorldState(w);
    g_loadRadius = 2;
    g_worldGen = WorldGenParams(); g_worldGen.type = GEN_WALKGRID;
    Stream(w, 8, 8, 40);
    CHECK(g_residentColumns.size() == 49); // (2*(2+1)+1)^2: one ring past the view radius
    CHECK(g_residentColumns.count(ColumnKey(0, 0)));

    // Snapshot one unmodified and one modified chunk, then walk away.
    int h = TerrainHeight(40, 8);
    w.Set(40, h, 8, BLOCK_CLAY, 3);
    ChunkCoord editedCC = World::ToChunk(40, h, 8);
    Chunk snapEdited = Chunk(); memcpy(snapEdited.blocks, w.FindChunk(editedCC)->blocks, CHUNK_CELLS); memcpy(snapEdited.state, w.FindChunk(editedCC)->state, CHUNK_CELLS);
    ChunkCoord plainCC = { 1, 1, 0 };
    Chunk snapPlain = Chunk(); memcpy(snapPlain.blocks, w.FindChunk(plainCC)->blocks, CHUNK_CELLS);

    Stream(w, 8 + 16 * 20, 8, 400);
    CHECK(!g_residentColumns.count(ColumnKey(1, 0)) && !g_residentColumns.count(ColumnKey(2, 0)));
    CHECK(g_evictedChunks.size() == 1); // only the modified chunk is kept
    CHECK(g_evictedChunks.count(editedCC));
    for (auto& kv : w.chunks) CHECK(ColumnDistance(kv.first.x, kv.first.z, g_lastPlayerChunkX, g_lastPlayerChunkZ) <= g_loadRadius + 1 + CHUNK_EVICT_MARGIN);

    Stream(w, 8, 8, 400); // come back
    Chunk* ce = w.FindChunk(editedCC); Chunk* cp = w.FindChunk(plainCC);
    CHECK(ce && ce->modified && ChunksEqual(*ce, snapEdited));
    CHECK(cp && !cp->modified && memcmp(cp->blocks, snapPlain.blocks, CHUNK_CELLS) == 0);
    CHECK(g_evictedChunks.empty());
}

static void TestMovement() {
    printf("sprint, crouch and power slide\n");
    World w; ResetWorldState(w);
    g_loadRadius = 2; g_worldGen = WorldGenParams(); g_worldGen.type = GEN_FLAT;
    Stream(w, 8, 8, 200);
    const int G = TerrainHeight(8, 8) + 1;   // feet level on the flat ground
    for (int x = 10; x <= 20; x++) for (int z = 5; z <= 11; z++) w.Set(x, G + 1, z, BLOCK_STONE); // a roof 1 block up: a crawlspace
    const float dt = 1.0f / 60.0f, EAST = 1.5707963f; // yaw pi/2: facing +X
    auto run = [&](Player& p, MoveInput in, float seconds) { for (int i = 0; i < (int)(seconds * 60); i++) UpdatePlayerPhysics(w, p, dt, in); };
    auto at = [&](float x, float z) { Player p; p.x = x; p.y = (float)G; p.z = z; p.yaw = EAST; run(p, MoveInput(), 0.2f); return p; };
    MoveInput walk; walk.fwd = true;
    MoveInput crawl = walk; crawl.crouch = true;
    MoveInput sprint = walk; sprint.sprint = true;

    // Standing, the crawlspace stops you at its mouth; crouched, you get in.
    Player p = at(6.5f, 8.5f);
    run(p, walk, 2.0f);
    CHECK(p.x > 9.5f && p.x < 9.8f && !p.crouching);
    run(p, crawl, 3.0f);
    CHECK(p.x > 12.0f && p.crouching && fabsf(p.y - G) < FACET_Y);
    // Letting go of crouch under the roof: still crouched (no room to stand).
    run(p, MoveInput(), 0.3f);
    CHECK(p.crouching && fabsf(p.y - G) < FACET_Y && p.eyeHeight < 0.8f);
    // Back out into the open, and you stand up by yourself.
    MoveInput back; back.back = true;
    run(p, back, 5.0f);
    CHECK(p.x < 9.7f && !p.crouching && p.eyeHeight > 1.5f);

    // Sprinting beats walking; crouching is slow.
    Player a = at(0.5f, 2.5f), b = at(0.5f, 2.5f), c = at(0.5f, 2.5f);
    run(a, walk, 1.0f); run(b, sprint, 1.0f); run(c, crawl, 1.0f);
    float dw = a.x - 0.5f, ds = b.x - 0.5f, dc = c.x - 0.5f;
    printf("    1 s: walk %.2f, sprint %.2f, crouch %.2f blocks\n", dw, ds, dc);
    CHECK(ds > dw * 1.3f && dc < dw * 0.5f && b.sprinting && !a.sprinting);

    // Power slide: sprint, then crouch -- a burst faster than the sprint,
    // bleeding off, ending crouched while crouch is held.
    Player s = at(-20.5f, 2.5f);
    run(s, sprint, 0.5f);
    MoveInput slide = sprint; slide.crouch = true;
    float x0 = s.x;
    run(s, slide, 0.25f);
    float burst = (s.x - x0) / 0.25f;
    printf("    slide: %.2f blocks/s just after starting (sprint %.2f)\n", burst, 7.5f);
    CHECK(PlayerSliding(s) && burst > 7.0f && s.crouching);
    // Looking to the side mid-slide leans the view toward where it's carrying you.
    s.yaw = 0.0f; // now facing +Z; the slide carries toward +X, i.e. the view's right
    run(s, slide, 0.3f);
    CHECK(s.roll > 0.08f && s.eyeHeight < 0.7f);
    run(s, slide, 1.5f);
    CHECK(!PlayerSliding(s) && s.crouching && fabsf(s.roll) < 0.02f);

    // Forgiving timing: crouch first and sprint a moment later slides; a
    // crouch just after letting go of sprint slides; a crouch long after doesn't.
    {
        MoveInput crouchWalk = walk; crouchWalk.crouch = true;
        MoveInput both = sprint; both.crouch = true;
        Player q = at(-20.5f, 2.5f);
        run(q, walk, 0.3f);
        run(q, crouchWalk, 0.15f);  // crouch pressed first...
        run(q, both, 0.05f);        // ...then sprint
        CHECK(PlayerSliding(q));
        Player r2 = at(-20.5f, 2.5f);
        run(r2, sprint, 0.5f);
        run(r2, walk, 0.2f);        // let go of sprint
        run(r2, crouchWalk, 0.05f); // crouch 0.2 s later
        CHECK(PlayerSliding(r2));
        Player late = at(-20.5f, 2.5f);
        run(late, sprint, 0.5f);
        run(late, walk, 1.0f);
        run(late, crouchWalk, 0.05f);
        CHECK(!PlayerSliding(late));
    }
    // Walking up a one-cell step (a slope on faceted ground, M1.7): the body
    // rises a block, and the view never jumps -- it glides with the eye's
    // easing -- then settles at standing height.
    {
        World sw; ResetWorldState(sw); Stream(sw, 40, 8, 300);
        int gy = TerrainHeight(40, 8);
        for (int x = 42; x <= 46; x++) for (int z = 5; z <= 11; z++) sw.Set(x, gy + 1, z, BLOCK_STONE);
        Player q; q.x = 40.5f; q.z = 8.5f; q.y = (float)(gy + 1); q.yaw = 1.5708f;
        MoveInput go; go.fwd = true;
        for (int i = 0; i < 10; i++) UpdatePlayerPhysics(sw, q, 1.0f / 60.0f, MoveInput());
        float startY = q.y, prevEye = q.y + q.eyeHeight, worstJump = 0;
        for (int i = 0; i < 60; i++) {
            UpdatePlayerPhysics(sw, q, 1.0f / 60.0f, go);
            float eye = q.y + q.eyeHeight;
            worstJump = std::max(worstJump, eye - prevEye);
            prevEye = eye;
        }
        for (int i = 0; i < 30; i++) UpdatePlayerPhysics(sw, q, 1.0f / 60.0f, MoveInput());
        printf("    step: x %.2f, rose %.2f, worst eye jump %.3f, eye %.3f\n", q.x, q.y - startY, worstJump, q.eyeHeight);
        CHECK(q.x > 43.0f && q.y - startY > 0.8f && worstJump < 0.2f && fabsf(q.eyeHeight - PLAYER_EYE) < 0.02f);
    }
    // And a slide carries you straight under the roof.
    Player u = at(0.5f, 8.5f);
    run(u, sprint, 0.9f);           // up to speed, well short of the mouth at x = 9.7
    CHECK(u.x < 9.0f);
    MoveInput dive = sprint; dive.crouch = true;
    run(u, dive, 0.05f);
    MoveInput none;
    run(u, none, 1.5f);             // let go of everything: momentum does the rest
    printf("    slid under the roof to x = %.2f\n", u.x);
    CHECK(u.x > 10.5f && u.crouching && fabsf(u.y - G) < FACET_Y);
}

static void TestPlayer() {
    printf("player spawn and unstick\n");
    World w; ResetWorldState(w);
    g_loadRadius = 1; g_worldGen = WorldGenParams(); g_worldGen.type = GEN_FLAT;
    Player p; p.y = (float)(TerrainHeight(8, 8) + 1);
    float minY = p.y;
    for (int t = 0; t < 120; t++) {
        Stream(w, p.x, p.z, 1);
        UpdatePlayerPhysics(w, p, 1.0f / 60.0f, false, false, false, false, false);
        minY = std::min(minY, p.y);
    }
    CHECK(p.onGround && fabsf(p.y - 13.0f) < FACET_Y && minY >= 13.0f - FACET_Y);

    // Below the world: put back on top of the column.
    Player v = p; v.y = -100.0f;
    UpdatePlayerPhysics(w, v, 1.0f / 60.0f, false, false, false, false, false);
    CHECK(fabsf(v.y - 13.0f) < FACET_Y);

    p.y = 8.0f; // buried
    int ticks = 0;
    while (ticks < 60) { UpdatePlayerPhysics(w, p, 1.0f / 60.0f, false, false, false, false, false); ticks++; if (p.onGround) break; }
    CHECK(p.onGround && fabsf(p.y - 13.0f) < FACET_Y);
}



static void TestScheduledUpdates() {
    printf("scheduled updates (gravity)\n");
    World w; ResetWorldState(w);
    g_loadRadius = 1; g_worldGen = WorldGenParams(); g_worldGen.type = GEN_FLAT;
    Stream(w, 8, 8, 20);
    // Live edits schedule nothing in walkgrid (falling ground parked, D12)...
    for (int y = 14; y < 19; y++) w.Set(8, y, 8, BLOCK_STONE);
    w.Set(8, 13, 8, BLOCK_DIRT);
    LiveEdit(w, 8, 13, 8, BLOCK_AIR);
    CHECK(ScheduledUpdateCount() == 0);
    // ...but the engine still runs gravity when asked: a 5-high column falls.
    MaybeQueueFall(w, 8, 14, 8);
    CHECK(ScheduledUpdateCount() == 1);
    int ticks = 0;
    while (ScheduledUpdateCount() > 0 && ticks < 200) { ProcessScheduledUpdates(w); ticks++; }
    CHECK(ScheduledUpdateCount() == 0);
    for (int y = 13; y < 18; y++) CHECK(w.Get(8, y, 8) == BLOCK_STONE);
    CHECK(w.Get(8, 18, 8) == BLOCK_AIR);
    printf("    5-block column settled in %d ticks\n", ticks);

    // The per-tick cap spreads a big collapse over several ticks.
    ClearScheduledUpdates();
    for (int x = 0; x < 16; x++) for (int z = 0; z < 16; z++) { w.Set(x, 20, z, BLOCK_DIRT); ScheduleUpdate(x, 20, z, UPD_GRAVITY, 0); }
    ProcessScheduledUpdates(w);
    CHECK(ScheduledUpdateCount() == 256); // 64 fell one cell and re-queued themselves; 192 still waiting their turn
    int fell = 0; for (int x = 0; x < 16; x++) for (int z = 0; z < 16; z++) if (w.Get(x, 19, z) == BLOCK_DIRT) fell++;
    CHECK(fell == MAX_UPDATES_PER_TICK);

    // Snapshot/restore keeps remaining delays.
    ClearScheduledUpdates();
    ScheduleUpdate(1, 2, 3, UPD_GRAVITY, 5);
    ProcessScheduledUpdates(w); ProcessScheduledUpdates(w);
    std::vector<PendingUpdate> snap = SnapshotScheduledUpdates();
    CHECK(snap.size() == 1 && snap[0].delay == 3);
    ClearScheduledUpdates();
    RestoreScheduledUpdates(snap);
    CHECK(ScheduledUpdateCount() == 1);
}

static void TestIcons() {
    printf("icons\n");
    VtexSet none; BlockTextureSet t; BuildBlockTextures(none, t);
    RenderBlockIcons(t);
    // Each material's icon is a lump: transparent corners, a solid body.
    for (int id = 1; id < BLOCK_COUNT; id++) {
        auto alpha = [&](int x, int y) { return t.icons[((size_t)y * t.iconsW + (size_t)id * BLOCK_TEX_SIZE + x) * 4 + 3]; };
        CHECK(alpha(0, 0) == 0 && alpha(BLOCK_TEX_SIZE - 1, 0) == 0);
        int opaque = 0;
        for (int y = 0; y < BLOCK_TEX_SIZE; y++) for (int x = 0; x < BLOCK_TEX_SIZE; x++) if (alpha(x, y) == 255) opaque++;
        CHECK(opaque > 400);
    }
}

static Vec3 XformPoint(const Mat4& m, Vec3 p) {
    float x = p.x * m.m[0][0] + p.y * m.m[1][0] + p.z * m.m[2][0] + m.m[3][0];
    float y = p.x * m.m[0][1] + p.y * m.m[1][1] + p.z * m.m[2][1] + m.m[3][1];
    float z = p.x * m.m[0][2] + p.y * m.m[1][2] + p.z * m.m[2][2] + m.m[3][2];
    float w = p.x * m.m[0][3] + p.y * m.m[1][3] + p.z * m.m[2][3] + m.m[3][3];
    return { x / w, y / w, z / w };
}

static void TestLibrary() {
    printf("block library and hotbar\n");
    const int W = 960, H = 680, count = g_placeableList.count;   // the smallest window
    // Hotbar: ten slots, on screen, left to right, no overlap.
    for (int i = 0; i < HOTBAR_SLOTS; i++) {
        UiRect r = HotbarSlotRect(W, H, i);
        CHECK(r.x0 >= 0 && r.x1 <= W && r.y1 <= H);
        if (i) CHECK(r.x0 >= HotbarSlotRect(W, H, i - 1).x1);
        CHECK(HotbarSlotAt(W, H, (r.x0 + r.x1) / 2, (r.y0 + r.y1) / 2) == i);
    }
    CHECK(HotbarSlotAt(W, H, 5, 5) == -1);
    // Library: every placeable block reachable, the grid above the hotbar.
    LibraryLayout L = ComputeLibraryLayout(W, H, count);
    CHECK(L.grid.y1 <= HotbarSlotRect(W, H, 0).y0 && L.panel.y0 >= 0 && L.panel.x0 >= 0 && L.panel.x1 <= W);
    int maxScroll = L.rows - L.visibleRows;
    for (int i = 0; i < count; i++) {
        int scroll = std::min(maxScroll, i / L.columns);
        UiRect c = LibraryCellRect(L, scroll, i);
        CHECK(LibraryCellAt(L, count, scroll, (c.x0 + c.x1) / 2, (c.y0 + c.y1) / 2) == i);
    }
    // A click selects; a small wobble is still a click.
    LibraryGesture g;
    LibraryPress(g, 3, 100, 100); LibraryMove(g, 103, 98);
    LibraryResult r = LibraryRelease(g, -1);
    CHECK(r.outcome == LibraryOutcome::Select && r.entry == 3);
    // A drag onto a slot assigns it there.
    LibraryPress(g, 5, 100, 100); LibraryMove(g, 180, 400);
    r = LibraryRelease(g, 7);
    CHECK(r.outcome == LibraryOutcome::Assign && r.entry == 5 && r.slot == 7);
    // A drag dropped anywhere else does nothing; a press on empty space does nothing.
    LibraryPress(g, 5, 100, 100); LibraryMove(g, 300, 100);
    CHECK(LibraryRelease(g, -1).outcome == LibraryOutcome::None);
    LibraryPress(g, -1, 100, 100);
    CHECK(LibraryRelease(g, 2).outcome == LibraryOutcome::None);
    // The default hotbar holds ten distinct placeable blocks.
    BlockID d[HOTBAR_SLOTS]; DefaultHotbar(d);
    for (int i = 0; i < HOTBAR_SLOTS; i++) { CHECK(g_blocks[d[i]].placeable); for (int j = 0; j < i; j++) CHECK(d[i] != d[j]); }
}

static void TestMusicLevel() {
    printf("music onset level\n");
    const int SR = 44100, CH = SR / 4;
    std::vector<int16_t> pcm(CH);
    float out[16];
    auto run = [&](MusicLevelMeter& m, auto sample, int chunks, float* maxOut, float* meanOut) {
        float mx = 0, sum = 0; int n = 0;
        for (int c = 0; c < chunks; c++) {
            for (int i = 0; i < CH; i++) pcm[i] = (int16_t)sample(c * CH + i);
            MeasureMusicLevels(m, pcm.data(), CH, 16, SR, out);
            if (c < 4) continue; // settle
            for (float v : out) { mx = std::max(mx, v); sum += v; n++; }
        }
        *maxOut = mx; *meanOut = sum / n;
    };
    // A sustained pad (the bulk of the day's music): dark once settled.
    MusicLevelMeter pad; float mx, mean;
    run(pad, [&](int i) { return 6000.0 * sin(i * 2 * 3.14159265 * 220.0 / SR); }, 20, &mx, &mean);
    CHECK(mx < 0.05f);
    // The same pad with a soft plucked note every half second: flashes on
    // each note, dark most of the time in between.
    MusicLevelMeter pl;
    run(pl, [&](int i) {
        double t = (double)i / SR, since = fmod(t, 0.5);
        return 6000.0 * sin(i * 2 * 3.14159265 * 220.0 / SR) + 2500.0 * exp(-since * 30.0) * sin(i * 2 * 3.14159265 * 1760.0 / SR);
    }, 20, &mx, &mean);
    printf("    plucks over a pad: peak %.2f, mean %.2f\n", mx, mean);
    CHECK(mx > 0.9f && mean < 0.35f);
    // What's shown can't strobe: fed notes at 8 a second (full on, full
    // off), the output never moves faster than the slew limit, settles to
    // a gentle ripple, and a single note swells and fades over about a second.
    {
        MusicGlow g; const float fdt = 1.0f / 60.0f;
        float maxStep = 0, lo = 1, hi = 0;
        for (int i = 0; i < 60 * 10; i++) {
            float before = g.shown;
            float v = MusicGlowStep(g, (i / 4) % 2 == 0 ? 1.0f : 0.0f, fdt); // 8 Hz square
            maxStep = std::max(maxStep, fabsf(v - before));
            if (i > 60 * 5) { lo = std::min(lo, v); hi = std::max(hi, v); }
        }
        printf("    8 Hz notes -> shown ripples %.2f..%.2f, fastest %.3f per frame\n", lo, hi, maxStep);
        CHECK(maxStep <= MUSIC_GLOW_MAX_RATE * fdt + 1e-5f);
        CHECK(hi - lo < 0.15f); // no flashing: at most a faint shimmer
        MusicGlow one; float peak = 0; int peakAt = 0, darkAgain = -1;
        for (int i = 0; i < 60 * 4; i++) {
            float v = MusicGlowStep(one, i < 6 ? 1.0f : 0.0f, fdt); // one 0.1 s note
            if (v > peak) { peak = v; peakAt = i; }
            if (darkAgain < 0 && i > peakAt && peak > 0 && v < peak * 0.2f) darkAgain = i;
        }
        CHECK(peak > 0.3f && peakAt >= 6 && darkAgain > 60); // a swell, not a blink: still glowing a second later
    }
    // Near-silence never flashes, however it wobbles.
    MusicLevelMeter q;
    run(q, [&](int i) { return (i / 441) % 7 == 0 ? 3.0 : -2.0; }, 20, &mx, &mean);
    CHECK(mx == 0.0f);
}

static void TestGlowLight() {
    printf("glow light grid\n");
    // No walkgrid material glows yet (M1.9): the grid stays empty and the
    // shader skips its lookup. (Voxistics' emitter checks left with its
    // roster; the engine code stays for when a glowing material comes.)
    World none; none.Set(0, 0, 0, BLOCK_STONE);
    int ox, oy, oz; GlowGridOrigin(10.5f, 12.6f, 10.5f, ox, oy, oz);
    GlowGrid g; BuildGlowGrid(none, ox, oy, oz, g);
    CHECK(g.emitters.empty() && g.texels.empty());
    for (int i = 0; i < BLOCK_COUNT; i++) CHECK(g_blocks[i].glow == GLOW_NONE);
}

// Camera-relative rendering (render.cpp, D13): placing a point relative to
// the eye and viewing it with a translation-free view lands it where the
// full world-space view would -- and far from the start it lands steadily,
// where the world-space product loses precision.
static void TestCameraRelative() {
    printf("camera-relative view\n");
    Vec3 f = Normalize({ 0.3f, -0.2f, 0.9f }), up = { 0, 1, 0 };
    auto apply = [](const Mat4& m, Vec3 p) { // row vector * matrix, then divide
        float x = p.x * m.m[0][0] + p.y * m.m[1][0] + p.z * m.m[2][0] + m.m[3][0];
        float y = p.x * m.m[0][1] + p.y * m.m[1][1] + p.z * m.m[2][1] + m.m[3][1];
        float z = p.x * m.m[0][2] + p.y * m.m[1][2] + p.z * m.m[2][2] + m.m[3][2];
        float w = p.x * m.m[0][3] + p.y * m.m[1][3] + p.z * m.m[2][3] + m.m[3][3];
        return Vec3{ x / w, y / w, z / w };
    };
    Mat4 proj = MatPerspectiveFovLH(0.8f, 16.0f / 9.0f, 0.1f, 500.0f);
    Mat4 rel = MatMul(MatLookToLH({ 0, 0, 0 }, f, up), proj);
    // Near the start the two agree.
    Vec3 eye = { 12.5f, 20.6f, -7.25f }, pt = { 15.0f, 18.0f, 1.0f };
    Vec3 a = apply(MatMul(MatLookToLH(eye, f, up), proj), pt), b = apply(rel, pt - eye);
    CHECK(fabsf(a.x - b.x) < 1e-4f && fabsf(a.y - b.y) < 1e-4f && fabsf(a.z - b.z) < 1e-4f);
    // A million blocks out, a point 1/8 block to the side of another must
    // land beside it on screen; relative, it does, and steadily.
    Vec3 far = { 1000000.0f, 40.0f, 1000000.0f }, p1 = far + Vec3{ 3.0f, -1.0f, 8.0f }, p2 = p1 + Vec3{ 0.125f, 0, 0 };
    Vec3 r1 = apply(rel, p1 - far), r2 = apply(rel, p2 - far);
    CHECK(r2.x - r1.x > 0.001f); // resolved, and to the right
}

static void TestSky() {
    printf("sky model and shadow projection\n");
    SkyState dawn = ComputeSky(0), noon = ComputeSky(1500), dusk = ComputeSky(3000), night = ComputeSky(3300);
    CHECK(fabsf(dawn.sunDir.y) < 1e-4f && dawn.sunDir.x > 0.99f);     // rises in the east (+X)
    CHECK(noon.sunDir.y > 0.99f && fabsf(noon.sunDir.z) < 1e-6f);   // straight overhead (the equator)
    // The compass holds it all together: the sun rises east and sets west,
    // the pole is north, a player at yaw 0 faces north and turns east.
    CHECK(Dot(dawn.sunDir, kEast) > 0.999f && Dot(dusk.sunDir, kWest) > 0.999f && Dot(CelestialPole(), kNorth) > 0.999f);
    // One sky: a star that sits where the sun rose is turned exactly with the
    // sun at every hour (the star field and the sun share their motion).
    for (float t = 0; t < 3600; t += 97) {
        SkyState st = ComputeSky(t);
        float R[3][3]; AxisAngleMatrix(CelestialPole(), st.starAngle, R);
        Vec3 star = { R[0][0], R[1][0], R[2][0] }; // R * east
        CHECK(Dot(star, st.sunDir) > 0.999f);
    }
    {
        Player facing; facing.yaw = 0; facing.pitch = 0;
        Vec3 fw, rt, upv; GetCameraVectors(facing, fw, rt, upv);
        CHECK(Dot(fw, kNorth) > 0.999f && Dot(rt, kEast) > 0.999f);
        CHECK(strcmp(CompassPoint(0.0f), "NORTH") == 0 && strcmp(CompassPoint(1.5708f), "EAST") == 0 &&
              strcmp(CompassPoint(3.1416f), "SOUTH") == 0 && strcmp(CompassPoint(-1.5708f), "WEST") == 0 &&
              strcmp(CompassPoint(0.785f), "NORTH-EAST") == 0);
    }
    CHECK(fabsf(dusk.sunDir.y) < 1e-3f && dusk.sunDir.x < -0.99f);    // sets in the west
    CHECK(night.sunDir.y < -0.8f && night.moonDir.y > 0.3f);          // moon up at night
    {   // ...and still up in the west at dawn (a new world's first sunrise), setting early in the morning
        SkyState d0 = ComputeSky(0.0f), d15 = ComputeSky(900.0f), late = ComputeSky(2700.0f);
        CHECK(d0.moonDir.y > 0.3f && d0.moonDir.x < 0 && d15.moonDir.y < 0 && late.moonDir.y < 0);
    }
    CHECK(noon.daylight == 1.0f && fabsf(night.daylight - NIGHT_LIGHT) < 1e-5f);
    CHECK(night.starsVisible == 1.0f && noon.starsVisible == 0.0f);
    CHECK(night.sunLight == 0.0f && noon.sunLight == 1.0f);
    SkyState wrap = ComputeSky(3600.0f);
    CHECK(fabsf(wrap.sunDir.x - dawn.sunDir.x) < 1e-4f && fabsf(wrap.sunDir.y - dawn.sunDir.y) < 1e-4f); // loops

    Vec3 eye = { 100.3f, 30.0f, -42.7f };
    Mat4 lvp = ShadowLightViewProj(eye, noon.sunDir, 64.0f, 200.0f, 2048);
    Vec3 c = XformPoint(lvp, eye);
    CHECK(fabsf(c.x) < 0.01f && fabsf(c.y) < 0.01f && c.z > 0.4f && c.z < 0.6f); // centred, mid-depth
    Vec3 towardSun = XformPoint(lvp, { eye.x + noon.sunDir.x * 10, eye.y + noon.sunDir.y * 10, eye.z + noon.sunDir.z * 10 });
    CHECK(towardSun.z < c.z && fabsf(towardSun.x - c.x) < 1e-3f);    // nearer the light, same texel
    Vec3 edge = XformPoint(lvp, { eye.x + 60, eye.y, eye.z });
    CHECK(fabsf(edge.x) < 1.0f && fabsf(edge.y) < 1.0f);              // 60 blocks out is still on the map
    // Moving less than a texel doesn't move the map.
    Mat4 lvp2 = ShadowLightViewProj({ eye.x + 0.001f, eye.y, eye.z }, noon.sunDir, 64.0f, 200.0f, 2048);
    CHECK(fabsf(lvp2.m[3][0] - lvp.m[3][0]) < 1e-3f || fabsf(lvp2.m[3][0] - lvp.m[3][0]) > 2.0f / 2048 * 0.9f);

    // Atmosphere (4.9): a white-gold high sun, an orange low one, none at
    // night; moonlight only at night; the sunset band only near sunset;
    // exposure lifted only at night; and no pops anywhere in the day.
    Atmosphere an = ComputeAtmosphere(noon), ad = ComputeAtmosphere(ComputeSky(2940)), ah = ComputeAtmosphere(night);
    CHECK(an.sunColor.x > ad.sunColor.x && an.sunColor.z / an.sunColor.x > 0.8f && ad.sunColor.z / ad.sunColor.x < 0.4f);
    CHECK(ah.sunColor.x == 0.0f && ah.moonColor.z > 0.1f && an.moonColor.z == 0.0f);
    CHECK(an.twilightAmount == 0.0f && ad.twilightAmount > 0.5f && ah.twilightAmount == 0.0f);
    CHECK(an.exposure == 1.0f && ah.exposure > 2.0f && ad.exposure < 1.5f);
    CHECK(an.zenith.z > an.zenith.x && an.horizon.x > an.zenith.x);    // deep blue overhead, paler at the horizon
    CHECK(an.ambientUp.x > an.ambientDown.x && ah.ambientUp.z > ah.ambientUp.x); // sky above; night light is blue
    Atmosphere prev = ComputeAtmosphere(ComputeSky(0));
    float worst = 0;
    for (int t = 1; t <= 3600; t++) {
        Atmosphere a = ComputeAtmosphere(ComputeSky((float)t));
        const Vec3* cur[] = { &a.sunColor, &a.moonColor, &a.zenith, &a.horizon, &a.ambientUp, &a.ambientDown };
        const Vec3* old[] = { &prev.sunColor, &prev.moonColor, &prev.zenith, &prev.horizon, &prev.ambientUp, &prev.ambientDown };
        for (int k = 0; k < 6; k++)
            worst = std::max({ worst, fabsf(cur[k]->x - old[k]->x), fabsf(cur[k]->y - old[k]->y), fabsf(cur[k]->z - old[k]->z) });
        worst = std::max({ worst, fabsf(a.exposure - prev.exposure), fabsf(a.twilightAmount - prev.twilightAmount) });
        prev = a;
    }
    CHECK(worst < 0.05f); // per second of game time: every change is a fade (the steepest, exposure at dusk, ~3%/s)
}




// Each colour-vision mode keeps the three pulse colours apart as that kind
// of vision sees them (simulated with the standard dichromat matrices;
// "no colour" by brightness alone). For comparison: the typical colours hold
// up fairly for the dichromats (they differ in brightness too), but by
// brightness alone blue and red nearly merge -- which the no-colour mode fixes.

static float TestTextWidth(const std::string& s, float scale) { return (float)s.size() * 8.0f * scale / 0.65f; }



// ---------------------------------------------------------------------
// World sound palette (docs/SOUND_PALETTE.md)

static double FindChordTime(int chord, double from) {
    MusicHarmony h, h2;
    for (double t = from; t < from + 200; t += 0.25) {
        MusicHarmonyAt(t, &h); MusicHarmonyAt(t + 3.0, &h2);
        if (h.chord == chord && !h.blending && h2.chord == chord && !h2.blending) return t;
    }
    return -1;
}

static bool InSafeSet(int chord, bool anchor, double hz) {
    int midi = (int)lround(69 + 12 * log2(hz / 440.0));
    int pc = ((midi % 12) + 12) % 12;
    if (anchor) return pc == 2 || pc == 7 || pc == 9;
    static const int sets[5][7] = { { 2, 4, 5, 7, 9, 0, -1 }, { 7, 9, 0, 2, 5, -1, -1 }, { 4, 7, 9, 11, 2, -1, -1 },
                                    { 9, 0, 2, 4, 7, -1, -1 }, { 2, 4, 5, 7, 9, 0, -1 } };
    for (int k = 0; k < 7; k++) if (sets[chord][k] == pc) return true;
    return false;
}

// Renders `seconds` of a palette from music time t0 (the clock running).
static std::vector<float> RenderPalette(SoundPalette& p, double t0, double seconds) {
    std::vector<float> out((size_t)(seconds * 44100) / 512 * 512);
    for (size_t i = 0; i < out.size(); i += 512) p.Render(out.data() + i, 512, t0 + i / 44100.0, true);
    return out;
}

static void TestMusicHarmony() {
    printf("music harmony + colour\n");
    MusicHarmony h;
    MusicHarmonyAt(20, &h);
    CHECK(h.section == MUSIC_DAWN && h.chord == MUSIC_DM9 && !h.pulsed);
    MusicHarmonyAt(1500, &h);
    CHECK(h.section == MUSIC_MIDDAY && h.pulsed && fabs(h.bpm - 124) < 1e-9);
    MusicHarmonyAt(3400, &h);
    CHECK(h.section == MUSIC_NIGHT && !h.pulsed);
    for (int c = 0; c < 4; c++) CHECK(FindChordTime(c, 1200) > 0); // every chord of the cycle shows up in Midday
    // Beats advance at the section's tempo.
    MusicHarmony a, b; MusicHarmonyAt(1500, &a); MusicHarmonyAt(1501, &b);
    CHECK(fabs((b.beat - a.beat) - 124.0 / 60.0) < 1e-6);
    // The neutral colour renders the track exactly as composed.
    const int N = 11025;
    std::vector<int16_t> x(N), y(N), z(N);
    MusicState s1, s2, s3; ResetMusicState(&s1); ResetMusicState(&s2); ResetMusicState(&s3);
    MusicColour neutral, far; far.positive = -1; far.activity = 1; far.mechanical = 1;
    for (int k = 0; k < 8; k++) {
        GenerateMusicChunk(1500 + k * 0.25, N, 1.0, &s1, x.data());
        GenerateMusicChunk(1500 + k * 0.25, N, 1.0, &s2, y.data(), &neutral);
        GenerateMusicChunk(1500 + k * 0.25, N, 1.0, &s3, z.data(), &far);
    }
    CHECK(x == y);
    // A far colour changes it, but only a little (small, bounded shifts).
    double diff = 0, ref = 0;
    for (int i = 0; i < N; i++) { diff += fabs((double)x[i] - z[i]); ref += fabs((double)x[i]); }
    CHECK(diff > 0 && diff < ref * 0.8);
}

static void TestSoundPalette() {
    printf("sound palette\n");
    const double chordT[4] = { FindChordTime(MUSIC_DM9, 1200), FindChordTime(MUSIC_G7SUS4, 1200),
                               FindChordTime(MUSIC_EM7, 1200), FindChordTime(MUSIC_A7SUS4, 1200) };
    // Every sound, under every chord, at the axis extremes: every pitch in
    // the chord's safe set, never above the -21 dB ceiling.
    int unsafe = 0, loud = 0, silentTonal = 0;
    const double ceiling = pow(10.0, (-21.0 + 0.5) / 20.0);
    for (int id = 0; id < SND_COUNT; id++)
        for (int c = 0; c < 4; c++)
            for (int corner = 0; corner < 4; corner++) {
                SoundPalette p;
                SoundAxes ax; ax.positive = (corner & 1) ? 1.0f : -1.0f; ax.activity = 0.5f; ax.mechanical = (corner & 2) ? 1.0f : 0.0f;
                p.SetAxes(ax);
                AmbientScene sc; sc.machines = 1; sc.musicBlockCount = 1; sc.musicBlockKey[0] = 3;
                p.SetScene(sc);
                std::vector<float> warm(512);
                p.Render(warm.data(), 512, chordT[c], true);
                SoundCue cue; cue.id = (SoundId)id; cue.material = MAT_STONE; cue.slot = c * 3; cue.strength = 1;
                p.Play(cue);
                if (id == SND_SLIDE) { RenderPalette(p, chordT[c], 0.5); p.Release(SND_SLIDE); }
                std::vector<float> out = RenderPalette(p, chordT[c] + 0.0116, 5.0);
                MusicHarmony h; MusicHarmonyAt(chordT[c], &h);
                double scale = h.masterGain * 0.78, peak = 0;
                for (float v : out) peak = std::max(peak, (double)fabs(v) / scale);
                if (peak > ceiling) { loud++; printf("  loud: %s %.1f dB\n", SoundName((SoundId)id), 20 * log10(peak)); }
                SoundPalette::NoteLog log[64];
                int n = p.RecentNotes(log, 64);
                for (int i = 0; i < n; i++)
                    if (!InSafeSet(log[i].chord, log[i].anchor, log[i].hz)) { unsafe++; printf("  unsafe: %s %.1f Hz\n", SoundName(log[i].id), log[i].hz); }
                (void)silentTonal;
            }
    CHECK(unsafe == 0);
    CHECK(loud == 0);

    // Gestures: a run of Sets climbs the ladder, a run of Takes falls.
    {
        SoundPalette p;
        SoundAxes ax; ax.positive = 0.5f; p.SetAxes(ax);
        std::vector<float> buf(512);
        double t = chordT[0];
        p.Render(buf.data(), 512, t, true);
        std::vector<double> hz;
        for (int i = 0; i < 4; i++) {
            SoundCue c; c.id = SND_SET; p.Play(c);
            SoundPalette::NoteLog log[64]; int n = p.RecentNotes(log, 64);
            hz.push_back(log[n - 1].hz);
            for (int k = 0; k < 26; k++) { t += 512 / 44100.0; p.Render(buf.data(), 512, t, true); } // ~0.3 s
        }
        CHECK(hz[1] > hz[0] && hz[2] > hz[1] && hz[3] > hz[2]);
        for (int k = 0; k < 400; k++) { t += 512 / 44100.0; p.Render(buf.data(), 512, t, true); } // the gesture ends
        std::vector<double> down;
        for (int i = 0; i < 3; i++) {
            SoundCue c; c.id = SND_TAKE; p.Play(c);
            SoundPalette::NoteLog log[64]; int n = p.RecentNotes(log, 64);
            down.push_back(log[n - 2].hz); // each Take logs two notes; the first is its ladder step
            for (int k = 0; k < 26; k++) { t += 512 / 44100.0; p.Render(buf.data(), 512, t, true); }
        }
        CHECK(down[1] < down[0] && down[2] < down[1]);
    }
    // Merge: two onsets inside 30 ms are one sound, not a flam.
    {
        SoundPalette p;
        std::vector<float> buf(512);
        p.Render(buf.data(), 512, chordT[0], true);
        SoundCue c; c.id = SND_SLOT; c.slot = 4;
        p.Play(c);
        int before = p.ActiveVoices();
        p.Play(c);
        CHECK(p.ActiveVoices() == before);
    }
    // Determinism: the same inputs render the same samples.
    {
        SoundPalette a, b;
        AmbientScene sc; sc.plants = 1; sc.water = 0.5f;
        SoundAxes ax; ax.activity = 0.8f; ax.positive = 0.6f; ax.mechanical = 0.1f;
        for (SoundPalette* p : { &a, &b }) { p->SetAxes(ax); p->SetScene(sc); p->SetAmbientEnabled(true); }
        SoundCue c; c.id = SND_UNVEIL;
        a.Play(c); b.Play(c);
        CHECK(RenderPalette(a, 700, 12) == RenderPalette(b, 700, 12));
    }
    // Density: calm spends far less of the ambient budget than busy, and
    // stays within 1 event per 4 bars (plus the occasional rare colour).
    {
        int counts[2];
        for (int k = 0; k < 2; k++) {
            SoundPalette p;
            AmbientScene sc; sc.plants = 1; sc.water = 0.4f; sc.machines = 0.5f;
            SoundAxes ax; ax.activity = k ? 1.0f : 0.0f; ax.positive = 0.5f; ax.mechanical = 0.3f;
            p.SetAxes(ax); p.SetScene(sc); p.SetAmbientEnabled(true);
            RenderPalette(p, 1300, 128.0); // 66 bars at 124 BPM
            counts[k] = p.ScheduledAmbientEvents();
        }
        CHECK(counts[0] >= 8 && counts[0] <= 66 / 4 + 4);
        CHECK(counts[1] > counts[0] * 3);
    }
    // Stereo placement: a block set to the listener's right sounds louder
    // on the right; Mono centres everything; an unplaced sound sits centre.
    {
        auto energy = [&](bool placeRight, bool mono, float& l, float& r) {
            SoundPalette p;
            p.SetListener(0, 0, 0, 0); // facing +Z: right is +X
            p.SetMono(mono);
            std::vector<float> lr(1024);
            p.RenderStereo(lr.data(), 512, chordT[0], true);
            SoundCue c; c.id = SND_SET; c.material = MAT_STONE;
            if (placeRight) { c.placed = true; c.x = 4; c.y = 0; c.z = 0; }
            p.Play(c);
            lr.assign(2 * 44100, 0.0f);
            for (int i = 0; i < 44100; i += 512) p.RenderStereo(lr.data() + 2 * i, std::min(512, 44100 - i), chordT[0] + i / 44100.0, true);
            l = r = 0;
            for (int i = 0; i < 44100; i++) { l += lr[2 * i] * lr[2 * i]; r += lr[2 * i + 1] * lr[2 * i + 1]; }
        };
        float l, r;
        energy(true, false, l, r);  CHECK(r > l * 2.0f);
        energy(true, true, l, r);   CHECK(fabsf(l - r) <= 1e-6f * (l + r) + 1e-12f);
        energy(false, false, l, r); CHECK(fabsf(l - r) <= 0.3f * (l + r)); // centred; only the ping-pong echo leans
    }
    // Footsteps keep the beat: walking steps land one per beat, sprinting
    // one per 8th, crouching every other beat; stopping stops them.
    {
        int counts[4] = {};
        for (int gait = 1; gait <= 3; gait++) {
            SoundPalette p;
            p.SetAmbientEnabled(true);
            p.SetGait(gait, gait == 3 ? MAT_STONE : MAT_EARTH);
            RenderPalette(p, 1500, 8.0);   // 8 s at 124 BPM = 16.5 beats
            counts[gait] = p.PlayedCount(SND_FOOTFALL);
            p.SetGait(SoundPalette::GAIT_NONE, MAT_EARTH);
            RenderPalette(p, 1508, 2.0);
            CHECK(p.PlayedCount(SND_FOOTFALL) == counts[gait]);
        }
        CHECK(counts[2] >= 15 && counts[2] <= 18);
        CHECK(counts[3] >= 31 && counts[3] <= 35);
        CHECK(counts[1] >= 7 && counts[1] <= 9);
    }
    // Pausing fades everything to silence.
    {
        SoundPalette p;
        std::vector<float> buf(512);
        p.Render(buf.data(), 512, chordT[1], true);
        SoundCue c; c.id = SND_BLOOM; p.Play(c);
        RenderPalette(p, chordT[1], 1.0);
        p.FadeOut(0.3f);
        std::vector<float> out = RenderPalette(p, chordT[1] + 1, 1.0);
        float tail = 0;
        for (size_t i = out.size() / 2; i < out.size(); i++) tail = std::max(tail, fabsf(out[i]));
        CHECK(tail == 0.0f);
        CHECK(p.ActiveVoices() == 0);
    }
    // The voice cap (M1.1): a flood of every sound at once holds at 48
    // voices, and an interaction still gets through when all are busy.
    {
        SoundPalette p;
        AmbientScene sc; sc.plants = 1; sc.water = 1; sc.machines = 1; sc.ember = 1; sc.glow = 1;
        SoundAxes ax; ax.activity = 1; p.SetAxes(ax); p.SetScene(sc); p.SetAmbientEnabled(true);
        p.SetGait(SoundPalette::GAIT_SPRINT, MAT_STONE);
        std::vector<float> lr(1024);
        double t = chordT[0];
        int most = 0;
        for (int i = 0; i < 600; i++) {
            if (i % 3 == 0)
                for (int k = 0; k < 4; k++) { SoundCue c; c.id = (SoundId)((i / 3 + k) % SND_COUNT); c.material = MAT_STONE; p.Play(c); }
            p.RenderStereo(lr.data(), 512, t, true);
            t += 512 / 44100.0;
            most = std::max(most, p.ActiveVoices());
        }
        CHECK(most == 48);
        int before = p.PlayedCount(SND_SET);
        SoundCue c; c.id = SND_SET; c.material = MAT_STONE; p.Play(c);
        CHECK(p.PlayedCount(SND_SET) == before + 1);
        SoundPalette::NoteLog log[64]; int n = p.RecentNotes(log, 64);
        CHECK(n > 0 && log[n - 1].id == SND_SET); // it sounded: a voice was stolen for it
        CHECK(p.ActiveVoices() <= 48);
    }
}

static void TestSoundscape() {
    printf("soundscape axes\n");
    World w;
    ResetWorldState(w);
    Stream(w, 8, 8, 300);
    int ground = TerrainHeight(8, 8);
    auto settle = [&](Soundscape& s, int seconds) {
        for (int f = 0; f < seconds * 60; f++) {
            s.CensusStep(w, 8, ground + 1, 8);
            SoundscapeInput in; in.dt = 1.0f / 60; in.musicSection = MUSIC_MORNING;
            s.Update(in);
        }
    };
    Soundscape wild;
    settle(wild, 30);
    CHECK(wild.HaveCensus());
    CHECK(wild.Axes().mechanical < 0.2f);   // open land reads organic
    CHECK(wild.Axes().positive > 0.2f);
    // Materials (M1.9): grassy ground soft, soils earthy, stones hard.
    CHECK(BlockSoundMaterial(BLOCK_MEADOW_GRASS) == MAT_PLANT && BlockSoundMaterial(BLOCK_MOSS) == MAT_PLANT);
    CHECK(BlockSoundMaterial(BLOCK_DIRT) == MAT_EARTH && BlockSoundMaterial(BLOCK_SAND) == MAT_EARTH);
    CHECK(BlockSoundMaterial(BLOCK_STONE) == MAT_STONE && BlockSoundMaterial(BLOCK_GRAVEL) == MAT_STONE);
    CHECK(BlockSoundClass(BLOCK_CLAY) == SC_NATURAL && BlockSoundClass(BLOCK_FOUNDATION) == SC_NEUTRAL);
}


static void TestPatchwork() {
    printf("flat v2 ground patchwork\n");
    WorldGenParams saved = g_worldGen;
    g_worldGen.type = GEN_FLAT; g_worldGen.version = 2; g_worldGen.seed = 12345;
    int counts[3] = {}, changes = 0;
    BlockID prev = BLOCK_AIR;
    for (int x = 0; x < 256; x++)
        for (int z = 0; z < 256; z++) {
            BlockID b = SurfaceBlockAt(x * 2, z * 2);
            counts[b == BLOCK_MEADOW_GRASS ? 0 : b == BLOCK_SAND ? 1 : 2]++;
            if (z > 0 && b != prev) changes++;
            prev = b;
        }
    const int total = 256 * 256;
    printf("    grass %.0f%%, sand %.0f%%, gravel %.0f%%\n", 100.0 * counts[0] / total, 100.0 * counts[1] / total, 100.0 * counts[2] / total);
    CHECK(counts[0] > total * 0.4 && counts[1] > total * 0.05 && counts[2] > total * 0.05);
    CHECK(changes > 200 && changes < total / 4);   // patches, not noise and not one field
    CHECK(SurfaceBlockAt(1000, -777) == SurfaceBlockAt(1000, -777)); // a pure function
    int differ = 0;
    for (int i = 0; i < 200; i++) { g_worldGen.seed = 12345; BlockID a = SurfaceBlockAt(i * 7, i * 3); g_worldGen.seed = 999; differ += a != SurfaceBlockAt(i * 7, i * 3); }
    CHECK(differ > 20);                              // each world its own
    // Generated columns wear it on top; flat v1 worlds keep plain dirt.
    World w; ResetWorldState(w);
    g_worldGen.seed = 12345;
    GenerateColumn(w, 0, 0);
    CHECK(w.Get(5, 12, 5) == SurfaceBlockAt(5, 5) && w.Get(5, 11, 5) == BLOCK_DIRT && w.Get(5, 13, 5) == BLOCK_AIR);
    ResetWorldState(w);
    g_worldGen.version = 1;
    GenerateColumn(w, 0, 0);
    CHECK(w.Get(5, 12, 5) == BLOCK_DIRT);
    g_worldGen = saved;
}

// ---- walkgrid-hills v1 (terrain.h, M1.4) ----
static void TestHills() {
    printf("walkgrid-hills terrain\n");
    // Pure: the same seed gives the same column, bit for bit; another seed differs.
    TerrainColumn a, b, c;
    HillsColumn(42, 3, -7, a); HillsColumn(42, 3, -7, b); HillsColumn(43, 3, -7, c);
    CHECK(a.chunks == b.chunks && a.cells == b.cells && a.present == b.present);
    CHECK(a.cells != c.cells);
    // Fingerprint of seed 1, column (0, 0): pins the generator's output. If
    // this changes, the output changed, and that's a new version (DESIGN 2.5).
    uint64_t h = 1469598103934665603ull;
    TerrainColumn f; HillsColumn(1, 0, 0, f);
    for (uint8_t x : f.cells) { h ^= x; h *= 1099511628211ull; }
    printf("  fingerprint %016llx\n", (unsigned long long)h);
    CHECK(h == HILLS_V1_FINGERPRINT);
    // Every cell up to the surface is ground, above it air; the floor holds.
    bool ok = true;
    for (int lz = 0; lz < 16; lz += 5)
        for (int lx = 0; lx < 16; lx += 5) {
            int top = HillsHeight(1, lx, lz);
            for (int y = 0; y < a.chunks * 16 && y < 80; y++) {
                uint8_t m = f.cells[(size_t)(y / 16) * CHUNK_CELLS + Chunk::LocalIndex(lx, y % 16, lz)];
                ok &= (y <= top) == (m != 0);
            }
            ok &= f.cells[Chunk::LocalIndex(lx, 0, lz)] == BLOCK_FOUNDATION;
        }
    CHECK(ok);
    // Across many columns: the starting materials all appear, nothing else.
    std::array<int, BLOCK_COUNT> seen{};
    for (int cz = -12; cz < 12; cz += 3)
        for (int cx = -12; cx < 12; cx += 3) {
            TerrainColumn t; HillsColumn(7, cx, cz, t);
            for (int lz = 0; lz < 16; lz++)
                for (int lx = 0; lx < 16; lx++) {
                    int y = HillsHeight(7, cx * 16 + lx, cz * 16 + lz);
                    seen[t.cells[(size_t)(y / 16) * CHUNK_CELLS + Chunk::LocalIndex(lx, y % 16, lz)]]++;
                }
        }
    int kinds = 0;
    for (int i = 0; i < BLOCK_COUNT; i++) kinds += seen[i] > 0;
    CHECK(seen[BLOCK_MEADOW_GRASS] > 0 && seen[BLOCK_DRY_TURF] > 0 && seen[BLOCK_SAND] > 0 && seen[BLOCK_STONE] > 0);
    CHECK(kinds >= 7);
    printf("  surface kinds in a 24x24-chunk sample: %d\n", kinds);
}

// ---- Job threads (jobs.h, M1.4) ----
static void TestJobs() {
    printf("job threads\n");
    JobsStart();
    CHECK(JobsThreadCount() >= 1 && JobsThreadCount() <= 4);
    // Streaming on real threads lands the same ground as generating inline.
    World w; ResetWorldState(w);
    g_loadRadius = 2;
    g_worldGen = WorldGenParams(); g_worldGen.type = GEN_WALKGRID; g_worldGen.seed = 99;
    for (int t = 0; t < 400 && (g_residentColumns.size() < 49 || ColumnsGenerating() > 0); t++) {
        EnsureChunksLoaded(0, 0);
        ProcessColumnGeneration(w);
        if (t % 8 == 7) JobsWaitIdle();
    }
    CHECK(g_residentColumns.size() == 49 && ColumnsGenerating() == 0);
    World ref; ResetWorldState(ref); // (keeps g_worldGen)
    bool same = true;
    for (int cz = -1; cz <= 1; cz++)
        for (int cx = -1; cx <= 1; cx++) {
            GenerateColumn(ref, cx, cz);
            for (int cy = 0; cy < 4; cy++) {
                Chunk* a = w.FindChunk({ cx, cy, cz }); Chunk* b = ref.FindChunk({ cx, cy, cz });
                same &= (a == nullptr) == (b == nullptr);
                if (a && b) same &= memcmp(a->blocks, b->blocks, CHUNK_CELLS) == 0;
            }
        }
    CHECK(same);
    // A column still generating when the world is reset is dropped on arrival.
    World w2; ResetWorldState(w2);
    EnsureChunksLoaded(40, 40);
    ProcessColumnGeneration(w2);        // submits a few (one may already have landed: threads are fast)
    w2.ClearChunks();                   // New Game / Load: the world goes with the streaming state,
    ResetColumnStreaming();             // as ResetWorldForNewGame and LoadGame do
    JobsWaitIdle();
    JobsApply(JOB_TERRAIN, 100);
    CHECK(g_residentColumns.empty() && w2.chunks.empty());
    JobsStop();
    CHECK(JobsThreadCount() == 0);
}

// ---- The ground mesh (groundmesh.h, M1.5) ----
static void TestGroundMesh() {
    printf("ground mesh\n");
    BlockTextureSet t; BuildBlockTextures(VtexSet(), t);
    InitGroundMaterials(t.faceLayer);
    World w; ResetWorldState(w);
    g_worldGen = WorldGenParams(); g_worldGen.type = GEN_WALKGRID; g_worldGen.seed = 5;
    for (int cz = -2; cz <= 2; cz++) for (int cx = -2; cx <= 2; cx++) GenerateColumn(w, cx, cz);
    // The copy matches the world, margin included; under the floor is solid.
    GroundCells gc; CopyGroundCells(w, { 0, 1, 0 }, gc);
    bool same = true;
    for (int y = 0; y < GROUND_GRID; y++) for (int z = 0; z < GROUND_GRID; z++) for (int x = 0; x < GROUND_GRID; x++)
        same &= gc.cells[(y * GROUND_GRID + z) * GROUND_GRID + x] == (uint8_t)w.Get(x - FACET_PAD, 16 + y - FACET_PAD, z - FACET_PAD);
    CHECK(same);
    GroundCells g0; CopyGroundCells(w, { 0, 0, 0 }, g0);
    CHECK(g0.cells[0] == BLOCK_FOUNDATION); // y = -2
    // Two neighbouring chunks' meshes meet exactly, once unpacked to world
    // positions: every edge left open by chunk (0, y, 0) along the seam at
    // x = 16 (corners sit within half a block of it) is closed by chunk
    // (1, y, 0)'s reverse edge.
    typedef std::array<int64_t, 3> P3;
    auto meshEdges = [&](int cx, std::map<std::array<int64_t, 6>, int>& edges) {
        for (int cy = 0; cy <= 3; cy++) {
            ChunkCoord cc{ cx, cy, 0 };
            GroundCells cells; CopyGroundCells(w, cc, cells);
            GroundMesh m; BuildGroundMesh(cells, 0, m);
            std::vector<P3> pts;
            for (auto& v : m.verts)
                pts.push_back({ (int64_t)v.x + (int64_t)cc.x * 16 * 2048, (int64_t)v.y + (int64_t)cc.y * 16 * 2048, (int64_t)v.z + (int64_t)cc.z * 16 * 2048 });
            for (size_t i = 0; i < m.idx.size(); i += 3)
                for (int k = 0; k < 3; k++) {
                    P3 a = pts[m.idx[i + k]], b = pts[m.idx[i + (k + 1) % 3]];
                    edges[{ a[0], a[1], a[2], b[0], b[1], b[2] }]++;
                }
        }
    };
    std::map<std::array<int64_t, 6>, int> eA, eB;
    meshEdges(0, eA); meshEdges(1, eB);
    auto world = [](int64_t q) { return q / 2048.0 - 2.0; };
    int seam = 0, unmatched = 0;
    for (auto& kv : eA) {
        const auto& e = kv.first;
        if (eA.count({ e[3], e[4], e[5], e[0], e[1], e[2] })) continue; // closed within A
        bool nearSeam = true;
        for (int k : { 0, 3 }) nearSeam &= world(e[k]) > 15.4 && world(e[k]) < 16.6 && world(e[k + 2]) > 1.0 && world(e[k + 2]) < 15.0;
        if (!nearSeam) continue;
        seam++;
        if (!eB.count({ e[3], e[4], e[5], e[0], e[1], e[2] })) unmatched++;
    }
    CHECK(seam > 0);
    CHECK(unmatched == 0);
    printf("  open edges along the seam: %d, unmatched: %d\n", seam, unmatched);
    // Winding: seen from above through the game's own matrices, every
    // up-facing triangle turns the way D3D draws as a front face.
    {
        World f; ResetWorldState(f);
        g_worldGen = WorldGenParams(); g_worldGen.type = GEN_FLAT;
        for (int cz = -1; cz <= 1; cz++) for (int cx = -1; cx <= 1; cx++) GenerateColumn(f, cx, cz);
        Mat4 vp = MatMul(MatLookToLH({ 8, 30, 8 }, { 0.05f, -1, 0.02f }, { 0, 0, 1 }), MatPerspectiveFovLH(1.0f, 1.5f, 0.1f, 500));
        auto screenSign = [&](Vec3 a, Vec3 b, Vec3 c) {
            float sx[3], sy[3]; Vec3 q[3] = { a, b, c };
            for (int i = 0; i < 3; i++) {
                float x = q[i].x * vp.m[0][0] + q[i].y * vp.m[1][0] + q[i].z * vp.m[2][0] + vp.m[3][0];
                float y = q[i].x * vp.m[0][1] + q[i].y * vp.m[1][1] + q[i].z * vp.m[2][1] + vp.m[3][1];
                float ww = q[i].x * vp.m[0][3] + q[i].y * vp.m[1][3] + q[i].z * vp.m[2][3] + vp.m[3][3];
                sx[i] = x / ww; sy[i] = -y / ww; // D3D screen: y down
            }
            float area = (sx[1] - sx[0]) * (sy[2] - sy[0]) - (sx[2] - sx[0]) * (sy[1] - sy[0]);
            return area > 0 ? 1 : -1;
        };
        GroundCells fc; CopyGroundCells(f, { 0, 0, 0 }, fc);
        GroundMesh fm; BuildGroundMesh(fc, 0, fm);
        int facetSign = 0, agree = 0, total = 0;
        for (size_t i = 0; i < fm.idx.size(); i += 3) {
            auto P = [&](const GroundVertex& v) { return Vec3{ v.x / 2048.0f - 2, v.y / 2048.0f - 2, v.z / 2048.0f - 2 }; };
            Vec3 a = P(fm.verts[fm.idx[i]]), b = P(fm.verts[fm.idx[i + 1]]), c = P(fm.verts[fm.idx[i + 2]]);
            if (Cross(b - a, c - a).y <= 0) continue; // up-facing only
            int sgn = screenSign(a, b, c);
            facetSign = sgn; total++; agree += sgn == 1;
        }
        // D3D's default rasterizer takes clockwise-on-screen as the front:
        // in y-down screen coordinates, a positive signed area. (Until M1.9
        // this compared against the old cube mesher, whose culling worked
        // in game; they agreed.)
        CHECK(facetSign == 1 && total > 0 && agree == total);
    }
}

// ---- Blending heights, column tops and sky light (M1.6) ----
static void TestSkyLight() {
    printf("blending heights, column tops, sky light\n");
    // Every texture layer has heights, and authored height maps vary.
    VtexSet art;
    {
        std::ifstream in("../assets/textures/natural.vtex");
        std::stringstream ss; ss << in.rdbuf();
        ParseVtex(ss.str(), "natural.vtex", art);
    }
    BlockTextureSet t; BuildBlockTextures(art, t);
    CHECK(!t.height.empty() && t.height[0].size() == (size_t)t.layerCount * BLOCK_TEX_SIZE * BLOCK_TEX_SIZE);
    int L = t.faceLayer[BLOCK_MEADOW_GRASS][FACE_POS_Z][FACE_POS_Y];
    uint8_t lo = 255, hi = 0;
    for (int i = 0; i < BLOCK_TEX_SIZE * BLOCK_TEX_SIZE; i++) { uint8_t h = t.height[0][(size_t)L * BLOCK_TEX_SIZE * BLOCK_TEX_SIZE + i]; lo = std::min(lo, h); hi = std::max(hi, h); }
    CHECK(hi - lo > 60);
    CHECK(t.height.back().size() == (size_t)t.layerCount); // down to 1x1
    InitGroundMaterials(t.faceLayer);

    // Column tops follow generation and edits.
    World w; ResetWorldState(w);
    g_worldGen = WorldGenParams(); g_worldGen.type = GEN_FLAT;
    for (int cz = -1; cz <= 1; cz++) for (int cx = -1; cx <= 1; cx++) GenerateColumn(w, cx, cz);
    int G = TerrainHeight(5, 5);
    CHECK(w.Top(5, 5) == G && w.Top(-7, 12) == G && w.Top(100, 100) == -1);
    w.Set(5, G, 5, BLOCK_AIR);
    CHECK(w.Top(5, 5) == G - 1);
    w.Set(5, G + 4, 5, BLOCK_STONE);
    CHECK(w.Top(5, 5) == G + 4);
    w.Set(5, G + 4, 5, BLOCK_AIR);
    CHECK(w.Top(5, 5) == G - 1);

    // Sky light: open on flat ground, low on a pit's floor and under an
    // overhang, in between on a cliff face.
    GroundCells gc; CopyGroundCells(w, { 0, 0, 0 }, gc);
    float flat = GroundSkyAt(gc, 8, G + 1, 8);
    for (int y = G - 5; y <= G; y++)
        for (int z = 6; z <= 9; z++) for (int x = 6; x <= 9; x++) w.Set(x, y, z, BLOCK_AIR);   // a 4x4 pit, 6 deep
    for (int x = 12; x <= 15; x++) for (int z = 2; z <= 14; z++) w.Set(x, G + 3, z, BLOCK_STONE); // a roof, 2 cells up
    for (int y = G + 1; y <= G + 8; y++) for (int z = 0; z < 16; z++) w.Set(2, y, z, BLOCK_STONE); // a wall, 8 high
    CopyGroundCells(w, { 0, 0, 0 }, gc);
    float pit = GroundSkyAt(gc, 8, G - 5, 8);
    float under = GroundSkyAt(gc, 13, G + 1, 8);
    float cliff = GroundSkyAt(gc, 3, G + 4, 8);
    printf("  sky: flat %.2f, pit floor %.2f, under a roof %.2f, cliff face %.2f\n", flat, pit, under, cliff);
    CHECK(flat > 0.99f);
    CHECK(pit < 0.4f);
    CHECK(under < 0.5f);
    CHECK(cliff > 0.4f && cliff < 0.8f);
    // And it reaches the vertices.
    GroundMesh gm; BuildGroundMesh(gc, 0, gm);
    int dark = 0, bright = 0;
    for (auto& v : gm.verts) { if (v.sky < 100) dark++; if (v.sky > 250) bright++; }
    CHECK(dark > 0 && bright > 0);
}

// ---- Walking and picking on facets (collide.h, M1.7) ----
static void TestFacetCollision() {
    printf("walking and picking on facets\n");
    World w; ResetWorldState(w);
    g_loadRadius = 3; g_worldGen = WorldGenParams(); g_worldGen.type = GEN_WALKGRID; g_worldGen.seed = 11;
    Stream(w, 8, 8, 300);
    std::vector<FacetTri> tris;
    // Scripted walks over hills: 16 headings, 4 s each at a sprint, jumping
    // when stopped. Never below the ground, never lifted out of it (no
    // entombing), never a jump of the view.
    int sank = 0, lifted = 0, jumpsOfView = 0, stuck = 0;
    float walked = 0;
    for (int k = 0; k < 16; k++) {
        Player p; p.x = 8.5f; p.z = 8.5f; p.y = (float)TerrainHeight(8, 8) + 1.5f; p.yaw = k * 0.3927f;
        for (int i = 0; i < 30; i++) UpdatePlayerPhysics(w, p, 1.0f / 60.0f, MoveInput());
        MoveInput go; go.fwd = true; go.sprint = true;
        float sx = p.x, sz = p.z, prevEye = p.y + p.eyeHeight;
        int still = 0;
        for (int i = 0; i < 240; i++) {
            float px = p.x, pz = p.z, py = p.y;
            bool wasOnGround = p.onGround;
            go.jump = still > 10;
            UpdatePlayerPhysics(w, p, 1.0f / 60.0f, go);
            float eye = p.y + p.eyeHeight;
            // Walking along the ground (not landing from a fall): the view glides.
            if (!go.jump && wasOnGround && p.onGround && fabsf(eye - prevEye) > 0.25f) jumpsOfView++;
            prevEye = eye;
            if (p.y - py > 0.9f && !go.jump) lifted++;
            still = (fabsf(p.x - px) + fabsf(p.z - pz) < 1e-4f) ? still + 1 : 0;
            if (p.onGround) {
                int x = (int)floorf(p.x), y = (int)floorf(p.y), z = (int)floorf(p.z);
                GatherFacets(w, x - 1, std::max(0, y - 2), z - 1, x + 2, y + 2, z + 2, tris);
                float g;
                if (GroundHeight(tris, p.x, p.z, p.y - 3.0f, p.y + 3.0f, 0.3f, &g) && g > p.y + 0.1f) sank++;
            }
        }
        if (still > 60) stuck++;
        walked += sqrtf((p.x - sx) * (p.x - sx) + (p.z - sz) * (p.z - sz));
    }
    printf("  16 walks: %.0f blocks in all; sank %d, lifted %d, view jumps %d, stuck at the end %d\n", walked, sank, lifted, jumpsOfView, stuck);
    CHECK(sank == 0 && lifted == 0 && jumpsOfView == 0);
    CHECK(walked > 16 * 12.0f);
    // Picking at many angles: the hit is a solid cell, the place cell is
    // empty and across one of its faces, and the hit lies on the ground
    // the player sees (the facet it met is within reach).
    int hits = 0, bad = 0;
    for (int a = 0; a < 24; a++)
        for (int e = 1; e <= 5; e++) {
            float yaw = a * 0.2618f, pitch = -e * 0.25f;
            Vec3 d = { sinf(yaw) * cosf(pitch), sinf(pitch), cosf(yaw) * cosf(pitch) };
            float ex = 8.5f, ez = 8.5f, ey = (float)TerrainHeight(8, 8) + 2.6f;
            int hx, hy, hz, px, py, pz; float dist;
            if (!FacetRaycast(w, { ex, ey, ez }, d, 8.0f, hx, hy, hz, px, py, pz, &dist)) continue;
            hits++;
            int adj = abs(px - hx) + abs(py - hy) + abs(pz - hz);
            if (!w.Solid(hx, hy, hz) || w.Solid(px, py, pz) || adj != 1 || dist <= 0 || dist > 8) bad++;
            // The facet it met belongs to that cell's face: within a block of the cell.
            Vec3 q = { ex + d.x * dist, ey + d.y * dist, ez + d.z * dist };
            if (fabsf(q.x - (hx + 0.5f)) > 1.01f || fabsf(q.y - (hy + 0.5f)) > 1.01f || fabsf(q.z - (hz + 0.5f)) > 1.01f) bad++;
        }
    printf("  picking: %d hits of 120 rays, %d wrong\n", hits, bad);
    CHECK(hits > 60 && bad == 0);
    // Straight down onto flat ground: the cell below, and the air above it.
    World f; ResetWorldState(f);
    g_worldGen = WorldGenParams(); g_worldGen.type = GEN_FLAT;
    Stream(f, 8, 8, 60);
    int hx, hy, hz, px, py, pz;
    CHECK(FacetRaycast(f, { 8.5f, 16.0f, 8.5f }, { 0, -1, 0 }, 8.0f, hx, hy, hz, px, py, pz));
    CHECK(hx == 8 && hy == 12 && hz == 8 && px == 8 && py == 13 && pz == 8);
    // From inside the air, never a back face: looking up at open sky, nothing.
    CHECK(!FacetRaycast(f, { 8.5f, 16.0f, 8.5f }, { 0, 1, 0 }, 8.0f, hx, hy, hz, px, py, pz));
    {
        // Collision on faceted ground (M1.7): a one-cell step is a slope you
        // walk up; a two-cell wall stops you. (Shapes are whole cells to the
        // facets until they leave in M1.9.)
        World g; ResetWorldState(g);
        g_loadRadius = 1; g_worldGen = WorldGenParams(); g_worldGen.type = GEN_FLAT;
        Stream(g, 8, 8, 20);
        for (int z = 6; z <= 10; z++) g.Set(10, 13, z, BLOCK_STONE);
        for (int x = 6; x <= 10; x++) { g.Set(x, 13, 11, BLOCK_STONE); g.Set(x, 14, 11, BLOCK_STONE); }
        Player p; p.x = 8.5f; p.z = 8.5f; p.y = 13.0f; p.yaw = 1.5707963f; // facing +X
        for (int i = 0; i < 10; i++) UpdatePlayerPhysics(g, p, 1.0f / 60.0f, false, false, false, false, false);
        CHECK(p.onGround && fabsf(p.y - 13.0f) < FACET_Y);
        for (int i = 0; i < 30; i++) UpdatePlayerPhysics(g, p, 1.0f / 60.0f, true, false, false, false, false);
        CHECK(p.x > 10.0f && p.y > 13.6f); // walked up onto the step
        Player q; q.x = 8.5f; q.z = 8.5f; q.y = 13.0f; q.yaw = 0.0f; // facing +Z, toward the wall
        for (int i = 0; i < 10; i++) UpdatePlayerPhysics(g, q, 1.0f / 60.0f, false, false, false, false, false);
        for (int i = 0; i < 90; i++) UpdatePlayerPhysics(g, q, 1.0f / 60.0f, true, false, false, false, false);
        CHECK(q.z < 11.0f && q.y < 13.0f + 1.0f); // stopped at the two-cell wall, not climbed it
    }
}

// ---- Detail bands (M1.8) ----
static void TestDetailBands() {
    printf("detail bands\n");
    // Levels by distance, with a chunk of hysteresis on the way down.
    CHECK(GroundWantLevel(0, -1, 2) == 2 && GroundWantLevel(1, -1, 2) == 2 && GroundWantLevel(2, -1, 2) == 1);
    CHECK(GroundWantLevel(3, -1, 2) == 1 && GroundWantLevel(4, -1, 2) == 0);
    CHECK(GroundWantLevel(2, 2, 2) == 2);  // just past the edge: keeps its level
    CHECK(GroundWantLevel(3, 2, 2) == 1);  // a chunk further: drops
    CHECK(GroundWantLevel(4, 1, 2) == 1 && GroundWantLevel(5, 1, 2) == 0);
    CHECK(GroundWantLevel(0, -1, 0) == 0 && GroundWantLevel(0, 2, 0) == 0); // off
    // Neighbouring chunks built at different levels meet exactly: every
    // edge chunk (0, y, 0) leaves open along x = 16 is closed by chunk
    // (1, y, 0)'s reverse edge, at each pairing of levels.
    BlockTextureSet t; BuildBlockTextures(VtexSet(), t);
    InitGroundMaterials(t.faceLayer);
    World w; ResetWorldState(w);
    g_worldGen = WorldGenParams(); g_worldGen.type = GEN_WALKGRID; g_worldGen.seed = 5;
    for (int cz = -2; cz <= 2; cz++) for (int cx = -2; cx <= 2; cx++) GenerateColumn(w, cx, cz);
    typedef std::array<int64_t, 3> P3;
    auto meshEdges = [&](int cx, int level, std::map<std::array<int64_t, 6>, int>& edges, int* tris) {
        for (int cy = 0; cy <= 3; cy++) {
            ChunkCoord cc{ cx, cy, 0 };
            GroundCells cells; CopyGroundCells(w, cc, cells);
            GroundMesh m; BuildGroundMesh(cells, level, m);
            *tris += (int)m.idx.size() / 3;
            std::vector<P3> pts;
            for (auto& v : m.verts)
                pts.push_back({ (int64_t)v.x + (int64_t)cc.x * 16 * 2048, (int64_t)v.y + (int64_t)cc.y * 16 * 2048, (int64_t)v.z + (int64_t)cc.z * 16 * 2048 });
            for (size_t i = 0; i < m.idx.size(); i += 3)
                for (int k = 0; k < 3; k++) {
                    P3 a = pts[m.idx[i + k]], b = pts[m.idx[i + (k + 1) % 3]];
                    edges[{ a[0], a[1], a[2], b[0], b[1], b[2] }]++;
                }
        }
    };
    auto world = [](int64_t q) { return q / 2048.0 - 2.0; };
    int trisAt[3] = {};
    for (int la = 0; la <= 2; la++)
        for (int lb = 0; lb <= 2; lb++) {
            std::map<std::array<int64_t, 6>, int> eA, eB;
            int ta = 0, tb = 0;
            meshEdges(0, la, eA, &ta); meshEdges(1, lb, eB, &tb);
            if (la == lb) trisAt[la] = ta;
            int seam = 0, unmatched = 0;
            for (auto& kv : eA) {
                const auto& e = kv.first;
                if (eA.count({ e[3], e[4], e[5], e[0], e[1], e[2] })) continue;
                bool nearSeam = true;
                for (int k : { 0, 3 }) nearSeam &= world(e[k]) > 15.4 && world(e[k]) < 16.6 && world(e[k + 2]) > 1.0 && world(e[k + 2]) < 15.0;
                if (!nearSeam) continue;
                seam++;
                if (!eB.count({ e[3], e[4], e[5], e[0], e[1], e[2] })) unmatched++;
            }
            CHECK(seam > 0 && unmatched == 0);
            if (unmatched) printf("  levels %d|%d: %d of %d seam edges unmatched\n", la, lb, unmatched, seam);
        }
    printf("  triangles in 4 chunks by level: %d, %d, %d\n", trisAt[0], trisAt[1], trisAt[2]);
    CHECK(trisAt[0] < trisAt[1] && trisAt[1] < trisAt[2]);
}

// ---- The faceted ground (facetmesh.h, M1.2) ----
// A test world: a lumpy blob of three materials with a dug pit, inside a
// box of air. Returns the cells (index (y * nz + z) * nx + x).
static std::vector<uint8_t> FacetTestCells(int n, uint32_t seed) {
    std::vector<uint8_t> c((size_t)n * n * n, 0);
    for (int y = 0; y < n; y++)
        for (int z = 0; z < n; z++)
            for (int x = 0; x < n; x++) {
                if (x < 3 || y < 3 || z < 3 || x >= n - 3 || y >= n - 3 || z >= n - 3) continue;
                uint32_t h = (uint32_t)(x * 73856093) ^ (uint32_t)(y * 19349663) ^ (uint32_t)(z * 83492791) ^ seed;
                h ^= h >> 13; h *= 0x5bd1e995u; h ^= h >> 15;
                float ground = n * 0.5f + 2.5f * sinf(x * 0.5f) + 2.0f * cosf(z * 0.4f + x * 0.1f);
                bool pit = x > n / 2 - 2 && x < n / 2 + 2 && z > n / 2 - 2 && z < n / 2 + 2 && y > ground - 4;
                bool speck = (h & 31) == 0 && y < ground + 1; // loose single cells, diagonal contacts included
                if ((y < ground && !pit) || speck) c[((size_t)y * n + z) * n + x] = (uint8_t)(1 + (h >> 20) % 3);
            }
    return c;
}
static int FacetTestBand(Vec3 p, void* user) {
    // Bands that change every few cells, so every stitch case appears.
    (void)user;
    int k = ((int)floorf(p.x / 3.0f) + 2 * (int)floorf(p.z / 5.0f) + (int)floorf(p.y / 4.0f)) % 3;
    return k < 0 ? k + 3 : k;
}
// Watertight and consistently wound: every directed edge a->b (by exact
// position) is matched by exactly as many b->a. A crack or a T-junction
// between detail levels leaves an edge unmatched.
static bool FacetWatertight(const FacetMesh& m, int* unmatched) {
    std::map<std::array<uint32_t, 6>, int> e;
    auto key = [&](uint32_t a, uint32_t b) {
        std::array<uint32_t, 6> k;
        memcpy(&k[0], &m.verts[a].pos, 12); memcpy(&k[3], &m.verts[b].pos, 12);
        return k;
    };
    for (size_t t = 0; t < m.idx.size(); t += 3)
        for (int i = 0; i < 3; i++) {
            uint32_t a = m.idx[t + i], b = m.idx[t + (i + 1) % 3];
            e[key(a, b)]++;
        }
    int bad = 0;
    for (auto& kv : e) {
        std::array<uint32_t, 6> r = { kv.first[3], kv.first[4], kv.first[5], kv.first[0], kv.first[1], kv.first[2] };
        auto it = e.find(r);
        if (it == e.end() || it->second != kv.second) bad++;
    }
    *unmatched = bad;
    return bad == 0;
}
static void TestFacets() {
    printf("faceted ground\n");
    const int n = 24;
    FacetMaterial mats[256];
    mats[1] = { true, 0.08f }; mats[2] = { true, 0.0f }; mats[3] = { true, 0.03f };
    std::vector<uint8_t> cells = FacetTestCells(n, 7);
    FacetGrid g; g.nx = g.ny = g.nz = n; g.cells = cells.data(); g.mats = mats;
    // Flat ground stays level before the jitter: every corner of a plane at
    // y = 5 sits at y = 5 when the jitter is off.
    {
        std::vector<uint8_t> flat((size_t)n * n * n, 0);
        for (int y = 0; y < 5; y++) for (int z = 0; z < n; z++) for (int x = 0; x < n; x++) flat[((size_t)y * n + z) * n + x] = 2;
        FacetGrid fg = g; fg.cells = flat.data();
        FacetShape s; s.jitter = 0; s.jitterSide = 0;
        Vec3 c; bool on = FacetCorner(fg, s, 10, 5, 10, &c);
        CHECK(on && c.y == 5.0f && c.x == 10.0f);
        CHECK(!FacetCorner(fg, s, 10, 8, 10, &c)); // in the air: no corner
        FacetBuildParams bp; bp.bx0 = bp.by0 = bp.bz0 = 2; bp.bx1 = bp.by1 = bp.bz1 = n - 2;
        FacetMesh fm; FacetBuild(fg, bp, fm);
        int up = 0, tris = fm.triangles();
        for (size_t t = 0; t < fm.idx.size(); t += 3) {
            Vec3 a = fm.verts[fm.idx[t]].pos, b = fm.verts[fm.idx[t + 1]].pos, cc = fm.verts[fm.idx[t + 2]].pos;
            if (Cross(b - a, cc - a).y > 0) up++;
        }
        CHECK(tris > 0 && up == tris); // wound out of the ground
    }
    // Watertight at every mix of detail levels, whole and cut into boxes.
    FacetBuildParams bp; bp.bx0 = bp.by0 = bp.bz0 = 0; bp.bx1 = bp.by1 = bp.bz1 = n;
    bp.band = FacetTestBand;
    FacetMesh whole; FacetBuild(g, bp, whole);
    int bad = 0;
    CHECK(FacetWatertight(whole, &bad));
    if (bad) printf("  %d unmatched edges\n", bad);
    CHECK(whole.quadsAtLevel[0] > 0 && whole.quadsAtLevel[1] > 0 && whole.quadsAtLevel[2] > 0);
    bp.selective = false;
    FacetMesh blanket; FacetBuild(g, bp, blanket);
    CHECK(FacetWatertight(blanket, &bad));
    CHECK(blanket.baseQuads == whole.baseQuads);
    {   // One smooth material: only creases are cut, so it costs less.
        std::vector<uint8_t> smooth = cells;
        for (auto& c : smooth) if (c) c = 2;
        FacetGrid sg = g; sg.cells = smooth.data();
        FacetMesh sel, all;
        FacetBuildParams sp = bp; FacetBuild(sg, sp, all);
        sp.selective = true; FacetBuild(sg, sp, sel);
        CHECK(all.triangles() > sel.triangles());
        CHECK(FacetWatertight(sel, &bad));
    }
    bp.selective = true;
    // Cut into eight boxes (as chunks are): together, the same triangles.
    std::vector<std::array<float, 9>> a, b;
    auto collect = [](const FacetMesh& m, std::vector<std::array<float, 9>>& out) {
        for (size_t t = 0; t < m.idx.size(); t += 3) {
            std::array<float, 9> tri;
            for (int i = 0; i < 3; i++) { Vec3 q = m.verts[m.idx[t + i]].pos; tri[i * 3] = q.x; tri[i * 3 + 1] = q.y; tri[i * 3 + 2] = q.z; }
            out.push_back(tri);
        }
    };
    collect(whole, a);
    FacetMesh parts;
    for (int k = 0; k < 8; k++) {
        FacetBuildParams q = bp;
        q.bx0 = (k & 1) ? n / 2 : 0; q.bx1 = (k & 1) ? n : n / 2;
        q.by0 = (k & 2) ? n / 2 : 0; q.by1 = (k & 2) ? n : n / 2;
        q.bz0 = (k & 4) ? n / 2 : 0; q.bz1 = (k & 4) ? n : n / 2;
        FacetMesh m; FacetBuild(g, q, m);
        collect(m, b);
        for (size_t i = 0; i < m.idx.size(); i++) parts.idx.push_back(m.idx[i] + (uint32_t)parts.verts.size());
        parts.verts.insert(parts.verts.end(), m.verts.begin(), m.verts.end());
    }
    std::sort(a.begin(), a.end()); std::sort(b.begin(), b.end());
    CHECK(a == b);
    CHECK(FacetWatertight(parts, &bad));
    // Stable: the same cells give the same mesh.
    FacetMesh again; FacetBuild(g, bp, again);
    CHECK(again.verts.size() == whole.verts.size() && again.idx == whole.idx);
    CHECK(memcmp(again.verts.data(), whole.verts.data(), whole.verts.size() * sizeof(FacetVertex)) == 0);
    // Weights sum to 255 and materials are real.
    bool okW = true;
    for (auto& v : whole.verts) okW &= (v.w[0] + v.w[1] + v.w[2] == 255) && v.mat[0] >= 1 && v.mat[0] <= 3;
    CHECK(okW);
}

// The string table (M1.10, D26): the file format, English under a
// language, {n} slots in any order, missing keys visible, UTF-8 both ways,
// and the shipped en.txt itself.
static void TestStringTable() {
    printf("string table\n");
    std::unordered_map<std::string, std::string> t;
    std::vector<std::string> errs;
    ParseStrings("\xEF\xBB\xBF# a comment\n  a.b = HELLO  \r\nbad line\nc = X\\nY\n\nBad.Key = no\nd.e=\n", "t.txt", t, errs);
    CHECK(t.size() == 3 && t["a.b"] == "HELLO" && t["c"] == "X\nY" && t.count("d.e") && t["d.e"].empty());
    CHECK(errs.size() == 2 && errs[0] == "t.txt:3: expected `key = text`" && errs[1].rfind("t.txt:6:", 0) == 0);
    SetStrings({ { "k", "{1} THEN {0}" }, { "one", "A{0}B{0}" }, { "odd", "{x} {5} {" } });
    CHECK(StrF("k", { "ONE", "TWO" }) == "TWO THEN ONE");
    CHECK(StrF("one", { "-" }) == "A-B-");
    CHECK(StrF("odd", { "Z" }) == "{x} {5} {"); // not slots, or no such argument: left as written
    CHECK(Str("missing.key") == "[missing.key]" && !HasStr("missing.key") && HasStr("k"));
    const std::string& a = Str("missing.key"); Str("another"); CHECK(a == "[missing.key]"); // stays valid
    // UTF-8: two, three and four bytes, round trip through UTF-16.
    std::string u = "A\xC3\xA9\xE2\x82\xAC\xF0\x9F\x8C\xB2"; // A, e-acute, euro sign, a tree
    size_t i = 0; std::vector<uint32_t> cps;
    while (i < u.size()) cps.push_back(DecodeUtf8(u, i));
    CHECK((cps == std::vector<uint32_t>{ 0x41, 0xE9, 0x20AC, 0x1F332 }));
    std::wstring w = Utf8ToWide(u);
    CHECK(WideToUtf8(w) == u);
    std::string back; for (uint32_t c : cps) AppendUtf8(back, c);
    CHECK(back == u);
    i = 0; std::string broken = "\xC3("; CHECK(DecodeUtf8(broken, i) == 0xFFFD && i == 1);
    SetStrings({ { "x", "\xC3\xA9T\xC3\xA9" }, { "font", "\xE2\x82\xAC" } });
    CHECK((StringCodepoints() == std::vector<uint32_t>{ 'T', 0xE9 })); // sorted, no repeats, not the font's name
    // The shipped English: loads clean, a language over it falls back to it.
    std::vector<std::string> problems;
    CHECK(LoadStrings("../assets/text", "en", problems) && problems.empty());
    CHECK(Str("title.new_game") == "NEW GAME" && Str("font") == "Consolas");
    CHECK(StrF("keys.row", { "JUMP", "Space" }) == "JUMP: [Space]");
    bool plain = true; // English needs nothing beyond ASCII and Latin-1 (the atlas's fixed part)
    for (uint32_t c : StringCodepoints()) plain &= c < 256;
    CHECK(plain);
    problems.clear();
    CHECK(LoadStrings("../assets/text", "no_such_language", problems) && problems.size() == 1); // English loaded; the missing file noted
    CHECK(Str("title.quit") == "QUIT"); // English still there under the missing language
    SetStrings({});
}

// Draw only what can be seen (M1.11, DESIGN.md 23.7): chunk openings, the
// visibility walk -- which must never hide a chunk a ray from the camera
// can reach -- and the heading-aware load order.
static void TestHiddenChunks() {
    printf("hidden chunks, load order\n");
    // Openings of a 16^3 cube of cells in a padded grid.
    FacetMaterial mats[256]; mats[1].solid = true;
    const int N = 20, P = 2;
    std::vector<uint8_t> cells((size_t)N * N * N, 0);
    FacetGrid g; g.nx = g.ny = g.nz = N; g.cells = cells.data(); g.mats = mats;
    auto fill = [&](int x0, int y0, int z0, int x1, int y1, int z1, uint8_t id) {
        for (int y = y0; y < y1; y++) for (int z = z0; z < z1; z++) for (int x = x0; x < x1; x++)
            cells[((size_t)(y + P) * N + (z + P)) * N + (x + P)] = id;
    };
    CHECK(FacetOpenings(g, P, P, P, 16) == FACET_ALL_OPEN);
    fill(0, 0, 0, 16, 16, 16, 1);
    CHECK(FacetOpenings(g, P, P, P, 16) == 0);
    fill(0, 7, 7, 16, 9, 9, 0); // a tunnel along x
    uint16_t o = FacetOpenings(g, P, P, P, 16);
    CHECK(o == (1u << FacetPairBit(0, 1)) && FacetFacesSee(o, 1, 0) && !FacetFacesSee(o, 0, 2));
    fill(0, 0, 0, 16, 16, 16, 0); fill(0, 0, 0, 16, 8, 16, 1); // ground filling the lower half
    o = FacetOpenings(g, P, P, P, 16);
    CHECK(!FacetFacesSee(o, 2, 3) && FacetFacesSee(o, 3, 0) && FacetFacesSee(o, 0, 1) && FacetFacesSee(o, 4, 5));
    fill(0, 0, 0, 16, 16, 16, 0); fill(0, 0, 0, 16, 16, 1, 1); fill(0, 0, 15, 16, 16, 16, 1); // walls at both z sides
    o = FacetOpenings(g, P, P, P, 16);
    CHECK(!FacetFacesSee(o, 4, 5) && !FacetFacesSee(o, 4, 0) && FacetFacesSee(o, 0, 1) && FacetFacesSee(o, 2, 3));
    int bits = 0; for (int a = 0; a < 6; a++) for (int b = a + 1; b < 6; b++) bits |= 1 << FacetPairBit(a, b);
    CHECK(bits == FACET_ALL_OPEN); // 15 pairs, 15 distinct bits

    // The walk on real ground: hills, with a sealed tunnel dug under them.
    BlockTextureSet t; BuildBlockTextures(VtexSet(), t);
    InitGroundMaterials(t.faceLayer);
    World w; ResetWorldState(w);
    g_worldGen = WorldGenParams(); g_worldGen.type = GEN_WALKGRID; g_worldGen.seed = 3;
    const int R = 4;
    for (int cz = -R - 1; cz <= R + 1; cz++) for (int cx = -R - 1; cx <= R + 1; cx++) GenerateColumn(w, cx, cz);
    int floorY = 1000; // the lowest ground top in the middle columns
    for (int z = -8; z < 24; z++) for (int x = -8; x < 24; x++) {
        int y = 255; while (y > 0 && !w.Solid(x, y, z)) y--;
        floorY = std::min(floorY, y);
    }
    int tunnelY = floorY - 20; // well under the ground, sealed at both ends
    CHECK(tunnelY > 4);
    for (int x = -8; x < 24; x++) for (int y = tunnelY; y < tunnelY + 3; y++) for (int z = 6; z < 9; z++) w.Set(x, y, z, BLOCK_AIR);
    std::unordered_map<long long, uint16_t> open;
    auto ckey = [](const ChunkCoord& c) { return ((long long)(c.x + 512) << 32) | ((long long)(c.z + 512) << 12) | (long long)c.y; };
    double openMs = 0;
    for (auto& kv : w.chunks) {
        GroundCells gc; CopyGroundCells(w, kv.first, gc);
        FacetGrid fg; fg.nx = fg.ny = fg.nz = GROUND_GRID; fg.cells = gc.cells; fg.mats = g_groundFacet;
        auto t0 = std::chrono::steady_clock::now();
        open[ckey(kv.first)] = FacetOpenings(fg, FACET_PAD, FACET_PAD, FACET_PAD, CHUNK_SIZE);
        openMs += std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    }
    printf("  openings: %.1f us a chunk on average (job threads; this machine, -O1)\n", 1000.0 * openMs / w.chunks.size());
    struct Ctx { std::unordered_map<long long, uint16_t>* open; decltype(ckey)* key; };
    Ctx ctx{ &open, &ckey };
    auto openings = [](const ChunkCoord& cc, void* u) -> int {
        Ctx* c = (Ctx*)u; auto it = c->open->find((*c->key)(cc));
        return it == c->open->end() ? -1 : (int)it->second;
    };
    auto always = [](const ChunkCoord&, void*) { return true; };
    int rows = 0; for (auto& kv : w.chunks) rows = std::max(rows, kv.first.y + 2); // capped as render.cpp caps it
    // Rays from the eye: the chunk of the first ground each ray meets, and
    // of the air just before it, must both be reached by the walk.
    auto rayCheck = [&](float ex, float ey, float ez, const std::vector<ChunkCoord>& seen, int& rays, int& missed) {
        std::unordered_map<long long, int> in;
        for (auto& c : seen) in[ckey(c)] = 1;
        uint32_t seed = 12345;
        auto rnd = [&]() { seed = seed * 1664525u + 1013904223u; return (seed >> 8) / 16777216.0f; };
        for (int i = 0; i < 3000; i++) {
            float u = rnd() * 2 - 1, a = rnd() * 6.2831853f, r = sqrtf(1 - u * u);
            float dx = r * cosf(a), dy = u, dz = r * sinf(a);
            float x = ex, y = ey, z = ez;
            int px = (int)floorf(x), py = (int)floorf(y), pz = (int)floorf(z);
            for (float d = 0; d < R * 16.0f; d += 0.05f) {
                x = ex + dx * d; y = ey + dy * d; z = ez + dz * d;
                if (y < 0 || y > 255) break;
                int cx = (int)floorf(x), cy = (int)floorf(y), cz = (int)floorf(z);
                if (std::abs(FloorDiv16(cx)) > R || std::abs(FloorDiv16(cz)) > R) break;
                if (w.Solid(cx, cy, cz)) {
                    rays++;
                    ChunkCoord hit{ FloorDiv16(cx), FloorDiv16(cy), FloorDiv16(cz) }, before{ FloorDiv16(px), FloorDiv16(py), FloorDiv16(pz) };
                    if (!in.count(ckey(hit)) || !in.count(ckey(before))) missed++;
                    break;
                }
                px = cx; py = cy; pz = cz;
            }
        }
    };
    std::vector<ChunkCoord> seen;
    int resident = (int)w.chunks.size();
    // On the surface: the tunnel's chunks are sealed off, and nothing a ray hits is missed.
    float sx = 8.5f, sz = 8.5f; int sy = 255; while (!w.Solid(8, sy, 8)) sy--;
    GroundVisibleChunks({ 0, FloorDiv16(sy + 2), 0 }, R, rows, openings, always, &ctx, seen);
    int reachedResident = 0; for (auto& c : seen) reachedResident += open.count(ckey(c)) ? 1 : 0;
    bool tunnelHidden = true;
    for (auto& c : seen)
        if (c.z == 0 && c.x >= -1 && c.x <= 1 && (c.y == FloorDiv16(tunnelY) || c.y == FloorDiv16(tunnelY + 2))) tunnelHidden = false;
    int rays = 0, missed = 0;
    rayCheck(sx, sy + 2.6f, sz, seen, rays, missed);
    printf("  from the surface: %d of %d resident chunks reached; %d rays met ground, %d missed\n", reachedResident, resident, rays, missed);
    CHECK(missed == 0 && rays > 1000);
    CHECK(tunnelHidden); // no chunk of the sealed tunnel is reached from above
    // In the tunnel: its chunks are reached; the surface far above isn't all.
    GroundVisibleChunks({ 0, FloorDiv16(tunnelY + 1), 0 }, R, rows, openings, always, &ctx, seen);
    int fromTunnel = 0; for (auto& c : seen) fromTunnel += open.count(ckey(c)) ? 1 : 0;
    rays = 0; missed = 0;
    rayCheck(0.5f, tunnelY + 1.6f, 7.5f, seen, rays, missed);
    printf("  from the tunnel: %d of %d resident chunks reached; %d rays met ground, %d missed\n", fromTunnel, resident, rays, missed);
    CHECK(missed == 0 && rays > 1000 && fromTunnel < reachedResident);
    // A frustum-like test: only chunks with x >= the camera's are in view.
    auto ahead = [](const ChunkCoord& cc, void*) { return cc.x >= 0; };
    GroundVisibleChunks({ 0, FloorDiv16(sy + 2), 0 }, R, rows, openings, ahead, &ctx, seen);
    bool allAhead = true; for (auto& c : seen) allAhead = allAhead && c.x >= 0;
    CHECK(allAhead && !seen.empty());
    // Above the world's top: starts from the top row, still sees the ground.
    GroundVisibleChunks({ 0, rows + 5, 0 }, R, rows, openings, always, &ctx, seen);
    int fromSky = 0; for (auto& c : seen) fromSky += open.count(ckey(c)) ? 1 : 0;
    CHECK(fromSky > 0);
    // Cost: the walk from the surface at the default distance, all in view.
    {
        const int RD = 12;
        auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < 20; i++) GroundVisibleChunks({ 0, FloorDiv16(sy + 2), 0 }, RD + 1, rows, openings, always, &ctx, seen);
        double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count() / 20;
        printf("  walk at render distance %d, everything in view: %zu chunks, %.3f ms (this machine, -O1)\n", RD, seen.size(), ms);
    }

    // Load order: the player's own column and its neighbours first; ahead
    // before behind; no heading, plain distance.
    CHECK(ColumnLoadOrder(0, 0, 1, 0) < ColumnLoadOrder(1, 1, 1, 0));
    CHECK(ColumnLoadOrder(-1, 1, 1, 0) < ColumnLoadOrder(3, 0, 1, 0)); // a neighbour behind beats ground ahead
    CHECK(ColumnLoadOrder(8, 0, 1, 0) < ColumnLoadOrder(-5, 0, 1, 0)); // 8 ahead before 5 behind
    CHECK(ColumnLoadOrder(0, 6, 0, 0) == ColumnLoadOrder(-6, 0, 0, 0));
    // The queue follows it: heading east, the first columns queued beyond
    // the nearest ring are east of the player; turning west re-orders them.
    ResetWorldState(w);
    g_worldGen = WorldGenParams(); g_worldGen.type = GEN_FLAT;
    int savedRadius = g_loadRadius; g_loadRadius = 6;
    EnsureChunksLoaded(0, 0, 1.0f, 0.0f);
    // The next 60 after the nearest ring lean well east: on average two
    // columns east of the player (a plain distance order averages 0).
    auto meanX = [] { double m = 0; for (size_t i = 9; i < 69; i++) m += g_pendingColumns[i].first; return m / 60; };
    CHECK(g_pendingColumns.size() == 15 * 15 && meanX() > 1.5);
    bool nearFirst = true; for (size_t i = 0; i < 9; i++) nearFirst = nearFirst && std::abs(g_pendingColumns[i].first) <= 1 && std::abs(g_pendingColumns[i].second) <= 1;
    CHECK(nearFirst);
    EnsureChunksLoaded(0, 0, -1.0f, 0.0f); // same chunk, turned round
    CHECK(meanX() < -1.5);
    g_loadRadius = savedRadius;
    ResetWorldState(w);
}

int main() {
    TestVtex();
    TestBlockTextures();
    TestSaveRoundTrip();
    TestStreaming();
    TestPatchwork();
    TestPlayer();
    TestMovement();
    TestIcons();
    TestScheduledUpdates();
    TestLibrary();
    TestMusicLevel();
    TestGlowLight();
    TestSky();
    TestCameraRelative();
    TestMusicHarmony();
    TestSoundPalette();
    TestSoundscape();
    TestFacets();
    TestHills();
    TestJobs();
    TestGroundMesh();
    TestSkyLight();
    TestFacetCollision();
    TestDetailBands();
    TestStringTable();
    TestHiddenChunks();
    printf("\n%d checks, %d failed\n", g_checks, g_failures);
    return g_failures ? 1 : 0;
}
