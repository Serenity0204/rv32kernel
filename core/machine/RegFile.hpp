#pragma once

#include "Common.hpp"
#include <array>
#include <assert.h>
#include <cstddef>

class RegFile
{
public:
    RegFile();
    ~RegFile() = default;
    Reg read(std::size_t index) const;
    void write(std::size_t index, Reg value);
    Reg operator[](std::size_t index) const;
    void reset();

private:
    std::array<Reg, 32> regs;
};

