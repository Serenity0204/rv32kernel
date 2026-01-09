#pragma once
#include "CPU.hpp"
#include "Exception.hpp"
#include "PhysicalMemoryManager.hpp"
#include "Process.hpp"

class Kernel
{
public:
    Kernel();
    ~Kernel();
    void run();

    bool createProcess(const std::string& filename);
    friend bool loadBinary(const std::string& filename, Kernel* kernel);

private:
    CPU cpu;
    PhysicalMemoryManager pmm;
    std::vector<Process*> processList;
    int currentProcessIndex = -1;

private:
    void schedule();
    void contextSwitch(Process* nextProc);

private:
    void handleSyscall(SyscallID syscallID);
    void handlePageFault(Addr faultAddr);
};
