#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace PhantomTrigger {
// Startup-only: checks module identity and all native entrypoint fingerprints
// before installing input hooks. No UI flag bypasses this check.
void InitializeNativeTrigger(std::uintptr_t base) noexcept;
namespace NativeProfileDetail {
struct CodeCheck {
    const char* failure;
    std::uintptr_t rva;
    std::array<unsigned char, 16> expected;
};
bool MatchesBuildIdNote(const unsigned char* data, std::size_t size) noexcept;
// Each optional native feature validates its own dependencies before hooking.
// Validation failure never disables unrelated, already-verified features.
bool ValidateEntrypoints(std::uintptr_t base, const CodeCheck* checks,
                         std::size_t count, const char*& failure) noexcept;
}
}
