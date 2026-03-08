#include "Kernel.hpp"
#include "Exception.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "Utils.hpp"

Kernel::Kernel()
    : scheduler(nullptr),
      vmm(nullptr),
      syscalls(nullptr),
      loader(nullptr)
{
    this->storageCtx.init<StubFileSystem, InMemoryDisk, FIFOPolicy>(NUM_DISK_BLOCKS, NUM_SWAP_BLOCKS, this->systemCtx.pmm.getTotalFrames());

    this->scheduler = new Scheduler(&this->systemCtx);
    this->syscalls = new SyscallHandler(&this->systemCtx, &this->storageCtx);
    this->vmm = new VirtualMemoryManager(&this->systemCtx, &this->storageCtx);
    this->loader = new Loader(&this->systemCtx);
}

Kernel::~Kernel()
{
    delete this->scheduler;
    delete this->vmm;
    delete this->syscalls;
    delete this->loader;
}

bool Kernel::createProcess(const std::string& filename)
{
    return this->loader->loadELF(filename);
}

bool Kernel::killProcess(int pid)
{
    if (pid < 0 || static_cast<size_t>(pid) >= this->systemCtx.processList.size())
    {
        LOG(KERNEL, ERROR, "Killing process with PID: " + std::to_string(pid) + " that does not exist.");
        return false;
    }
    Process* process = this->systemCtx.processList[pid];
    std::vector<Thread*>& allThreads = process->getThreads();
    for (Thread* thread : allThreads)
        thread->setState(ThreadState::TERMINATED);

    LOG(KERNEL, INFO, "Killing Process " + std::to_string(pid) + " (CRASHED)");
    return true;
}

void Kernel::init()
{
    bool hasReady = !this->systemCtx.activeThreads.empty();

    if (!hasReady)
    {
        LOG(KERNEL, WARNING, "No READY processes.");
        return;
    }

    this->systemCtx.cpu.enableVM(true);
    this->scheduler->yield();

    LOG(KERNEL, INFO, "Simulation started...");
}

void Kernel::step()
{
    if (this->systemCtx.cpu.isHalted()) return;

    try
    {
        STATS.incInstructions();
        this->systemCtx.cpu.step();
        this->systemCtx.timer.tick(USER_MODE_TICK_TIME);
        this->systemCtx.cpu.advancePC();
    }
    catch (SyscallException& sys)
    {
        this->systemCtx.timer.tick(ENTER_KERNEL_MODE_TIME);

        // will handle pc increment individually, exit will yield
        SyscallStatus status = this->syscalls->dispatch(sys.getSyscallID());

        // on error, kill the process that's doing the syscall
        if (status.error)
        {
            bool killed = this->killProcess(this->systemCtx.getCurrentThread()->getProcess()->getPid());
            // if not killed, kernel panic
            if (!killed) this->systemCtx.cpu.halt();
            // after killing, yield
            this->scheduler->yield();
            return;
        }

        if (status.needReschedule)
        {
            this->scheduler->yield();
            return;
        }
    }
    catch (PageFaultException& pf)
    {
        this->systemCtx.timer.tick(ENTER_KERNEL_MODE_TIME);
        bool pageFaultHandled = this->vmm->handlePageFault(pf.getFaultAddr());
        // If VMM returns false, it was a crash -> reschedule
        bool crashed = !pageFaultHandled;
        if (crashed)
        {
            // terminate all threads related to this segfault process(current process)
            bool killed = this->killProcess(this->systemCtx.getCurrentThread()->getProcess()->getPid());

            // if not killed, kernel panic
            if (!killed) this->systemCtx.cpu.halt();

            // after killing, yield
            this->scheduler->yield();
            return;
        }
    }
    catch (std::exception& e)
    {
        LOG(KERNEL, ERROR, "Unexpected exception: " + std::string(e.what()));
        this->systemCtx.cpu.halt();
        return;
    }

    if (this->systemCtx.timer.isInterruptPending())
    {
        LOG(KERNEL, INFO, "Timer Interrupt.");
        this->scheduler->yield();
    }
}

bool Kernel::isRunning()
{
    return !this->systemCtx.cpu.isHalted();
}
