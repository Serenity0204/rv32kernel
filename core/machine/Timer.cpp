#include "Timer.hpp"

Timer::Timer(std::size_t quantum) : timeQuantum(quantum), currentTicks(0), interruptPending(false)
{
}

bool Timer::isInterruptPending()
{
    return this->interruptPending;
}

void Timer::tick(std::size_t cycles)
{
    this->currentTicks += cycles;
    if (this->currentTicks >= this->timeQuantum) this->interruptPending = true;
}

void Timer::reset()
{
    this->currentTicks = 0;
    this->interruptPending = false;
}
