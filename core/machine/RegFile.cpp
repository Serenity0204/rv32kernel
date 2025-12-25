#include "RegFile.hpp"

RegFile::RegFile()
{
    this->reset();
}

void RegFile::reset()
{
    this->regs.fill(0);
}

Reg RegFile::read(std::size_t index) const
{
    assert(index < 32);
    return this->regs[index];
}

void RegFile::write(std::size_t index, Reg value)
{
    assert(index < 32);

    // RISC-V: x0 is hard-wired to zero
    if (index == 0) return;
    this->regs[index] = value;
}

Reg RegFile::operator[](std::size_t index) const
{
    return this->read(index);
}
