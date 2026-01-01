#pragma once
#include "Common.hpp"
#include <vector>

class Memory
{
public:
    Memory();
    ~Memory() = default;
    Word load(Addr addr, Word size);
    void store(Addr addr, Word size, Word value);

    friend bool loadBinary(const std::string& filename, Memory& memory);

private:
    std::vector<Byte> memory;
};