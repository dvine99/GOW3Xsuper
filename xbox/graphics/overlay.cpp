// SPDX-License-Identifier: GPL-2.0-or-later
#include "overlay.h"
#include "spartanport_frame_pacing.h"
extern "C" void runtime_file_profile_report(void);
#include "video_core/renderer_vulkan/vk_instance.h"
#include <vk_mem_alloc.h>
#include <windows.h>
#include <winrt/Windows.System.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

namespace Gow3Uwp::Overlay {
namespace {
using Clock=std::chrono::steady_clock;
const auto started=Clock::now();
SpartanFramePacing::Window frame_times;
Clock::time_point last_frame{};
bool MetricsEnabled() {
    // UWP loads graphics-debug.txt and the saved menu after CRT initialization.
    static const bool metrics=[] { auto value=std::getenv("SPARTAN_UWP_METRICS");return value&&*value=='1'; }();
    return metrics;
}
}
bool Visible() { return MetricsEnabled(); }
void Presented() {
    if(!MetricsEnabled()) return;
    const auto now=Clock::now();
    if(last_frame!=Clock::time_point{}) {
        frame_times.Add(std::chrono::duration<double,std::milli>(now-last_frame).count());
    }
    last_frame=now;
}
void Render(const Vulkan::Instance& instance) {
    if(!MetricsEnabled()) return;
    thread_local bool apartment=false;
    if(!apartment) {
        try {winrt::init_apartment(winrt::apartment_type::multi_threaded);}
        catch(winrt::hresult_error const&) {}
        apartment=true;
    }
    const auto now=Clock::now();
    static auto reported=Clock::time_point{},sampled=Clock::time_point{};
    if(now-reported<std::chrono::seconds(5)) return;
    static uint64_t cpu_time=0;
    uint64_t used=0,limit=0;
    double cpu=-1;
    try {
        used=winrt::Windows::System::MemoryManager::AppMemoryUsage();
        limit=winrt::Windows::System::MemoryManager::AppMemoryUsageLimit();
    } catch(winrt::hresult_error const&) {}
    FILETIME created{},exited{},kernel{},user{};
    if(GetProcessTimes(GetCurrentProcess(),&created,&exited,&kernel,&user)) {
        auto ticks=[](FILETIME t) { return (uint64_t(t.dwHighDateTime)<<32)|t.dwLowDateTime; };
        const auto total=ticks(kernel)+ticks(user);
        if(sampled!=Clock::time_point{}) {
            const double wall=std::chrono::duration<double>(now-sampled).count()*1e7*
                std::max(1u,std::thread::hardware_concurrency());
            cpu=std::clamp(double(total-cpu_time)/wall*100,0.0,100.0);
        }
        cpu_time=total;
        sampled=now;
    }
    // Heap counters avoid walking every allocation to produce a diagnostic sample.
    std::array<VmaBudget,VK_MAX_MEMORY_HEAPS> budgets{};
    vmaGetHeapBudgets(instance.GetAllocator(),budgets.data());
    uint64_t allocated=0,reserved=0;
    for(const auto& budget:budgets) {
        allocated+=budget.statistics.allocationBytes;
        reserved+=budget.statistics.blockBytes;
    }
    const auto pacing=frame_times.Take();
    std::printf("UWP metrics: t=%lld fps=%.1f ms=%.1f cpu=%.1f ram=%llu limit=%llu gpu=%llu reserved=%llu MiB "
        "frames=%llu total_ms=%.3f p95_ms=%.1f p99_ms=%.1f max_ms=%.1f over50=%llu over100=%llu\n",
        static_cast<long long>(std::chrono::duration_cast<std::chrono::seconds>(now-started).count()),
        pacing.Fps(),pacing.Mean(),cpu,
        static_cast<unsigned long long>(used>>20),static_cast<unsigned long long>(limit>>20),
        static_cast<unsigned long long>(allocated>>20),static_cast<unsigned long long>(reserved>>20),
        static_cast<unsigned long long>(pacing.frames),pacing.total_ms,pacing.p95_ms,pacing.p99_ms,pacing.max_ms,
        static_cast<unsigned long long>(pacing.over50),static_cast<unsigned long long>(pacing.over100));
    reported=now;
    runtime_file_profile_report();
}
}
