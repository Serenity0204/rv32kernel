#include "Kernel.hpp"
#include "Exception.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "Utils.hpp"

Kernel::Kernel()
    : scheduler(&ctx), vmm(&ctx), syscalls(&ctx), loader(&ctx)
{
}

bool Kernel::createProcess(const std::string& filename)
{
    return this->loader.loadELF(filename);
}

void Kernel::run()
{
    // ADMISSION: Move NEW -> READY
    bool hasReady = !this->ctx.activeThreads.empty();

    if (!hasReady)
    {
        LOG(KERNEL, WARNING, "No READY processes.");
        return;
    }

    this->ctx.cpu.enableVM(true);

    // start the first process
    this->scheduler.yield();

    LOG(KERNEL, INFO, "Simulation started...");

    while (!this->ctx.cpu.isHalted())
    {
        try
        {
            STATS.incInstructions();
            this->ctx.cpu.step();
            this->ctx.timer.tick(USER_MODE_TICK_TIME);
            this->ctx.cpu.advancePC();
        }
        catch (SyscallException& sys)
        {
            this->ctx.timer.tick(ENTER_KERNEL_MODE_TIME);

            // will handle pc increment individually, exit will yield
            bool exited = this->syscalls.dispatch(sys.getSyscallID());
            if (exited)
            {
                this->scheduler.yield();
                continue;
            }
        }
        catch (PageFaultException& pf)
        {
            this->ctx.timer.tick(ENTER_KERNEL_MODE_TIME);
            bool pageFaultHandled = this->vmm.handlePageFault(pf.getFaultAddr());
            // If VMM returns false, it was a crash -> reschedule
            bool crashed = !pageFaultHandled;
            if (crashed)
            {
                this->scheduler.yield();
                continue;
            }
        }
        catch (std::exception& e)
        {
            LOG(KERNEL, ERROR, "Unexpected exception: " + std::string(e.what()));
            this->ctx.cpu.halt();
            break;
        }

        if (this->ctx.timer.isInterruptPending())
        {
            LOG(KERNEL, INFO, "Timer Interrupt.");
            this->scheduler.yield();
        }
    }

    STATS.printSummary();
}