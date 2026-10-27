// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <array>
#include <atomic>
#include <cstdio>
#include <string_view>

// Shared by the Xbox bootstrap, settings persistence and in-game menu. These
// switches select existing renderer paths; changing them requires a new process.
namespace SpartanXboxPerformance {
struct Option {
    const char* key;
    const char* environment;
    const char* label;
    const char* help;
    int initial;
    int count;
    std::array<const char*, 3> labels;
    std::array<const char*, 3> values;
};
inline constexpr Option Options[] = {
    {"xbox_record_thread", "SPARTAN_VK_RECORD_THREAD", "Threaded command recording",
     "Records graphics commands on a separate CPU thread.", 1, 2, {"Off", "On"}, {"0", "1"}},
    {"xbox_draw_pipeline", "SPARTAN_DRAW_PIPE", "Pipelined draw preparation",
     "Overlaps draw preparation and recording while preserving draw order.", 1, 2, {"Off", "On"}, {"0", "1"}},
    {"xbox_copy_threads", "SPARTAN_COPY_THREADS", "Memory copy workers",
     "Copies guest buffers on up to two CPU workers. More workers may add contention.", 2, 3,
     {"Off", "1", "2"}, {"0", "1", "2"}},
    {"xbox_pipeline_cache", "SPARTAN_PIPELINE_CACHE", "Persistent pipeline cache",
     "Reuses saved pipelines between launches. Turning it off does not delete saved files.", 1, 2,
     {"Off", "On"}, {"0", "1"}},
    {"xbox_pipeline_binaries", "SPARTAN_PIPELINE_BINARIES", "Driver pipeline binaries",
     "Reuses driver binaries when supported. Otherwise the renderer uses its normal cache.", 1, 2,
     {"Off", "On"}, {"0", "1"}},
    {"xbox_baked_pipelines", "SPARTAN_BAKED_PIPELINES", "Prepare graphics state in advance",
     "Prepares Dozen pipeline state before drawing to reduce first-use stalls.", 1, 2,
     {"Off", "On"}, {"0", "1"}},
    {"xbox_parallel_warmup", "SPARTAN_PARALLEL_WARMUP", "Parallel shader cache loading",
     "Uses multiple CPU workers to prepare saved pipelines before gameplay.", 1, 2,
     {"Off", "On"}, {"0", "1"}},
    {"xbox_stream_memory", "SPARTAN_STREAM_MEMORY", "Upload buffer memory",
     "Cached is the Xbox default. Uncached is an advanced comparison for buffer upload stalls.", 0, 2,
     {"Cached", "Uncached"}, {"cached", "host"}},
    {"xbox_metrics", "SPARTAN_UWP_METRICS", "Log frame pacing and memory",
     "Logs FPS, slow frames, CPU use and GPU allocations every five seconds. Adds profiling overhead.", 0, 2,
     {"Off", "On"}, {"", "1"}},
    {"xbox_frame_stats", "SPARTAN_FRAME_STATS", "Log CPU frame stalls",
     "Reports compilation, command recording and synchronization waits. Adds profiling overhead.", 0, 2,
     {"Off", "On"}, {"", "1"}},
    {"xbox_io_stats", "SPARTAN_IO_STATS", "Log resource loading",
     "Reports file reads, guest copies and paging; also enables frame pacing logs.", 0, 2,
     {"Off", "On"}, {"", "1"}},
    {"xbox_recorder_timings", "SPARTAN_RECORDER_TIMINGS", "Log slow graphics commands",
     "Reports CPU recording commands taking at least 10 ms. This does not measure GPU execution time.", 0, 2,
     {"Off", "On"}, {"", "1"}},
    {"xbox_wait_diagnostics", "SPARTAN_PERF_DIAG", "Log CPU/GPU synchronization",
     "Reports CPU submission and semaphore waits; also enables CPU frame stall logs. No GPU timestamps.", 0, 2,
     {"Off", "On"}, {"", "1"}},
};
inline constexpr size_t Count = std::size(Options);

inline int Parse(const Option& option, std::string_view value) {
    // Reject malformed or out-of-range saved values instead of silently enabling a path.
    if (value.size() == 1 && value[0] >= '0' && value[0] < '0' + option.count)
        return value[0] - '0';
    return option.initial;
}

struct Settings {
    std::array<std::atomic<int>, Count> selected{};
    std::array<int, Count> startup{};
    Settings() {
        for (size_t i = 0; i < Count; ++i) selected[i] = startup[i] = Options[i].initial;
    }
    bool Set(std::string_view key, std::string_view value) {
        for (size_t i = 0; i < Count; ++i) {
            if (key == Options[i].key) {
                selected[i] = Parse(Options[i], value);
                return true;
            }
        }
        return false;
    }
    template <class GetEnvironment>
    void CaptureStartup(GetEnvironment get) {
        for (size_t i = 0; i < Count; ++i) {
            if (const char* value = get(Options[i].environment)) {
                for (int choice = 0; choice < Options[i].count; ++choice) {
                    if (std::string_view(value) == Options[i].values[choice] ||
                        (choice == 0 && Options[i].values[0][0] == '\0' && std::string_view(value) == "0"))
                        selected[i] = choice;
                }
            }
            startup[i] = selected[i].load();
        }
    }
    int Changes() const {
        int count = 0;
        for (size_t i = 0; i < Count; ++i) count += selected[i] != startup[i];
        return count;
    }
    void Save(FILE* file) const {
        for (size_t i = 0; i < Count; ++i)
            std::fprintf(file, "%s=%d\n", Options[i].key, selected[i].load());
    }
};

// Call before renderer initialization. Private graphics-debug.txt / environment
// overrides win over saved choices. Empty diagnostic values REMOVE the variable:
// several legacy counters test presence, so setting them to "0" would enable work.
template <class GetSetting, class GetEnvironment, class SetEnvironment>
void Apply(GetSetting setting, GetEnvironment get, SetEnvironment set) {
    for (const auto& option : Options) {
        if (!get(option.environment)) {
            const int value = Parse(option, setting(option.key));
            set(option.environment, option.values[value]);
        }
    }
    // These reports are emitted by the corresponding periodic reporter. An
    // explicit private override of "0" still wins, including for dependencies.
    if (const char* waits = get("SPARTAN_PERF_DIAG"); waits && waits[0] == '1' && !get("SPARTAN_FRAME_STATS"))
        set("SPARTAN_FRAME_STATS", "1");
    if (const char* io = get("SPARTAN_IO_STATS"); io && io[0] == '1' && !get("SPARTAN_UWP_METRICS"))
        set("SPARTAN_UWP_METRICS", "1");
}
} // namespace SpartanXboxPerformance
