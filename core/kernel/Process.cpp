#include "Process.hpp"

Process::Process(int id, std::string name)
{
    this->pcb = new PCB(id, name);
}

Process::~Process()
{
    if (this->pcb != nullptr)
    {
        delete this->pcb;
        this->pcb = nullptr;
    }
}

Thread* Process::createThread(Addr entryPC, Word arg)
{
    bool isExecutable = false;

    // check the passed in address is valid
    for (const Segment& seg : this->pcb->segments)
    {
        if (entryPC >= seg.vaddr && entryPC < (seg.vaddr + seg.memSize))
        {
            if (seg.flags & PF_X)
            {
                isExecutable = true;
                break;
            }
        }
    }
    if (!isExecutable) return nullptr;

    Addr stackSize = this->pcb->threads.empty() ? MAIN_STACK_SIZE : THREAD_STACK_SIZE;
    Addr stackTop = this->pcb->nextStackBase;

    // for next thread
    this->pcb->nextStackBase = stackTop - stackSize - GUARD_PAGE_SIZE;

    int tid = static_cast<int>(this->pcb->threads.size());
    Thread* thread = new Thread(tid, this, entryPC, stackTop);

    thread->getRegs().write(10, arg);
    this->pcb->threads.push_back(thread);
    return thread;
}
