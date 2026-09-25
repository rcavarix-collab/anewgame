// world.cpp
//
// Implementations for world.h: the Chunk destructor (needs the real
// <d3d11.h> for ->Release(), which world.h itself deliberately avoids
// including), gravity, terrain generation/column loading, player
// physics, and the DDA raycast.

#ifndef NOMINMAX // also set project-wide (walkgrid.vcxproj)
#define NOMINMAX // see render.cpp for why this precedes windows.h (pulled in transitively via d3d11.h here)
#endif
#include "world.h"
#include "terrain.h"
#include "jobs.h"
#include "collide.h"
#include <d3d11.h>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <cstring>
#include <memory>
#include <queue>
#include <windows.h> // QueryPerformanceCounter for new-world seeds

Chunk::~Chunk() {
    if (vb) vb->Release();
    if (ib) ib->Release();
}

uint64_t g_chunkVersionCounter = 0;
int g_loadRadius = 3; // chunks, horizontal only (Section 2.4); a Graphics Settings slider now [1,8]

World g_world;
Player g_player;
float g_dayTimeSeconds = 0.0f;

// =======================================================================
// Part V - Scheduled block updates
// =======================================================================

uint32_t g_worldTick = 0;

namespace {
struct LaterFirst {
    bool operator()(const ScheduledUpdate& a, const ScheduledUpdate& b) const {
        if (a.due != b.due) return (int32_t)(a.due - b.due) > 0; // wrap-safe
        return (int32_t)(a.seq - b.seq) > 0;
    }
};
std::priority_queue<ScheduledUpdate, std::vector<ScheduledUpdate>, LaterFirst> g_updates;
uint32_t g_updateSeq = 0;
// Pending updates per column, kept in step with the queue so eviction can
// ask "is something still changing here?" without scanning it.
std::unordered_map<long long, int> g_updatesPerColumn;

void ForgetColumnUpdate(int x, int z) {
    auto it = g_updatesPerColumn.find(ColumnKey(FloorDiv16(x), FloorDiv16(z)));
    if (it != g_updatesPerColumn.end() && --it->second <= 0) g_updatesPerColumn.erase(it);
}

// ---- Handlers, one per UpdateKind ----

// Gravity: fall one cell if still unsupported, then re-check what was
// resting on top and whether this block keeps falling -- both a tick
// later, so a column of blocks comes down one cell per tick, visibly.
void UpdateGravity(World& w, int x, int y, int z) {
    BlockID id = w.Get(x, y, z);
    if (id == BLOCK_AIR || g_blocks[id].foundational) return; // stale entry
    if (y - 1 < Y_MIN) return;
    if (w.Solid(x, y - 1, z)) return; // became supported since queued

    // The state byte travels with the block. (A per-block data record
    // wouldn't -- every block that has one is foundational today, so none
    // can fall; a falling data block would need its record moved.)
    w.SetRaw(x, y - 1, z, id, w.GetState(x, y, z));
    w.SetRaw(x, y, z, BLOCK_AIR);

    MaybeQueueFall(w, x, y + 1, z, 1); // whatever was resting on top
    MaybeQueueFall(w, x, y - 1, z, 1); // keep falling if still unsupported
}

// Grass cover: grass that's been cut off from the sky since its check was
// queued turns to dirt. Uncovered in the meantime, it lives.
void UpdateGrassCover(World& w, int x, int y, int z) {
    if (w.Get(x, y, z) != BLOCK_MEADOW_GRASS) return; // stale entry
    if (OpenToSky(w, x, y, z)) return;
    w.SetRaw(x, y, z, BLOCK_DIRT);
}

using UpdateHandler = void (*)(World&, int, int, int);
const UpdateHandler kHandlers[UPD_KIND_COUNT] = {
    UpdateGravity,
    UpdateGrassCover,
};
} // namespace

void ScheduleUpdate(int x, int y, int z, UpdateKind kind, uint32_t delayTicks) {
    g_updates.push({ x, y, z, g_worldTick + delayTicks, g_updateSeq++, kind });
    g_updatesPerColumn[ColumnKey(FloorDiv16(x), FloorDiv16(z))]++;
}

void ProcessScheduledUpdates(World& w) {
    int n = 0;
    while (!g_updates.empty() && n < MAX_UPDATES_PER_TICK) {
        ScheduledUpdate u = g_updates.top();
        if ((int32_t)(u.due - g_worldTick) > 0) break; // nothing else is due yet
        g_updates.pop();
        ForgetColumnUpdate(u.x, u.z);
        if (u.kind < UPD_KIND_COUNT) kHandlers[u.kind](w, u.x, u.y, u.z);
        n++;
    }
    g_worldTick++;
}

void ClearScheduledUpdates() {
    g_updates = decltype(g_updates)();
    g_updatesPerColumn.clear();
    g_worldTick = 0;
    g_updateSeq = 0;
}

size_t ScheduledUpdateCount() { return g_updates.size(); }

std::vector<PendingUpdate> SnapshotScheduledUpdates() {
    std::vector<PendingUpdate> out;
    auto copy = g_updates; // small in practice: only what's changing right now
    while (!copy.empty()) {
        const ScheduledUpdate& u = copy.top();
        int32_t d = (int32_t)(u.due - g_worldTick);
        out.push_back({ u.x, u.y, u.z, u.kind, (uint32_t)(d > 0 ? d : 0) });
        copy.pop();
    }
    return out;
}

void RestoreScheduledUpdates(const std::vector<PendingUpdate>& updates) {
    for (const PendingUpdate& u : updates)
        if (u.kind < UPD_KIND_COUNT) ScheduleUpdate(u.x, u.y, u.z, u.kind, u.delay);
}

void MaybeQueueFall(World& w, int x, int y, int z, uint32_t delayTicks) {
    if (y < Y_MIN || y > Y_MAX) return;
    BlockID id = w.Get(x, y, z);
    if (id == BLOCK_AIR) return;
    if (g_blocks[id].foundational) return;
    if (!g_blocks[id].solid) return;
    if (y - 1 < Y_MIN) return;         // resting on the world floor
    if (w.Solid(x, y - 1, z)) return;  // supported
    ScheduleUpdate(x, y, z, UPD_GRAVITY, delayTicks);
}

bool OpenToSky(World& w, int x, int y, int z) {
    for (int yy = y + 1; yy <= Y_MAX && yy <= y + 64; yy++)
        if (BlockShadesGrass(w.Get(x, yy, z))) return false;
    return true;
}

void LiveEdit(World& w, int x, int y, int z, BlockID id, uint8_t state) {
    // Falling ground and grass die-back are parked (D12, M1.9): nothing is
    // scheduled. To bring them back, queue MaybeQueueFall(w, x, y + 1, z)
    // here and the grass-cover check for the first grass below (git history
    // before M1.9 has the code).
    w.Set(x, y, z, id, state);
}

// =======================================================================
// World generation and chunk loading
// =======================================================================

// Generators (Section 2.5). Each (type, version) must keep producing
// exactly the same terrain forever, because saves store only what the
// player changed and regenerate the rest -- so an output-changing edit
// to a generator means a new version alongside the old one, not an
// in-place change.
WorldGenParams g_worldGen;

const char* WorldGenName(WorldGenType t) {
    switch (t) {
    case GEN_WALKGRID: return "walkgrid-hills";
    case GEN_FLAT: return "flat";
    default: return "?";
    }
}
bool WorldGenFromName(const char* name, WorldGenType& out) {
    for (int t = 0; t < GEN_TYPE_COUNT; t++)
        if (strcmp(name, WorldGenName((WorldGenType)t)) == 0) { out = (WorldGenType)t; return true; }
    return false;
}
uint32_t WorldGenLatestVersion(WorldGenType t) {
    switch (t) {
    case GEN_WALKGRID: return 1;
    case GEN_FLAT: return 2; // v2: the surface is a patchwork of three grounds (SurfaceBlockAt)
    default: return 0;
    }
}

// New worlds: walkgrid-hills (M1.4). Existing worlds keep the generator
// they were made with, since it's stored in each save.
WorldGenParams DefaultNewWorldGen() {
    WorldGenParams p;
    p.type = GEN_WALKGRID;
    p.version = WorldGenLatestVersion(p.type);
    // Mixed from the clock so worlds differ.
    LARGE_INTEGER t; QueryPerformanceCounter(&t);
    uint64_t x = (uint64_t)t.QuadPart * 0x9E3779B97F4A7C15ull;
    x ^= x >> 31; x *= 0xBF58476D1CE4E5B9ull; x ^= x >> 29;
    p.seed = x;
    return p;
}

static const int FLAT_V1_HEIGHT = 12;

int TerrainHeight(const WorldGenParams& gen, int wx, int wz) {
    if (gen.type == GEN_FLAT) return FLAT_V1_HEIGHT;
    return HillsHeight(gen.seed, wx, wz);
}
int TerrainHeight(int wx, int wz) { return TerrainHeight(g_worldGen, wx, wz); }

std::unordered_set<long long> g_residentColumns;
std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> g_evictedChunks;

long long ColumnKey(int cx, int cz) {
    return ((long long)(uint32_t)cx << 32) | (uint32_t)cz;
}
static void DecodeColumnKey(long long key, int& cx, int& cz) {
    cx = (int)(uint32_t)((uint64_t)key >> 32);
    cz = (int)(uint32_t)((uint64_t)key & 0xFFFFFFFFu);
}

int ColumnDistance(int cx, int cz, int playerChunkX, int playerChunkZ) {
    long long adx = (long long)cx - playerChunkX; if (adx < 0) adx = -adx;
    long long adz = (long long)cz - playerChunkZ; if (adz < 0) adz = -adz;
    long long d = adx > adz ? adx : adz;
    return d > INT32_MAX ? INT32_MAX : (int)d;
}

static const int COLUMN_CHUNKS = Y_MAX / CHUNK_SIZE + 1;

// ---- Column tops (DESIGN.md 23.4) ----
int World::Top(int x, int z) const {
    int cx = FloorDiv16(x), cz = FloorDiv16(z);
    auto it = columnTops.find(ColumnKey(cx, cz));
    if (it == columnTops.end()) return -1;
    return it->second[LocalOf(z, cz) * CHUNK_SIZE + LocalOf(x, cx)];
}
void World::NoteCell(int x, int y, int z, bool solid) {
    int cx = FloorDiv16(x), cz = FloorDiv16(z);
    auto it = columnTops.find(ColumnKey(cx, cz));
    if (it == columnTops.end()) return;
    int16_t& t = it->second[LocalOf(z, cz) * CHUNK_SIZE + LocalOf(x, cx)];
    if (solid) { if (y > t) t = (int16_t)y; return; }
    if (y != t) return;
    // The top was taken: look down for the next solid cell.
    int ny = y - 1;
    while (ny >= Y_MIN && !Solid(x, ny, z)) ny--;
    t = (int16_t)(ny >= Y_MIN ? ny : -1);
}
void World::ComputeColumnTops(int cx, int cz) {
    auto& tops = columnTops[ColumnKey(cx, cz)];
    tops.fill(-1);
    int left = CHUNK_SIZE * CHUNK_SIZE;
    for (int cy = COLUMN_CHUNKS - 1; cy >= 0 && left > 0; cy--) {
        Chunk* c = FindChunk({ cx, cy, cz });
        if (!c) continue;
        for (int lz = 0; lz < CHUNK_SIZE; lz++)
            for (int lx = 0; lx < CHUNK_SIZE; lx++) {
                int16_t& t = tops[lz * CHUNK_SIZE + lx];
                if (t >= 0) continue;
                for (int ly = CHUNK_SIZE - 1; ly >= 0; ly--)
                    if (BlockSolid((BlockID)c->blocks[Chunk::LocalIndex(lx, ly, lz)])) { t = (int16_t)(cy * CHUNK_SIZE + ly); left--; break; }
            }
    }
}

bool ColumnNeighborhoodResident(int cx, int cz) {
    for (int dz = -1; dz <= 1; dz++)
        for (int dx = -1; dx <= 1; dx++)
            if (!g_residentColumns.count(ColumnKey(cx + dx, cz + dz))) return false;
    return true;
}

// A column arriving changes what the meshes of every chunk in the 3x3
// columns around it should be (face culling across shared faces,
// ambient occlusion across edges and corners) -- and may be the last
// neighbour a waiting chunk needed before it can be meshed at all.
static void MarkColumnNeighborhoodDirty(World& w, int cx, int cz) {
    for (int dz = -1; dz <= 1; dz++)
        for (int dx = -1; dx <= 1; dx++)
            for (int cy = 0; cy < COLUMN_CHUNKS; cy++)
                w.MarkChunkDirty({ cx + dx, cy, cz + dz });
}

// Evicting frees unmodified chunks outright (the generator rebuilds them
// bit-for-bit on return) and keeps only modified ones, minus their GPU
// buffers, in g_evictedChunks -- so memory held for places the player
// has left scales with what they changed there, not with distance
// walked.
static void EvictColumnFromWorld(World& w, int cx, int cz) {
    w.columnTops.erase(ColumnKey(cx, cz));
    for (int cy = 0; cy < COLUMN_CHUNKS; cy++) {
        ChunkCoord cc{ cx, cy, cz };
        std::unique_ptr<Chunk> c = w.TakeChunk(cc);
        if (!c || !c->modified) continue; // unmodified: destroyed here
        if (c->vb) { c->vb->Release(); c->vb = nullptr; }
        if (c->ib) { c->ib->Release(); c->ib = nullptr; }
        c->indexCount = 0; c->opaqueIndexCount = 0;
        c->dirty = true;
        g_evictedChunks[cc] = std::move(c);
    }
}

// A column with updates still pending isn't evicted: gravity only ever
// moves a block straight down within its own column, and evicting it
// mid-cascade would make every remaining entry read air and be dropped
// as stale, leaving the rest of the structure floating on return.
// (Long-delay updates, e.g. machine timers, will want to travel with
// their chunk instead -- see DESIGN.md 5.4.)
static bool ColumnHasPendingUpdates(int cx, int cz) {
    return g_updatesPerColumn.count(ColumnKey(cx, cz)) != 0;
}

static inline BlockID TerrainBlockAt(int wy, int surface) {
    if (wy == 0) return BLOCK_FOUNDATION;
    if (wy >= surface - 2) return BLOCK_DIRT;
    return BLOCK_STONE;
}

// Flat v2's ground (DESIGN.md 2.5): the plain's top layer is a patchwork
// of three materials -- soft grass, crunchy sand, hard pebbles -- in
// fractal blobs: three octaves of seeded value noise (48, 20 and 8 blocks
// across), summed and thresholded, so sand and pebble patches sit apart in
// a sea of grass, with ragged, blobby edges. A few hashes per column, once,
// when the column generates.
// (Since M1.9 the patchwork is meadow grass, sand and gravel: the old roster's
// coastal sand and river pebble are gone, D41.)
static const BlockID kPatchSoft = BLOCK_MEADOW_GRASS, kPatchCrunch = BLOCK_SAND, kPatchHard = BLOCK_GRAVEL;
static inline double LatticeValue(int64_t x, int64_t z, uint64_t seed) {
    uint64_t h = seed ^ ((uint64_t)x * 0x9E3779B97F4A7C15ull) ^ ((uint64_t)z * 0xC2B2AE3D27D4EB4Full);
    h ^= h >> 33; h *= 0xFF51AFD7ED558CCDull; h ^= h >> 33; h *= 0xC4CEB9FE1A85EC53ull; h ^= h >> 33;
    return (double)(h >> 11) * (1.0 / 9007199254740992.0);
}
static double ValueNoise(double x, double z, uint64_t seed) {
    double fx = floor(x), fz = floor(z);
    int64_t ix = (int64_t)fx, iz = (int64_t)fz;
    double u = x - fx, v = z - fz;
    u = u * u * (3 - 2 * u); v = v * v * (3 - 2 * v);
    double a = LatticeValue(ix, iz, seed), b = LatticeValue(ix + 1, iz, seed);
    double c = LatticeValue(ix, iz + 1, seed), d = LatticeValue(ix + 1, iz + 1, seed);
    return (a + (b - a) * u) + ((c + (d - c) * u) - (a + (b - a) * u)) * v;
}
BlockID SurfaceBlockAt(int wx, int wz) { return SurfaceBlockAt(g_worldGen.seed, wx, wz); }
BlockID SurfaceBlockAt(uint64_t seed, int wx, int wz) {
    const uint64_t s = seed;
    double n = 0.55 * ValueNoise(wx / 48.0, wz / 48.0, s)
             + 0.30 * ValueNoise(wx / 20.0, wz / 20.0, s ^ 0x5bd1e995ull)
             + 0.15 * ValueNoise(wx / 8.0, wz / 8.0, s ^ 0x27d4eb2full);
    if (n < 0.34) return kPatchCrunch;
    if (n > 0.64) return kPatchHard;
    return kPatchSoft;
}

// A column's terrain, from the generator's settings alone (a copy, never
// g_worldGen: this runs on a job thread). Pure: same settings, same cells.
static void ComputeColumn(const WorldGenParams& gen, int cx, int cz, TerrainColumn& out) {
    if (gen.type == GEN_WALKGRID) { HillsColumn(gen.seed, cx, cz, out); return; }
    // Flat (the test ground): FLAT_V1_HEIGHT everywhere, dirt over stone
    // on a foundation floor; v2's top layer is the patchwork.
    const bool patchwork = gen.version >= 2;
    const int h = FLAT_V1_HEIGHT;
    out.chunks = h / CHUNK_SIZE + 1;
    out.present.assign(out.chunks, true);
    out.cells.assign((size_t)out.chunks * CHUNK_CELLS, 0);
    for (int lz = 0; lz < CHUNK_SIZE; lz++)
        for (int lx = 0; lx < CHUNK_SIZE; lx++) {
            BlockID top = patchwork ? SurfaceBlockAt(gen.seed, cx * CHUNK_SIZE + lx, cz * CHUNK_SIZE + lz) : BLOCK_DIRT;
            for (int y = 0; y <= h; y++) {
                BlockID b = TerrainBlockAt(y, h);
                if (y == h && patchwork) b = top;
                out.cells[(size_t)(y / CHUNK_SIZE) * CHUNK_CELLS + Chunk::LocalIndex(lx, y % CHUNK_SIZE, lz)] = (uint8_t)b;
            }
        }
}

// Main thread: a computed column becomes resident. Chunks only exist
// where they hold ground (Section 2.1); whatever the player changed here
// (kept from an eviction, or read from the save) replaces the generated
// chunk wholesale.
static void ApplyColumn(World& w, int cx, int cz, const TerrainColumn& col) {
    g_residentColumns.insert(ColumnKey(cx, cz));
    for (int cy = 0; cy < col.chunks && cy < COLUMN_CHUNKS; cy++) {
        if (!col.present[cy]) continue;
        Chunk* c = w.GetOrCreateChunk({ cx, cy, cz });
        memcpy(c->blocks, col.cells.data() + (size_t)cy * CHUNK_CELLS, CHUNK_CELLS);
    }
    for (int cy = 0; cy < COLUMN_CHUNKS; cy++) {
        auto it = g_evictedChunks.find({ cx, cy, cz });
        if (it == g_evictedChunks.end()) continue;
        w.AdoptChunk(it->first, std::move(it->second));
        g_evictedChunks.erase(it);
    }
    w.ComputeColumnTops(cx, cz);
    // The sky light of every chunk around reads these tops (23.4).
    MarkColumnNeighborhoodDirty(w, cx, cz);
}

void GenerateColumn(World& w, int cx, int cz) {
    if (g_residentColumns.count(ColumnKey(cx, cz))) return; // already resident -- nothing to do
    TerrainColumn col;
    ComputeColumn(g_worldGen, cx, cz, col);
    ApplyColumn(w, cx, cz, col);
}

int g_lastPlayerChunkX = INT32_MIN, g_lastPlayerChunkZ = INT32_MIN;

// Columns queued for generation but not yet generated. Entering view
// range only enqueues a column; ProcessColumnGeneration below drains a
// capped number per tick, following the exact pattern the falling-block
// queue already established (Section 5.1): a hard per-tick work cap
// instead of an unbounded burst, so crossing into a large unexplored
// area -- or the initial spawn, which needs the whole load radius at
// once -- can't spike a single frame.
std::deque<std::pair<int, int>> g_pendingColumns;
std::unordered_set<long long> g_pendingColumnSet;
std::deque<std::pair<int, int>> g_pendingEvictions;
std::unordered_set<long long> g_pendingEvictionSet;

void EnsureChunksLoaded(int playerChunkX, int playerChunkZ) {
    // Recomputed only when the player's chunk coordinate actually
    // changes (Section 2.4) -- not every frame.
    if (playerChunkX == g_lastPlayerChunkX && playerChunkZ == g_lastPlayerChunkZ) return;
    g_lastPlayerChunkX = playerChunkX;
    g_lastPlayerChunkZ = playerChunkZ;

    // Queued ring by ring outward from the player's own column, so the
    // ground under and around them always generates first. (This used
    // to be a corner-to-corner raster order, which put the player's own
    // column halfway down the queue -- dozens of ticks at spawn, long
    // enough to fall into where the ground was about to appear.)
    // Generation runs one ring past the view radius: a chunk is only
    // meshed once all 8 neighbouring columns exist (face culling and AO
    // read across them -- see RebuildDirtyChunks), so this extra ring is
    // what lets the outermost visible ring be meshed.
    int genRadius = g_loadRadius + 1;
    for (int ring = 0; ring <= genRadius; ring++) {
        for (int dx = -ring; dx <= ring; dx++) {
            for (int dz = -ring; dz <= ring; dz++) {
                if (dx != -ring && dx != ring && dz != -ring && dz != ring) continue; // ring edge only
                int cx = playerChunkX + dx, cz = playerChunkZ + dz;
                long long key = ColumnKey(cx, cz);
                if (g_residentColumns.count(key) || g_pendingColumnSet.count(key)) continue;
                g_pendingColumnSet.insert(key);
                g_pendingColumns.push_back({ cx, cz });
            }
        }
    }

    // Resident columns that have drifted past a margin beyond the load
    // radius (not right at its edge, so a player oscillating near the
    // boundary doesn't thrash evict/restore every other step) are
    // queued for eviction, drained through the same bounded-per-tick
    // pattern column generation already uses (Section 5.1's philosophy
    // applied here too). g_residentColumns only ever holds roughly the
    // loaded area's worth of keys, so this scan stays cheap regardless
    // of how much total ground the player has covered this session.
    for (long long key : g_residentColumns) {
        int cx, cz; DecodeColumnKey(key, cx, cz);
        int dist = ColumnDistance(cx, cz, playerChunkX, playerChunkZ);
        if (dist > genRadius + CHUNK_EVICT_MARGIN && !g_pendingEvictionSet.count(key)) {
            g_pendingEvictionSet.insert(key);
            g_pendingEvictions.push_back({ cx, cz });
        }
    }
}

// Bumped by ResetColumnStreaming: a column generated for the world before
// a New Game or Load is dropped when it arrives.
static uint32_t g_streamEpoch = 0;
static int g_columnsGenerating = 0;
int ColumnsGenerating() { return g_columnsGenerating; }

// Queued columns go to the job threads (a few per tick, a few in flight);
// finished ones are made resident here on the main thread, which alone
// writes the world. A column stays in g_pendingColumnSet until it lands,
// so it's never queued twice.
void ProcessColumnGeneration(World& w) {
    int submitted = 0;
    while (!g_pendingColumns.empty() && submitted < MAX_COLUMN_GENS_PER_TICK && g_columnsGenerating < MAX_COLUMNS_IN_FLIGHT) {
        auto col = g_pendingColumns.front();
        g_pendingColumns.pop_front();
        long long key = ColumnKey(col.first, col.second);
        // Queued back when the player was elsewhere and they've since
        // moved on: don't generate a column only to evict it again.
        if (g_residentColumns.count(key) ||
            ColumnDistance(col.first, col.second, g_lastPlayerChunkX, g_lastPlayerChunkZ) > g_loadRadius + 1) {
            g_pendingColumnSet.erase(key);
            continue;
        }
        auto result = std::make_shared<TerrainColumn>();
        WorldGenParams gen = g_worldGen;   // a copy: the job never reads the live settings
        uint32_t epoch = g_streamEpoch;
        int cx = col.first, cz = col.second;
        g_columnsGenerating++;
        submitted++;
        JobsSubmit(JOB_TERRAIN,
            [result, gen, cx, cz] { ComputeColumn(gen, cx, cz, *result); },
            [result, epoch, cx, cz, key, &w] {
                if (epoch != g_streamEpoch) return;   // made for a world that's gone
                g_columnsGenerating--;
                g_pendingColumnSet.erase(key);
                if (g_residentColumns.count(key)) return;
                if (ColumnDistance(cx, cz, g_lastPlayerChunkX, g_lastPlayerChunkZ) > g_loadRadius + 1) return; // left behind
                ApplyColumn(w, cx, cz, *result);
            });
    }
    JobsApply(JOB_TERRAIN, MAX_COLUMN_APPLIES_PER_TICK);
}

void ResetColumnStreaming() {
    g_streamEpoch++;
    g_columnsGenerating = 0;
    g_residentColumns.clear();
    g_evictedChunks.clear();
    g_pendingColumns.clear();
    g_pendingColumnSet.clear();
    g_pendingEvictions.clear();
    g_pendingEvictionSet.clear();
    g_lastPlayerChunkX = INT32_MIN;
    g_lastPlayerChunkZ = INT32_MIN;
}

void ProcessColumnEviction(World& w) {
    int n = (int)std::min<size_t>(MAX_COLUMN_EVICTIONS_PER_TICK, g_pendingEvictions.size());
    for (int i = 0; i < n; i++) {
        auto col = g_pendingEvictions.front();
        g_pendingEvictions.pop_front();
        long long key = ColumnKey(col.first, col.second);
        g_pendingEvictionSet.erase(key);
        if (!g_residentColumns.count(key)) continue;
        // The player can walk back into range between enqueue and now --
        // evicting then would punch a hole inside the load radius that
        // nothing refills until the next chunk crossing.
        if (ColumnDistance(col.first, col.second, g_lastPlayerChunkX, g_lastPlayerChunkZ)
                <= g_loadRadius + 1 + CHUNK_EVICT_MARGIN) continue;
        if (ColumnHasPendingUpdates(col.first, col.second)) {
            g_pendingEvictionSet.insert(key);
            g_pendingEvictions.push_back(col); // retry once the cascade drains
            continue;
        }
        EvictColumnFromWorld(w, col.first, col.second);
        g_residentColumns.erase(key);
    }
}

// =======================================================================
// Section 4.7 - Player physics
// =======================================================================

// BoxIntersectsSolid tests every voxel cell the box's full vertical
// extent overlaps (not just a few discrete height samples) -- a
// complete AABB-vs-voxel-grid overlap rather than sampled points.
static bool BoxIntersectsSolid(World& w, float cx, float cy, float cz, float height = PLAYER_HEIGHT) {
    float ax0 = cx - PLAYER_HALFW, ax1 = cx + PLAYER_HALFW;
    float ay0 = cy,                ay1 = cy + height;
    float az0 = cz - PLAYER_HALFW, az1 = cz + PLAYER_HALFW;
    int minX = (int)floor(ax0), maxX = (int)floor(ax1);
    int minY = (int)floor(ay0), maxY = (int)floor(ay1);
    int minZ = (int)floor(az0), maxZ = (int)floor(az1);
    for (int x = minX; x <= maxX; x++)
        for (int y = minY; y <= maxY; y++)
            for (int z = minZ; z <= maxZ; z++) {
                if (BlockSolid(w.Get(x, y, z))) return true; // every material is a whole cell (M1.9)
            }
    return false;
}

// ---- Walking on facets (DESIGN.md 23.5) ----
// The floor is the faceted surface itself (collide.h: the base facets the
// ground is drawn with). Cells still stop the body -- walls, ceilings,
// tunnels -- but only from BODY_SKIP above the feet: where a one-cell step
// becomes a slope, the cube's top edge stands up to about 0.66 above the
// facets (half a cell of slope plus the jitter), and it mustn't stop the
// body halfway up. A wall is at least a whole cell, so it still blocks.
static const float BODY_SKIP = 0.7f;
static const float HEAD_SKIP = 0.15f;  // the head may meet a ceiling cell this far: ceilings' facets hang as much lower or higher
static const float CLIMB = 1.05f;      // the most the feet rise onto the ground ahead: a one-cell slope, never a two-cell wall
static const float STICK = 0.35f;      // walking downhill, the feet stay on the ground within this drop a tick
static const float WALK_UP = 0.55f;    // steepest ground walked up: unit normal y (~57 degrees); a one-cell step is ~45
static const float LAND_UP = 0.30f;    // steepest ground landed on
static std::vector<FacetTri> g_facetsNear; // this tick's facets around the player (reused)

// The ground under the player's footprint: the highest facet point under
// the centre or near a corner (so the player stands on an edge) in [lo, hi].
static bool FootprintGround(float x, float z, float lo, float hi, float minUp, float* g) {
    const float k = PLAYER_HALFW - 0.05f;
    const float pts[5][2] = { { 0, 0 }, { -k, -k }, { k, -k }, { -k, k }, { k, k } };
    bool any = false;
    float best = -1e30f;
    for (auto& q : pts) {
        float h;
        if (GroundHeight(g_facetsNear, x + q[0], z + q[1], lo, hi, minUp, &h) && h > best) { best = h; any = true; }
    }
    if (any) *g = best;
    return any;
}
// The body, between the facet-dip allowance at the feet and the head's,
// against cells: a crouched player (0.9) still fits a one-cell crawlspace
// over jittered ground.
static bool BodyBlocked(World& w, float x, float feet, float z, float height) {
    return BoxIntersectsSolid(w, x, feet + BODY_SKIP, z, height - BODY_SKIP - HEAD_SKIP);
}

static bool ColumnResidentAt(float x, float z) {
    return g_residentColumns.count(ColumnKey(FloorDiv16((int)floor(x)), FloorDiv16((int)floor(z)))) != 0;
}

// Movement speeds and the power slide, blocks per second (Section 4.7).
static const float WALK_SPEED = 4.5f;
static const float SPRINT_SPEED = 7.5f;
static const float CROUCH_SPEED = 1.8f;
static const float SLIDE_START_SPEED = 10.0f; // the burst when a sprint drops into a slide
static const float SLIDE_FRICTION = 1.5f;     // per second: speed falls as e^(-friction * t) on the ground
static const float SLIDE_MAX_SECONDS = 1.4f;
// The slide forgives timing: crouch and sprint may come in either order,
// up to this far apart (a sprint just let go of still counts, and a crouch
// pressed a moment early -- or just before landing -- waits for it).
static const float SLIDE_SPRINT_GRACE = 0.4f; // seconds since sprinting
static const float SLIDE_PRESS_GRACE = 0.3f;  // seconds a crouch press stays fresh
static const float SLIDE_LEAN_ROLL = 0.21f;   // radians (~12 degrees) of lean at full sideways slide
static const float SLIDE_LEAN_PITCH = 0.08f;  // radians of dip at full forward slide

void UpdatePlayerPhysics(World& w, Player& p, float dt, const MoveInput& in) {
    // Ground that doesn't exist yet reads as air. Until the player's own
    // column is generated (spawn, a load, a teleport-sized jump) hold
    // them exactly where they are instead of letting them fall into the
    // space the terrain is about to fill.
    if (!ColumnResidentAt(p.x, p.z)) { p.velY = 0.0f; return; }
    // Safety net: however the player got below the world (it shouldn't be
    // possible -- the floor can't be broken), put them back on the highest
    // solid block of their column rather than falling forever.
    if (p.y < Y_MIN - 32.0f) {
        int bx = (int)floor(p.x), bz = (int)floor(p.z), top = Y_MIN;
        for (int y = Y_MAX; y >= Y_MIN; y--) if (w.Solid(bx, y, bz)) { top = y + 1; break; }
        p.y = (float)top;
        p.velY = 0.0f;
        return;
    }
    // No room to stand (a load or a block placed in a crawlspace) but room
    // to crouch: crouch, rather than being pushed up out of it.
    if (!p.crouching && BodyBlocked(w, p.x, p.y, p.z, PLAYER_HEIGHT) && !BodyBlocked(w, p.x, p.y, p.z, PLAYER_CROUCH_HEIGHT))
        p.crouching = true;
    // Never entombed: if the body overlaps solid blocks anyway (terrain
    // that appeared around an edge, a block placed or fallen onto the
    // player), lift them a block per tick until they're standing free.
    if (BodyBlocked(w, p.x, p.y, p.z, PlayerHeight(p))) {
        p.y = floorf(p.y) + 1.0f;
        p.velY = 0.0f;
        p.onGround = false;
        return;
    }

    {   // The facets around the player, once a tick.
        int x = (int)floorf(p.x), y = (int)floorf(p.y), z = (int)floorf(p.z);
        GatherFacets(w, x - 3, std::max(Y_MIN, y - 3), z - 3, x + 4, std::min(Y_MAX + 1, y + 4), z + 4, g_facetsNear);
    }

    Vec3 f, r, u;
    GetCameraVectors(p, f, r, u);
    float fx = f.x, fz = f.z;
    float rx = r.x, rz = r.z;
    float len = sqrtf(fx * fx + fz * fz);
    if (len > 0.0001f) { fx /= len; fz /= len; }

    float mx = 0, mz = 0;
    if (in.fwd)  { mx += fx; mz += fz; }
    if (in.back) { mx -= fx; mz -= fz; }
    if (in.right){ mx += rx; mz += rz; }
    if (in.left) { mx -= rx; mz -= rz; }
    float mlen = sqrtf(mx * mx + mz * mz);
    if (mlen > 0.0001f) { mx /= mlen; mz /= mlen; } // unit direction (or zero)

    // Crouch, sprint and the power slide. A fresh crouch press while
    // sprinting on the ground drops into a slide: a burst of speed along
    // the way the player was running that bleeds off over about a second,
    // at crouch height (so a slide goes under a 1-block gap). Otherwise
    // holding crouch crouches; letting go stands up only where there's
    // room to.
    bool crouchPressed = in.crouch && !p.crouchHeld;
    p.crouchHeld = in.crouch;
    p.crouchBuffer = crouchPressed ? SLIDE_PRESS_GRACE : std::max(0.0f, p.crouchBuffer - dt);
    // Sprinting, or trying to (sprint + forward with crouch already held).
    bool sprintIntent = in.sprint && in.fwd && !in.back;
    p.sinceSprint = (sprintIntent || p.sprinting) ? 0.0f : p.sinceSprint + dt;
    if (p.crouchBuffer > 0.0f && in.crouch && p.onGround && p.sinceSprint <= SLIDE_SPRINT_GRACE &&
        !PlayerSliding(p) && mlen > 0.0001f) {
        p.slideTime = 1e-4f;
        p.slideVX = mx * SLIDE_START_SPEED; p.slideVZ = mz * SLIDE_START_SPEED;
        p.crouching = true;
        p.crouchBuffer = 0.0f; // one press, one slide
    }
    if (PlayerSliding(p)) {
        float speed = sqrtf(p.slideVX * p.slideVX + p.slideVZ * p.slideVZ);
        if (in.jump || speed < CROUCH_SPEED || p.slideTime > SLIDE_MAX_SECONDS) p.slideTime = 0.0f; // over (a jump ends it)
    }
    bool sliding = PlayerSliding(p);
    if (in.crouch || sliding) p.crouching = true;
    else if (p.crouching && !BodyBlocked(w, p.x, p.y, p.z, PLAYER_HEIGHT)) p.crouching = false;
    p.sprinting = in.sprint && in.fwd && !in.back && !p.crouching;

    // Horizontal moves, one axis at a time. Don't walk off the edge of
    // generated ground. On the ground, the feet rise onto the facets ahead
    // (up to a one-cell slope, never a wall); the camera doesn't jump but
    // glides up with the eye's easing. Returns whether the move happened.
    const float h = PlayerHeight(p);
    auto tryMove = [&](float dx, float dz) {
        if (dx == 0.0f && dz == 0.0f) return true;
        float nx = p.x + dx, nz = p.z + dz;
        if (!ColumnResidentAt(nx, nz)) return false;
        float feet = p.y, g;
        if (p.onGround && FootprintGround(nx, nz, feet - 0.01f, feet + CLIMB, WALK_UP, &g) && g > feet) feet = g;
        if (BodyBlocked(w, nx, feet, nz, h)) return false;
        if (feet > p.y) { p.eyeHeight -= feet - p.y; p.y = feet; }
        p.x = nx; p.z = nz;
        return true;
    };
    if (sliding) {
        // Momentum, not input: friction on the ground, none in the air;
        // a wall stops that axis.
        if (p.onGround) {
            float k = expf(-SLIDE_FRICTION * dt);
            p.slideVX *= k; p.slideVZ *= k;
        }
        if (!tryMove(p.slideVX * dt, 0.0f)) p.slideVX = 0.0f;
        if (!tryMove(0.0f, p.slideVZ * dt)) p.slideVZ = 0.0f;
        p.slideTime += dt;
    } else {
        float speed = p.crouching ? CROUCH_SPEED : (p.sprinting ? SPRINT_SPEED : WALK_SPEED);
        tryMove(mx * speed * dt, 0.0f);
        tryMove(0.0f, mz * speed * dt);
    }

    const float GRAVITY = 20.0f;
    const float JUMP_SPEED = 7.0f;
    if (p.onGround && in.jump) { p.velY = JUMP_SPEED; p.onGround = false; }
    p.velY -= GRAVITY * dt;
    if (p.velY < -50.0f) p.velY = -50.0f;

    float dy = p.velY * dt;
    float g;
    if (p.onGround && p.velY <= 0.0f) {
        // Keep to the ground: over bumps and down slopes, the feet follow
        // the facets and the camera eases after them (no judder).
        if (FootprintGround(p.x, p.z, p.y - STICK, p.y + 0.5f, LAND_UP, &g)) {
            p.eyeHeight -= g - p.y;
            p.y = g;
            p.velY = 0.0f;
        } else {
            p.onGround = false; // walked off an edge
            p.y += dy;
        }
    } else if (dy < 0.0f) {
        // Falling: land on the first ground the feet reach (or already
        // dipped into, by less than half a cell).
        if (FootprintGround(p.x, p.z, p.y + dy - 0.01f, p.y + 0.5f, LAND_UP, &g)) {
            p.y = g;
            p.velY = 0.0f;
            p.onGround = true;
        } else {
            p.y += dy;
        }
    } else {
        // Rising: the body stops at a ceiling.
        if (BodyBlocked(w, p.x, p.y + dy, p.z, h)) p.velY = 0.0f;
        else p.y += dy;
        p.onGround = false;
    }

    // Camera: the eye drops when crouching and further in a slide, and a
    // slide leans the view into the way it's carrying the player relative
    // to where they look -- sideways rolls toward that side, straight
    // ahead dips forward, backwards tips back -- scaled by its speed.
    float eyeTarget = sliding ? PLAYER_SLIDE_EYE : (p.crouching ? PLAYER_CROUCH_EYE : PLAYER_EYE);
    float rollTarget = 0.0f, pitchTarget = 0.0f;
    if (sliding) {
        float speed = sqrtf(p.slideVX * p.slideVX + p.slideVZ * p.slideVZ);
        if (speed > 1e-3f) {
            float amount = speed / SLIDE_START_SPEED; amount = amount > 1.0f ? 1.0f : amount;
            float sx = p.slideVX / speed, sz = p.slideVZ / speed;
            rollTarget = (sx * rx + sz * rz) * SLIDE_LEAN_ROLL * amount;
            pitchTarget = -(sx * fx + sz * fz) * SLIDE_LEAN_PITCH * amount;
        }
    }
    float ease = 1.0f - expf(-dt / 0.08f);
    p.eyeHeight += (eyeTarget - p.eyeHeight) * ease;
    p.roll += (rollTarget - p.roll) * ease;
    p.leanPitch += (pitchTarget - p.leanPitch) * ease;
}

// =======================================================================
// Section 4.5 - Amanatides-Woo exact voxel DDA raycast for block picking
// =======================================================================

bool Raycast(World& w, float ox, float oy, float oz, float dx, float dy, float dz, float maxDist,
             int& hitX, int& hitY, int& hitZ, int& placeX, int& placeY, int& placeZ) {
    float len = sqrtf(dx * dx + dy * dy + dz * dz);
    if (len < 1e-6f) return false;
    dx /= len; dy /= len; dz /= len;

    int voxX = (int)floor(ox), voxY = (int)floor(oy), voxZ = (int)floor(oz);
    int stepX = dx > 0 ? 1 : (dx < 0 ? -1 : 0);
    int stepY = dy > 0 ? 1 : (dy < 0 ? -1 : 0);
    int stepZ = dz > 0 ? 1 : (dz < 0 ? -1 : 0);

    auto tDeltaOf = [](float d) { return d == 0.0f ? FLT_MAX : fabsf(1.0f / d); };
    float tDeltaX = tDeltaOf(dx), tDeltaY = tDeltaOf(dy), tDeltaZ = tDeltaOf(dz);

    auto tMaxOf = [](float origin, int vox, int step, float d) {
        if (step == 0) return FLT_MAX;
        float boundary = step > 0 ? (float)(vox + 1) : (float)vox;
        return (boundary - origin) / d;
    };
    float tMaxX = tMaxOf(ox, voxX, stepX, dx);
    float tMaxY = tMaxOf(oy, voxY, stepY, dy);
    float tMaxZ = tMaxOf(oz, voxZ, stepZ, dz);

    int prevX = voxX, prevY = voxY, prevZ = voxZ;
    float traveled = 0.0f;

    if (w.Solid(voxX, voxY, voxZ)) {
        hitX = voxX; hitY = voxY; hitZ = voxZ;
        placeX = voxX; placeY = voxY; placeZ = voxZ;
        return true;
    }

    while (traveled <= maxDist) {
        prevX = voxX; prevY = voxY; prevZ = voxZ;
        if (tMaxX < tMaxY && tMaxX < tMaxZ) {
            voxX += stepX; traveled = tMaxX; tMaxX += tDeltaX;
        } else if (tMaxY < tMaxZ) {
            voxY += stepY; traveled = tMaxY; tMaxY += tDeltaY;
        } else {
            voxZ += stepZ; traveled = tMaxZ; tMaxZ += tDeltaZ;
        }

        // Past the starting cell, walk-through blocks (plants) are hits
        // too; the cell the eye is in only counts if it's solid, so
        // standing in grass doesn't make it the target of every click.
        if (w.Pickable(voxX, voxY, voxZ)) {
            hitX = voxX; hitY = voxY; hitZ = voxZ;
            placeX = prevX; placeY = prevY; placeZ = prevZ;
            return true;
        }
    }
    return false;
}
