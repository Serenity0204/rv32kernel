#pragma once
#include "Common.hpp"
#include "Memory.hpp"

class Bus
{
public:
    Bus(Memory& memory_ref);
    ~Bus() = default;
    Word load(Addr addr, Word size);
    void store(Addr addr, Word size, Word value);

private:
    Memory& memory;
};
