# Xbox performance — GOW3Xsuper controls and diagnostics

[Brazilian Portuguese version](PERFORMANCE.pt-BR.md)

**Current distribution: Beta 0 (1.0.26300.308).** The .436 release was deleted.
The .436 first-start preparation section below records a withdrawn experiment;
it does not describe the Beta 0 binary. See the [complete report](../MODIFICATIONS.md).

Baseline: `alpha-1.0.26299.2139`, commit
`fe8c66ed3a9bf151557c8f0af57009bdc955b042` from `Cosmo/gow3_xbox`.

This work integrates existing performance controls into the menu and corrects
metric collection. There is no gameplay benchmark for these changes: the Linux
sandbox has no Xbox, Windows SDK, running Dozen instance or game dump/save.
No FPS improvement has been measured. Downstream versions .308/.436 have built,
signed and verified Windows/UWP packages, but only Beta 0 (.308) remains in
[Releases](../RELEASES.md); console validation is separate.

## Menu

During gameplay, press **RS + LT** (or **Insert**) and open **Performance**.
The **Advanced performance - next start** section contains:

| Option | Alpha 6 default | Effect |
| --- | --- | --- |
| Threaded command recording | On | Record Vulkan commands on another thread. |
| Pipelined draw preparation | On | Overlap preparation and recording, preserving order. |
| Memory copy workers | 2 | Use 0, 1 or 2 workers for buffer copies. |
| Persistent pipeline cache | On | Reuse pipelines between launches. Disabling it does not delete files. |
| Driver pipeline binaries | On | Reuse binaries when supported by the driver. |
| Prepare graphics state in advance | On | Prepare Dozen pipeline state before draws. |
| Parallel shader cache loading | On | Load saved pipelines using multiple workers before gameplay. |
| Upload buffer memory | Cached | Uncached is an alternative for transfer comparisons, not model streaming. |
| Log frame pacing and memory | Off | Frame intervals, CPU and GPU allocations every five seconds. |
| Log CPU frame stalls | Off | Compilation, recording and wait times. |
| Log resource loading | Off | File reads, copies and paging; also enables frame pacing logs. |
| Log slow graphics commands | Off | Commands whose CPU recording takes at least 10 ms. |
| Log CPU/GPU synchronization | Off | Semaphore and submission waits; also enables stall logs. |

Close the menu to save. Close and reopen the app to apply these options. The
menu shows pending changes. `LocalState/user/graphics.cfg` stores choices under
`xbox_*` keys. Invalid values fall back to that option's default.
Private `graphics-debug.txt` settings take precedence, including when they
disable diagnostics required by another report. An override outside the menu's
choices (for example, four workers) remains valid; remove it to control that
option through the menu.

Asynchronous compilation of new shaders, queued frames, resolution, FPS limit,
deferred readbacks and previous-frame readbacks were already in the menu.
**Precompile shaders** already exists in the launcher and covers only the
supported shader subset; it does not eliminate all compilation during gameplay.
No aggressive new defaults are introduced: selections retain the Alpha 6 profile
and new diagnostics are disabled by default.

## Diagnostic corrections and overhead

- Logging now checks whether it is enabled after Xbox settings are read.
  Previously, a global variable could capture the absence of `SPARTAN_UWP_METRICS`
  during process initialization.
- The presentation path calls the interval collector. Previously, `Presented()`
  had no callers, so the log received no frame samples.
- Each reporting window records all intervals: count, sum, mean, maximum,
  p95/p99 estimates and counts above 50/100 ms. Pauses longer than one second
  are retained. The histogram has a fixed size and no per-frame allocation;
  percentiles round up in 1 ms increments. In the 2,000 ms-and-above bucket,
  the observed maximum is used as an upper bound.
- VMA heap counters replace the previous report's full allocation scan. This
  reduces profiling work itself; it is not evidence of a gameplay FPS increase.
- `cpu=-1` means no sample is available. CPU is process utilization normalized
  by logical thread count. `gpu` and `reserved` are memory amounts, not GPU
  utilization percentages or execution times. On Xbox they are part of unified
  memory and must not be added to the app's RAM usage.

## Collecting and comparing results

1. Enable **Log frame pacing and memory** and **Log CPU frame stalls**. To
   investigate waits, also enable **Log CPU/GPU synchronization**. Use resource
   read and slow-command logs only as needed: instrumentation adds overhead.
2. Restart, load the same save and reproduce the same camera, route and combat
   over equal intervals for each configuration. Identify the first run (cold
   cache) and subsequent run (warm cache) separately.
3. Download the app's `loader.log` through Device Portal. Use the existing
   `xbox/scripts` tools with your local device configuration.
4. From the repository root, run:

   ```bash
   python3 scripts/analyze_xbox_performance.py /path/to/loader.log > performance.json
   ```

   The report calculates FPS as total frames divided by measured time, groups
   waits by location and shows peak memory and slow-frame counts. Reported
   percentiles are the worst **per window**; averaging percentiles across
   windows cannot yield a percentile for the entire run. Old logs without the
   new fields are counted but not used to fabricate measurements. With no
   usable samples, the command exits with code 1.
5. Logs include menus and loading. To compare gameplay, supply a bounded segment
   of the same scene from both logs, including complete reporting windows.
   Change one option at a time. To measure final FPS, repeat with detailed
   diagnostics disabled.

Long waits, synchronous compilation and recording cost are **investigation
clues**. Alone, they do not prove GPU saturation. The original `xbox/README.md`
records a hang in an attempt using GPU timestamps; this work does not re-enable
that technique. Actual GPU execution profiling still requires tools compatible
with the console/driver and a reproduction on the device.

## Coverage of the requested work

| Area | Status in this work |
| --- | --- |
| CPU/GPU profiling | CPU/frame pacing collection corrected; GPU memory and wait logs exposed; analyzer added. Actual GPU execution profiling remains unverified. |
| Redundant shader calculations | The recompiler already performs constant propagation and dead-code elimination (`gpu/shadps4/shader_recompiler/recompiler.cpp`). No new semantic transformation was introduced. |
| Batching and instancing | Not implemented: translated draws may affect memory and depend on ordering. Compatible groups must first be identified. |
| Multithreaded rendering | Existing recording, preparation and copy paths exposed in the menu. |
| CPU/GPU synchronization | Existing readback options retained; recording controls and wait diagnostics added. Mandatory linear readbacks remain enabled. |
| Cache and precompilation | Persistent cache, driver binaries, Dozen state and parallel warmup made configurable. Launcher precompilation retained. |
| Texture compression | BC1/BC3 and other game-format uploads already exist. Additional recompression is not implemented; formats, aliases, render targets and readbacks must be preserved. |
| Texture/model streaming | Existing paging and collection under pressure retained. Progressive asset streaming is not implemented; it requires metadata and control of the game's loader. |
| Memory and transfers | Worker/upload memory controls exposed; metrics use heap counters. |
| Dynamic resolution | Not implemented. Xbox selects resolution patches at startup; runtime changes require validation of targets and memory preparation. |
| FSR | Not enabled. The Xbox version forces the upscaler off and records an FSR1 shader failure. The Dozen path needs repair and validation before a working option can be offered. |
| Lighting and shadows | Not implemented: passes and visual equivalence need investigation. Research controls were not turned into quality settings. |
| Frustum/occlusion culling | Not implemented: reliable spatial bounds and visibility are unavailable in the translation layer. |
| LOD | Not implemented: requires alternative models and selection in game logic. |
| Post-processing | Not implemented: passes and their dependencies must be mapped before reducing cost. |
| GPU-driven rendering | Not implemented: requires redesigning command submission and validating translated draw dependencies. |
| Translation layer | Advance Dozen state preparation controls and slow-command diagnostics exposed. |
| Stuttering | Diagnostics corrected and existing compilation/cache mechanisms exposed; no measured reduction yet. |
| Bandwidth | Cached/uncached upload buffers and worker comparisons available; physical bandwidth has not been measured. |

## Verification

Portable tests cover persistence, invalid values, private override precedence,
diagnostic dependencies, restart notices, histograms, long pauses and log
aggregation. The options test builds with and without `GOW3_UWP`; this validates
configuration logic, not Xbox APIs. Full Windows/UWP builds and packaging passed
for .308/.436; .436 also passed the native shader cache selftest. Controller
navigation, visual reproduction and console benchmarks remain unverified in this
environment. Packages are experimental test releases; Beta 0 is the current one.

## Historical experiment: first-start preparation after 1.0.26300.308

**This section describes withdrawn .436 work, not Beta 0.** The .343 changes
were reverted. On normal startup, that experimental line automatically prepares
a missing/incompatible shader library, warms ten texture upload converters and
loads recorded pipelines before starting the game. **Precompile shaders** runs
those stages from the menu; a valid library and prepared pipelines are reused.

A draw awaiting compilation is not discarded in Xbox **Start game** mode in
that line. The previous path could lose one-time draws that write to textures
used later. Unknown states still compile on use and cause waits; preparation
does not know all game states. **Fast start (may miss graphics)** retains the
earlier background compilation/pending-draw skipping option. Its help explains
that relationship in Portuguese, Spanish and English.

When testing that historical line, normal **Start game** logs
`Xbox shader startup: complete`; preparation logs list reused/compiled variants
and loaded pipelines. If file preparation fails, the error is logged and the
normal compiler remains the fallback. Confirming the reported image and loading
times requires an actual console test. The photo alone does not establish the
cause of all observed corruption.
