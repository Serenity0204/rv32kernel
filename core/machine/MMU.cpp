#include "MMU.hpp"
#include "Exception.hpp"

MMU::MMU(Bus& bus_ref) : bus(bus_ref)
{
    this->reset();
}

void MMU::reset()
{
    this->vmEnabled = false;
    this->currentTable = nullptr;
}

void MMU::setPageTable(PageTable* table)
{
    this->currentTable = table;
}

void MMU::enableVM(bool enabled)
{
    this->vmEnabled = enabled;
}

Word MMU::loadVirtualMemory(Addr vaddr, std::size_t size)
{
    Addr paddr = this->translate(vaddr);
    return this->bus.load(paddr, size);
}

void MMU::storeVirtualMemory(Addr vaddr, std::size_t size, Word value)
{
    Addr paddr = this->translate(vaddr);
    this->bus.store(paddr, size, value);
}

Word MMU::loadPhysicalMemory(Addr paddr, std::size_t size)
{
    return this->bus.load(paddr, size);
}

void MMU::storePhysicalMemory(Addr paddr, std::size_t size, Word value)
{
    this->bus.store(paddr, size, value);
}

Word MMU::fetch(Addr vaddr)
{
    // Fetch is always 4 bytes
    Addr paddr = this->translate(vaddr);
    return this->bus.load(paddr, 4);
}

Addr MMU::translate(Addr vaddr)
{
    if (!this->vmEnabled) return vaddr;

    uint32_t vpn = (vaddr >> 12);
    uint32_t offset = (vaddr & 0xFFF);

    if (this->currentTable == nullptr || this->currentTable->count(vpn) == 0) throw PageFaultException(vaddr);

    Addr pte = this->currentTable->at(vpn);
    Addr ppn = (pte & 0xFFFFF000);
    return ppn | offset;
}