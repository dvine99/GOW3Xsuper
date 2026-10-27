#include "spartanport_frame_pacing.h"
#include <cassert>
#include <limits>

int main() {
    SpartanFramePacing::Window window;
    assert(window.Take().frames == 0);
    window.Add(0);
    window.Add(-1);
    window.Add(std::numeric_limits<double>::infinity());
    window.Add(std::numeric_limits<double>::quiet_NaN());
    assert(window.Take().frames == 0);
    for (int i = 0; i < 95; ++i) window.Add(16.5);
    for (int i = 0; i < 4; ++i) window.Add(50);
    window.Add(2500); // A multi-second loading hitch must not be discarded.
    auto s = window.Take();
    assert(s.frames == 100 && s.over50 == 1 && s.over100 == 1);
    assert(s.total_ms == 4267.5 && s.max_ms == 2500);
    assert(s.p95_ms == 17 && s.p99_ms == 50);
    assert(s.Mean() == 42.675 && s.Fps() > 23.43 && s.Fps() < 23.44);
    assert(window.Take().frames == 0); // Windows never overlap.
    window.Add(2000);
    window.Add(3000);
    s = window.Take();
    assert(s.p95_ms == 3000 && s.p99_ms == 3000);
    window.Add(100);
    window.Add(100.1);
    s = window.Take();
    assert(s.over50 == 2 && s.over100 == 1);
    // Sampling remains complete past the old 120-frame ring buffer's capacity.
    for (int i = 0; i < 10000; ++i) window.Add(20);
    s = window.Take();
    assert(s.frames == 10000 && s.Fps() == 50 && s.p99_ms == 20);
}
