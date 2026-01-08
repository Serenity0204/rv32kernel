#pragma once
#include "Bus.hpp"
#include "Common.hpp"

class MMU
{
public:
    MMU(Bus& bus_ref);
    ~MMU() = default;

    // virtual memory
    Word loadVirtualMemory(Addr vaddr, std::size_t size);
    void storeVirtualMemory(Addr vaddr, std::size_t size, Word value);

    // physical memory direct access
    Word loadPhysicalMemory(Addr paddr, std::size_t size);
    void storePhysicalMemory(Addr paddr, std::size_t size, Word value);

    // fetch instruction
    Word fetch(Addr vaddr);

    // Control
    void setPageTable(PageTable* table);
    void enableVM(bool enabled);
    void reset();

private:
    Bus& bus;
    PageTable* currentTable;
    bool vmEnabled;

    Addr translate(Addr vaddr);
};