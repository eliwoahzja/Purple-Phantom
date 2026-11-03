#include "Trigger/TriggerCompatibility.h"
#include "Trigger/TriggerRuntime.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    using namespace PhantomTrigger;
    const std::array<unsigned char, 16> source{{1, 2, 3, 4}};
    std::array<unsigned char, 16> copy{};
    assert(CompatibilityDetail::ReadSelf(reinterpret_cast<std::uintptr_t>(source.data()),
                                         copy.data(), copy.size()));
    assert(copy == source);
    assert(!CompatibilityDetail::ReadSelf(0, copy.data(), copy.size()));
    assert(!CompatibilityDetail::ReadSelf(1, copy.data(), copy.size()));
    assert(!CompatibilityDetail::ReadSelf(1, copy.data(), 257));
    assert(!CompatibilityDetail::ReadSelf(1, nullptr, 1));
    const auto pageSize = static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
    void* page = mmap(nullptr, pageSize, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    assert(page != MAP_FAILED);
    const auto address = reinterpret_cast<std::uintptr_t>(page);
    assert(munmap(page, pageSize) == 0);
    assert(!CompatibilityDetail::ReadSelf(address, copy.data(), copy.size()));

    auto report = CaptureLoadedCompatibility();
    assert(!report.moduleFound);
    char text[1400];
    FormatCompatibility(report, text, sizeof(text));
    assert(std::strstr(text, "not found") && std::strstr(text, "does not enable"));
    report.arm64 = report.moduleFound = true;
    for (auto& sample : report.samples) {
        sample.name = "Synthetic test sample";
        sample.rva = 0x1234;
        sample.bytes = source;
        sample.readable = true;
        sample.hasExpectedPrefix = true;
    }
    FormatCompatibility(report, text, sizeof(text));
    assert(std::strstr(text, "01020304") && std::strstr(text, "prefix differs"));
    char tiny[5] = {'x','x','x','x','!'};
    FormatCompatibility(report, tiny, 4);
    assert(tiny[3] == '\0' && tiny[4] == '!');
    FormatCompatibility(report, nullptr, 0);
    SetSettings({true, Mode::Both, true});
    assert(GetRuntimeStatus() == RuntimeStatus::WaitingForBinaryVerification);
    SetSettings({});
    std::puts("trigger compatibility tests passed");
}
