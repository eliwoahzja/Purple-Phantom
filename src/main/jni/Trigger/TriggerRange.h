#pragma once
#include <cmath>

namespace PhantomTrigger {
// The center viewport ray starts on the near clip plane. Detection covers the
// visible scene, independently of a weapon's default firing/trace distance.
inline float VisibleTraceDistance(float nearPlane, float farPlane) noexcept {
    if (!std::isfinite(nearPlane) || !std::isfinite(farPlane) ||
        nearPlane < 0.f || farPlane <= nearPlane) return 0.f;
    const float distance = farPlane - nearPlane;
    return std::isfinite(distance) && distance > 0.f ? distance : 0.f;
}
}
