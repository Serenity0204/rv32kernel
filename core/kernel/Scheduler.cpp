#include "Scheduler.hpp"
#include "Logger.hpp"

Scheduler::Scheduler(SystemContext* context) : systemCtx(context) {}

void Scheduler::yield()
{
    if (this->systemCtx->activeThreads.empty()) return;

    // Round Robin: Find the next READY thread
    int nextIndex = this->systemCtx->currentThreadIndex;
    std::size_t attempts = 0;
    bool found = false;
    std::size_t count = this->systemCtx->activeThreads.size();

    do
    {
        nextIndex = (nextIndex + 1) % count;
        attempts++;
        if (this->systemCtx->activeThreads[nextIndex]->getState() == ThreadState::READY)
        {
            found = true;
            break;
        }
    } while (attempts <= count);

    if (!found)
    {
        bool canRunCurrentProcess = this->checkCurrentThreadRunnable();
        if (canRunCurrentProcess)
        {
            this->systemCtx->timer.reset();
            return;
        }

        // Check if everyone is terminated
        bool allDead = this->checkAllTerminated();
        if (allDead)
        {
            LOG(SCHEDULER, INFO, "All threads terminated.");
            this->systemCtx->cpu.halt();
        }
        return;
    }

    // reset the timer
    this->systemCtx->timer.reset();

    // Perform Switch
    if (nextIndex != this->systemCtx->currentThreadIndex)
    {
        Thread* nextThread = this->systemCtx->activeThreads[nextIndex];
        Process* proc = nextThread->getProcess();
        LOG(SCHEDULER, INFO, "Switching to Thread " + std::to_string(nextThread->getTid()) + " (PID " + std::to_string(proc->getPid()) + ")");
        this->contextSwitch(nextIndex);
    }
}

void Scheduler::contextSwitch(std::size_t nextIndex)
{
    STATS.incContextSwitches();
    this->systemCtx->timer.tick(CONTEXT_SWITCH_TIME);

    Thread* nextThread = this->systemCtx->activeThreads[nextIndex];
    Thread* currentThread = this->systemCtx->getCurrentThread();

    // Save Current State (if valid)
    if (currentThread != nullptr && currentThread->getState() != ThreadState::TERMINATED)
    {
        currentThread->getRegs() = this->systemCtx->cpu.getRegs();
        currentThread->setPC(this->systemCtx->cpu.getPC());

        if (currentThread->getState() == ThreadState::RUNNING) currentThread->setState(ThreadState::READY);
    }

    this->systemCtx->cpu.getRegs() = nextThread->getRegs();
    this->systemCtx->cpu.setPC(nextThread->getPC());
    nextThread->setState(ThreadState::RUNNING);

    // check if it's switch within the same process
    if (currentThread == nullptr || currentThread->getProcess()->getPid() != nextThread->getProcess()->getPid())
        this->systemCtx->cpu.setPageTable(nextThread->getProcess()->getPageTable());

    this->systemCtx->currentThreadIndex = nextIndex;
}

bool Scheduler::checkCurrentThreadRunnable()
{
    Thread* current = this->systemCtx->getCurrentThread();
    if (current == nullptr) return false;
    if (current->getState() == ThreadState::RUNNING) return true;
    return false;
}

bool Scheduler::checkAllTerminated()
{
    for (auto* t : this->systemCtx->activeThreads)
    {
        if (t->getState() != ThreadState::TERMINATED) return false;
    }
    return true;
}
