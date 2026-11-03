#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace PhantomTrigger {

// Diagnostics only: these snapshots never approve a profile or call game code.
struct CodeSample {
    const char* name = "";
    std::uintptr_t rva = 0;
    std::array<unsigned char, 16> bytes{};
    bool readable = false;
    bool hasExpectedPrefix = false;
    bool prefixMatches = false;
};

struct CompatibilityReport {
    bool moduleFound = false;
    bool arm64 = false;
    std::array<CodeSample, 7> samples{};
};

CompatibilityReport CaptureLoadedCompatibility() noexcept;
void FormatCompatibility(const CompatibilityReport& report,
                         char* output, std::size_t capacity) noexcept;

namespace CompatibilityDetail {
// Kernel-mediated reads fail with an error if a mapping disappears. No direct
// dereference, signal handler, permissions change, or game function call occurs.
bool ReadSelf(std::uintptr_t address, void* output, std::size_t size) noexcept;
}
} // namespace PhantomTrigger
