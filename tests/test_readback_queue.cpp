#include <cstdint>
#include <cassert>
#include <vector>
#include "video_core/texture_cache/readback_queue.h"

int main() {
    VideoCore::ReadbackQueue<int> queue;
    std::vector<int> published;
    auto complete = [&](auto& entry) {
        if (entry.valid) published.push_back(entry.payload);
    };
    queue.Push(0x1000, 4096, 3, 1);
    queue.Push(0x1000, 4096, 4, 2);
    assert(queue.RequiredTick(0x1fff, 8) == 4);
    assert(queue.RequiredTick(0x2000, 8) == 0);
    queue.Retire([](auto tick) { return tick <= 2; }, complete);
    assert(published.empty());
    queue.Retire([](auto tick) { return tick <= 3; }, complete);
    assert((published == std::vector<int>{1}));
    unsigned canceled = 0;
    queue.Cancel(0x1001, 1, [&](auto&) { ++canceled; });
    queue.Cancel(0x1001, 1, [&](auto&) { ++canceled; });
    assert(canceled == 1);
    queue.Push(0x1000, 4096, 5, 3);
    queue.Retire([](auto) { return true; }, complete);
    assert((published == std::vector<int>{1, 3}));
    assert(queue.Bytes() == 0);
    assert(!queue.CanFit(65ull << 20));
    for (int i = 0; i < 64; ++i) queue.Push(0x1000, 1, 6, i);
    assert(!queue.CanFit(1));
    assert(queue.RequiredTick(0x1ff0, 1) == 6);
    assert(queue.RequiredTick(0x3000, 1) == 0);
    queue.Retire([](auto) { return true; }, [](auto&) {});
    queue.Push(0x1000, 64ull << 20, 7, 4);
    assert(!queue.CanFit(1));
    assert(queue.RequiredTick(0x1000, 0) == 0);
    queue.Retire([](auto) { return true; }, [](auto&) {});
    assert(queue.CanFit(64ull << 20));

    // Page protection cannot be released for a neighboring write: the CPU could
    // next write inside the copy without another fault, losing its update on retirement.
    queue.Push(0x10000c00, 4, 8, 5);
    assert(queue.RequiredTick(0x10000100, 8) == 8);
    assert(queue.RequiredTick(0x10000c00, 4) == 8);
    int backing = 0;
    queue.Retire([](auto tick) { return tick <= 7; }, [&](auto&) { backing = 5; });
    assert(backing == 0 && queue.RequiredTick(0x10000100, 8) == 8);
    // The fault handler waits for tick 8 before allowing either CPU write.
    queue.Retire([](auto tick) { return tick <= 8; }, [&](auto& entry) {
        assert(entry.valid && entry.watched);
        backing = entry.payload;
    });
    assert(backing == 5);
    backing = 99;
    queue.Retire([](auto) { return true; }, [&](auto& entry) { backing = entry.payload; });
    assert(backing == 99 && queue.RequiredTick(0x10000c00, 4) == 0);
}
