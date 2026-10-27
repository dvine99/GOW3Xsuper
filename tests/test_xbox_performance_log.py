import math
import unittest

import paths  # noqa: F401
from analyze_xbox_performance import summarize


class PerformanceLogTest(unittest.TestCase):
    def test_weighted_fps_and_worst_window_quantiles(self):
        result = summarize([
            'UWP metrics: t=5 fps=60 cpu=40 ram=4000 gpu=1000 reserved=1100 frames=300 total_ms=5000 p95_ms=17 p99_ms=30 max_ms=45 over50=0 over100=0',
            'UWP metrics: t=10 fps=10 cpu=10 ram=4100 gpu=1200 reserved=1300 frames=50 total_ms=5000 p95_ms=100 p99_ms=200 max_ms=500 over50=10 over100=2',
        ])
        self.assertEqual(result['fps'], 35)
        self.assertEqual(result['frames'], 350)
        self.assertTrue(math.isclose(result['mean_frame_ms'], 10000 / 350))
        self.assertEqual(result['worst_window_p99_ms'], 200)
        self.assertEqual(result['peak_app_mib'], 4100)
        self.assertEqual(result['peak_process_cpu_percent'], 40)
        self.assertEqual(result['frames_over_100ms'], 2)

    def test_waits_are_grouped_and_ranked(self):
        result = summarize([
            'Wait profile: cache.cpp:12; 3 calls, CPU submit 5.000 ms, semaphore wait 70.000 ms, max wait 40.000 ms',
            'Wait profile: cache.cpp:12; 2 calls, CPU submit 6.000 ms, semaphore wait 10.000 ms, max wait 10.000 ms',
            'Wait profile: present.cpp:1; 1 calls, CPU submit 1.000 ms, semaphore wait 2.000 ms, max wait 2.000 ms',
        ])
        row = result['wait_sites'][0]
        self.assertEqual(row['site'], 'cache.cpp:12')
        self.assertEqual(row['calls'], 5)
        self.assertEqual(row['semaphore_wait_ms'], 80)
        self.assertEqual(row['max_wait_ms'], 40)
        self.assertIsNone(result['fps'])

    def test_old_empty_and_invalid_samples_cannot_claim_fps(self):
        result = summarize([
            'unrelated log line',
            'UWP metrics: fps=0 ms=0 ram=4096',
            'UWP metrics: t=0 frames=0 total_ms=0 p95_ms=0 p99_ms=0 max_ms=0 over50=0 over100=0',
            'UWP metrics: t=5 frames=1 total_ms=0 p95_ms=0 p99_ms=0 max_ms=0 over50=0 over100=0',
        ])
        self.assertIsNone(result['fps'])
        self.assertIsNone(result['peak_process_cpu_percent'])
        self.assertEqual(result['legacy_metric_lines'], 1)
        self.assertEqual(result['malformed_lines'], 1)
        self.assertEqual(result['intervals'], 0)


if __name__ == '__main__':
    unittest.main()
