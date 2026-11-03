#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

// REFERENCE ONLY. The C# dump has declarations, not native method bodies or a
// library fingerprint. These RVAs are never resolved or called by this header.
// Source: https://huggingface.co/datasets/jeudhehs/dump/blob/main/dump%201.6.57.cs
namespace PhantomTrigger {
namespace Dump1657 {

struct Vector2 { float x, y; };
struct Vector3 { float x, y, z; };
struct Collider;
struct Pawn;
struct Weapon;
struct AttackableTarget;
struct ImpactInfo; // Managed GameEngine class; never a stack value.
struct CheckFireParam; // Managed global class; setters can have side effects.
struct WeaponFireModeUnit;
struct BaseGame;
struct AttackableColliderMap;
struct AttackableTargetList; // Managed generic list, never std::vector.

// Custom ARM64 UnityEngine.RaycastHit layout from this dump. m_Collider is a
// managed Collider reference, not the int instance ID in newer Unity versions.
struct RaycastHit {
    Vector3 point;
    Vector3 normal;
    std::int32_t faceId;
    float distance;
    Vector2 uv;
    Collider* collider;
};

struct HitResult {
    Vector3 location;
    Vector3 normal;
    float distance;
    std::int32_t materialType;
    Collider* collider;
};

static_assert(sizeof(void*) == 8, "Dump 1.6.57 reference supports 64-bit ABI only");
static_assert(std::is_standard_layout<RaycastHit>::value, "RaycastHit layout");
static_assert(sizeof(Vector3) == 12 && sizeof(Vector2) == 8, "Unity vector layout");
static_assert(sizeof(RaycastHit) == 48, "Custom RaycastHit size");
static_assert(offsetof(RaycastHit, normal) == 0xC, "RaycastHit normal");
static_assert(offsetof(RaycastHit, faceId) == 0x18, "RaycastHit faceId");
static_assert(offsetof(RaycastHit, distance) == 0x1C, "RaycastHit distance");
static_assert(offsetof(RaycastHit, uv) == 0x20, "RaycastHit UV");
static_assert(offsetof(RaycastHit, collider) == 0x28, "RaycastHit Collider pointer");
static_assert(sizeof(HitResult) == 40, "HitResult size");
static_assert(offsetof(HitResult, distance) == 0x18, "HitResult distance");
static_assert(offsetof(HitResult, collider) == 0x20, "HitResult Collider pointer");

enum class QueryTriggerInteraction : std::int32_t { UseGlobal = 0, Ignore = 1, Collide = 2 };
enum class WeaponFireMode : std::int32_t { None = 0, Auto = 1, Burst = 2, Single = 3 };
enum class FireColliderShape : std::int32_t { Capsule = 0, Obb = 1, Other = 2 };

// Managed declaration signatures only. IL2CPP wrappers can require a trailing
// MethodInfo pointer and architecture-specific struct marshalling. They are not
// native function-pointer typedefs and must not be reinterpret_cast as such.
using PhysicsRaycastDeclaration = bool(Vector3, Vector3, RaycastHit&, float,
                                       std::int64_t, QueryTriggerInteraction);
using FullTraceDeclaration = bool(Vector3, Vector3, float, std::int32_t,
                                  ImpactInfo*, AttackableTargetList*, QueryTriggerInteraction);
using UpdateFireButtonsDeclaration = void(WeaponFireModeUnit*, CheckFireParam*);

struct ReferenceMethod {
    const char* declaration;
    std::uintptr_t rva;
};

inline constexpr ReferenceMethod Methods[] = {
    {"UnityEngine.Physics.static bool Raycast(Vector3,Vector3,out RaycastHit,float,long,QueryTriggerInteraction)", 0x506C118},
    {"GameEngine.PhysicsUtil.static bool SingleLineCheckNoAlloc(Vector3,Vector3,float,int,ImpactInfo,List<AttackableTarget>,QueryTriggerInteraction=0)", 0xA18FEB4},
    {"GameEngine.PhysicsUtil.static long GetLayerMask(int traceFlag)", 0xA18BEF4},
    {"GameEngine.PhysicsUtil.static bool SingleLineCheckAttackableTargetNoAlloc_Complete(AttackableTarget,Vector3,Vector3,float,int,ImpactInfo)", 0xA191B3C},
    {"GameEngine.PhysicsUtil.static bool RaycastAvatarCollider(int,AvatarCollider,Vector3,Vector3,ref HitResult)", 0xA19B95C},
    {"BaseGame.instance bool IsInSmoke(Vector3)", 0x9CC3518},
    {"BaseGame.instance bool IsSmokeInMiddle(Vector3,Vector3)", 0x9CC36E0},
    {"BaseGame.instance bool IsSmokeClusterNotInSameSide(Vector3,Vector3)", 0x9CC38AC},
    {"GameEngine.ImpactInfo.static ImpactInfo Alloc()", 0x64B554C},
    {"GameEngine.ImpactInfo.static void Recycle(ImpactInfo)", 0x64B56F0},
    {"GamePlay.static AttackableColliderMap get_AttackableColliderMap()", 0x856A020},
    {"AttackableColliderMap.instance AttackableTarget GetAttackableTargetForCollider(Collider)", 0x773FA68},
    {"GameBase.Pawn.instance bool IsAiming()", 0x519BB10},
    {"GameBase.Pawn.virtual bool CanFire(bool ignoreChangingFireModeTransition=false)", 0x5260EC0},
    {"GameBase.Weapon.instance float GetAimingPercentage()", 0x5113B4C},
    {"GameBase.Weapon.instance bool AmmoCanFire()", 0x50EB73C},
    {"GameBase.Weapon.virtual bool CanFire()", 0x5126CC4},
    {"GameBase.Weapon.instance bool get_IsSingleShot()", 0x50EBA5C},
    {"GameBase.Weapon.instance bool get_IsBurstFire()", 0x50EBB28},
    {"GameBase.Weapon.instance WeaponFireComponent get_CurrentFireComponent()", 0x50EB1E4},
    {"GameBase.Weapon.instance byte get_CurrentFireMode()", 0x50EB240},
    {"GameBase.Weapon.instance int get_ShotFired()", 0x50EC09C},
    {"WeaponFireModeBase.instance void UpdateFireInput(float)", 0xBB4AE14},
    {"WeaponFireModeBase.instance void CheckFireInput(CheckFireParam)", 0xBB4BF28},
    {"WeaponFireModeUnit.instance void UpdateFireButtonStatus(CheckFireParam)", 0xBB4B66C},
    {"WeaponFireModeMgr.static WeaponFireModeBase get_FireModeControl()", 0xBB503EC}
};

// These are NOT interchangeable: traceFlag is converted by GetLayerMask into
// a 64-bit physics layer mask. No numeric traceFlag definitions were in the dump.
// Weapon.CurrentFireMode's byte must not be assumed to use WeaponFireMode enum.
// Full ADS endpoint, manual input mapping, pool lifetime, return conventions,
// library base, native signatures, and callback ordering need binary/device tests.

} // namespace Dump1657
} // namespace PhantomTrigger
