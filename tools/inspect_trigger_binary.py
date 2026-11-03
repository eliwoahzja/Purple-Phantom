#!/usr/bin/env python3
"""Inspect a candidate game library; this does not validate or activate hooks.

Only Python's standard library is required. RVAs below come from the supplied
1.6.57 dump and are hypotheses until checked against the original game binary.
"""

import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import sys
import tempfile


ELF_HEADER = struct.Struct("<16sHHIQQQIHHHHHH")
PROGRAM_HEADER = struct.Struct("<IIQQQQQQ")
SECTION_HEADER = struct.Struct("<IIQQQQIIQQ")
NOTE_HEADER = struct.Struct("<III")
U64_LIMIT = 1 << 64

# Reference information only. The inspector never creates a runtime profile.
REFERENCE_BINDINGS = (
    ("WeaponFireModeUnit.UpdateFireButtonStatus", 0xBB4B66C,
     "public instance void UpdateFireButtonStatus(CheckFireParam fireResult)"),
    ("Weapon.get_IsSingleShot", 0x50EBA5C, "public instance bool get_IsSingleShot()"),
    ("Weapon.get_IsBurstFire", 0x50EBB28, "public instance bool get_IsBurstFire()"),
    ("Weapon.get_CurrentFireComponent", 0x50EB1E4,
     "public instance WeaponFireComponent get_CurrentFireComponent()"),
    ("Weapon.get_ShotFired", 0x50EC09C, "public instance int get_ShotFired()"),
    ("WeaponFireModeBase.UpdateFireInput", 0xBB4AE14,
     "public instance void UpdateFireInput(float deltaTime)"),
    ("GamePlay.get_MatchGame", 0x8569B10,
     "public static MatchGame get_MatchGame()"),
    ("GamePlay.get_LocalPawn", 0x856AA08,
     "public static Pawn get_LocalPawn()"),
    ("GamePlay.get_AttackableColliderMap", 0x856A020,
     "public static AttackableColliderMap get_AttackableColliderMap()"),
    ("PhysicsUtil.SingleLineCheckNoAlloc", 0xA18F6C4,
     "public static bool SingleLineCheckNoAlloc(Vector3 startTrace, Vector3 direction, "
     "float distance, int traceFlag, ImpactInfo impactInfo, "
     "QueryTriggerInteraction queryTriggerInteraction = 0)"),
    ("PhysicsUtil.SingleLineCheckNoAlloc.ignoreTargets", 0xA18FEB4,
     "public static bool SingleLineCheckNoAlloc(Vector3 startTrace, Vector3 direction, "
     "float distance, int traceFlag, ImpactInfo impactInfo, "
     "List<AttackableTarget> ignoreTargets, QueryTriggerInteraction queryTriggerInteraction = 0)"),
    ("PhysicsUtil.SingleLineCheckAttackableTargetNoAlloc_Complete", 0xA191B3C,
     "public static bool SingleLineCheckAttackableTargetNoAlloc_Complete("
     "AttackableTarget attackableTarget, Vector3 startTrace, Vector3 direction, "
     "float distance, int obstacleFlag, ImpactInfo impactInfo)"),
    ("PhysicsUtil.GetLayerMask", 0xA18BEF4,
     "public static long GetLayerMask(int traceFlag)"),
    ("AttackableColliderMap.GetAttackableTargetForCollider", 0x773FA68,
     "public instance AttackableTarget GetAttackableTargetForCollider(Collider col)"),
    ("BaseGame.IsInSmoke", 0x9CC3518,
     "public instance bool IsInSmoke(Vector3 point)"),
    ("BaseGame.IsSmokeInMiddle", 0x9CC36E0,
     "public instance bool IsSmokeInMiddle(Vector3 source, Vector3 target)"),
    ("BaseGame.IsSmokeClusterNotInSameSide", 0x9CC38AC,
     "public instance bool IsSmokeClusterNotInSameSide(Vector3 source, Vector3 target)"),
    ("Pawn.IsAiming", 0x519BB10, "public instance bool IsAiming()"),
    ("Weapon.GetAimingPercentage", 0x5113B4C,
     "public instance float GetAimingPercentage()"),
    ("Weapon.AmmoCanFire", 0x50EB73C, "public instance bool AmmoCanFire()"),
    ("Pawn.CanFire", 0x5260EC0,
     "public virtual instance bool CanFire(bool ignoreChangingFireModeTransition = false)"),
    ("Weapon.CanFire", 0x5126CC4, "public virtual instance bool CanFire()"),
    ("Pawn.StartFire", 0x5260A54, "public virtual instance void StartFire()"),
    ("Pawn.StopFire", 0x5260B78,
     "public virtual instance void StopFire(bool isImmidiately = false)"),
    ("Pawn.FireButtonDown", 0x5263038,
     "public virtual instance void FireButtonDown()"),
    ("Pawn.FireButtonUp", 0x52632A0,
     "public virtual instance void FireButtonUp()"),
    ("ImpactInfo.Alloc", 0x64B554C, "public static ImpactInfo Alloc()"),
    ("ImpactInfo.Recycle", 0x64B56F0,
     "public static void Recycle(ImpactInfo info)"),
)


class ElfError(ValueError):
    """An unsupported or structurally invalid ELF input."""


def checked_end(start, size, limit, label):
    if start < 0 or size < 0 or start > limit or size > limit - start:
        raise ElfError(f"{label} lies outside its containing range")
    return start + size


def file_range(data, start, size, label):
    return checked_end(start, size, len(data), label)


def read_notes(data, offset, size, label):
    end = file_range(data, offset, size, label)
    cursor = offset
    ids = []
    while cursor < end:
        remaining = end - cursor
        if remaining < NOTE_HEADER.size:
            if remaining <= 3 and not any(data[cursor:end]):
                break
            raise ElfError(f"{label} has a truncated note header")
        name_size, desc_size, note_type = NOTE_HEADER.unpack_from(data, cursor)
        cursor += NOTE_HEADER.size
        name_end = checked_end(cursor, name_size, end, f"{label} note name")
        name = data[cursor:name_end]
        cursor = checked_end(cursor, (name_size + 3) & ~3, end,
                             f"{label} padded note name")
        desc_end = checked_end(cursor, desc_size, end, f"{label} note descriptor")
        if note_type == 3 and name == b"GNU\0":
            if not 1 <= desc_size <= 128:
                raise ElfError("GNU build ID must contain between 1 and 128 bytes")
            ids.append(data[cursor:desc_end].hex())
        cursor = checked_end(cursor, (desc_size + 3) & ~3, end,
                             f"{label} padded note descriptor")
    return ids


def inspect(data, source_name):
    if len(data) < ELF_HEADER.size:
        raise ElfError("file is shorter than an ELF64 header")
    (ident, elf_type, machine, version, entry, phoff, shoff, flags, ehsize,
     phentsize, phnum, shentsize, shnum, shstrndx) = ELF_HEADER.unpack_from(data)
    if ident[:4] != b"\x7fELF":
        raise ElfError("input does not have the ELF magic")
    if ident[4:7] != b"\x02\x01\x01":
        raise ElfError("only ELF64, little-endian, ELF version 1 is supported")
    if machine != 183:
        raise ElfError("input is not an AArch64 ELF (e_machine must be 183)")
    if elf_type not in (2, 3):
        raise ElfError("only executable or shared-object ELF files are supported")
    if version != 1 or ehsize != ELF_HEADER.size:
        raise ElfError("invalid ELF version or ELF64 header size")
    if not phnum or phnum == 0xFFFF:
        raise ElfError("a nonempty, non-extended program-header table is required")
    if phentsize != PROGRAM_HEADER.size or phoff < ehsize:
        raise ElfError("invalid ELF64 program-header entry size or table offset")
    file_range(data, phoff, phnum * phentsize, "program-header table")

    loads = []
    notes = set()
    for index in range(phnum):
        (kind, segment_flags, offset, address, physical_address, file_size,
         memory_size, alignment) = PROGRAM_HEADER.unpack_from(data, phoff + index * phentsize)
        if kind == 0:  # PT_NULL fields are unspecified.
            continue
        file_range(data, offset, file_size, f"program header {index} file data")
        checked_end(address, memory_size, U64_LIMIT, f"program header {index} virtual data")
        if kind == 1:  # PT_LOAD
            if file_size > memory_size:
                raise ElfError(f"PT_LOAD {index} has p_filesz greater than p_memsz")
            if alignment > 1:
                if alignment & (alignment - 1):
                    raise ElfError(f"PT_LOAD {index} alignment is not a power of two")
                if address % alignment != offset % alignment:
                    raise ElfError(f"PT_LOAD {index} address and offset are not congruent")
            loads.append({
                "program_header_index": index,
                "file_offset": offset,
                "virtual_address": address,
                "file_size": file_size,
                "memory_size": memory_size,
                "flags": segment_flags,
                "permissions": "".join(ch for bit, ch in ((4, "R"), (2, "W"), (1, "X"))
                                       if segment_flags & bit),
                "alignment": alignment,
            })
        elif kind == 4:  # PT_NOTE
            notes.add((offset, file_size))
    if not loads:
        raise ElfError("input has no PT_LOAD segments")

    if shnum == 0:
        if shoff or shstrndx:
            raise ElfError("extended or inconsistent section numbering is unsupported")
    else:
        if shentsize != SECTION_HEADER.size or shoff < ehsize:
            raise ElfError("invalid ELF64 section-header entry size or table offset")
        if shstrndx >= shnum:
            raise ElfError("section-name string-table index is out of range")
        file_range(data, shoff, shnum * shentsize, "section-header table")
        for index in range(shnum):
            (_, kind, _, _, offset, size, _, _, alignment, _) = SECTION_HEADER.unpack_from(
                data, shoff + index * shentsize)
            if alignment > 1 and alignment & (alignment - 1):
                raise ElfError(f"section {index} alignment is not a power of two")
            if kind not in (0, 8):  # SHT_NULL / SHT_NOBITS have no file data.
                file_range(data, offset, size, f"section {index} file data")
            if kind == 7:  # SHT_NOTE
                notes.add((offset, size))

    build_ids = sorted(set(build_id for offset, size in sorted(notes)
                           for build_id in read_notes(data, offset, size, "ELF note data")))
    candidates = []
    for name, address, signature in REFERENCE_BINDINGS:
        matches = []
        for segment in loads:
            delta = address - segment["virtual_address"]
            if (segment["flags"] & 1 and delta >= 0
                    and delta + 4 <= segment["file_size"]
                    and delta + 4 <= segment["memory_size"]):
                offset = segment["file_offset"] + delta
                matches.append({
                    "program_header_index": segment["program_header_index"],
                    "file_offset": offset,
                    "first_instruction_hex": data[offset:offset + 4].hex(),
                })
        candidates.append({
            "name": name,
            "rva": f"0x{address:X}",
            "reference_signature": signature,
            "executable_file_backed_matches": matches,
        })
    return {
        "report_version": 1,
        "analysis": "structural ELF inspection only; no semantic validation or runtime activation",
        "source": source_name,
        "size_bytes": len(data),
        "sha256": hashlib.sha256(data).hexdigest(),
        "gnu_build_id": build_ids[0] if len(build_ids) == 1 else None,
        "gnu_build_ids": build_ids,
        "elf": {
            "class": "ELF64", "endianness": "little", "machine": "AArch64",
            "type": "ET_DYN" if elf_type == 3 else "ET_EXEC",
            "entry_point": entry, "flags": flags,
            "program_header_count": phnum, "section_header_count": shnum,
            "load_segments": loads,
        },
        "reference_dump": {
            "version_label": "1.6.57",
            "url": "https://huggingface.co/datasets/jeudhehs/dump/blob/main/dump%201.6.57.cs",
        },
        "rva_interpretation": "RVA is compared directly with ELF p_vaddr; "
                              "confirm the game's module load bias before native use",
        "candidate_bindings": candidates,
        "abi_caveats": [
            "Executable coverage and instruction bytes do not identify a function or prove its signature.",
            "The original target game library is required; this app's compiled overlay library is not a substitute.",
            "Physics.Raycast layerMask and PhysicsUtil.GetLayerMask return use 64-bit C# long.",
            "ARM64 RaycastHit is 48 bytes with a managed Collider pointer at offset 0x28; "
            "HitResult is 40 bytes with Collider at 0x20.",
            "ImpactInfo is a managed class: use the engine's Alloc/Recycle lifetime; "
            "never pass a stack-constructed imitation.",
            "Vector3 argument/return ABI, hidden MethodInfo parameters, virtual dispatch, "
            "method boundaries, and module load bias require binary validation.",
            "Trace flags are engine flags, not raw layer masks; QueryTriggerInteraction 0 means UseGlobal.",
            "UpdateFireInput frequency and ordering, detailed model collisions, smoke semantics, "
            "ADS completion, and fire-input ownership require matching-binary and device validation.",
            "This report neither adds runtime profiles nor authorizes native calls.",
        ],
    }


def write_report(path, text):
    destination = Path(path)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(mode="w", encoding="utf-8", dir=destination.parent,
                                         prefix=f".{destination.name}.", delete=False) as handle:
            temporary = handle.name
            handle.write(text + "\n")
        os.replace(temporary, destination)
        temporary = None
    finally:
        if temporary is not None:
            os.unlink(temporary)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binary", type=Path, help="original ARM64 game library to inspect")
    parser.add_argument("--output", type=Path, help="also write the JSON report to this file")
    args = parser.parse_args(argv)
    try:
        if args.output is not None:
            same_file = args.output.resolve() == args.binary.resolve()
            if args.output.exists() and args.binary.exists():
                same_file = same_file or os.path.samefile(args.output, args.binary)
            if same_file:
                raise ElfError("--output must not refer to the input binary")
        report = inspect(args.binary.read_bytes(), str(args.binary))
        rendered = json.dumps(report, indent=2, sort_keys=True)
        if args.output is not None:
            write_report(args.output, rendered)
    except (OSError, ElfError) as exc:
        print(json.dumps({"analysis": "structural ELF inspection only", "error": str(exc)},
                         sort_keys=True))
        return 2
    print(rendered)
    return 0


if __name__ == "__main__":
    sys.exit(main())
