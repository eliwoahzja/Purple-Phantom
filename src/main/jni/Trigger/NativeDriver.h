#pragma once

#include "TriggerRuntime.h"
#include <cstdint>

namespace PhantomTrigger {
// Caller must verify the complete native profile before installation.
bool InstallNativeDriver(std::uintptr_t base) noexcept;
RuntimeStatus NativeStatus() noexcept;
const char* NativeStatusText() noexcept;
FireMode NativeDetectedMode() noexcept;
bool NativeViewportCenter(float& x, float& y) noexcept;
// Only static reason strings may be passed; no game object/string ownership.
void NativeDriverFailure(const char* staticReason) noexcept;
// Called by the existing local bullet-creation hooks. Numeric identity only;
// never dereference or retain a managed object through this notification.
void NotifyNativeShot(std::uintptr_t component) noexcept;
} // namespace PhantomTrigger
