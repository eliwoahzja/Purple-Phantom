# Crash triage for the movement-options candidate

The user reported repeated crashes while using commit
`49e2bbb8f0cc59251976781246458d0bfed1fced`, followed by the game's environment
error **15035**, which also appears with the module disabled. The screenshot is
an environment/login message, not a native crash backtrace. It does not establish
the crash location or the cause, scope, or duration of an account restriction.
The game's Help option or official Garena support must resolve the login issue.

The existing module also starts unconditional library-patching code from
`JNI_OnLoad`. Turning individual feature toggles off therefore does not produce
an unmodified client. A clean-client check requires the module to be completely
disabled or removed, as the user reports having done.

## Source hardening

- Remove duplicate startup hooks at `libunity.so + 0x9FDF8E0` and
  `libunity.so + 0x9FDEE28`. The latter reused the same original-function output
  pointer. Each target now receives a single registration; existing handlers
  and their feature settings are unchanged.
- Remove unused ESP health reads through `PlayerInfo + 0x78`. The supplied dump
  identifies that member as `m_BRPlayerState`, an integer, not an
  `AttackableTargetInfo` pointer. Dereferencing it as a pointer is invalid.
  Health rendering already uses the pawn's separate, checked health-info path.
- Retain debugging information in the NDK intermediate library. The installed
  library is stripped by the NDK; the intermediate version remains available
  for future source-level crash symbolication.

These are confirmed source hazards. Without a crash log, none can be identified
as the cause of the user's observed crashes. These changes cannot clear 15035
or establish that the module is stable on the device.

## Preserve the failing build

The published movement-options library has:

- SHA-256: `5eef046c601c81b6a59e098ff595ab13c530175d90f45d69133d46c6c189816f`
- Build ID: `31894550327fb732e3dac9c1f1a5dccbff0a70b0`

Its original object files were relinked into an offline symbols file before the
next rebuild. Every allocated section's bytes, size, and virtual address were
compared with the published library and match, including the original Build ID.
This reference lives outside the repository at
`/workspace/build-validation/crash-reference-49e2bbb/`, with the verification
record `symbol-reference.json`. It is for offline symbolication, not installation.

Unfinished sniper release-mode and diagnostics work is saved separately as a
local Git stash and `/workspace/trigger-sniper-diagnostics-wip.patch`. It is not
part of this source change or a new gameplay candidate.

## Collect existing crash evidence

Disable the module and restore the official installation for the clean-client
check. Do not repeatedly load the crashing module to generate another failure.

If ADB is available, read any crash evidence still retained on the device:

```sh
adb logcat -b crash -d -v threadtime > codm-crash.txt
adb logcat -d -v threadtime > codm-logcat.txt
```

An exported native tombstone is also useful. Preserve the `Fatal signal` or
exception header, crashing thread, backtrace, library offsets, Build IDs, and
the events immediately before the crash. Include the device model, Android
version, enabled options, and action underway. Existing log buffers can rotate;
the login screenshot cannot substitute for this evidence.

## Validation

```sh
python3 -B tests/native_hook_registration_tests.py
```

Build the production ARM64 library and verify that the intermediate library
retains `.debug_info` while the installed library does not. Device acceptance
remains pending, and no replacement library is offered as a remedy for the
environment/login restriction.
