// jobs.cpp -- see jobs.h (FOUNDATIONS.md 3). Layer 1.
//
// One shared queue of work, one finished-queue per channel. The job
// threads take work in submission order; nothing here knows what the
// work is.

#include "jobs.h"
#include <algorithm>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

namespace {

struct Job {
    JobChannel ch;
    std::function<void()> work, apply;
};

std::mutex g_lock;
std::condition_variable g_wake, g_idle;
std::deque<Job> g_waiting;
std::deque<std::function<void()>> g_finished[JOB_CHANNEL_COUNT];
int g_outstanding[JOB_CHANNEL_COUNT] = {};
int g_running = 0;
bool g_quit = false;
std::vector<std::thread> g_threads;

void Worker() {
    std::unique_lock<std::mutex> lk(g_lock);
    for (;;) {
        g_wake.wait(lk, [] { return g_quit || !g_waiting.empty(); });
        if (g_quit) return;
        Job j = std::move(g_waiting.front());
        g_waiting.pop_front();
        g_running++;
        lk.unlock();
        j.work();
        lk.lock();
        g_running--;
        g_finished[j.ch].push_back(std::move(j.apply));
        if (g_waiting.empty() && g_running == 0) g_idle.notify_all();
    }
}

} // namespace

void JobsStart() {
    if (!g_threads.empty()) return;
    unsigned cores = std::thread::hardware_concurrency(); // D21: a limit to respect, never kept
    int n = std::max(1, std::min(4, (int)cores - 2));
    g_quit = false;
    for (int i = 0; i < n; i++) g_threads.emplace_back(Worker);
}

void JobsStop() {
    {
        std::lock_guard<std::mutex> lk(g_lock);
        g_quit = true;
    }
    g_wake.notify_all();
    for (auto& t : g_threads) t.join();
    g_threads.clear();
    std::lock_guard<std::mutex> lk(g_lock);
    g_waiting.clear();
    for (auto& f : g_finished) f.clear();
    for (int& o : g_outstanding) o = 0;
}

int JobsThreadCount() { return (int)g_threads.size(); }

void JobsSubmit(JobChannel ch, std::function<void()> work, std::function<void()> apply) {
    if (g_threads.empty()) { // inline (tests): the work now, the apply when asked
        work();
        g_finished[ch].push_back(std::move(apply));
        g_outstanding[ch]++;
        return;
    }
    {
        std::lock_guard<std::mutex> lk(g_lock);
        g_waiting.push_back({ ch, std::move(work), std::move(apply) });
        g_outstanding[ch]++;
    }
    g_wake.notify_one();
}

int JobsApply(JobChannel ch, int maxCount) {
    int done = 0;
    while (done < maxCount) {
        std::function<void()> apply;
        {
            std::lock_guard<std::mutex> lk(g_lock);
            if (g_finished[ch].empty()) break;
            apply = std::move(g_finished[ch].front());
            g_finished[ch].pop_front();
            g_outstanding[ch]--;
        }
        apply(); // outside the lock: it may submit more work
        done++;
    }
    return done;
}

int JobsOutstanding(JobChannel ch) {
    std::lock_guard<std::mutex> lk(g_lock);
    return g_outstanding[ch];
}

void JobsWaitIdle() {
    std::unique_lock<std::mutex> lk(g_lock);
    g_idle.wait(lk, [] { return g_waiting.empty() && g_running == 0; });
}
