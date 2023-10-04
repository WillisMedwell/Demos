#pragma once

#include <cstdint>
#include <limits>

namespace Tools {

double randomInRange(const double min, const double max)
{
    static uint64_t s[2] = { 0x1234567890ABCDEF, 0x234567890ABCDEF1 };
    uint64_t s1 = s[0];
    const uint64_t s0 = s[1];
    s[0] = s0;
    s1 ^= s1 << 23;
    s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);

    double scaled = static_cast<double>(s[1]) / static_cast<double>(std::numeric_limits<uint64_t>::max());
    return min + scaled * (max - min);
}
}