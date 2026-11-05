#include "NativeProfile.h"
#include "NativeDriver.h"
#include "TriggerCompatibility.h"
#include <array>
#include <atomic>
#include <cstring>
#include <limits>
#include <link.h>

namespace PhantomTrigger {
namespace {
template<std::size_t Count>
bool MatchesNote(const unsigned char* data, std::size_t size,
                 const unsigned char (&expected)[Count]) noexcept {
    if (!data || size < 12) return false;
    std::uint32_t nameSize = 0, valueSize = 0, type = 0;
    std::memcpy(&nameSize, data, 4);
    std::memcpy(&valueSize, data + 4, 4);
    std::memcpy(&type, data + 8, 4);
    return nameSize == 4 && valueSize == sizeof(expected) && type == 3 &&
        size >= 16 + sizeof(expected) && std::memcmp(data + 12, "GNU\0", 4) == 0 &&
        std::memcmp(data + 16, expected, sizeof(expected)) == 0;
}
}

bool NativeProfileDetail::MatchesBuildIdNote(const unsigned char* data,
                                            std::size_t size) noexcept {
    constexpr unsigned char expected[]{0x13,0x05,0xbd,0xee,0xa3,0x0b,0x98,0x9f};
    return MatchesNote(data,size,expected);
}

bool NativeProfileDetail::MatchesModuleBuildIdNote(const unsigned char* data,
                                                  std::size_t size) noexcept {
    // Independent of the trigger identity above: all legacy offsets in Main
    // and its headers still belong to this 1.6.57 binary.
    constexpr unsigned char moduleBuild[]{0x13,0x05,0xbd,0xee,0xa3,0x0b,0x98,0x9f};
    return MatchesNote(data,size,moduleBuild);
}

#if defined(__ANDROID__) && defined(__aarch64__) && !defined(STUDIO_STANDALONE_PREVIEW)
namespace {
using NativeProfileDetail::CodeCheck;
#include "NativeProfileChecks.inc"

bool Range(const dl_phdr_info& info, std::uintptr_t rva,
           std::size_t size, unsigned permissions) noexcept {
    for (std::size_t i = 0; i < info.dlpi_phnum; ++i) {
        const auto& segment = info.dlpi_phdr[i];
        if (segment.p_type == PT_LOAD && (segment.p_flags & permissions) == permissions &&
            rva >= segment.p_vaddr && rva - segment.p_vaddr <= segment.p_memsz &&
            size <= segment.p_memsz - (rva - segment.p_vaddr)) return true;
    }
    return false;
}
struct Validation {
    std::uintptr_t base;
    const CodeCheck* checks;
    std::size_t count;
    bool (*identityMatcher)(const unsigned char*,std::size_t) noexcept =
        NativeProfileDetail::MatchesBuildIdNote;
    bool valid = false;
    const char* failure = "The loaded game library could not be identified";
};
int Validate(dl_phdr_info* info, std::size_t, void* argument) noexcept {
    auto& state = *static_cast<Validation*>(argument);
    if (!info || !info->dlpi_name || info->dlpi_addr != state.base) return 0;
    const auto slash = std::strrchr(info->dlpi_name, '/');
    if (std::strcmp(slash ? slash + 1 : info->dlpi_name, "libunity.so") != 0) return 0;
    bool identity = false;
    for (std::size_t i = 0; i < info->dlpi_phnum; ++i) {
        const auto& note = info->dlpi_phdr[i];
        if (note.p_type != PT_NOTE || note.p_filesz < 24 || note.p_filesz > 256 ||
            note.p_vaddr > std::numeric_limits<std::uintptr_t>::max() - state.base ||
            !Range(*info, note.p_vaddr, note.p_filesz, PF_R)) continue;
        std::array<unsigned char, 256> bytes{};
        if (CompatibilityDetail::ReadSelf(state.base + note.p_vaddr, bytes.data(), note.p_filesz) &&
            state.identityMatcher(bytes.data(), note.p_filesz)) identity = true;
    }
    if (!identity) {
        state.failure = "Unsupported game library build ID";
        return 1;
    }
    for (std::size_t i = 0; i < state.count; ++i) {
        const auto& check = state.checks[i];
        std::array<unsigned char, 16> actual{};
        if (check.rva > std::numeric_limits<std::uintptr_t>::max() - state.base ||
            !Range(*info, check.rva, actual.size(), PF_R | PF_X) ||
            !CompatibilityDetail::ReadSelf(state.base + check.rva, actual.data(), actual.size()) ||
            actual != check.expected) {
            state.failure = check.failure;
            return 1;
        }
    }
    state.valid = true;
    return 1;
}
}
#endif

bool NativeProfileDetail::ValidateEntrypoints(std::uintptr_t base,
        const CodeCheck* requested, std::size_t count, const char*& failure) noexcept {
    failure = "Native profile verification unavailable";
#if defined(__ANDROID__) && defined(__aarch64__) && !defined(STUDIO_STANDALONE_PREVIEW)
    if (!base || !requested || count == 0) return false;
    Validation validation{base,requested,count};
    dl_iterate_phdr(Validate, &validation);
    failure = validation.valid ? nullptr : validation.failure;
    return validation.valid;
#else
    (void)base; (void)requested; (void)count;
    return false;
#endif
}

bool ValidateSupportedGameProfile(std::uintptr_t base, const char*& failure) noexcept {
#if defined(__ANDROID__) && defined(__aarch64__) && !defined(STUDIO_STANDALONE_PREVIEW)
    failure = "Native game profile verification unavailable";
    if (!base) return false;
    Validation validation{base,checks,sizeof(checks)/sizeof(checks[0])};
    validation.identityMatcher = NativeProfileDetail::MatchesModuleBuildIdNote;
    dl_iterate_phdr(Validate,&validation);
    failure = validation.valid ? nullptr : validation.failure;
    return validation.valid;
#else
    (void)base;
    failure = "Native game profile verification unavailable";
    return false;
#endif
}

void InitializeNativeTrigger(std::uintptr_t base) noexcept {
#if defined(__ANDROID__) && defined(__aarch64__) && !defined(STUDIO_STANDALONE_PREVIEW)
    static std::atomic<bool> attempted{false};
    if (!base || attempted.exchange(true, std::memory_order_acq_rel)) return;
    const char* failure = nullptr;
    if (!NativeProfileDetail::ValidateEntrypoints(base,checks,
            sizeof(checks)/sizeof(checks[0]),failure)) {
        NativeDriverFailure(failure);
        return;
    }
    InstallNativeDriver(base);
#else
    (void)base;
#endif
}
}
