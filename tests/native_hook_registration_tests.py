"""Check literal startup hook targets for duplicate registrations."""
from collections import defaultdict
from pathlib import Path
import re
import unittest


class StartupHookTests(unittest.TestCase):
    def test_unsupported_game_stops_both_startup_paths(self):
        source = (Path(__file__).resolve().parents[1] /
                  'src/main/jni/Main.cpp').read_text()
        primary = source.split('void Init_Thread()\n{', 1)[1].split(
            '__attribute__((constructor))', 1)[0]
        companion = source.split('void Init_Thread1()\n{', 1)[1].split(
            'void Init_Thread()', 1)[0]
        rejection = re.search(
            r'if\s*\(!PhantomTrigger::ValidateSupportedGameProfile\(.*?\)\)\s*\{(.*?)\n    \}',
            primary, re.S)
        self.assertIsNotNone(rejection)
        self.assertIn('StartupProfileState::Rejected', rejection[1])
        self.assertIn('return;', rejection[1])
        self.assertLess(rejection.end(), primary.index('InitializeNativeTrigger'))
        self.assertLess(primary.index('InitializeNativeTrigger'), primary.index('InstallCaptureOnly'))
        self.assertLess(companion.index('StartupProfileState::Pending'), companion.index('Tools::GetBaseAddress'))
        self.assertRegex(companion, r'if\s*\(.*?StartupProfileState::Supported\) return;')
        self.assertLess(companion.index('StartupProfileState::Supported'), companion.index('MemoryPatch::'))

    def test_each_literal_native_target_is_registered_once(self):
        source = (Path(__file__).resolve().parents[1] /
                  'src/main/jni/Main.cpp').read_text()
        patterns = (
            r'DobbyHook\(\s*\(void\*\)\s*getAbsoluteAddress\(\s*"([^"]+)"\s*,\s*(0x[0-9a-fA-F]+)\s*\)',
            r'HOOK_LIB(?:_NO_ORIG)?\(\s*"([^"]+)"\s*,\s*"(0x[0-9a-fA-F]+)"',
        )
        targets = defaultdict(list)
        for pattern in patterns:
            for match in re.finditer(pattern, source):
                library, address = match.groups()
                targets[library, int(address, 16)].append(
                    source.count('\n', 0, match.start()) + 1)
        self.assertTrue(targets, 'No startup hooks were inspected')
        duplicates = {f'{library}+{address:#x}': lines
                      for (library, address), lines in targets.items()
                      if len(lines) > 1}
        self.assertFalse(duplicates, f'Duplicate startup hook registrations: {duplicates}')


if __name__ == '__main__':
    unittest.main()
