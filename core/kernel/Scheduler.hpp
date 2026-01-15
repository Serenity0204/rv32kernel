#pragma once
#include "KernelContext.hpp"
#include "Logger.hpp"

class Scheduler
{
public:
    Scheduler(KernelContext* context) : ctx(context) {}
    void schedule();

private:
    void contextSwitch(std::size_t nextIndex);
    void checkAllTerminated();

private:
    KernelContext* ctx;
};