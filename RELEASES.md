# GOW3Xsuper Releases

**[Open Releases and download this fork's builds](https://github.com/dvine99/GOW3Xsuper/releases)**

| Version | Tag | Purpose |
| --- | --- | --- |
| **Beta 0 — 1.0.26300.308** | [alpha-1.0.26300.0308](https://github.com/dvine99/GOW3Xsuper/releases/tag/alpha-1.0.26300.0308) | Current published release; original .308 package. |

Versions .343 and .436 were withdrawn; the .436 release and its attachments
were deleted. The technical history remains in Git and in the report. Read the
[changelog](CHANGELOG.md) before switching versions.

## Download contents

- `GOW3xbox<version>.zip`: the `.appx` application and VCLibs dependency in `Dependencies`.
- A `.sha256` file to verify the Beta 0 ZIP.
- No game dump, game executable or game assets; source code corresponds to the specified tag.
- A package signed with the project's public test key for Developer Mode;
  this is not a Microsoft Store release.

In PowerShell, run `Get-FileHash .\GOW3xbox<version>.zip -Algorithm SHA256` and
compare the result with the `.sha256` file. See the [README](README.md) for
installation and requirements.

## Publishing future versions

For maintainers with repository permissions:

1. Open **Actions → Xbox build → Run workflow** and select the desired revision.
   The workflow must be merged into the default branch to appear for manual
   dispatch; until then, pushes to the configured work branch produce artifacts.
2. Enable prerelease publication (`release`), enter a title and changes
   (`whats_new`, with items separated by `|`), and run it.
3. The workflow builds, tests, indexes resources, packages and signs the app.
   Resolve any failures before publishing. The release step attaches the approved ZIP.
4. Check the tag, commit, manifest version and download. Record changes in the
   [CHANGELOG](CHANGELOG.md) and update the README's version table.
5. Document Xbox test results separately from build results. Do not announce
   visual fixes or FPS improvements without evidence from the console.

The ZIP uses the actual app version. The tag pads its final component with zeros
for ordering (`308` → `0308`); the manifest and ZIP retain `308`.
**Beta 0** is the public release name; its existing technical tag was preserved.
Do not replace a published version with a different binary. Mark problematic
versions as withdrawn and publish fixes as a new version.

## Origins and credits in each release

Notes should identify **GOW3Xsuper → gow3_xbox (Devran Cosmo Uenal) → SpartanPort
(Felipe Prado and contributors)**, link the full credits and describe downstream
changes, limitations and installation steps. Beta 0 uses the exact ZIP from the
successful .308 build listed in the changelog; renaming it does not modify the
app or constitute a new build.
