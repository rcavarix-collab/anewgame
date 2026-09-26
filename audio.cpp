// audio.cpp
//
// Section 10 / Part XIV - Audio (chunked, clock-synced day-cycle music).
//
// One persistent source voice plays the day-cycle music (music_synth.cpp
// generates it), generated in small chunks continuously anchored to the
// live day clock (g_dayTimeSeconds) rather than baked once as a fixed
// loop -- see DESIGN.md Part XIV for why: this is what makes it
// structurally impossible for playback to drift out of sync with the
// clock, rather than merely unlikely. Master and Music are separate
// settings/sliders so adding an SFX channel later is just another source
// voice under the same mastering voice, not a change to the mixing model.
//
// The world sound palette plays on a second voice and renders on its own
// thread too (M1.1): the frame only posts to its mailbox. Costs: the
// music thread and the world thread each render a few percent of a core
// while sounding and sleep otherwise; the main thread pays a lock and a
// copy per call. Layer 4 (presentation); DESIGN.md 10.1, 10.4.
//
// Silent at the title screen by construction: nothing ever calls
// StartMusicPlayback() until a game actually begins (New Game/Load
// Game), and nothing resumes it after Quit to Title. Silent during any
// paused menu too, per an explicit request -- losing the music when you
// pause reads as "time itself stopped," which is the point.

#ifndef NOMINMAX // also set project-wide (walkgrid.vcxproj)
#define NOMINMAX
#endif
#include <windows.h>
#include <xaudio2.h>
#include "audio.h"
#include "music_synth.h"
#include "world.h"   // g_dayTimeSeconds
#include "settings.h" // g_masterVolume / g_musicVolume / g_musicIntensity
#include "musiclevel.h"
#include <cstdint>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#pragma comment(lib, "xaudio2.lib")

// The music is composed against a fixed hour; a different day length would
// silently desync every section and chord from the clock.
static_assert((double)DAY_LENGTH_SECONDS == MUSIC_DAY_LENGTH, "music day length must match world.h DAY_LENGTH_SECONDS");

static IXAudio2* g_xaudio2 = nullptr;
static IXAudio2MasteringVoice* g_masteringVoice = nullptr;
static IXAudio2SourceVoice* g_musicVoice = nullptr;
static MusicState g_musicState;
static double g_nextChunkStartTime = -1.0; // -1 = inactive (title screen / paused)
// Quarter-second chunks, 16 of lookahead (4 s buffered) -- enough slack
// for a brief message-loop stall (e.g. dragging the window) before the
// queue runs dry.
static const int MUSIC_CHUNK_SAMPLES = MUSIC_SAMPLE_RATE / 4;
static const int MUSIC_LOOKAHEAD_CHUNKS = 16;

// The music is synthesized on its own thread (Part XIV): the frame never
// pays for it, however busy the section. The worker keeps the lookahead
// full, generating each chunk *outside* the lock (from copies of the
// synth state, colour and intensity) and publishing it -- levels, start
// time, submission -- in a brief locked step. Starting or stopping
// playback bumps g_musicEpoch under the lock, so a chunk begun for the old
// position is simply thrown away. The main thread's readers (audible time,
// the music level) take the lock only for a few reads.
static std::mutex g_musicLock;
static std::condition_variable g_musicWake;
static std::thread g_musicThread;
static std::atomic<bool> g_musicQuit{ false };
static uint32_t g_musicEpoch = 0;

// Fixed pool of PCM buffers, reused cyclically instead of new/delete per
// chunk. XAudio2 reads a submitted buffer from its own thread until it
// stops counting it in BuffersQueued; submission and consumption are
// both FIFO, so with one more slot than the lookahead, the slot about to
// be overwritten is always older than every buffer still queued.
static const int MUSIC_POOL_SIZE = MUSIC_LOOKAHEAD_CHUNKS + 1;
static int16_t (*g_musicPool)[MUSIC_CHUNK_SAMPLES] = nullptr;
static int g_musicPoolNext = 0;
// Note onsets (musiclevel.h) in each pooled chunk, 1/64 s steps, measured
// when the chunk is synthesized; read back at the chunk actually playing,
// so anything that reacts to the music follows what's heard, not the
// audio generated seconds ahead (Section 10.4).
static const int LEVEL_STEPS = 16;
static float g_chunkLevels[MUSIC_POOL_SIZE][LEVEL_STEPS] = {};
static MusicLevelMeter g_levelMeter;  // carried from chunk to chunk, in generation order
static double g_levelLastNow = 0;     // last CurrentMusicLevel call, seconds (QPC)
static MusicGlow g_musicGlow;         // the slow, flash-safe swell the block shows
static int g_levelSlot = -1;          // pool slot last seen playing
static double g_levelSlotStart = 0;   // when it started, seconds (QPC)
static float g_levelSmoothed = 0;
// The music time each pooled chunk starts at, so the palette can ask what's
// audible now (AudibleMusicTime).
static double g_chunkStartTime[MUSIC_POOL_SIZE] = {};
// The soundscape colour the track leans toward (docs/SOUND_PALETTE.md 6).
static MusicColour g_musicColour;

// ---- World sound palette: a second voice, on its own thread (M1.1) ---
// The palette renders on the world worker, never on the frame: busy
// moments used to cost the main thread up to 15 ms (forecast F11). The
// main thread only posts to a small mailbox -- cues, releases and fades in
// order, plus the latest state (axes, scene, listener, gait) -- and the
// worker applies them before each render. g_palette and everything marked
// "worker only" are touched by the worker alone once it has started.
static IXAudio2SourceVoice* g_worldVoice = nullptr;
static SoundPalette* g_palette = nullptr;
static const int WORLD_BUFFER_SAMPLES = 512;   // 11.6 ms
// Buffers kept queued: 3 (~35 ms). The worker is woken as each buffer
// ends, so the queue no longer follows the frame time; if it ever runs dry
// mid-sound (the thread was held up), it queues one deeper, up to 8.
static const int WORLD_QUEUE_MIN = 3, WORLD_QUEUE_MAX = 8;
static const int WORLD_POOL = WORLD_QUEUE_MAX + 2;
static int g_worldQueue = WORLD_QUEUE_MIN;      // worker only
static int16_t (*g_worldPool)[WORLD_BUFFER_SAMPLES * 2] = nullptr; // stereo, interleaved
static int g_worldPoolNext = 0;                 // worker only
static bool g_worldIdle = true;                 // worker only: nothing sounding, no buffers rendered
static bool g_worldSounding = false;            // worker only: rendered continuously last pass

// The mailbox. Commands keep their order; the state is latest-wins.
enum WorldCmdType : uint8_t { WC_PLAY, WC_RELEASE, WC_FADE };
struct WorldCmd { WorldCmdType type; SoundCue cue; float seconds; };
// 64 between two worker passes (~12 ms apart while sounding) is far more
// than play produces. Past the cap, new cues are dropped; the last 8
// slots are kept for releases and fades, so a held slide or a pause is
// never lost.
static const int WORLD_MAILBOX = 64, WORLD_MAILBOX_CUES = WORLD_MAILBOX - 8;
struct WorldState {
    SoundAxes axes; AmbientScene scene;
    float intensity = 1.0f, listener[4] = {};
    bool mono = false, ambient = false;
    float stepGain = 1.0f;
    int gait = SoundPalette::GAIT_NONE; SoundMaterial ground = MAT_NONE;
};
static std::mutex g_worldLock;                  // guards the mailbox and the flags below
static std::condition_variable g_worldWake;
static std::thread g_worldThread;
static WorldCmd g_worldCmds[WORLD_MAILBOX];
static int g_worldCmdCount = 0;
static WorldState g_worldState;
static bool g_worldStateNew = false;
static bool g_worldUrgent = false;  // wake now: a cue, a gait or play starting/stopping
static bool g_worldKick = false;    // a buffer ended: top the queue up
static bool g_worldQuit = false;

// Set after Stop+Flush: the flush only takes effect on the audio
// thread's next processing pass, so no pool slot may be rewritten until
// BuffersQueued has actually reached zero.
static bool g_musicNeedsDrain = false;

void ApplyAudioVolumes() {
    if (g_musicVoice) g_musicVoice->SetVolume(g_masterVolume * g_musicVolume);
    if (g_worldVoice) g_worldVoice->SetVolume(g_masterVolume * g_worldVolume);
}

static UINT32 QueuedMusicBuffers() {
    XAUDIO2_VOICE_STATE vstate;
    g_musicVoice->GetState(&vstate, XAUDIO2_VOICE_NOSAMPLESPLAYED);
    return vstate.BuffersQueued;
}

// Normally already drained (a pause lasts far longer than one ~10 ms
// processing pass), so this rarely waits at all. If it somehow doesn't
// drain, the old pool is abandoned rather than risk rewriting memory the
// audio thread may still read -- a small leak beats a use-after-free.
static void WaitForMusicDrain() {
    if (!g_musicNeedsDrain) return;
    for (int i = 0; i < 100 && QueuedMusicBuffers() > 0; i++) Sleep(1);
    if (QueuedMusicBuffers() > 0) {
        OutputDebugStringA("audio: music voice did not drain after flush; abandoning buffer pool\n");
        g_musicPool = new int16_t[MUSIC_POOL_SIZE][MUSIC_CHUNK_SAMPLES];
        g_musicPoolNext = 0;
    }
    g_musicNeedsDrain = false;
}

static void MusicWorker() {
    std::unique_lock<std::mutex> lk(g_musicLock);
    while (!g_musicQuit) {
        bool due = g_musicVoice && g_musicPool && g_nextChunkStartTime >= 0.0 && !g_musicNeedsDrain
                   && QueuedMusicBuffers() < (UINT32)MUSIC_LOOKAHEAD_CHUNKS;
        if (!due) { g_musicWake.wait_for(lk, std::chrono::milliseconds(20)); continue; }
        // Take the job: which slot, from when, with what -- then let go.
        uint32_t epoch = g_musicEpoch;
        int slot = g_musicPoolNext;
        int16_t* chunk = g_musicPool[slot];
        double start = g_nextChunkStartTime;
        double intensity = (double)g_musicIntensity;
        MusicColour colour = g_musicColour;
        MusicState state = g_musicState;
        MusicLevelMeter meter = g_levelMeter;
        lk.unlock();
        float levels[LEVEL_STEPS];
        GenerateMusicChunk(start, MUSIC_CHUNK_SAMPLES, intensity, &state, chunk, &colour);
        MeasureMusicLevels(meter, chunk, MUSIC_CHUNK_SAMPLES, LEVEL_STEPS, MUSIC_SAMPLE_RATE, levels);
        lk.lock();
        if (epoch != g_musicEpoch || g_musicQuit) continue; // playback restarted or stopped meanwhile: stale
        g_musicState = state;
        g_levelMeter = meter;
        memcpy(g_chunkLevels[slot], levels, sizeof(levels));
        g_chunkStartTime[slot] = start;
        g_musicPoolNext = (slot + 1) % MUSIC_POOL_SIZE;
        g_nextChunkStartTime = start + (double)MUSIC_CHUNK_SAMPLES / MUSIC_SAMPLE_RATE;
        XAUDIO2_BUFFER buf = {};
        buf.AudioBytes = MUSIC_CHUNK_SAMPLES * sizeof(int16_t);
        buf.pAudioData = (const BYTE*)chunk;
        g_musicVoice->SubmitSourceBuffer(&buf);
    }
}

// Called on New Game, Load Game, Resume from Pause, and Quick Load --
// every path that either starts a game or changes g_dayTimeSeconds out
// from under the music. Always resets MusicState to a clean zero-state
// (Section 10's filter/arp-scheduler reset-on-discontinuity policy) and
// re-anchors to whatever g_dayTimeSeconds is *right now*, so there is
// never a stale, independently-advancing audio position to reconcile.
void StartMusicPlayback() {
    if (!g_musicVoice) return;
    {
        std::lock_guard<std::mutex> lk(g_musicLock);
        g_musicVoice->Stop();
        g_musicVoice->FlushSourceBuffers();
        g_musicNeedsDrain = true;
        WaitForMusicDrain();
        ResetMusicState(&g_musicState);
        g_levelMeter = MusicLevelMeter();
        g_nextChunkStartTime = g_dayTimeSeconds;
        g_musicEpoch++; // anything the worker had begun belongs to the old position
        // Playing an empty queue is silence; the worker's first chunk (a
        // millisecond or two) starts the sound.
        g_musicVoice->Start();
    }
    g_musicWake.notify_one();
}

// Called whenever any menu opens during play (pause is silence, by
// request -- it signals the passage of in-game time stopping, not a
// real-time-continues-in-the-background pause) and on Quit to Title.
// Doesn't wait for the drain itself -- the next StartMusicPlayback does,
// by which point it has almost always already happened.
void StopMusicPlayback() {
    if (!g_musicVoice) return;
    std::lock_guard<std::mutex> lk(g_musicLock);
    g_musicVoice->Stop();
    g_musicVoice->FlushSourceBuffers();
    g_musicNeedsDrain = true;
    g_nextChunkStartTime = -1.0;
    g_musicEpoch++;
}

// The worker keeps the queue topped up by itself; a nudge each frame just
// means a freshly drained queue is noticed without waiting out its timeout.
void RefillMusicQueueIfNeeded() {
    if (g_musicVoice) g_musicWake.notify_one();
}

static double NowSeconds() {
    LARGE_INTEGER f, n; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&n);
    return (double)n.QuadPart / (double)f.QuadPart;
}
// The pool slot XAudio2 is playing now and when it started (first seen),
// shared by the music level and the audible-time readback. -1: none.
static int PlayingSlot(double now, UINT32 queued) {
    if (queued == 0) return -1;
    // Pool slots are used in order, so the oldest still-queued buffer --
    // the one playing -- is `queued` slots behind the next free one.
    int slot = (g_musicPoolNext - (int)queued + MUSIC_POOL_SIZE) % MUSIC_POOL_SIZE;
    if (slot != g_levelSlot) { g_levelSlot = slot; g_levelSlotStart = now; }
    return slot;
}

double AudibleMusicTime() {
    std::lock_guard<std::mutex> lk(g_musicLock);
    if (!g_musicVoice || g_nextChunkStartTime < 0.0 || !g_musicPool) return g_dayTimeSeconds;
    double now = NowSeconds();
    int slot = PlayingSlot(now, QueuedMusicBuffers());
    if (slot < 0) return g_dayTimeSeconds;
    double into = now - g_levelSlotStart;
    double chunk = (double)MUSIC_CHUNK_SAMPLES / MUSIC_SAMPLE_RATE;
    return g_chunkStartTime[slot] + (into < 0 ? 0 : into > chunk ? chunk : into);
}

float CurrentMusicLevel() {
    std::lock_guard<std::mutex> lk(g_musicLock);
    // Silent (title, menus, paused): the glow resets; it swells back in
    // from dark when the music starts again.
    if (!g_musicVoice || g_nextChunkStartTime < 0.0 || !g_musicPool) {
        g_levelSmoothed = 0; g_levelSlot = -1; g_musicGlow = MusicGlow(); g_levelLastNow = 0;
        return 0.0f;
    }
    double now = NowSeconds();
    double dt = g_levelLastNow > 0 ? now - g_levelLastNow : 0.0;
    g_levelLastNow = now;
    float step = (float)(dt < 0.25 ? dt : 0.25);
    // A slow swell with the notes, never a flash per note (musiclevel.h:
    // photosensitivity), the same at any frame rate.
    UINT32 queued = QueuedMusicBuffers();
    if (queued == 0) return g_levelSmoothed = MusicGlowStep(g_musicGlow, 0.0f, step); // starved: fade out gently
    int slot = PlayingSlot(now, queued);
    int q = (int)((now - g_levelSlotStart) * MUSIC_SAMPLE_RATE / (MUSIC_CHUNK_SAMPLES / LEVEL_STEPS));
    q = q < 0 ? 0 : (q >= LEVEL_STEPS ? LEVEL_STEPS - 1 : q);
    g_levelSmoothed = MusicGlowStep(g_musicGlow, g_chunkLevels[slot][q], step);
    return g_levelSmoothed;
}

// Failure anywhere here (no audio device, driver issue, etc.) leaves
// g_musicVoice null and every subsequent audio call a silent no-op via
// the null checks above -- a machine with no usable audio device still
// gets a fully playable game, just a silent one, rather than a startup
// failure.
// XAudio2 calls this on its own thread as each world buffer finishes: wake
// the worker to top the queue up. Only a flag and a notify, never work.
struct WorldVoiceCallback : IXAudio2VoiceCallback {
    void STDMETHODCALLTYPE OnBufferEnd(void*) override {
        { std::lock_guard<std::mutex> lk(g_worldLock); g_worldKick = true; }
        g_worldWake.notify_one();
    }
    void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
    void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
    void STDMETHODCALLTYPE OnStreamEnd() override {}
    void STDMETHODCALLTYPE OnBufferStart(void*) override {}
    void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
    void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}
};
static WorldVoiceCallback g_worldCallback;
static void WorldWorker();

bool InitAudio() {
    if (FAILED(XAudio2Create(&g_xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR))) return false;
    if (FAILED(g_xaudio2->CreateMasteringVoice(&g_masteringVoice))) return false;

    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = MUSIC_SAMPLE_RATE;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (WORD)((wfx.nChannels * wfx.wBitsPerSample) / 8);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if (FAILED(g_xaudio2->CreateSourceVoice(&g_musicVoice, &wfx))) return false;
    g_musicPool = new int16_t[MUSIC_POOL_SIZE][MUSIC_CHUNK_SAMPLES];
    // The palette's voice is optional: without it the game just has music.
    // Stereo (the music stays mono): world sounds sit where they happen.
    WAVEFORMATEX wfx2 = wfx;
    wfx2.nChannels = 2;
    wfx2.nBlockAlign = (WORD)(2 * wfx.wBitsPerSample / 8);
    wfx2.nAvgBytesPerSec = wfx2.nSamplesPerSec * wfx2.nBlockAlign;
    if (SUCCEEDED(g_xaudio2->CreateSourceVoice(&g_worldVoice, &wfx2, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &g_worldCallback))) {
        g_worldPool = new int16_t[WORLD_POOL][WORLD_BUFFER_SAMPLES * 2];
        g_palette = new SoundPalette();
        g_worldVoice->Start();
        g_worldQuit = false;
        g_worldThread = std::thread(WorldWorker); // sleeps until something is posted
    } else {
        g_worldVoice = nullptr;
    }
    ApplyAudioVolumes();
    g_musicQuit = false;
    g_musicThread = std::thread(MusicWorker); // idles until playback starts
    // Deliberately not started here -- the title screen is silent by
    // design (Section 13); playback only begins via StartMusicPlayback().
    return true;
}

void ShutdownAudio() {
    if (g_musicThread.joinable()) { // the worker first: it submits to the music voice
        g_musicQuit = true;
        g_musicWake.notify_one();
        g_musicThread.join();
    }
    if (g_worldThread.joinable()) { // the world worker too: it owns the palette and submits to its voice
        { std::lock_guard<std::mutex> lk(g_worldLock); g_worldQuit = true; }
        g_worldWake.notify_one();
        g_worldThread.join();
    }
    if (g_worldVoice) { g_worldVoice->Stop(); g_worldVoice->DestroyVoice(); g_worldVoice = nullptr; }
    delete[] g_worldPool; g_worldPool = nullptr;
    delete g_palette; g_palette = nullptr;
    if (g_musicVoice) { g_musicVoice->Stop(); g_musicVoice->DestroyVoice(); g_musicVoice = nullptr; }
    if (g_masteringVoice) { g_masteringVoice->DestroyVoice(); g_masteringVoice = nullptr; }
    if (g_xaudio2) { g_xaudio2->Release(); g_xaudio2 = nullptr; }
    // DestroyVoice is synchronous, so nothing can still be reading these.
    delete[] g_musicPool;
    g_musicPool = nullptr;
}

// ---------------------------------------------------------------------
// World sound palette (docs/SOUND_PALETTE.md)
// ---------------------------------------------------------------------

static UINT32 QueuedWorldBuffers() {
    XAUDIO2_VOICE_STATE vs;
    g_worldVoice->GetState(&vs, XAUDIO2_VOICE_NOSAMPLESPLAYED);
    return vs.BuffersQueued;
}

// Worker only. Tops the palette's queue up to g_worldQueue buffers. Each
// buffer is rendered for the music time it will be heard at: what's
// audible now plus what's already queued ahead of it.
static void PumpWorldSound() {
    UINT32 queued = QueuedWorldBuffers();
    if (g_worldIdle && g_palette->Silent()) { g_worldSounding = false; return; } // nothing to say: render nothing
    // Ran dry while sounding: the thread was held up longer than the queue
    // lasts, so keep one more buffer ahead from now on (bounded).
    if (queued == 0 && g_worldSounding && g_worldQueue < WORLD_QUEUE_MAX) g_worldQueue++;
    bool running;
    { std::lock_guard<std::mutex> lk(g_musicLock); running = g_nextChunkStartTime >= 0.0; }
    double t = AudibleMusicTime() + (double)queued * WORLD_BUFFER_SAMPLES / MUSIC_SAMPLE_RATE;
    float buf[WORLD_BUFFER_SAMPLES * 2];
    while (queued < (UINT32)g_worldQueue) {
        g_palette->RenderStereo(buf, WORLD_BUFFER_SAMPLES, t, running);
        int16_t* out = g_worldPool[g_worldPoolNext];
        g_worldPoolNext = (g_worldPoolNext + 1) % WORLD_POOL;
        for (int i = 0; i < WORLD_BUFFER_SAMPLES * 2; i++) {
            float v = buf[i] > 1.0f ? 1.0f : buf[i] < -1.0f ? -1.0f : buf[i];
            out[i] = (int16_t)(v * 32767.0f);
        }
        XAUDIO2_BUFFER xb = {};
        xb.AudioBytes = WORLD_BUFFER_SAMPLES * 2 * sizeof(int16_t);
        xb.pAudioData = (const BYTE*)out;
        g_worldVoice->SubmitSourceBuffer(&xb);
        queued++;
        if (running) t += (double)WORLD_BUFFER_SAMPLES / MUSIC_SAMPLE_RATE;
    }
    g_worldIdle = g_palette->Silent();
    g_worldSounding = !g_worldIdle;
}

// The world worker: take the mailbox, apply it to the palette, render,
// sleep until a buffer ends or something is posted. Silent and idle, it
// sleeps outright (the 1 s timeout is only a safety net).
static void WorldWorker() {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL); // a 35 ms queue can't wait behind the frame
    static WorldCmd cmds[WORLD_MAILBOX]; // this thread's own copy: no allocation per pass
    WorldState st;
    std::unique_lock<std::mutex> lk(g_worldLock);
    while (!g_worldQuit) {
        int n = g_worldCmdCount;
        std::copy(g_worldCmds, g_worldCmds + n, cmds);
        g_worldCmdCount = 0;
        bool haveState = g_worldStateNew;
        if (haveState) st = g_worldState;
        g_worldStateNew = g_worldUrgent = g_worldKick = false;
        lk.unlock();
        if (haveState) {
            g_palette->SetAxes(st.axes);
            g_palette->SetScene(st.scene);
            g_palette->SetIntensity(st.intensity);
            g_palette->SetListener(st.listener[0], st.listener[1], st.listener[2], st.listener[3]);
            g_palette->SetMono(st.mono);
            g_palette->SetFootstepGain(st.stepGain);
            g_palette->SetAmbientEnabled(st.ambient);
            g_palette->SetGait(st.gait, st.ground);
            if (st.ambient) g_worldIdle = false; // the scheduler may place something this bar
        }
        for (int i = 0; i < n; i++) {
            const WorldCmd& c = cmds[i];
            if (c.type == WC_PLAY) { g_palette->Play(c.cue); g_worldIdle = false; }
            else if (c.type == WC_RELEASE) g_palette->Release(c.cue.id);
            else { g_palette->FadeOut(c.seconds); g_palette->SetAmbientEnabled(false); }
        }
        PumpWorldSound();
        lk.lock();
        g_worldWake.wait_for(lk, std::chrono::seconds(1),
                             [] { return g_worldQuit || g_worldCmdCount > 0 || g_worldUrgent || g_worldKick; });
    }
}

// Main thread: posting is a copy under the lock, never a render.
static void PostWorld(WorldCmdType type, const SoundCue& cue, float seconds) {
    if (!g_palette) return;
    {
        std::lock_guard<std::mutex> lk(g_worldLock);
        int cap = type == WC_PLAY ? WORLD_MAILBOX_CUES : WORLD_MAILBOX;
        if (g_worldCmdCount >= cap) return; // past the cap: dropped (see WORLD_MAILBOX)
        g_worldCmds[g_worldCmdCount++] = { type, cue, seconds };
    }
    g_worldWake.notify_one();
}

void PlayWorldSound(const SoundCue& cue) { PostWorld(WC_PLAY, cue, 0.0f); }
void ReleaseWorldSound(SoundId id) { SoundCue c; c.id = id; PostWorld(WC_RELEASE, c, 0.0f); }
void FadeWorldSounds(float seconds) {
    if (!g_palette) return;
    // The state too: until the next frame posts, the worker must not
    // re-enable the scheduler from the last playing frame's state.
    { std::lock_guard<std::mutex> lk(g_worldLock); g_worldState.ambient = false; }
    PostWorld(WC_FADE, SoundCue(), seconds);
}
void SetWorldGait(int gait, SoundMaterial ground) {
    if (!g_palette) return;
    bool changed;
    {
        std::lock_guard<std::mutex> lk(g_worldLock);
        changed = gait != g_worldState.gait || ground != g_worldState.ground;
        g_worldState.gait = gait; g_worldState.ground = ground;
        if (changed) g_worldStateNew = g_worldUrgent = true; // a first step shouldn't wait for a buffer
    }
    if (changed) g_worldWake.notify_one();
}

void UpdateWorldSound(const SoundAxes& axes, const AmbientScene& scene, bool playing, const float listener[4]) {
    bool musicRunning;
    {
        std::lock_guard<std::mutex> lk(g_musicLock);
        g_musicColour.positive = axes.positive;
        g_musicColour.activity = axes.activity;
        g_musicColour.mechanical = axes.mechanical;
        musicRunning = g_nextChunkStartTime >= 0.0;
    }
    if (!g_palette) return;
    bool live = playing && musicRunning, urgent;
    {
        std::lock_guard<std::mutex> lk(g_worldLock);
        WorldState& w = g_worldState;
        urgent = live != w.ambient || g_monoAudio != w.mono; // play starting or stopping is heard at once
        w.axes = axes; w.scene = scene; w.intensity = g_musicIntensity;
        for (int i = 0; i < 4; i++) w.listener[i] = listener[i];
        w.mono = g_monoAudio; w.ambient = live; w.stepGain = g_footstepVolume;
        g_worldStateNew = true;
        if (urgent) g_worldUrgent = true;
    }
    if (urgent) g_worldWake.notify_one();
}
