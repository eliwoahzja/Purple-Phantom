/*========================================================
                AUTHORIZATION NOTICE
========================================================

TOOL NAME  : ANTIPROC
TYPE       : OBFUSCATOR
OWNER      : TG @CKYMINT

--------------------------------------------------------

THIS SOFTWARE IS A PRIVATE AND RESTRICTED TOOL.

BY USING, ACCESSING, OR MODIFYING THIS CODE,
YOU AUTOMATICALLY AGREE TO THE FOLLOWING TERMS:

- This tool is NOT open for resale or redistribution.
- Unauthorized commercial use is STRICTLY prohibited.
- Reverse engineering or repackaging is NOT allowed.
- Sharing this tool without permission is FORBIDDEN.
- This project is fully PRIVATE and intended for
  authorized users only.

--------------------------------------------------------

WARNING:
Any violation of these terms may result in removal
of access and further action from the developer.

========================================================*/

#pragma once
#include <cstddef>
#include <cstring>
#include <string>

#ifndef ANTIPROC_DEFAULT_KEY
#define ANTIPROC_DEFAULT_KEY antiproc::generate_key(__LINE__ ^ __COUNTER__)
#endif

namespace antiproc {
    using size_type = std::size_t;
    using key_type = unsigned long long;

    constexpr key_type generate_key(key_type seed) {
        key_type key = seed;
        key ^= (key >> 33);
        key *= 0xff51afd7ed558ccdULL;
        key ^= (key >> 33);
        key *= 0xc4ceb9fe1a85ec53ULL;
        key ^= (key >> 33);
        key |= 0x0101010101010101ULL; 
        return key;
    }

    constexpr char xor_byte(char c, key_type key, size_type i) {
        return c ^ static_cast<char>(key >> ((i % 8) * 8));
    }

    constexpr void cipher(char* data, size_type size, key_type key) {
        for (size_type i = 0; i < size; ++i)
            data[i] ^= static_cast<char>(key >> ((i % 8) * 8));
    }

    template <size_type N, key_type KEY>
    class obfuscator {
    public:
        constexpr obfuscator(const char(&data)[N]) {
            for (size_type i = 0; i < N; ++i)
                m_data[i] = xor_byte(data[i], KEY, i);
        }
        constexpr const char* data() const { return m_data; }
    private:
        char m_data[N]{};
    };

    inline void secure_zero(void* ptr, size_type len) {
        volatile char* v = reinterpret_cast<volatile char*>(ptr);
        while (len--) *v++ = 0;
    }

    template <size_type N, key_type KEY>
    class obfuscated_data {
    public:
        constexpr obfuscated_data(const obfuscator<N, KEY>& obf) {
            std::memcpy(m_data, obf.data(), N);
        }

        ~obfuscated_data() {
            secure_zero(m_data, N);
        }

        const char* c_str() {
            decrypt();
            return m_data;
        }

        std::string to_string() {
            decrypt();
            return std::string(m_data, N - 1); 
        }

        void decrypt() {
            if (m_encrypted) {
                cipher(m_data, N, KEY);
                m_encrypted = false;
            }
        }

        void encrypt() {
            if (!m_encrypted) {
                cipher(m_data, N, KEY);
                m_encrypted = true;
            }
        }

        bool is_encrypted() const { return m_encrypted; }

    private:
        char m_data[N];
        bool m_encrypted{ true };
    };

    template <size_type N, key_type KEY = ANTIPROC_DEFAULT_KEY>
    constexpr auto make_obfuscator(const char(&data)[N]) {
        return obfuscator<N, KEY>(data);
    }
}

#define ANTIPROC_OBFUSCATE(data) ANTIPROC_OBFUSCATE_KEY(data, ANTIPROC_DEFAULT_KEY)

#define ANTIPROC_OBFUSCATE_KEY(data, key) \
    []() -> antiproc::obfuscated_data<sizeof(data), key>& { \
        constexpr auto obf = antiproc::make_obfuscator<sizeof(data), key>(data); \
        static antiproc::obfuscated_data<sizeof(data), key> instance(obf); \
        return instance; \
    }()

// These will now route directly through the new isolated obfuscator
#define DEOBF(data)        (ANTIPROC_OBFUSCATE(data).c_str())
#define DEOBFSTR(data)     (ANTIPROC_OBFUSCATE(data).to_string())
