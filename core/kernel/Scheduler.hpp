#pragma once
#include "KernelContext.hpp"

class Scheduler
{
public:
    Scheduler(KernelContext* context);
    void yield();

private:
    void contextSwitch(std::size_t nextIndex);
    bool checkAllTerminated();
    bool checkCurrentThreadRunnable();

private:
    KernelContext* ctx;
};