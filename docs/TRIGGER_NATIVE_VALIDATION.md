# Native trigger validation

The original ARM64 **game** library for the installed game build is required.
It is not included in this repository. The supplied 1.6.57 C# dump records
candidate addresses and declarations; it does not contain method bodies or
establish that those addresses match the installed binary. The app's compiled
overlay library is not a substitute for the original game library.

## Implementation status

This change implements the testable firing state machine, target-query pipeline,
session-only controls, independent dot rendering, dump reference layouts, and
offline binary inspector. **It does not yet implement an active game adapter or
install trigger hooks.** There are no approved binary profiles; enabling the UI
switch stores a request and continues to show unavailable. The pure input helper
can produce intents in tests, but nothing applies them to game input.

The remaining implementation needs the original game library: native query and
input adapters, verified weapon-mode detection, live viewport publication,
lifecycle generation tracking, and device acceptance. A binary inspection report
alone is insufficient to fill in those semantics.

## Reproducible local checks

```sh
bash tests/run_trigger_tests.sh
bash tests/run_trigger_runtime_tests.sh
bash tests/run_trigger_overlay_tests.sh
python3 -B tests/trigger_binary_tests.py
```

All three C++ suites passed with AddressSanitizer and UndefinedBehaviorSanitizer.
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

The compiled list of accepted native trigger profiles starts empty. The trigger
must remain inactive until a matching binary profile has passed the checks below.
Do not activate a profile based on a version string, executable address coverage,
pointer plausibility, or a report from the inspector alone.

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
the compiled native profile list remains empty.
