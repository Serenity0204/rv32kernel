#pragma once
#include "CPU.hpp"
#include "PhysicalMemoryManager.hpp"
#include "Process.hpp"
#include "Stats.hpp"
#include "Timer.hpp"
#include <vector>

struct KernelContext
{
public:
    CPU cpu;
    PhysicalMemoryManager pmm;
    std::vector<Process*> processList;

    std::vector<Thread*> activeThreads;
    int currentThreadIndex = -1;

    Timer timer;

    KernelContext() : timer(TIME_QUANTUM)
    {
        this->pmm.init();
    }

    ~KernelContext()
    {
        for (auto* p : this->processList) delete p;
        this->processList.clear();
        this->activeThreads.clear();
    }

    inline Thread* getCurrentThread()
    {
        if (currentThreadIndex == -1) return nullptr;
        if (static_cast<std::size_t>(currentThreadIndex) >= activeThreads.size()) return nullptr;

        return this->activeThreads[this->currentThreadIndex];
    }
};