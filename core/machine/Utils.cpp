#include "Utils.hpp"

int32_t Utils::signExtend(uint32_t val, int bits)
{
    uint32_t m = uint32_t(1) << (bits - 1);
    return int32_t((val ^ m) - m);
}

uint32_t Utils::extractBits(uint32_t value, int start, int end)
{
    int len = end - start + 1;
    uint32_t mask = (1ULL << len) - 1;
    return (value >> start) & mask;
}