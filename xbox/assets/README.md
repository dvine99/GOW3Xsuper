# Startup artwork

`SplashScreen.png` is the packaged 620 × 300 RGB PNG splash screen, derived from
`GoWIII_Logo.webp` supplied by the task owner. The logo keeps its aspect ratio on
a black background. Source attachment SHA-256:
25079871328de44ac87b86999e857bb24375244cb56e5be6a57c859134f213ac

Conversion (FFmpeg; no image tools required by the Xbox package build):

```sh
ffmpeg -i GoWIII_Logo.webp -vf 'scale=580:260:force_original_aspect_ratio=decrease,pad=620:300:(ow-iw)/2:(oh-ih)/2:color=black,format=rgb24' -frames:v 1 -update 1 SplashScreen.png
```

`AppxManifest.xml` uses this asset as the OS startup splash. Both the CMake layout
and `scripts/assets.ps1` copy the checked-in PNG.

## Xbox cover and shell icons

`CoverSource.png` is the original 2048 × 2048 artwork supplied by the task owner.
Source SHA-256: `e072d74fe2a93e9860decca91ad4bbe842c2e1fb2d9f1b84c9a5036a64c1507f`.
The 15 PNGs under `tiles/` are rendered directly from that source with FFmpeg
Lanczos downsampling, without enlarging an existing small icon. Square variants
preserve the whole composition. The wide tile uses a centered vertical crop that
keeps the title complete. All packaged shell images are opaque RGB PNGs.

| Manifest resource | Base (100%) | 200% | 400% |
| --- | --- | --- | --- |
| StoreLogo | 50 × 50 | 100 × 100 | 200 × 200 |
| Square44x44Logo | 44 × 44 | 88 × 88 | 176 × 176 |
| Square150x150Logo | 150 × 150 | 300 × 300 | 600 × 600 |
| Square310x310Logo | 310 × 310 | 620 × 620 | 1240 × 1240 |
| Wide310x150Logo | 310 × 150 | 620 × 300 | 1240 × 600 |

Base filenames remain available for development layouts and fallback selection;
the larger variants have `.scale-200` and `.scale-400` qualifiers. The manifest
also declares the large and wide resources in `uap:DefaultTile`. Windows/Xbox
chooses which resource and scale to use; this does not control the shell's layout.

Regenerate only when changing the source artwork (FFmpeg required):

```sh
python3 scripts/generate_xbox_artwork.py
python3 -m unittest discover -s tests -p test_xbox_artwork.py -v
```

CMake copies `tiles/` into layout/Assets. `scripts/assets.ps1` refreshes those
files before packaging without a font or image-processing dependency. The source
art is not packaged. `scripts/package.ps1` creates `resources.pri` with the
Windows SDK MakePRI tool before MakeAppx. Automatic resource splitting is removed
from the generated config so all resolutions are included in the same `.appx`.
See [icon scales](https://learn.microsoft.com/en-us/windows/apps/design/iconography/app-icon-construction#how-icon-sizes-relate-to-the-msix-manifest)
and [manual resource indexing](https://learn.microsoft.com/en-us/windows/uwp/app-resources/using-mrt-for-converted-desktop-apps-and-games).

Device check after installing an updated package: inspect the Home tile and the
information screen opened with X. If the previous art is still shown, close the
system screen and restart the console before checking again. Keep the installed
package/data; uninstalling is not required for this check. Native shell selection,
cache refresh and visual quality must be confirmed on Xbox; Linux image checks
do not establish them.
