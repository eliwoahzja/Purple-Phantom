import contextlib
import importlib.util
import io
from pathlib import Path
import struct
import tempfile
import unittest

spec = importlib.util.spec_from_file_location(
    'inspector', Path(__file__).resolve().parents[1] / 'tools/inspect_trigger_binary.py')
inspector = importlib.util.module_from_spec(spec)
spec.loader.exec_module(inspector)


def fixture():
    data = bytearray(0x218)
    ident = b'\x7fELF\x02\x01\x01' + bytes(9)
    inspector.ELF_HEADER.pack_into(data, 0, ident, 3, 183, 1, 0, 64, 0, 0,
                                   64, 56, 2, 0, 0, 0)
    inspector.PROGRAM_HEADER.pack_into(data, 64, 1, 5, 0x100,
                                       0xBB4B66C, 0, 64, 64, 4)
    inspector.PROGRAM_HEADER.pack_into(data, 120, 4, 4, 0x200, 0, 0, 20, 20, 4)
    data[0x100:0x104] = bytes.fromhex('c0035fd6')
    inspector.NOTE_HEADER.pack_into(data, 0x200, 4, 4, 3)
    data[0x20c:0x214] = b'GNU\0\x12\x34\x56\x78'
    return data


class BinaryInspectionTests(unittest.TestCase):
    def test_identity_and_executable_candidate_are_only_structural(self):
        report = inspector.inspect(fixture(), 'synthetic.so')
        self.assertEqual(report['gnu_build_id'], '12345678')
        self.assertIn('no semantic validation', report['analysis'])
        match = next(x for x in report['candidate_bindings']
                     if x['name'] == 'WeaponFireModeUnit.UpdateFireButtonStatus')
        self.assertEqual(match['executable_file_backed_matches'][0]['first_instruction_hex'],
                         'c0035fd6')

    def test_truncated_wrong_architecture_and_invalid_segment(self):
        cases = [b'', fixture()[:100]]
        wrong_arch = fixture()
        struct.pack_into('<H', wrong_arch, 18, 62)
        cases.append(wrong_arch)
        invalid = fixture()
        struct.pack_into('<Q', invalid, 64 + 40, 1)  # p_memsz < p_filesz
        cases.append(invalid)
        for data in cases:
            with self.subTest(length=len(data)), self.assertRaises(inspector.ElfError):
                inspector.inspect(data, 'bad.so')

    def test_accessor_prefix_mismatch_is_reported(self):
        data = fixture()
        struct.pack_into('<Q', data, 64 + 16, 0xA81F798)
        report = inspector.inspect(data, 'synthetic.so')
        candidate = next(x for x in report['candidate_bindings']
                         if x['name'] == 'CheckFireParam.set_CheckFire')
        self.assertFalse(candidate['executable_file_backed_matches'][0]['accessor_prefix_matches'])
        data[0x100:0x108] = inspector.REFERENCE_PREFIXES['CheckFireParam.set_CheckFire']
        report = inspector.inspect(data, 'synthetic.so')
        candidate = next(x for x in report['candidate_bindings']
                         if x['name'] == 'CheckFireParam.set_CheckFire')
        self.assertTrue(candidate['executable_file_backed_matches'][0]['accessor_prefix_matches'])
        self.assertIn('no semantic validation', report['analysis'])

    def test_inspection_cannot_overwrite_original(self):
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / 'original.so'
            original = bytes(fixture())
            source.write_bytes(original)
            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(inspector.main([str(source), '--output', str(source)]), 2)
            self.assertEqual(source.read_bytes(), original)


if __name__ == '__main__':
    unittest.main()
