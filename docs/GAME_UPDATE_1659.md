# Game update 1.6.59

The updated files were inspected from dataset revision
`2a376e8ee0bf5d907b4dfedbeabad8e1b75c45c4` of
<https://huggingface.co/datasets/jeudhehs/dump>. Reference copies are preserved
outside this repository alongside the previous 1.6.57 files. No supplied
binaries were executed.

## Verified upload identities

Both APK manifests identify package `com.garena.game.codm`, version code
**28100**; the base manifest gives version **1.6.59**. The ABI split is
`config.arm64_v8a`.

| File | SHA-256 |
| --- | --- |
| `Dump 1.6.59.cs` | `f370b52ac099770074b888a745e0d255407c6ea6d5314f10561a10e2927954b6` |
| `base.apk` | `6c819169a2ca9cd01c23285fe52945f93d92abaedb1f7926da68290b150e886c` |
| `split_config.arm64_v8a.apk` | `23766431e05e11d5bb8377b2a888804b6df58af01364988705fe7c40001af4f2` |
| Extracted `libunity.so` | `4e2e723da70deefb2cb986375645ae2fcab47343472cb7fdfcb49077b4919111` |
| Extracted `libanort.so` | `b0cd26677896beaf9252bd5da620a211d6d5d4d4079d0b0daeb334c75c4bb259` |
| Extracted `global-metadata.dat` | `0f4546625b24f33493ae83f7b2c37ed639eb72f4e3b8535619814db0e9f32ff5` |

`libunity.so` is a 259,122,456-byte AArch64 ELF with GNU build ID
`056d6390cc712865`. The former build ID was `1305bdeea30b989f`.
The base APK contains normal IL2CPP metadata, version 23, size 63,814,100 bytes.

The additional text upload is an annotated native-address list. It has no
fatal-signal header, timestamps, stack frames, or Build IDs and cannot identify
the cause of the previously reported crashes. Existing symbols for the failing
1.6.57 candidate remain available for an actual crash backtrace.

## Declaration and layout changes

Matching by assembly, namespace, declaring class, and complete method signature
found unique declarations for **54 of 56** trigger/movement dependencies. The
two native hotfix helpers have no managed declaration. The
[candidate map](reference/trigger-movement-1.6.59-candidates.json) is reference
data only; it does not approve native calls or enable a new runtime profile.

Selected instance-field changes:

| Field | 1.6.57 | 1.6.59 |
| --- | --- | --- |
| Pawn alive | `0x548` | `0x550` |
| Pawn crouching | `0x4C8` | `0x4D0` |
| Pawn prone | `0x4C9` | `0x4D1` |
| Pawn physics state | `0x510` | `0x518` |
| Pawn head bone | `0x308` | `0x310` |
| Pawn player info | `0x5C0` | `0x5C8` |
| Pawn mesh | `0x628` | `0x630` |
| Weapon burst flag | `0x20C` | `0x22C` |
| Weapon release-fire flag | `0x245` | `0x265` |
| Weapon sprint-fire delay | `0x9F8` | `0xAA0` |

`Weapon.CanFire` moves from virtual slot 269 to 274. The instant/projectile
bullet callbacks move from slots 112/124 to 114/126. The inspected
`CheckFireParam`, `CheckFireResult`, `ImpactInfo`, avatar-collider, and fire-input
controller field layouts remain unchanged. A single RVA or field-offset delta
cannot migrate these changes correctly.

## Unsupported-build startup protection

Previously, the trigger and movement adapters rejected an unmatched game, but
legacy startup hooks and patches could still run with their old addresses.
Both startup threads now stop before native mutations unless the loaded game
passes the complete compiled profile check. The whole-module build-ID pin is
independent of the trigger's identity: updating the trigger alone cannot enable
old Main/SkinHax or other legacy offsets.

**1.6.59 remains unsupported at runtime.** This change prevents stale-address
startup; it does not port the gameplay features. The old native profile,
field accesses, and hooks have not been relabeled as compatible.

Local validation passed: the ASan/UBSan compatibility suite, startup-path and
duplicate-hook checks, binary/dependency checks, strict ARM64 compilation, and
the production NDK build. The tests explicitly reject the new build-ID note.

Before enabling 1.6.59, verify matching native bodies, field layouts, virtual
dispatch, helper implementations, hotfix IDs, and the input/collision contracts.
Legacy Main/ESP/SkinHax paths also need their own migration. The earlier device
crashes remain unresolved without a real crash log, and the reported login
restriction must be handled separately through official support.
