#pragma once
#include "Common.hpp"
#include "PTE.hpp"
#include "RegFile.hpp"
#include "Thread.hpp"
#include <string>
#include <vector>

// for ELF
struct Segment
{
    Addr vaddr;
    size_t memSize;
    size_t fileSize;
    size_t fileOffset;
    uint32_t flags;
};

class Process
{

private:
    struct PCB
    {
        // info
        int pid;
        std::string name;

        // shared info
        PageTable* pageTable;
        std::vector<Segment> segments;
        std::vector<Thread*> threads;
        Addr nextStackBase;

        PCB(int id, std::string n) : pid(id), name(n)
        {
            this->pageTable = new PageTable();
            this->nextStackBase = STACK_TOP;
        }
        ~PCB()
        {
            if (this->pageTable != nullptr)
            {
                delete this->pageTable;
                this->pageTable = nullptr;
            }
            for (Thread* thread : this->threads) delete thread;
            this->threads.clear();
        }
    };

public:
    Process(int id, std::string name);
    ~Process();

    Thread* createThread(Addr entryPC, Word arg);

public:
    // getters
    inline int getPid() const { return this->pcb->pid; }
    inline std::string getName() const { return this->pcb->name; }
    inline PageTable* getPageTable() { return this->pcb->pageTable; }
    inline std::vector<Segment>& getSegments() { return this->pcb->segments; }
    inline std::vector<Thread*>& getThreads() { return this->pcb->threads; }

private:
    PCB* pcb;
};