// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace SpartanFramePacing {
// Single present-thread accumulator. Bounded storage, no allocations or sorting
// on the frame path. Percentiles round UP to 1 ms; the overflow bucket uses max.
struct Sample {
    uint64_t frames{}, over50{}, over100{};
    double total_ms{}, max_ms{}, p95_ms{}, p99_ms{};
    double Mean() const { return frames ? total_ms / frames : 0; }
    double Fps() const { return total_ms > 0 ? frames * 1000.0 / total_ms : 0; }
};
class Window {
public:
    void Add(double milliseconds) {
        if (!std::isfinite(milliseconds) || milliseconds <= 0) return;
        ++sample.frames;
        sample.total_ms += milliseconds;
        sample.max_ms = std::max(sample.max_ms, milliseconds);
        sample.over50 += milliseconds > 50;
        sample.over100 += milliseconds > 100;
        const auto bucket = static_cast<size_t>(std::min(std::ceil(milliseconds), double(Buckets - 1)));
        ++histogram[bucket];
    }
    Sample Take() {
        Sample result = sample;
        result.p95_ms = Percentile(95);
        result.p99_ms = Percentile(99);
        sample = {};
        histogram.fill(0);
        return result;
    }
private:
    double Percentile(uint64_t percent) const {
        if (!sample.frames) return 0;
        const uint64_t rank = (sample.frames * percent + 99) / 100;
        uint64_t count = 0;
        for (size_t i = 0; i < Buckets; ++i) {
            count += histogram[i];
            if (count >= rank)
                return i == Buckets - 1 ? sample.max_ms : double(i);
        }
        return sample.max_ms;
    }
    static constexpr size_t Buckets = 2001;
    std::array<uint64_t, Buckets> histogram{};
    Sample sample{};
};
} // namespace SpartanFramePacing
