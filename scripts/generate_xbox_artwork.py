#!/usr/bin/env python3
"""Regenerate checked-in Xbox shell images from the owner's original cover.

Requires FFmpeg only when artwork changes, not during Windows package builds.
"""
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'xbox/assets/CoverSource.png'
DESTINATION = ROOT / 'xbox/assets/tiles'
SIZES = {
    'StoreLogo': (50, 50),
    'Square44x44Logo': (44, 44),
    'Square150x150Logo': (150, 150),
    'Square310x310Logo': (310, 310),
    'Wide310x150Logo': (310, 150),
}


def main():
    ffmpeg = shutil.which('ffmpeg')
    if not ffmpeg:
        raise SystemExit('FFmpeg is required to regenerate artwork')
    DESTINATION.mkdir(parents=True, exist_ok=True)
    for name, (width, height) in SIZES.items():
        for scale in (100, 200, 400):
            w, h = width * scale // 100, height * scale // 100
            suffix = '' if scale == 100 else f'.scale-{scale}'
            # Every size comes directly from the 2048px source. The wide tile
            # crops the vertical surplus centrally, retaining the complete logo.
            filters = (f'scale={w}:{h}:force_original_aspect_ratio=increase:flags=lanczos,'
                       f'crop={w}:{h},setsar=1,format=rgb24')
            subprocess.run([ffmpeg, '-nostdin', '-v', 'error', '-y', '-i', str(SOURCE),
                            '-vf', filters, '-frames:v', '1', '-update', '1',
                            str(DESTINATION / f'{name}{suffix}.png')], check=True)
    print('Generated 15 Xbox shell assets from CoverSource.png')


if __name__ == '__main__':
    main()
