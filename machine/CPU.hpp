#pragma once
#include "Bus.hpp"
#include "Common.hpp"
#include "RegFile.hpp"

class CPU
{
public:
    CPU(Bus& bus_ref);
    ~CPU();

    void step();
    void reset();
    void dumpRegisters();

private:
    Bus& bus;
    RegFile regs;
    Addr pc;

    Word fetch();
    void execute(Word inst);
};
