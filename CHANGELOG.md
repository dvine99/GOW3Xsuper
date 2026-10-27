# Downstream release history

The versions below belong to **GOW3Xsuper**. They are experimental prereleases
of the fork of [Devran Cosmo Uenal's work](https://github.com/Cosmo/gow3_xbox),
which derives from [SpartanPort](https://github.com/FelipePrado0/SpartanPort).

## 1.0.26300.436 — 2026-10-27 — Withdrawn

The release and its attachments were deleted at the maintainer's request. The
record below describes historical work; these changes are not part of Beta 0.
Git history was preserved for traceability.

- Returned to our **1.0.26300.308** baseline, reverting the .343 changes.
- Automatically prepared a missing/incompatible library during normal startup.
- Extended menu precompilation to prepare the library, texture upload converters
  and recorded pipelines.
- Made normal startup wait for required compilation instead of omitting draws;
  retained fast startup as an alternative with a warning.
- Expanded cache integrity/readiness checks and native selftests.
- Retained the three-language interface, cover/splash, controls and diagnostics from .308.

Source: [`e640bfa5596cb4998e4433f07fb3a1de9ed96934`](https://github.com/dvine99/GOW3Xsuper/commit/e640bfa5596cb4998e4433f07fb3a1de9ed96934).
[Successful Windows/UWP build](https://github.com/dvine99/GOW3Xsuper/actions/runs/40449954030).
The package and dependency were verified; visual results and console performance
**remain unconfirmed**. New shaders may cause pauses.

## 1.0.26300.343 — 2026-10-27 — Withdrawn

Experiments with readback, compilation scheduling and package metadata. The
maintainer reported failure and asked to disregard this version. Its changes
were reverted in the subsequent development line; **no download of this version
is recommended**. Git history remains available for traceability.

## Beta 0 — 1.0.26300.308 — 2026-10-27 — Current published release

The release previously called “previous baseline” was renamed **Beta 0**. The
package, internal version and checksum are unchanged; it was not rebuilt.

[Release and download](https://github.com/dvine99/GOW3Xsuper/releases/tag/alpha-1.0.26300.0308)

- Brazilian Portuguese, Spanish and English for the app interface.
- God of War III display name, startup logo and cover at multiple scales.
- Thirteen advanced controls for existing mechanisms and persistent settings.
- Corrected metric collection and a performance log analyzer.
- Build workflow, indexed resources, signing and a ZIP for Xbox testing.
- Explicit UTF-8 decoding in resource tests, including on Windows.

Source: [`ac0da17362ad5b0e50e0393d3e53b830ae54fc3c`](https://github.com/dvine99/GOW3Xsuper/commit/ac0da17362ad5b0e50e0393d3e53b830ae54fc3c).
[Successful Windows/UWP build](https://github.com/dvine99/GOW3Xsuper/actions/runs/40441138489).
The maintainer selected this version as the baseline for further work. That is
not a controlled benchmark or a guarantee that it is free of defects.

## Inherited baseline

`Cosmo/gow3_xbox`, `alpha-1.0.26299.2139`, commit
`fe8c66ed3a9bf151557c8f0af57009bdc955b042`. For earlier milestones, see the
[upstream development log](xbox/README.md) and [full credits](xbox/CREDITS.md).
