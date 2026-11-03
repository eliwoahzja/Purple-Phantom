#include "NativeQueries.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace PhantomTrigger {
namespace {
using Address = std::uintptr_t;
using Object = void*;
using Method = const void*;

// Old IL2CPP static entrypoints in this build reserve x0 for a null static
// receiver. Managed parameters follow it; MethodInfo remains the last argument.
template<class Function>
Function Bind(Address base, Address rva) noexcept {
    return reinterpret_cast<Function>(base + rva);
}

template<class Value>
Value Read(Address object, std::size_t offset) noexcept {
    Value value{};
    if (object && offset <= std::numeric_limits<Address>::max() - object &&
        sizeof(value) <= std::numeric_limits<Address>::max() - object - offset)
        std::memcpy(&value, reinterpret_cast<const void*>(object + offset), sizeof(value));
    return value;
}

struct NativeRay { Point3 origin, direction; };
static_assert(sizeof(NativeRay) == 24, "ARM64 camera Ray return layout");
static_assert(sizeof(Point3) == 12, "ARM64 Vector3 HFA argument layout");

constexpr int LocalBulletTrace = 0x800A;
constexpr std::size_t MaximumPawnCount = 512;

bool Finite(const Point3& value) noexcept {
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}

// The native callback owns the match/list lifetime on the game thread. Read the
// base List<Pawn> backing count, not PawnList's optional fake-count property.
bool EnemyMember(Address match, Address pawn) noexcept {
    if (!match || !pawn) return false;
    const Address list = Read<Address>(match, 0x178);
    if (!list) return false;
    const int count = Read<int>(list, 0x18);
    const Address items = Read<Address>(list, 0x10);
    if (count <= 0 || static_cast<std::size_t>(count) > MaximumPawnCount || !items)
        return false;
    const auto capacity = Read<std::uint64_t>(items, 0x18);
    if (capacity < static_cast<std::uint64_t>(count) || capacity > 4096) return false;
    for (int i = 0; i < count; ++i) {
        if (Read<Address>(items, 0x20 + static_cast<std::size_t>(i) * sizeof(Address)) == pawn)
            return true;
    }
    return false;
}

struct OwnedImpact {
    Address base;
    Object object;
    OwnedImpact(Address library, Object value) noexcept : base(library), object(value) {}
    OwnedImpact(const OwnedImpact&) = delete;
    OwnedImpact& operator=(const OwnedImpact&) = delete;
    ~OwnedImpact() noexcept {
        if (object)
            Bind<void(*)(Object, Object, Method)>(base, 0x64B56F0)(nullptr, object, nullptr);
    }
};

bool BodyGroup(int group, bool avatar) noexcept {
    // EHitGroup: Shield=6, GunShield=32. Default=0 is accepted only when the
    // native avatar-detail path explicitly supplied it, never from a broad hit.
    return group != 6 && group != 32 &&
           ((group >= 1 && group <= 31) || (group == 0 && avatar));
}
} // namespace

bool BuildNativeRay(Address base, ViewRay& ray,
                    float& normalizedX, float& normalizedY) noexcept {
    normalizedX = normalizedY = 0.5f;
    if (!base || !std::isfinite(ray.maxDistance) || ray.maxDistance <= 0) return false;
    const auto camera = Bind<Object(*)(Object, Method)>(base, 0x5024010)(nullptr, nullptr);
    if (!camera) return false;
    const Point3 center{0.5f, 0.5f, 0};
    const auto native = Bind<NativeRay(*)(Object, Point3, Method)>(base, 0x5023E78)(camera, center, nullptr);
    if (!Finite(native.origin) || !Finite(native.direction)) return false;
    const double lengthSquared = static_cast<double>(native.direction.x) * native.direction.x +
                                 static_cast<double>(native.direction.y) * native.direction.y +
                                 static_cast<double>(native.direction.z) * native.direction.z;
    if (!std::isfinite(lengthSquared) || lengthSquared < 0.5 || lengthSquared > 1.5) return false;
    const auto screenCenter = Bind<Point3(*)(Object, Point3, Method)>(base, 0x5023DFC)(camera, center, nullptr);
    const int width = Bind<int(*)(Object, Method)>(base, 0x50504BC)(nullptr, nullptr);
    const int height = Bind<int(*)(Object, Method)>(base, 0x5050520)(nullptr, nullptr);
    if (!Finite(screenCenter) || width <= 0 || height <= 0) return false;
    normalizedX = screenCenter.x / static_cast<float>(width);
    normalizedY = 1.0f - screenCenter.y / static_cast<float>(height);
    if (!std::isfinite(normalizedX) || !std::isfinite(normalizedY) ||
        normalizedX < 0 || normalizedX > 1 || normalizedY < 0 || normalizedY > 1) return false;
    const float inverseLength = static_cast<float>(1.0 / std::sqrt(lengthSquared));
    ray.origin = native.origin;
    ray.direction = {native.direction.x * inverseLength,
                     native.direction.y * inverseLength,
                     native.direction.z * inverseLength};
    return true;
}

bool NativeTargetQueries::TraceBody(const ContextIdentity& context,
                                   const ViewRay& ray, BodyHit& result) noexcept {
    result = {};
    if (!base_ || !match_ || !pawn_ || context.match == 0 ||
        !Finite(ray.origin) || !Finite(ray.direction) ||
        !std::isfinite(ray.maxDistance) || ray.maxDistance <= 0) return false;
    OwnedImpact impact{base_, Bind<Object(*)(Object, Method)>(base_, 0x64B554C)(nullptr, nullptr)};
    if (!impact.object) return false;
    using BroadTrace = bool(*)(Object, Point3, Point3, float, int, Object, int, Method);
    const bool broadHit = Bind<BroadTrace>(base_, 0xA18F6C4)(
        nullptr, ray.origin, ray.direction, ray.maxDistance, LocalBulletTrace,
        impact.object, 0, nullptr);
    if (!broadHit) return true;
    result.hit = true;
    result.nearest = true;
    const Address collider = Read<Address>(reinterpret_cast<Address>(impact.object), 0x18);
    if (!collider) return false;
    const Object map = Bind<Object(*)(Object, Method)>(base_, 0x856A020)(nullptr, nullptr);
    if (!map) return false;
    const Object target = Bind<Object(*)(Object, Object, Method)>(base_, 0x773FA68)(
        map, reinterpret_cast<Object>(collider), nullptr);
    const Address targetAddress = reinterpret_cast<Address>(target);
    // The closest world collider, ally, or non-player object blocks acquisition.
    if (!target || targetAddress == pawn_ || !EnemyMember(match_, targetAddress)) return true;
    result.enemy = true;
    const Address info = Read<Address>(targetAddress, 0x78);
    const float health = Read<float>(info, 0x34);
    result.alive = Read<unsigned char>(targetAddress, 0x548) != 0 && info &&
                   std::isfinite(health) && health > 0;
    if (!result.alive) return true;

    // Native refinement selects avatar-detail or legacy fire colliders, honors
    // its native nearest-detail rules, and may replace the pooled object by ref.
    // Only recycle the final pointer after this call.
    using DetailTrace = bool(*)(Object, Object, Object, Point3, Point3,
                               float, int, Object*, Method);
    const bool detailed = Bind<DetailTrace>(base_, 0xA19C4C4)(
        nullptr, target, reinterpret_cast<Object>(collider), ray.origin,
        ray.direction, ray.maxDistance, 0, &impact.object, nullptr);
    if (!detailed || !impact.object) return true;
    const Address address = reinterpret_cast<Address>(impact.object);
    const int group = Read<int>(address, 0x70);
    const bool avatar = Read<unsigned char>(address, 0x8D) != 0;
    result.detailed = BodyGroup(group, avatar);
    if (!result.detailed) return true;
    result.point = Read<Point3>(address, 0x28);
    result.distance = Read<float>(address, 0x5C);
    const auto actor = Read<std::uint32_t>(info, 0x30);
    if (actor == 0) return false;
    // Invocation-scoped token, never a managed address. No target pointer or
    // token is retained by this adapter between callbacks.
    result.targetGeneration = (context.match * 0x100000001b3ULL) ^ actor;
    if (!result.targetGeneration) result.targetGeneration = actor;
    return true;
}

Occlusion NativeTargetQueries::CheckWorld(const ContextIdentity&,
                                         const ViewRay& ray, const BodyHit& hit) noexcept {
    if (!base_ || !std::isfinite(hit.distance) || hit.distance <= 0) return Occlusion::Unavailable;
    float blockerDistance = 0;
    using WorldTrace = bool(*)(Object, Point3, Point3, float, float*, int, Method);
    const bool blocked = Bind<WorldTrace>(base_, 0xA1905E0)(
        nullptr, ray.origin, ray.direction, hit.distance, &blockerDistance, 0, nullptr);
    if (!blocked) return Occlusion::Clear;
    if (!std::isfinite(blockerDistance) || blockerDistance < 0) return Occlusion::Unavailable;
    const float tolerance = std::max(0.0001f,
        8.0f * std::numeric_limits<float>::epsilon() * hit.distance);
    return blockerDistance + tolerance < hit.distance ? Occlusion::Blocked : Occlusion::Clear;
}

Occlusion NativeTargetQueries::CheckSmoke(const ContextIdentity&,
                                         const ViewRay& ray, const BodyHit& hit) noexcept {
    if (!base_ || !match_ || !Finite(ray.origin) || !Finite(hit.point))
        return Occlusion::Unavailable;
    const auto match = reinterpret_cast<Object>(match_);
    using PointQuery = bool(*)(Object, Point3, Method);
    using SegmentQuery = bool(*)(Object, Point3, Point3, Method);
    const auto inSmoke = Bind<PointQuery>(base_, 0x9CC3518);
    if (inSmoke(match, ray.origin, nullptr) || inSmoke(match, hit.point, nullptr) ||
        Bind<SegmentQuery>(base_, 0x9CC36E0)(match, ray.origin, hit.point, nullptr) ||
        Bind<SegmentQuery>(base_, 0x9CC38AC)(match, ray.origin, hit.point, nullptr))
        return Occlusion::Blocked;
    return Occlusion::Clear;
}
} // namespace PhantomTrigger
