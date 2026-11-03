#include "TriggerCompatibility.h"

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <limits>
#include <link.h>
#include <unistd.h>

namespace PhantomTrigger {
namespace {
struct Probe {
    const char* name;
    std::uintptr_t rva;
    std::array<unsigned char, 8> expected;
    bool compare;
};
constexpr Probe probes[] = {
    {"Burst getter", 0x50EBB28, {0x00,0x30,0x48,0x39,0xc0,0x03,0x5f,0xd6}, true},
    {"Fire-input setter", 0xA81F798, {0x01,0x4c,0x00,0x39,0xc0,0x03,0x5f,0xd6}, true},
    {"Fire-held getter", 0xA81F818, {0x00,0x70,0x40,0x39,0xc0,0x03,0x5f,0xd6}, true},
    {"Shot acknowledgement", 0x50EC09C, {}, false},
    {"Body trace", 0xA18FEB4, {}, false},
    {"Input update", 0xBB4AE14, {}, false},
    {"Smoke query", 0x9CC36E0, {}, false}
};
static_assert(sizeof(probes) / sizeof(probes[0]) == 7, "report probe count");

bool ExecutableRange(const dl_phdr_info& info, std::uintptr_t rva,
                     std::size_t size) noexcept {
    for (std::size_t i = 0; i < info.dlpi_phnum; ++i) {
        const auto& segment = info.dlpi_phdr[i];
        if (segment.p_type != PT_LOAD || (segment.p_flags & (PF_R | PF_X)) != (PF_R | PF_X))
            continue;
        if (rva >= segment.p_vaddr && rva - segment.p_vaddr <= segment.p_memsz &&
            size <= segment.p_memsz - (rva - segment.p_vaddr)) return true;
    }
    return false;
}

int Capture(dl_phdr_info* info, std::size_t, void* destination) noexcept {
    if (!info || !info->dlpi_name) return 0;
    const char* slash = std::strrchr(info->dlpi_name, '/');
    const char* name = slash ? slash + 1 : info->dlpi_name;
    if (std::strcmp(name, "libunity.so") != 0) return 0;
    auto& report = *static_cast<CompatibilityReport*>(destination);
    report.moduleFound = true;
    for (std::size_t i = 0; i < report.samples.size(); ++i) {
        auto& sample = report.samples[i];
        const auto& probe = probes[i];
        sample.name = probe.name;
        sample.rva = probe.rva;
        sample.hasExpectedPrefix = probe.compare;
        if (!ExecutableRange(*info, probe.rva, sample.bytes.size()) ||
            probe.rva > std::numeric_limits<std::uintptr_t>::max() - info->dlpi_addr)
            continue;
        sample.readable = CompatibilityDetail::ReadSelf(
            info->dlpi_addr + probe.rva, sample.bytes.data(), sample.bytes.size());
        sample.prefixMatches = sample.readable && probe.compare &&
            std::memcmp(sample.bytes.data(), probe.expected.data(), probe.expected.size()) == 0;
    }
    return 1;
}
} // namespace

bool CompatibilityDetail::ReadSelf(std::uintptr_t address, void* output,
                                   std::size_t size) noexcept {
    if (!address || !output || size == 0 || size > 256 ||
        size > std::numeric_limits<std::uintptr_t>::max() - address) return false;
    // Use ordinary file I/O only. Vendor seccomp can terminate processes using
    // process_vm_readv; do not issue that syscall even as an optional fast path.
    // /proc/self/mem reports invalid/unmapped addresses as ordinary read errors.
    if (address > static_cast<std::uintptr_t>(std::numeric_limits<off_t>::max())) return false;
    const int descriptor = open("/proc/self/mem", O_RDONLY | O_CLOEXEC);
    if (descriptor < 0) return false;
    const auto copied = pread(descriptor, output, size, static_cast<off_t>(address));
    close(descriptor);
    return copied == static_cast<ssize_t>(size);
}

CompatibilityReport CaptureLoadedCompatibility() noexcept {
    CompatibilityReport report;
#if defined(__aarch64__)
    report.arm64 = true;
    dl_iterate_phdr(Capture, &report);
#else
    // Never interpret another architecture's code as ARM64.
    (void)&Capture;
#endif
    return report;
}

void FormatCompatibility(const CompatibilityReport& report, char* output,
                         std::size_t capacity) noexcept {
    if (!output || capacity == 0) return;
    std::size_t used = 0;
    auto append = [&](const char* format, auto... args) {
        if (used >= capacity - 1) return;
        const int count = std::snprintf(output + used, capacity - used, format, args...);
        if (count < 0) return;
        const auto written = static_cast<std::size_t>(count);
        used += written < capacity - used ? written : capacity - used - 1;
    };
    output[0] = '\0';
    append("Read-only trigger check v1\nARM64: %s; game library: %s\n",
           report.arm64 ? "yes" : "no", report.moduleFound ? "loaded" : "not found");
    if (report.moduleFound) {
        for (const auto& sample : report.samples) {
            append("%s @ %llx: ", sample.name, static_cast<unsigned long long>(sample.rva));
            if (!sample.readable) { append("unreadable\n"); continue; }
            for (auto byte : sample.bytes) append("%02x", static_cast<unsigned>(byte));
            if (sample.hasExpectedPrefix)
                append(" [%s]", sample.prefixMatches ? "prefix matches" : "prefix differs");
            append("\n");
        }
    }
    append("This report does not enable automatic firing.");
}
} // namespace PhantomTrigger
