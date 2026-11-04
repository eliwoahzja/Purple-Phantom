#pragma once

#include "MovementRuntime.h"
#include <cmath>

namespace PhantomMovement {
// Invocation-local values; no game object is retained across callbacks.
struct LocalContext {
    std::uintptr_t match = 0;
    std::uintptr_t pawn = 0;
    std::uintptr_t localPawn = 0;
    bool alive = false;
};

inline bool IsLocalPawn(const LocalContext& context) noexcept {
    return context.match && context.pawn && context.pawn == context.localPawn && context.alive;
}

inline bool IsLocalController(const LocalContext& context, std::uintptr_t controller,
                              std::uintptr_t localController) noexcept {
    return controller && controller == localController && IsLocalPawn(context);
}

struct Stance {
    bool crouched = false;
    bool prone = false;
    int physicsState = 0;
    bool transitioning = false;
};

inline bool ShouldStand(const Stance& stance) noexcept {
    // PhysWalking=1. Slides, prone, vehicles, and airborne states keep their
    // native stance. StandUp(false,false) still decides whether headroom exists.
    return stance.crouched && !stance.prone && stance.physicsState == 1 && !stance.transitioning;
}

inline float AdjustSprintDelay(const Settings& settings, bool compatible,
                               const LocalContext& context, bool ordinaryGun,
                               float nativeDelay) noexcept {
    return settings.noSprintToFireDelay && compatible && IsLocalPawn(context) &&
           ordinaryGun && std::isfinite(nativeDelay) && nativeDelay > 0.f ? 0.f : nativeDelay;
}
} // namespace PhantomMovement
