#!/usr/bin/env python3
"""Summarize an Xbox loader.log; no game assets or third-party packages needed."""
import argparse
import json
import math
from pathlib import Path
import re

METRICS = re.compile(r"UWP metrics: (.*)")
FIELD = re.compile(r"([a-zA-Z0-9_]+)=(-?\d+(?:\.\d+)?)")
WAIT = re.compile(
    r"Wait profile: (.+); (\d+) calls, CPU submit ([\d.]+) ms, "
    r"semaphore wait ([\d.]+) ms, max wait ([\d.]+) ms"
)
REQUIRED = {"t", "frames", "total_ms", "max_ms", "p95_ms", "p99_ms", "over50", "over100"}


def summarize(lines):
    samples, waits = [], {}
    legacy_samples = malformed = 0
    for line in lines:
        if match := METRICS.search(line):
            row = {key: float(value) for key, value in FIELD.findall(match[1])}
            if not REQUIRED <= row.keys():
                legacy_samples += 1
                continue
            if (not all(math.isfinite(value) and value >= 0 for key, value in row.items() if key != "cpu")
                    or row["frames"] != int(row["frames"])
                    or row["over100"] > row["over50"] or row["over50"] > row["frames"]
                    or (row["frames"] > 0 and row["total_ms"] <= 0)):
                malformed += 1
                continue
            if row["frames"]:
                samples.append(row)
        elif match := WAIT.search(line):
            site, calls, submit, wait, maximum = match.groups()
            calls, submit, wait, maximum = int(calls), float(submit), float(wait), float(maximum)
            if not all(math.isfinite(value) for value in (submit, wait, maximum)):
                malformed += 1
                continue
            entry = waits.setdefault(site, {"site": site, "calls": 0, "cpu_submit_ms": 0,
                                           "semaphore_wait_ms": 0, "max_wait_ms": 0})
            entry["calls"] += calls
            entry["cpu_submit_ms"] += submit
            entry["semaphore_wait_ms"] += wait
            entry["max_wait_ms"] = max(entry["max_wait_ms"], maximum)
    frames = sum(s["frames"] for s in samples)
    elapsed = sum(s["total_ms"] for s in samples)
    peak = lambda key: max((s[key] for s in samples if key in s), default=None)
    return {
        "intervals": len(samples), "frames": int(frames), "measured_ms": elapsed,
        "fps": frames * 1000 / elapsed if elapsed else None,
        "mean_frame_ms": elapsed / frames if frames else None,
        "max_frame_ms": peak("max_ms"),
        # Window quantiles cannot be averaged into a run-wide percentile.
        "worst_window_p95_ms": peak("p95_ms"), "worst_window_p99_ms": peak("p99_ms"),
        "frames_over_50ms": int(sum(s["over50"] for s in samples)),
        "frames_over_100ms": int(sum(s["over100"] for s in samples)),
        "peak_process_cpu_percent": max((s["cpu"] for s in samples if 0 <= s.get("cpu", -1) <= 100), default=None),
        "peak_app_mib": peak("ram"), "peak_gpu_allocated_mib": peak("gpu"),
        "peak_gpu_reserved_mib": peak("reserved"),
        "legacy_metric_lines": legacy_samples, "malformed_lines": malformed,
        "wait_sites": sorted(waits.values(), key=lambda s: s["cpu_submit_ms"] + s["semaphore_wait_ms"], reverse=True),
        "notes": ["Present intervals include loading and menus; compare the same save, scene and duration.",
                  "Percentiles are rounded-up per-window estimates, not whole-run percentiles.",
                  "Wait times describe CPU waits, not GPU execution time or GPU utilization.",
                  "GPU allocations are part of Xbox unified memory; do not add them to app RAM."],
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path, help="loader.log downloaded from the app")
    args = parser.parse_args()
    try:
        with args.log.open(encoding="utf-8", errors="replace") as stream:
            result = summarize(stream)
    except OSError as error:
        parser.exit(2, f"Cannot read log: {error}\n")
    print(json.dumps(result, indent=2, allow_nan=False))
    if not result["intervals"] and not result["wait_sites"]:
        parser.exit(1, "No usable profiling samples. Enable frame pacing logs, restart and record gameplay.\n")


if __name__ == "__main__":
    main()
