# God of War III — experimental Xbox port

**GOW3Xsuper is a community fork maintained by [dvine99](https://github.com/dvine99).**
It runs **God of War III Remastered on Xbox Series X|S in Developer Mode**,
using the player's own dump. **Xbox One is not supported.**

[Download our releases](https://github.com/dvine99/GOW3Xsuper/releases) ·
[Maintained modifications](MODIFICATIONS.md) · [Changelog](CHANGELOG.md) ·
[Full credits](xbox/CREDITS.md) · [Build the port](xbox/README.md)

![App cover](xbox/assets/CoverSource.png)

## Origins: a fork of a fork

```text
SpartanPort — Felipe Prado and contributors
└── gow3_xbox — Devran Cosmo Uenal (Cosmo)
    └── GOW3Xsuper — maintenance and adaptations by dvine99
```

- **[SpartanPort](https://github.com/FelipePrado0/SpartanPort)**, formerly
  `gow3_pc`: Felipe Prado and contributors' PC foundation, including the loader,
  runtime, graphics integration, menus and patches.
- **[gow3_xbox](https://github.com/Cosmo/gow3_xbox)**: the fork by
  **[Devran Cosmo Uenal](https://github.com/Cosmo)** that adapts that foundation
  to Xbox. It is our repository's **immediate upstream**. Credit for the Xbox
  port and its integration belongs to him and the upstream contributors.
- **[GOW3Xsuper](https://github.com/dvine99/GOW3Xsuper)**: our fork of that port,
  with the changes described below. We do not claim authorship of the inherited work.

We also depend on bloodborne_pc, shadPS4, Mesa/Dozen and other projects.
See [Credits](xbox/CREDITS.md) for their authors, components and licenses.

## What is this port?

This is an **experimental UWP application for Developer Mode**. Its loader and
runtime execute code from a PS4 dump; the graphics layer derived from shadPS4
translates shaders and commands to Vulkan, and **Mesa/Dozen translates Vulkan
to Direct3D 12** on Xbox. It is not a native commercial Xbox edition of God of War.

**No game files are distributed.** You need your own dump of God of War III
Remastered; the project's reference dump is **CUSA01715**. This project is not
affiliated with or endorsed by Sony Interactive Entertainment or Santa Monica
Studio. God of War belongs to its respective rights holders.

## What we modify and maintain

| Area | Work in this fork |
| --- | --- |
| Interface | Brazilian Portuguese, Spanish and English, with a persistent selection in the startup menu and settings; translations cover app-owned text. |
| Visual identity | **God of War III** display name, startup logo and new cover with resolution variants for Xbox tiles. |
| Settings | 13 advanced controls for existing recording, copying, memory, cache and diagnostic paths. |
| Profiling | Corrections to frame pacing, p95/p99, memory and wait reporting; a log analyzer for comparisons. |
| Experimental history | Later attempts to improve first-start preparation are documented in the report; versions .343 and .436 were withdrawn from distribution. |
| Distribution | Windows/UWP workflow with tests, image resources, packaging, test signing and ZIPs named **GOW3xbox<version>.zip**. |

See the [detailed inventory](MODIFICATIONS.md), including inherited features,
limitations and withdrawn changes. App interface localization **does not translate
dialogue, audio or text supplied by the game files**.

## Releases from this fork

| Version | Status | Download |
| --- | --- | --- |
| **Beta 0 — 1.0.26300.308** | Current published release | [GOW3xbox1.0.26300.308.zip](https://github.com/dvine99/GOW3Xsuper/releases/tag/alpha-1.0.26300.0308) |

Versions **1.0.26300.343 and 1.0.26300.436 were withdrawn**. Beta 0 is the original
.308 package and does not include those later experiments. Published versions are
available in **[GOW3Xsuper Releases](https://github.com/dvine99/GOW3Xsuper/releases)**.
See [how we publish releases](RELEASES.md) and the [changelog](CHANGELOG.md).

## Installation on Xbox Series X|S

1. Enable **Developer Mode** and open **Xbox Device Portal** from your PC.
2. Download the desired release ZIP and extract it on your PC.
3. Use **Add** to install the `.appx` and the dependency in `Dependencies`.
   If Dev Home lists it as an App, change its type to **Game**.
4. Copy your dump — the folder containing `eboot.bin`, `sce_module`, `sce_sys`
   and `dvdroot_ps4` — to USB or `D:\DevelopmentFiles` on the console. Automatic
   discovery checks `\dumps\GodOfWar3` on USB and up to three levels below
   `D:\DevelopmentFiles`; use **Choose game folder** if needed.
5. Open **God of War III**, choose your language and run **Prepare** on first use.
   In **Beta 0**, use **Precompile shaders** to prepare ahead of time, then
   **Start game**, which loads the recorded shader cache.

Wait for preparation to finish. Unknown shaders and states may still compile
during gameplay. **Start game without loading shaders** is an alternative that
may omit graphics while compilation is pending; use normal startup when testing
the first boot.

During gameplay, **RS + LT** opens the menu. The default resolution is 720p.
Advanced options marked as applying on the next start require closing and
reopening the app. The internal package identity was preserved so updates can
install over the existing app and retain its data.

## Current status and known issues

- Beta 0 passed the Windows/UWP build, packaging and file-integrity checks.
  These checks do not replace testing on a console.
- Corrupted textures/text on first boot, ghosting and stuttering have been reported.
  **Beta 0 does not include the attempted .436 fixes, and a visual fix for the
  reported scene has not been confirmed on Xbox.**
- No FPS improvement has been measured for this fork. Shader preparation can
  increase loading time, and new compilations can cause pauses during gameplay.
- Developer Mode has limited memory; higher resolutions increase pressure.
  FSR and dynamic resolution have not been enabled as working Xbox features.

When reporting a problem, include the app version, console model, language,
resolution, reproduction steps, a screenshot and `loader.log`, without personal
data. See the [performance and diagnostic guide](xbox/PERFORMANCE.md)
([Brazilian Portuguese](xbox/PERFORMANCE.pt-BR.md)).

## License and acknowledgements

We retain the [GPL-2.0 license](LICENSE) and third-party notices. Credits extend
beyond this fork's maintainers: read the [full author and license list](xbox/CREDITS.md).
