# Credits

God of War III (GOW3Xsuper, a fork of gow3_xbox) runs God of War III Remastered on the Xbox Series X|S in
Developer Mode from the player's own dump. It contains no game code or game assets.

## Fork lineage and current maintenance

**SpartanPort → Cosmo/gow3_xbox → dvine99/GOW3Xsuper**

- **Devran Cosmo Uenal (Cosmo)** — [gow3_xbox](https://github.com/Cosmo/gow3_xbox),
  the immediate upstream of this repository. Credit for the Xbox adaptation and
  integration belongs to him and the upstream contributors, building on SpartanPort.
- **Felipe Prado and contributors** — [SpartanPort](https://github.com/FelipePrado0/SpartanPort),
  the upstream PC foundation from which gow3_xbox was forked.
- **dvine99** — [GOW3Xsuper](https://github.com/dvine99/GOW3Xsuper), maintenance of
  this downstream fork: app localization, artwork integration, advanced settings,
  profiling corrections, first-start shader preparation and build/distribution work.
  See the [maintained modifications](../MODIFICATIONS.md) and [version history](../CHANGELOG.md)
  in the source repository. Changes withdrawn after testing are documented separately.

This is an unofficial experimental port, not an Xbox release by Sony Interactive
Entertainment or Santa Monica Studio. God of War and its game assets belong to
their respective owners. No game files are distributed here.

## Built on

- **SpartanPort** (formerly gow3_pc) by Felipe Prado and its contributors
  (https://github.com/FelipePrado0/SpartanPort): the PC port and launcher of God of War III
  Remastered. Its native loader, runtime, renderer integration, in-game menu and game patches
  are the base inherited through gow3_xbox. GPL-2.0.
- **bloodborne_pc** by Supermedo (https://github.com/Supermedo/bloodborne_pc), which SpartanPort
  started from. GPL-2.0.
- **shadPS4** (https://github.com/shadps4-emu/shadPS4): the PS4 graphics emulation, its shader
  recompiler and Vulkan renderer (`gpu/shadps4`). GPL-2.0.
- **KytyPS5 for Xbox** (https://github.com/Cosmo/KytyPS5): the Xbox memory pager, the
  performance overlay, the Back-button handling and the package signing follow its Xbox port.
  GPL-2.0.
- **Mesa / Dozen**, Vulkan on Direct3D 12, from the fork
  https://github.com/Cosmo/mesa_ai_slop_version (`vulkan_dzn.dll`). MIT.
- **DirectX Shader Compiler** (`dxil.dll`) by Microsoft. Its licenses are in `ThirdParty`.

## Game patches

`patches/CUSA01623.xml` holds shadPS4/GoldHEN-format patches by **cuesta4**
and **kvicken**, ported and extended by SpartanPort (Felipe Prado). Skip Intro and Skip Any Video
build on the PS3 patches by **illusion0001** (illusionyy).

## Libraries in the package

| Library | License |
|---|---|
| FFmpeg 7.1.1 (static, decoders the game uses) | LGPL-2.1 |
| SDL 3 | zlib |
| fmt | MIT |
| Zydis, Zycore | MIT |
| xxHash | BSD-2-Clause |
| Vulkan Memory Allocator | MIT |
| Vulkan-Headers | Apache-2.0 / MIT |
| robin-map | MIT |
| Boost (headers) | BSL-1.0 |
| LibAtrac9 | MIT |
| Dear ImGui | MIT |
| miniz | MIT |
| magic_enum | MIT |
| half | MIT |
| sirit | BSD-3-Clause |
| xbyak | BSD-3-Clause |
| FSR Vulkan | MIT |
| AMD GCN register headers | MIT (in the headers) |
| DejaVu fonts | Bitstream Vera / public domain |
| winpthreads (MSYS2 UCRT64) | MIT / ZPL |

Each license text is in the package's `ThirdParty` folder.

## License

The launcher is GPL-2.0 (`LICENSE.txt`). FFmpeg is linked statically under the LGPL-2.1: it is
ffmpeg-7.1.1 from https://ffmpeg.org, built by `xbox/scripts/build-deps.ps1` with the
configuration shown there. With this project's source you can rebuild the launcher against a
modified FFmpeg.
