#include "CPU.hpp"

CPU::CPU(Bus& bus_ref) : bus(bus_ref)
{
    this->reset();
}

CPU::~CPU()
{
}

void CPU::reset()
{
    this->pc = MEMORY_BASE;
    this->regs.reset();
}

Word CPU::fetch()
{
    return this->bus.load(this->pc, 4);
}

void CPU::execute(Word inst)
{
    std::cout << "Exec: 0x" << std::hex << inst << std::endl;
}

void CPU::step()
{
    Word inst = this->fetch();
    this->pc += 4;
    this->execute(inst);
}

void CPU::dumpRegisters()
{
    std::cout << "PC: 0x" << std::hex << this->pc << "\n";
    for (int i = 0; i < 32; ++i)
    {
        std::cout << "x" << std::dec << i << ": " << std::hex << this->regs[i] << "\n";
    }
}