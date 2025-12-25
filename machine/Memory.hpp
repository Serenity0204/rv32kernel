#pragma once
#include "Common.hpp"
#include <vector>

class Memory
{
public:
    Memory();
    ~Memory();
    Word load(Addr addr, Word size);
    void store(Addr addr, Word size, Word value);

private:
    std::vector<Byte> memory;
};