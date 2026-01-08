#pragma once
#include "CPU.hpp"
#include "Exception.hpp"
#include "PhysicalMemoryManager.hpp"

class Kernel
{
public:
    Kernel();
    ~Kernel();
    void run();
    void handleSyscall(SyscallID syscallID);
    void handlePageFault(Addr faultAddr);
    friend bool loadBinary(const std::string& filename, Kernel* kernel);

private:
    CPU cpu;
    PhysicalMemoryManager pmm;
    PageTable* currentTable;
};
