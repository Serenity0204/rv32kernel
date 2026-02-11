#pragma once
#include "KernelContext.hpp"
#include "Loader.hpp"
#include "Process.hpp"
#include "Scheduler.hpp"
#include "SyscallHandler.hpp"
#include "VirtualMemoryManager.hpp"

class Kernel
{
public:
    Kernel();
    ~Kernel() = default;
    void init();
    void step();
    bool isRunning();

    bool createProcess(const std::string& filename);
    bool killProcess(int pid);

private:
    KernelContext ctx;

private:
    Scheduler scheduler;
    VirtualMemoryManager vmm;
    SyscallHandler syscalls;
    Loader loader;
};
