# Stance and sprint-to-fire controls

Both controls appear under **Movement → Stance and firing**, start disabled,
and are session-only. They use one atomic settings snapshot; the existing saved
config layout is unchanged. They operate independently of the trigger bot.

## No crouch

The local controller's `CheckCrouch` request is suppressed while enabled. The
adapter matches the receiver to `Gameplay.LocalPC`, its current pawn to
`Gameplay.LocalPawn`, and requires a live match and alive pawn. The live-match
check precedes the local-controller getter, whose native implementation can
throw without its gameplay owner. Remote players retain native behavior.

After the original `UpdateLocalPawn`, a crouched, walking local pawn can request
`StandUp(immediately=false, force=false)` once per outer callback. The adapter
skips prone, slides, airborne/vehicle states, and stance transitions. Normal
native clearance and camera-transition behavior remain in control: low ceilings
and world/server-required crouching are not forced away. Turning the option
off immediately restores normal crouch requests.

The input and local-update hooks form one installation group. A partial
installation is rolled back. Native hotfixes for crouch input, local update, or
standing leave those paths unchanged and publish an unavailable status.

## No sprint-to-fire delay

The adapter calls the original `Weapon.get_DelaySprintFire` first, preserving
metadata initialization and owner/property evaluation. It overrides a positive,
finite result with zero only for an ordinary firearm owned by the current alive
local pawn in a live match. Other pawns, melee/special weapons, invalid values,
and active sprint get-ready hotfixes keep their native result. There are no
persistent weapon-field writes or saved pointers to restore on disable.

This removes the sprint-specific penalty. The native
`WS_WeaponMoveHipFireGetReady` state (11) computes
`max(DelaySprintFire, GetFireInterval)`, so the normal firing-interval floor still
applies. ADS, equip, reload, bolt, and firing cadence are unchanged. A countdown
already initialized before enabling the option finishes normally; subsequent
reads use the current setting. The existing spectate/network delay hooks are
unrelated and unchanged.

## Native profile

The supplied ARM64 1.6.57 library has GNU build ID `1305bdeea30b989f` and SHA-256
`91781232f051c712cbc7f78ebdb95f4faa739b70c9b2567c05b9fa5b8c53ed1f`.
Every called/hooked entrypoint has a sixteen-byte startup fingerprint, checked
against readable executable segments before hook installation. The shared
validator checks this build ID on each feature's dependency group. A mismatch
disables only that optional feature; it cannot approve or disable the trigger.

| Native entrypoint | RVA | ABI |
| --- | --- | --- |
| PlayerController.CheckCrouch | `0x666AE64` | `void(controller, metadata)` |
| PlayerController.UpdateLocalPawn | `0x666894C` | `void(controller, float dt, metadata)` |
| Gameplay.get_LocalPC | `0x856A8A8` | `controller(null receiver, metadata)` |
| Pawn.StandUp | `0x518BC48` | `void(pawn, bool immediately, bool force, metadata)` |
| Pawn.IsInTransition | `0x51B463C` | `bool(pawn, metadata)` |
| Weapon.get_DelaySprintFire | `0x513CBC4` | `float(weapon, metadata)` |

The crouch methods are clear in the supplied original file. The sprint getter's
analysis uses source page `0x514C000` at target `0x513C000`, XOR `0x46`, consistent
with the previously documented protected-page analysis. Its field at weapon
`+0x9F8`, callers, owner-property evaluation, and native state duration were
cross-checked against the dump and native call flow. Only expected instruction
bytes are compiled into the profile; no decoded library is shipped. Startup
must match the loaded getter prefix before that hook can be installed.

## Verification

```sh
bash tests/run_movement_tests.sh
python3 -B tests/movement_binary_tests.py
bash tests/run_trigger_compatibility_tests.sh
python3 -B tests/trigger_binary_tests.py
```

The movement sanitizer suite covers defaults, independent atomic settings,
match loss/death/respawn/remote ownership, stance exclusions, finite native
timing, and restoration when disabled. Static checks cover all native
fingerprints and independent dependency groups. The shared validator and
production ARM64 library also require compilation with the existing NDK target.

On-device acceptance remains pending. Check normal crouch before/after the
toggle; activation while already crouched; low ceilings; prone and slides;
death/respawn and repeated matches; manual and trigger firing from sprint with
automatic, semi-auto, and sniper weapons; and enabling/disabling during an
existing transition. Confirm that each requested feature shows a ready status,
then compare firing cadence and frame times with both options disabled.
