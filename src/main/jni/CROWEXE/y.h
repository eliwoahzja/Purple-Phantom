#pragma once
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <fstream>
#include <sys/mman.h>
#include <unistd.h>

bool ANTIHOOK(const std::string& targetName) {
    std::ifstream maps("/proc/self/maps");
    if (!maps.is_open())
        return false;

    std::string line;
    size_t successCount = 0;

    while (std::getline(maps, line)) {
        if (line.find(targetName) == std::string::npos)
            continue;

        uintptr_t start = 0, end = 0, offset = 0;
        char perms[5] = {0};
        char dev[32] = {0};
        ino_t inode = 0;
        char path[256] = {0};

        int parsed = sscanf(
            line.c_str(),
            "%lx-%lx %4s %lx %31s %lu %255s",
            &start, &end, perms, &offset, dev, &inode, path
        );

        if (parsed < 6 || start == 0 || end <= start)
            continue;

        size_t size = end - start;
        void* address = reinterpret_cast<void*>(start);

        int prot = 0;
        if (strchr(perms, 'r')) prot |= PROT_READ;
        if (strchr(perms, 'w')) prot |= PROT_WRITE;
        if (strchr(perms, 'x')) prot |= PROT_EXEC;

        void* temp = mmap(
            nullptr,
            size,
            PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
        );

        if (temp == MAP_FAILED)
            continue;

        if (!(prot & PROT_READ)) {
            if (mprotect(address, size, prot | PROT_READ) != 0) {
                munmap(temp, size);
                continue;
            }
        }

        memcpy(temp, address, size);

        void* remapped = mremap(
            temp,
            size,
            size,
            MREMAP_MAYMOVE | MREMAP_FIXED,
            address
        );

        if (remapped == MAP_FAILED) {
            munmap(temp, size);
            continue;
        }

        mprotect(address, size, prot);
        successCount++;
    }

    return successCount > 0;
}

