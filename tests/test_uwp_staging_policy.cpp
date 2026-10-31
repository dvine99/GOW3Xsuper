// SPDX-License-Identifier: GPL-2.0-or-later
#include <cassert>
#include "video_core/renderer_vulkan/uwp_staging_policy.h"

int main() {
    constexpr std::uint64_t mib = 1024 * 1024;
    using Vulkan::SelectUwpStagingPolicy;
    auto roomy = SelectUwpStagingPolicy(3000 * mib, 5120 * mib, 8);
    assert(roomy.keep_blocks == 8 && roomy.idle_frames == 600);
    auto boundary = SelectUwpStagingPolicy(3840 * mib, 5120 * mib, 8);
    assert(boundary.keep_blocks == 4 && boundary.idle_frames == 120);
    assert(SelectUwpStagingPolicy(5121 * mib, 5120 * mib, 8).keep_blocks == 4);
    assert(SelectUwpStagingPolicy(0, 0, 8).keep_blocks == 4);
    assert(SelectUwpStagingPolicy(4000 * mib, 5120 * mib, 2).keep_blocks == 2);
}
