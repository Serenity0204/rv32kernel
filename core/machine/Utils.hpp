#pragma once
#include "Common.hpp"
#include <cstdint>

namespace Utils
{
    int32_t signExtend(uint32_t val, int bits);
    uint32_t extractBits(uint32_t value, int start, int end);
};