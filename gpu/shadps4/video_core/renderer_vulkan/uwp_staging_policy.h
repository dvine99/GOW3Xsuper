// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <algorithm>
#include <cstdint>

namespace Vulkan {

struct UwpStagingPolicy {
    std::uint64_t keep_blocks;
    std::uint64_t idle_frames;
};

// Scene loads have previously increased app usage by 570 MiB in five seconds.
// Keep the existing 1280 MiB headroom used by the texture collector.
constexpr UwpStagingPolicy SelectUwpStagingPolicy(std::uint64_t usage, std::uint64_t limit,
                                                   std::uint64_t requested_blocks) {
    constexpr std::uint64_t headroom = 1280ull * 1024 * 1024;
    const bool pressured = limit == 0 || usage >= limit || limit - usage <= headroom;
    return {pressured ? std::min<std::uint64_t>(requested_blocks, 4) : requested_blocks,
            pressured ? 120u : 600u};
}

} // namespace Vulkan
