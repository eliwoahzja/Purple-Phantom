# Native trigger validation

The user supplied the ARM64 game library through the linked dataset on
2026-11-03. The 1.6.57 C# dump records candidate addresses and declarations;
it does not establish that those addresses match the installed runtime code.
The large binary stays outside this repository.

## Supplied binary findings

- File size: `246851416` bytes; ELF64 little-endian AArch64.
- SHA-256: `91781232f051c712cbc7f78ebdb95f4faa739b70c9b2567c05b9fa5b8c53ed1f`.
- GNU build ID: `1305bdeea30b989f`.
- `Weapon.get_IsSingleShot` at `0x50EBA5C` calls the active component getter
  and then `WeaponFireComponent.get_IsSingleShot`. The burst getter at
  `0x50EBB28` reads the dump's weapon field at `0x20C` and returns.
- The clear `UpdateFireInput` path calls `UpdateFireButtonStatus` before
  `CheckFireInput`, but it also has a hotfix delegate path. Static inspection
  alone does not prove which path the installed game executes.
- The dump's eight-byte `set_CheckFire` at `0xA81F798` should store the bool
  at `0x13` and return (`014c0039c0035fd6`). The supplied file instead has
  `e64646f2ce24047f` there. The held-input getter also differs, and the
  recorded body-trace/shot-acknowledgement routines contain long runs of
  bytes that do not disassemble as coherent ARM64 instructions.

The two supplied in-game screenshots subsequently confirmed the loaded
`set_CheckFire`, `IsPress`, burst getter, shot counter, input update, body-trace,
and smoke-query windows. The loaded setters match their dump field layouts.
For static analysis only, selected encoded/reordered pages were reconstructed
and cross-checked against those samples, declared field offsets, and native
call graphs. No reconstructed library or decoding code is shipped or installed.
The startup profile checks the actual loaded instructions before using them.

## Read-only device report

Build this branch's production native library using the project's existing
process. In the running game's menu, enter **Aim → Game compatibility**, then
press **Check game compatibility** after entering a match. Share the report
shown there (screenshots are sufficient).

The check locates loaded `libunity.so` through the loader, validates each
requested code window against its readable executable segments, and captures
only seven fixed sixteen-byte code windows. Ordinary `pread` calls on
`/proc/self/mem` handle unavailable or removed mappings as errors. It does not dereference game objects,
call game functions, change memory permissions, patch instructions, install
hooks, or enable firing. It runs only on the explicit button press. If the
process denies access to its memory file, the report says unreadable. No raw
process-memory syscalls or signal handlers are used.

The diagnostic button remains read-only. After native installation, its input
update window may show this module's hook; this is expected. It does not install
or approve the trigger itself.

## Implementation status

The production ARM64 target now contains a native adapter for the supplied
1.6.57 build. Startup verifies GNU build ID `1305bdeea30b989f` and all 50 compiled
entrypoint fingerprints against readable executable mappings before installing
four input/lifecycle hooks. Missing or changed code leaves firing disabled with
an explicit status. Partial hook installation is rolled back; no settings flag
or JSON file can bypass the validation.

The adapter merges its own press/hold/release source after native manual input
capture and before the game's remaining input checks. Native input takes
priority, ADS flags are preserved, and native cadence/bolt/reload gates remain
in control. Target loss and invalid state revoke synthetic input. Lifecycle
hooks and per-context generation counters prevent reuse across matches, pawns,
weapons, components, and input controllers. Relevant native input hotfix paths
are excluded; the adapter does not assume a patched delegate follows the
inspected code flow. With trigger and dot disabled and no owned/pending input,
the adapter takes an early path without weapon queries, camera construction,
or physics. While active, single-shot acknowledgement uses a monotonic local
bullet-creation sequence from the existing bullet callbacks, not the game's
resettable per-fire-group shot count. The callbacks keep their prior aim logic
and merely notify the current local component identity.

A center ray from the active game camera first uses the native local bullet
trace (`0x800A`) across the camera's visible clip depth (`farClip - nearClip`), maps the nearest collider to its target, and invokes native
avatar/legacy damage-collider refinement. A broad box alone is insufficient.
Detection is independent of the weapon's authored trace range, so nearby and
distant visible enemies follow the same eligibility rule. Native weapon travel
and firing rules remain game-controlled. Separate world and smoke queries cover
the actual detailed hit segment. The
pooled hit object is invocation-local and recycled exactly once. Camera viewport
coordinates are published atomically to the dot renderer; game pointers stay
on the game-input thread.

Ordinary automatic and single-shot firearms are classified by their active
component's firing properties. Base-weapon and COD-sniper input preparation
handlers are recognized. A sniper's generic single-shot/release marker is not
its effective firing mode: the native controller predicate determines whether
that configuration actually requires release-to-fire. Down-triggered snipers
can use repeated native single taps; true release-to-fire configurations remain
unsupported.

Cancellation never uses an up-edge while that edge could fire. It publishes a
source-owned StopFire result after the native input calculation, with StartFire
and public fire edges cleared, leaving aim flags unchanged. A context-keyed
pending cleanup consumes a safe release before a fresh down if the same weapon
returns to down-triggered mode. A second check after weapon input preparation
catches mode/marker changes within the update. Cancellation tests cover these
transitions, manual takeover, and context replacement.

Burst, true release/charge, melee, special weapons, unreviewed input handlers,
unknown states, and native basic auto-fire profiles are excluded. Manual fire
remains available. Settings are session-only and start disabled.

**Device acceptance is still pending for this connected build.** The screenshots
validate code windows, not firing behavior or performance. Run the scenarios
below before treating the feature as stable.

Known conservative behavior:

- An enemy's nearest broad collider that misses detailed body geometry ends
  acquisition for that update; a farther enemy behind that envelope is not
  searched. Exposed limbs and overlapping players need device verification.
- The game's special smoke-cluster query can suppress a shot near smoke even
  when the simple ray-volume test would be clear. It is retained because the
  ordinary smoke-segment query skips that smoke type.
- Entry-point checks reject conflicting hooks and code variants. The module
  deliberately does not guess a replacement offset or bypass native profiles.

## Reproducible local checks

```sh
bash tests/run_trigger_tests.sh
bash tests/run_trigger_runtime_tests.sh
bash tests/run_trigger_overlay_tests.sh
bash tests/run_trigger_compatibility_tests.sh
python3 -B tests/trigger_binary_tests.py
```

All four C++ suites passed with AddressSanitizer and UndefinedBehaviorSanitizer.
The Python suite passed for structural ELF inspection, malformed input, and
protecting the original binary from accidental output overwrite. The reference
layout header was also compiled with the Android ARM64 compiler.

The production ARM64 native target passed using NDK 22.0.7026061, with build
outputs outside the repository and the existing checked-in library untouched:

```sh
ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=src/main/jni/Android.mk \
  NDK_APPLICATION_MK=src/main/jni/Application.mk \
  NDK_OUT=/tmp/phantom-build/obj NDK_LIBS_OUT=/tmp/phantom-build/libs -j4
```

The optional preview target fails on both original main and this change because
its existing preview state lacks `speed_min`, `speed_max`, and `HIDEESP`.
Those unrelated preview definitions were left unchanged. No APK packaging,
live game firing, frame-time benchmark, or device stability claim has been made.

The compiled native profile is specific to this supplied build. Do not add a
new profile based only on a version string, executable coverage, or a structural
inspection report; recheck the native ABI, call flow, and device behavior.

## Inspect the original library

Run from the repository root with Python 3:

```sh
python3 tools/inspect_trigger_binary.py /path/to/original/libunity.so --output /tmp/trigger-binary-report.json
```

The tool reads ELF64 little-endian AArch64 input, computes its SHA256 and GNU
build ID, and reports whether each dump candidate's first instruction lies in
file-backed executable `PT_LOAD` data. It prints JSON and optionally writes the
same report. Malformed or unsupported input returns exit code 2 with a JSON
error. Extended ELF header numbering is intentionally unsupported.
Known short accessors also report their expected instruction prefix and whether
the supplied bytes match. A match does not imply semantic or runtime approval.

The tool cannot determine whether a candidate is the expected function, approve
a runtime profile, activate hooks, or establish collision, smoke, timing, or
firing behavior. RVAs are compared to ELF virtual addresses; native integration
must separately establish the correct module and load bias. Missing or multiple
GNU build IDs are reported rather than converted into an identity match.

## Validate bindings before enabling native calls

- Match the original library's architecture, SHA256, GNU build ID, module load
  bias, and relevant code to the selected dump. Never enable on an identity
  mismatch. Revalidate every new game binary.
- Inspect method bodies/call sites to establish exact function boundaries,
  static versus instance calls, virtual dispatch, hidden IL2CPP metadata
  arguments, and the ARM64 ABI for `Vector3` and value-type returns. This build
  uses 64-bit `long` layer masks. Its `RaycastHit` contains a managed `Collider`
  pointer at `0x28`; `ImpactInfo` is a managed class with engine pool ownership.
- Establish the native firing update boundary on the game thread. The candidate
  `WeaponFireModeBase.UpdateFireInput(float)` must run with fresh camera, ADS,
  and weapon state, including updates with no physical fire-button press. Verify
  ordering against input consumption and prevent duplicate decisions per update.
  Rendering callbacks and movement-component ticks are not timing evidence.
  Inspect `WeaponFireModeUnit.UpdateFireButtonStatus(CheckFireParam)` after its
  original manual-input capture and before `CheckFireInput` consumes the result.
  Verify fire-only merging cannot create `StartAim`/`StopAim` through the user's
  native firing profile, including profiles that normally scope when firing.
- Establish full-body hit semantics and trace flags from the game's own weapon
  path. `PhysicsUtil.GetLayerMask(int)` converts engine trace flags; do not guess
  raw layer bits. Verify whether broad hits need the engine's avatar/detail
  collider refinement. Preserve nearer blockers and exclude the local pawn.
- Establish the selected primary weapon's current fire mode, readiness,
  ammunition, reload, burst, bolt, and charge behavior. Weapon class names alone
  cannot distinguish automatic and semi-automatic assault rifles. Identify the
  input transition that yields exactly one legal shot for tap weapons.
- Establish manual-input ownership independently of synthetic trigger input.
  Manual input has priority. A synthetic release must release only the trigger's
  own input; it must never cancel a physical press or inherit an old pawn/weapon.

## Required device scenarios

Record the exact game binary identity, device/OS, weapon/loadout, native update
and fire timestamps, frame times, input edges, and crash logs for each run.

| Scenario | Required result |
| --- | --- |
| Dot beside the model or between limbs | No synthetic shot, including when inside an ESP box or broad movement collider. |
| Dot over exposed head, chest, hips, arms/hands, or legs/feet | Every supported damage collider can qualify; another covered body part must not veto the exposed hit. |
| Partial cover, crouching, prone, leaning, animation, and different operators | Only the exposed body intersection qualifies; custom avatar colliders remain accurate. |
| Wall, door, shield, nearer player, or other solid cover in front of the hit | No synthetic shot through the blocker. |
| Smoke between camera and body, smoke at either endpoint, and smoke-cluster effects | No synthetic shot while the game's visibility query reports obstruction. Clearing smoke permits the first eligible update. |
| ADS Only: hip fire, scope opening, fully scoped, and scope closing | Fire only while fully scoped. No added timer after the full-ADS state is reached. |
| Both: hip fire and scoped transitions | The same body/visibility rule applies throughout; no automatic scoping. |
| Trigger enabled, disabled, or mode changed | Only trigger state and input change; the weapon profile, ADS timing, recoil, spread, fire rate, and unrelated feature settings remain intact. |
| Empty magazine and manual reload | No repeated synthetic press while empty or reloading; resume only after the game reports legal ammunition and readiness. |
| Automatic rifle/SMG and sustained valid hit | Hold only trigger-owned primary fire. Stop promptly when eligibility is lost. Native cadence remains intact. |
| Semi-auto rifle, semi-auto assault-rifle mode, marksman, and tap-only sniper | One native down/up cycle per permitted shot; repeat only when ready and still eligible. No frame-spammed edges, duplicate shots, or artificial cooldown. |
| Burst mode, bolt action, charge/release weapon, and mode switching | Preserve native burst/bolt/charge rules. Unsupported input contracts remain inactive rather than being treated as automatic fire. |
| Physical fire held before, during, and after synthetic acquisition | Manual input takes priority and is never released by the trigger; physical scope input is preserved. |
| Death, respawn, weapon swap/drop, spectator/lobby transitions, app pause/resume, and repeated matches | No stale target/pawn/weapon dereference or stuck input; reacquire only valid current state. |

“No added delay” means the first eligible native update can request a legal shot.
It does not remove the game's ADS animation, weapon cadence, bolt/reload/charge
rules, frame interval, or network latency. Verify first-shot timing separately
from subsequent legal shot cadence.

Run an extended session covering the transitions above with the menu closed and
with unrelated features toggled. Compare frame-time distribution and native
memory use with trigger disabled. Require no crashes, stuck firing, unbounded
per-frame allocations, or repeated expensive initialization. Save the binary
inspection report and device evidence with the reviewed native profile. Until
that evidence exists, the structural inspector remains a diagnostic tool and
a new or changed profile must remain disabled until its native validation passes.
