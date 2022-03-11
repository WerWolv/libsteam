#pragma once

#include <steam.hpp>

#include <array>

namespace steam {

    template<u32 Polynomial = 0x04C11DB7>
    [[nodiscard]] u32 crc32(const auto &data, u32 initialValue = 0x00) {
        // Lookup table generation
        constexpr static auto Table = [] {
            std::array<u32, 256> table = {0};

            for (u32 i = 0; i < 256; i++) {
                u32 c = i;
                for (size_t j = 0; j < 8; j++) {
                    if (c & 1)
                        c = Polynomial ^ (c >> 1);
                    else
                        c >>= 1;
                }
                table[i] = c;
            }

            return table;
        }();

        // CRC32 calculation
        u32 crc = initialValue;
        for (u8 byte : data) {
            crc = Table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
        }

        return ~crc;
    }


}