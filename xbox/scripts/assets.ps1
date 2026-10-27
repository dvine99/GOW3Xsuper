#requires -Version 7.2
# Copy pre-rendered shell images; no image-generation tools needed on Windows.
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$assets=Join-Path $root '.local/build/layout/Assets'
New-Item -ItemType Directory -Force $assets | Out-Null
foreach($name in @('StoreLogo','Square44x44Logo','Square150x150Logo','Square310x310Logo','Wide310x150Logo')) {
    foreach($suffix in @('','.scale-200','.scale-400')) {
        $file="$name$suffix.png"
        Copy-Item -LiteralPath (Join-Path $root "assets/tiles/$file") -Destination (Join-Path $assets $file) -Force
    }
}
Copy-Item -LiteralPath (Join-Path $root 'assets/SplashScreen.png') -Destination (Join-Path $assets 'SplashScreen.png') -Force
