"""Keep all native Xbox interface labels/help in the three-language catalog."""
import ast
import importlib.util
import json
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
STRING = r'"(?:[^"\\]|\\.)*"'


def strings(source):
    # Collapse adjacent C++ string literals, including explanatory // comments.
    source = re.sub(r'//[^\n]*', '', source)
    return [ ''.join(ast.literal_eval(s) for s in re.findall(STRING, match.group()))
             for match in re.finditer(STRING + r'(?:\s*' + STRING + r')*', source) ]


def xbox_source(source):
    """Select Xbox branches; unknown compile guards keep their primary branch."""
    active, stack, lines = True, [], []
    for line in source.splitlines():
        directive = re.match(r'\s*#\s*(ifdef|ifndef|if|else|elif|endif)\b(.*)', line)
        if directive:
            op, name = directive.groups()
            name = name.strip().split('//')[0].strip()
            if op in ('if', 'ifdef', 'ifndef'):
                defined = name in ('GOW3_UWP', '_WIN32')
                condition = not defined if op == 'ifndef' else defined if op == 'ifdef' else True
                stack.append((active, condition))
                active = active and condition
            elif op in ('else', 'elif'):
                parent, condition = stack[-1]
                active = parent and not condition
            else:
                active, _ = stack.pop()
        elif active:
            lines.append(line)
    return '\n'.join(lines)


class AppTranslations(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.rows = json.loads((ROOT / 'xbox/locales/interface.json').read_text(encoding='utf-8'))
        cls.keys = {row[0] for row in cls.rows}

    def test_catalog_and_format_contract(self):
        self.assertEqual(len(self.rows), len(self.keys))
        # Only these catalog keys are printf formats; prose percentages aren't formats.
        printf = {'loading %u%%', 'Loading shader cache %u%%', 'Changes applied on next start: %d'}
        for row in self.rows:
            self.assertEqual(len(row), 3)
            self.assertTrue(all(isinstance(s, str) and s.strip() for s in row))
            self.assertEqual([s.count('{}') for s in row], [row[0].count('{}')] * 3)
            if row[0] in printf:
                conversions = [re.findall(r'%(?:%|[du])', s) for s in row]
                self.assertEqual(conversions, [conversions[0]] * 3)

    def test_generated_header_current(self):
        spec = importlib.util.spec_from_file_location('catalog_generator', ROOT / 'scripts/generate_xbox_translations.py')
        generator = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(generator)
        self.assertEqual(generator.generate(), (ROOT / 'gpu/shim/spartanport_ui_catalog.h').read_text(encoding='utf-8'))

    def test_xbox_overlay_and_performance_help_coverage(self):
        source = (ROOT / 'gpu/shim/spartanport_overlay.cpp').read_text(encoding='utf-8').split('namespace SpartanOverlay {', 1)[1]
        option_source = (ROOT / 'gpu/shim/spartanport_xbox_performance.h').read_text(encoding='utf-8').split('Options[] = {', 1)[1].split('inline constexpr size_t', 1)[0]
        # Names/units/controller tokens, ImGui IDs and environment keys are not prose.
        neutral = {'xbox', 'nintendo', 'name', 'value', 'native', 'FSR 3.1', 'FSR 4',
                   'FSR 4.1.1', 'TAA', 'DLSS', 'FPS', ' FPS', 'Surface', 'Swapchain',
                   'cached', 'host', '  %s', '> %s', 'spartanport',
                   '4K', '4K · 3840 × 2160', '720p', '720p · 1280 × 720',
                   '900p', '900p · 1600 × 900', '  (Insert / RS+LT)', '  (Insert / R3+L2)',
                   '  (Insert / RS+ZL)', '1x', '2x', '4x', '8x', '16x'}
        missing = []
        for value in strings(xbox_source(source)) + strings(option_source):
            if (value in self.keys or value in neutral or not re.search('[a-zA-Z]', value)
                or value.startswith(('##', 'SPARTAN_', 'xbox_', 'Overlay:', 'Settings:'))
                or re.fullmatch(r'%[.\d*]*[sufd].*', value)):
                continue
            missing.append(value)
        self.assertEqual(sorted(set(missing)), [])

    def test_game_dialog_bridge_keys(self):
        source = (ROOT / 'src/runtime_services.c').read_text(encoding='utf-8')
        for key in ('Save game', 'Load game', 'Delete save', 'New save', 'Text'):
            self.assertIn('\"' + key + '\"', source)
            self.assertIn(key, self.keys)

    def test_bootstrap_and_notification_keys(self):
        for path in ('xbox/platform/main.cpp', 'gpu/shadps4/video_core/renderer_vulkan/vk_presenter.cpp'):
            source = (ROOT / path).read_text(encoding='utf-8')
            for call in re.finditer(r'(?:SpartanUi::Format|SpartanUi::T|\bT|\bask|\bstop|ShowNotice|choose_dump)\s*\(\s*(' + STRING + r'(?:\s*' + STRING + r')*)', source):
                key = strings(call[1])[0]
                self.assertIn(key, self.keys, (path, key))


if __name__ == '__main__':
    unittest.main()
