#pragma once

#include "Common.hpp"
#include "RegFile.hpp"
#include <string>
#include <vector>

class Process;

enum class ThreadState
{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
};

class Thread
{
private:
    struct TCB
    {
        int tid;
        ThreadState state;
        Addr pc;
        RegFile regs;
        Process* parent;
        Addr stackTop;

        TCB(int id, Process* p, Addr entryPC, Addr stackPtr) : tid(id), state(ThreadState::NEW), pc(entryPC), parent(p), stackTop(stackPtr)
        {
            this->regs.reset();
            this->regs.write(2, stackPtr);
        }
    };

public:
    Thread(int id, Process* parent, Addr entry, Addr stack);

    ~Thread();
    inline int getTid() const { return this->tcb->tid; }
    inline ThreadState getState() const { return this->tcb->state; }
    inline void setState(ThreadState s) { this->tcb->state = s; }

    inline Addr getPC() const { return this->tcb->pc; }
    inline void setPC(Addr pc) { this->tcb->pc = pc; }
    inline Addr getStackTop() const { return this->tcb->stackTop; }

    inline RegFile& getRegs() { return this->tcb->regs; }
    inline Process* getProcess() const { return this->tcb->parent; }

private:
    TCB* tcb;
};