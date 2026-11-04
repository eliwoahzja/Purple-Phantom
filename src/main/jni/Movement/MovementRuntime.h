#pragma once

#include <cstdint>

namespace PhantomMovement {

struct Settings {
    bool noCrouch = false;
    bool noSprintToFireDelay = false;
};

// Session-only settings are exchanged as one atomic snapshot by the runtime.
Settings GetSettings() noexcept;
void SetSettings(const Settings& settings) noexcept;
const char* GetNoCrouchStatusText() noexcept;
const char* GetSprintToFireStatusText() noexcept;
void InitializeNativeMovement(std::uintptr_t base) noexcept;

} // namespace PhantomMovement
