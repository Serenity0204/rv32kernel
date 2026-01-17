#pragma once
#include "CPU.hpp"
#include "PhysicalMemoryManager.hpp"
#include "Process.hpp"
#include "Stats.hpp"
#include "Timer.hpp"
#include <vector>

using ProcessState = Process::ProcessState;

struct KernelContext
{
public:
    CPU cpu;
    PhysicalMemoryManager pmm;
    std::vector<Process*> processList;
    int currentProcessIndex = -1;
    Timer timer;

    KernelContext() : timer(TIME_QUANTUM)
    {
        this->pmm.init();
    }

    ~KernelContext()
    {
        for (auto* p : this->processList) delete p;
        this->processList.clear();
    }
};