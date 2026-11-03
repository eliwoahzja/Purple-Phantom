#pragma once
#include <cstddef>
#include <cstdint>

namespace PhantomTrigger {
// Startup-only: checks module identity and all native entrypoint fingerprints
// before installing input hooks. No UI flag bypasses this check.
void InitializeNativeTrigger(std::uintptr_t base) noexcept;
namespace NativeProfileDetail {
bool MatchesBuildIdNote(const unsigned char* data, std::size_t size) noexcept;
}
}
