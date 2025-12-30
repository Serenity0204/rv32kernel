#pragma once
#include "Bus.hpp"
#include "Common.hpp"
#include "Decoder.hpp"
#include "Executor.hpp"
#include "RegFile.hpp"
#include "Utils.hpp"

class CPU
{
public:
    friend class Executor;
    CPU(Bus& bus_ref);
    ~CPU() = default;

    void step();
    void reset();
    void dumpRegisters();
    inline Addr getPC() { return this->pc; }
    inline Word readReg(size_t index) { return this->regs[index]; }
    inline bool isHalted() { return this->halted; }

private:
    bool halted;
    Bus& bus;
    RegFile regs;
    Addr pc;

    Word fetch();
    void execute(Word instr);
};
