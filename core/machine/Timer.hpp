#pragma once
#include "Exception.hpp"
#include <cstddef>

class Timer
{
public:
    Timer(std::size_t quantum);
    ~Timer() = default;
    void tick(std::size_t cycles = 1);
    void reset();
    bool isInterruptPending();

private:
    std::size_t timeQuantum;
    std::size_t currentTicks;
    bool interruptPending;
};