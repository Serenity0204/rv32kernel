#pragma once
#include "KernelContext.hpp"

class Scheduler
{
public:
    Scheduler(SystemContext* context);
    void yield();

private:
    void contextSwitch(std::size_t nextIndex);
    bool checkAllTerminated();
    bool checkCurrentThreadRunnable();

private:
    SystemContext* systemCtx;
};