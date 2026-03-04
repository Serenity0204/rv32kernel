#pragma once
#include "CPU.hpp"
#include "DiskInterface.hpp"
#include "InMemoryDisk.hpp"
#include "PageReplacementPolicy.hpp"
#include "PhysicalMemoryManager.hpp"
#include "Process.hpp"
#include "Stats.hpp"
#include "StubFileSystem.hpp"
#include "SwapManager.hpp"
#include "Timer.hpp"
#include "VFSInterface.hpp"
#include <vector>

struct KernelContext
{
public:
    CPU cpu;
    PhysicalMemoryManager pmm;
    std::vector<Process*> processList;

    std::vector<Thread*> activeThreads;
    int currentThreadIndex = -1;

    Timer timer;

    // disk and fs
    VFSInterface* vfs;
    DiskInterface* disk;
    PageReplacementPolicy* pageReplacementPolicy;
    SwapManager* swap;

    KernelContext() : timer(TIME_QUANTUM)
    {
        this->initSubsystem<StubFileSystem, InMemoryDisk, FIFOPolicy>(NUM_DISK_BLOCKS);
        this->swap = new SwapManager(this->disk, NUM_DISK_BLOCKS, NUM_SWAP_BLOCKS);
    }

    ~KernelContext()
    {
        for (auto* p : this->processList) delete p;
        this->processList.clear();
        this->activeThreads.clear();

        delete this->vfs;
        delete this->disk;
        delete this->pageReplacementPolicy;
        delete this->swap;
    }

    template <typename VFSImpl, typename DiskImpl, typename PageReplacementPolicyImpl>
    inline void initSubsystem(std::size_t numBlocks)
    {
        this->pmm.init();
        static_assert(std::is_base_of<VFSInterface, VFSImpl>::value, "VFSImpl must derive from VFSInterface");
        static_assert(std::is_base_of<DiskInterface, DiskImpl>::value, "DiskImpl must derive from DiskInterface");
        static_assert(std::is_base_of<PageReplacementPolicy, PageReplacementPolicyImpl>::value, "PageReplacementPolicyImpl must derive from PageReplacementPolicy");

        this->disk = new DiskImpl(numBlocks);
        this->vfs = new VFSImpl(this->disk);
        this->pageReplacementPolicy = new PageReplacementPolicyImpl();
        this->pageReplacementPolicy->init(this->pmm.getTotalFrames());
    }

    inline Thread* getCurrentThread()
    {
        if (currentThreadIndex == -1) return nullptr;
        if (static_cast<std::size_t>(currentThreadIndex) >= activeThreads.size()) return nullptr;

        return this->activeThreads[this->currentThreadIndex];
    }
};
