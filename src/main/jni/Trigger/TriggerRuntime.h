#pragma once

#include <cstddef>
#include <cstdint>
#include "TriggerCore.h"

namespace PhantomTrigger {

// UI requests are one atomic snapshot. They cannot approve a binary or install
// native hooks. Production validates its compiled native profile at startup.
Settings GetSettings() noexcept;
void SetSettings(const Settings& settings) noexcept;

enum class RuntimeStatus : std::uint8_t {
    Disabled,
    WaitingForBinaryVerification,
    WaitingForContext,
    Ready,
    InvalidNativeState
};

RuntimeStatus GetRuntimeStatus() noexcept;
const char* GetRuntimeStatusText() noexcept;
FireMode GetDetectedFireMode() noexcept;
const char* GetDetectedFireModeText() noexcept;

// Normalized coordinates use the top-left render origin. The fallback is the
// screen center; false means no verified native camera viewport is available.
bool GetViewportCenter(float& normalizedX, float& normalizedY) noexcept;

struct Point3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct ViewRay {
    Point3 origin{};
    Point3 direction{}; // Unit vector through the exact center of the dot.
    float maxDistance = 0.0f;
};

struct BodyHit {
    bool hit = false;
    bool enemy = false;
    bool alive = false;
    bool detailed = false; // Native damage/model collider, never a broad box.
    bool nearest = false; // No skipped nearer body, shield, or world blocker.
    std::uint64_t targetGeneration = 0; // Generation token, never a game pointer.
    Point3 point{};
    float distance = 0.0f;
};

enum class Occlusion : std::uint8_t { Unavailable, Clear, Blocked };

// DTO-only callbacks keep game objects, managed allocations, and method ABIs
// inside the future approved native profile. A query error is not clear LOS.
class TargetQueries {
public:
    virtual ~TargetQueries() = default;
    virtual bool TraceBody(const ContextIdentity& context, const ViewRay& ray,
                           BodyHit& result) noexcept = 0;
    virtual Occlusion CheckWorld(const ContextIdentity& context,
                                 const ViewRay& ray, const BodyHit& hit) noexcept = 0;
    // Must include source/target in smoke and smoke between them, including
    // smoke-cluster variants, rather than relying on physics particle collision.
    virtual Occlusion CheckSmoke(const ContextIdentity& context,
                                 const ViewRay& ray, const BodyHit& hit) noexcept = 0;
};

enum class TargetResult : std::uint8_t {
    InvalidRay,
    QueryUnavailable,
    NoEligibleBody,
    InvalidHit,
    WorldBlocked,
    SmokeBlocked,
    Eligible
};

TargetResult EvaluateTarget(TargetQueries& queries,
                            const ContextIdentity& context,
                            const ViewRay& ray) noexcept;

// Share the acquisition gate with the native adapter so an ineligible update
// needs neither camera construction nor physics. Owned input still requires
// fresh visibility checks, including while a single-shot weapon is cycling.
bool NeedsTargetQuery(const Settings& settings, const Frame& frame,
                      const Controller& controller) noexcept;

// Pure adapter coordinator: computes an intent and never calls a native method,
// installs a hook, publishes Ready, or approves bindings. The compiled native
// binary profile must obtain a consistent local frame at its validated native
// input boundary, call this once, then merge the result into native fire input.
// frame.bindingsReady is input to a pure computation, not runtime activation.
Intent EvaluateInput(const Settings& settings, const Frame& frame,
                     const ViewRay& ray, TargetQueries& queries,
                     Controller& controller) noexcept;

} // namespace PhantomTrigger
