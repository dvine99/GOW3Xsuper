"""Validate the artwork the manifest and package actually ship (no image SDK)."""
import binascii
from pathlib import Path
import struct
import unittest
import xml.etree.ElementTree as ET
import zlib

ROOT = Path(__file__).resolve().parents[1]
ASSETS = ROOT / 'xbox/assets'
UAP = '{http://schemas.microsoft.com/appx/manifest/uap/windows10}'


def png_info(path):
    data = path.read_bytes()
    assert data[:8] == b'\x89PNG\r\n\x1a\n', path
    pos, compressed, size = 8, bytearray(), None
    while pos < len(data):
        length = struct.unpack('>I', data[pos:pos+4])[0]
        kind, body = data[pos+4:pos+8], data[pos+8:pos+8+length]
        crc = struct.unpack('>I', data[pos+8+length:pos+12+length])[0]
        assert binascii.crc32(kind + body) & 0xffffffff == crc, path
        if kind == b'IHDR':
            width, height, depth, color, compression, filtering, interlace = struct.unpack('>IIBBBBB', body)
            assert (depth, color, compression, filtering, interlace) == (8, 2, 0, 0, 0), path
            size = width, height
        if kind == b'IDAT':
            compressed.extend(body)
        pos += length + 12
        if kind == b'IEND':
            break
    assert size and pos == len(data), path
    assert len(zlib.decompress(compressed)) == (size[0] * 3 + 1) * size[1], path
    return size


class XboxArtwork(unittest.TestCase):
    def test_manifest_resources_have_matching_scale_variants(self):
        manifest = ET.parse(ROOT / 'xbox/AppxManifest.xml').getroot()
        resources = [manifest.find('{*}Properties/{*}Logo').text]
        visual = manifest.find('.//' + UAP + 'VisualElements')
        self.assertEqual(visual.attrib['DisplayName'], 'God of War III')
        resources += [visual.attrib[key] for key in ('Square150x150Logo', 'Square44x44Logo')]
        tile = visual.find(UAP + 'DefaultTile')
        resources += [tile.attrib[key] for key in ('Square310x310Logo', 'Wide310x150Logo')]
        expected = {'StoreLogo': (50, 50), 'Square44x44Logo': (44, 44),
                    'Square150x150Logo': (150, 150), 'Square310x310Logo': (310, 310),
                    'Wide310x150Logo': (310, 150)}
        self.assertEqual({Path(s.replace('\\', '/')).stem for s in resources}, set(expected))
        for resource in resources:
            name = Path(resource.replace('\\', '/')).stem
            w, h = expected[name]
            for scale in (100, 200, 400):
                suffix = '' if scale == 100 else f'.scale-{scale}'
                self.assertEqual(png_info(ASSETS / 'tiles' / f'{name}{suffix}.png'),
                                 (w * scale // 100, h * scale // 100))
        self.assertEqual(len(list((ASSETS / 'tiles').glob('*.png'))), 15)
        self.assertEqual(png_info(ASSETS / 'SplashScreen.png'), (620, 300))

    def test_package_indexes_assets_before_packing(self):
        script = (ROOT / 'xbox/scripts/package.ps1').read_text(encoding='utf-8')
        self.assertLess(script.index('makepri.exe" new'), script.index('makeappx.exe" pack'))
        self.assertIn("SelectNodes('/resources/packaging')", script)
        self.assertIn("'resources.pri'", script)
        self.assertIn('/mn $manifest', script)
        copy_script = (ROOT / 'xbox/scripts/assets.ps1').read_text(encoding='utf-8')
        self.assertNotIn('DrawString', copy_script)
        self.assertNotIn('CoverSource', copy_script)
        for name in ('Square310x310Logo', 'Wide310x150Logo', '.scale-200', '.scale-400'):
            self.assertIn(name, copy_script)


if __name__ == '__main__':
    unittest.main()
