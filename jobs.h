// jobs.h
//
// A small, fixed pool of job threads (FOUNDATIONS.md 3; DESIGN.md Part
// XXII): terrain generation and mesh building run here from copies, and
// their results are applied on the main thread, which alone writes the
// world and calls D3D11.
//
//   JobsSubmit(channel, work, apply)  work runs on a job thread;
//                                     apply is queued for the main thread
//   JobsApply(channel, max)           main thread: runs up to `max` applies
//
// A job's apply must check that its result still belongs (the world may
// have been reset, the chunk edited again): results carry a version and
// stale ones are dropped by the caller (world.cpp, the mesh queue).
//
// Size: from the processor count (a harmless read, D21), leaving two
// cores for the main and audio threads: clamp(cores - 2, 1, 4). Never
// recorded, stored or shown beyond the F3 overlay's own count. Without
// JobsStart (the native tests), work runs inline on submit.
//
// Layer 1 (base). Cost: a lock and a queue push per submit; the threads
// sleep when there's nothing to do.

#pragma once

#include <functional>

enum JobChannel { JOB_TERRAIN = 0, JOB_MESH, JOB_CHANNEL_COUNT };

void JobsStart();
void JobsStop();
int JobsThreadCount();          // 0 when not started (inline mode)

void JobsSubmit(JobChannel ch, std::function<void()> work, std::function<void()> apply);
// Main thread: applies up to `maxCount` finished jobs of this channel, in
// the order they finished. Returns how many it applied.
int JobsApply(JobChannel ch, int maxCount);
// Submitted and not yet applied (waiting, running or finished).
int JobsOutstanding(JobChannel ch);
// Blocks until every submitted job's work has finished (not applied).
void JobsWaitIdle();
