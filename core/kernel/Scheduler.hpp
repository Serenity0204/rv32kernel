#pragma once
#include "KernelContext.hpp"

class Scheduler
{
public:
    Scheduler(KernelContext* context);
    void yield();
    bool admitProcesses();

private:
    void contextSwitch(std::size_t nextIndex);
    bool checkAllTerminated();
    bool checkCurrentProcessRunnable();

private:
    KernelContext* ctx;
};