"""Static dependency coverage checks, not native ABI or gameplay approval."""
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[1] / 'src/main/jni'
MOVEMENT = ROOT / 'Movement'
ENTRY = re.compile(r'\{"[^"]+",\s*(0x[0-9A-Fa-f]+),\s*\{([^}]+)\}\}')


def fingerprints(text):
    return {int(address, 16): bytes(int(byte, 16) for byte in
            re.findall(r'0x([0-9A-Fa-f]{2})\b', values))
            for address, values in ENTRY.findall(text)}


class MovementDependencyTests(unittest.TestCase):
    def test_every_native_dependency_has_a_complete_code_check(self):
        source = (MOVEMENT / 'MovementRuntime.cpp').read_text()
        used = {int(value, 16) for value in re.findall(r'\b0x[0-9A-Fa-f]{7,8}\b', source)}
        checked = fingerprints((MOVEMENT / 'NativeMovementChecks.inc').read_text())
        self.assertFalse(used - checked.keys(), 'Native dependency lacks startup validation')
        self.assertTrue(checked)
        for address, code in checked.items():
            with self.subTest(address=hex(address)):
                self.assertEqual(len(code), 16)

    def test_shared_dependencies_use_the_same_runtime_fingerprint(self):
        movement = fingerprints((MOVEMENT / 'NativeMovementChecks.inc').read_text())
        trigger = fingerprints((ROOT / 'Trigger/NativeProfileChecks.inc').read_text())
        shared = movement.keys() & trigger.keys()
        self.assertTrue(shared)
        for address in shared:
            with self.subTest(address=hex(address)):
                self.assertEqual(movement[address], trigger[address])

    def test_optional_features_validate_separate_dependency_groups(self):
        source = (MOVEMENT / 'MovementRuntime.cpp').read_text()
        crouch = source.split('void InstallCrouch() noexcept {', 1)[1].split(
            'void InstallSprint() noexcept {', 1)[0]
        sprint = source.split('void InstallSprint() noexcept {', 1)[1].split('#endif', 1)[0]
        for name, status, body in [('crouch', 'crouchStatus', crouch),
                                   ('sprint', 'sprintStatus', sprint)]:
            with self.subTest(feature=name):
                self.assertIn(f'Validate(commonChecks,{status})', body)
                self.assertIn(f'Validate({name}Checks,{status})', body)
                self.assertNotIn(f'Validate({"sprint" if name == "crouch" else "crouch"}Checks', body)


if __name__ == '__main__':
    unittest.main()
