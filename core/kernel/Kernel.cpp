#include "Kernel.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>
using ProcessState = Process::ProcessState;

Kernel::Kernel()
{
    this->pmm.init();
}

Kernel::~Kernel()
{
    for (std::size_t i = 0; i < this->processList.size(); ++i)
    {
        if (this->processList[i] != nullptr)
        {
            delete this->processList[i];
            this->processList[i] = nullptr;
        }
    }
    this->processList.clear();
}

void Kernel::handleSyscall(SyscallID syscallID)
{
    switch (syscallID)
    {
    case SyscallID::SYS_EXIT:
    {
        Word exitCode = this->cpu.readReg(10);
        if (this->currentProcessIndex != -1)
        {
            Process* current = this->processList[this->currentProcessIndex];
            current->setState(ProcessState::TERMINATED);
            std::cout << "[Kernel] Process " << current->getPid() << " exited with code " << exitCode << ".\n";
            this->schedule();
            return;
        }
        // no process left
        this->cpu.halt();
        return;
    }
    case SyscallID::SYS_WRITE:
    {
        Word rawFD = this->cpu.readReg(10);
        Word addr = this->cpu.readReg(11);
        Word count = this->cpu.readReg(12);

        FileDescriptor fd = static_cast<FileDescriptor>(rawFD);
        if (fd == FileDescriptor::STDOUT || fd == FileDescriptor::STDERR)
        {
            std::vector<char> hostBuffer(count);
            for (Word i = 0; i < count; ++i)
            {
                char c = this->cpu.loadVirtualMemory(addr + static_cast<Addr>(i), 1);
                hostBuffer[i] = c;
            }

            int hostFD = (fd == FileDescriptor::STDOUT) ? STDOUT_FILENO : STDERR_FILENO;
            ssize_t written = ::write(hostFD, hostBuffer.data(), count);
            this->cpu.writeReg(10, written);
            this->cpu.advancePC();
            return;
        }
        // other fd, not supported yet
        this->cpu.writeReg(10, static_cast<Word>(-1));
        this->cpu.advancePC();

        return;
    }
    case SyscallID::SYS_READ:
    {
        Word rawFD = this->cpu.readReg(10);
        Word addr = this->cpu.readReg(11);
        Word count = this->cpu.readReg(12);
        FileDescriptor fd = static_cast<FileDescriptor>(rawFD);
        if (fd == FileDescriptor::STDIN)
        {
            std::vector<char> hostBuffer(count);
            ssize_t bytesRead = ::read(STDIN_FILENO, hostBuffer.data(), count);

            if (bytesRead > 0)
            {
                for (ssize_t i = 0; i < bytesRead; ++i)
                {
                    char rawChar = hostBuffer[i];
                    uint8_t byte = static_cast<uint8_t>(rawChar);
                    this->cpu.storeVirtualMemory(addr + static_cast<Addr>(i), 1, static_cast<Word>(byte));
                }
            }
            // other fd, not supported yet
            this->cpu.writeReg(10, static_cast<Word>(bytesRead));
            this->cpu.advancePC();
            return;
        }

        this->cpu.writeReg(10, static_cast<Word>(-1));
        this->cpu.advancePC();
        return;
    }
    default:
    {
        std::cout << "Unimplemented syscall id:" << static_cast<int>(syscallID) << std::endl;
        exit(1);
    }
    }
}

void Kernel::handlePageFault(Addr faultAddr)
{
    // if it's under stack limit, allocate one physical page and set the page table entry
    if (faultAddr >= STACK_LIMIT && faultAddr < STACK_TOP)
    {
        Addr paddr = this->pmm.allocateFrame();
        Addr vpn = faultAddr >> 12;

        if (this->currentProcessIndex != -1)
            (*this->processList[this->currentProcessIndex]->getPageTable())[vpn] = (paddr & 0xFFFFF000) | 0x1;

        return;
    }

    // If it's not stack, it's a real crash (SegFault)
    std::cerr << "Segmentation Fault: Access to invalid address 0x" << std::hex << faultAddr << "\n";
    if (this->currentProcessIndex != -1)
    {
        this->processList[this->currentProcessIndex]->setState(ProcessState::TERMINATED);
        this->schedule();
        return;
    }

    this->cpu.halt();
}

bool Kernel::createProcess(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    // if already max processes, fail the creation
    if (this->processList.size() == MAX_PROCESS) return false;

    int newPid = this->processList.size() + 1;
    Process* process = new Process(newPid, filename);

    Addr vaddr = MEMORY_BASE;
    char buffer[PAGE_SIZE];

    while (file.read(buffer, 4096) || file.gcount() > 0)
    {
        size_t bytesRead = file.gcount();

        Addr paddr = this->pmm.allocateFrame();

        Addr vpn = vaddr >> 12;

        (*process->getPageTable())[vpn] = (paddr & 0xFFFFF000) | 0x1;

        // 3. Write data to RAM
        for (size_t i = 0; i < bytesRead; i++) this->cpu.storePhysicalMemory(paddr + i, 1, static_cast<Word>(buffer[i]));
        vaddr += PAGE_SIZE;
    }

    this->processList.push_back(process);
    return true;
}

void Kernel::contextSwitch(Process* nextProcess)
{
    // Save Current State (if valid)
    if (this->currentProcessIndex != -1)
    {
        Process* current = this->processList[currentProcessIndex];
        if (current->getState() == ProcessState::RUNNING)
        {
            current->setState(ProcessState::READY);
            current->getRegs() = this->cpu.getRegs();
            current->setPC(this->cpu.getPC());
        }
    }

    this->cpu.getRegs() = nextProcess->getRegs();
    this->cpu.setPC(nextProcess->getPC());
    this->cpu.setPageTable(nextProcess->getPageTable());
    nextProcess->setState(ProcessState::RUNNING);
}

void Kernel::schedule()
{
    if (processList.empty()) return;

    // Round Robin: Find the next READY process
    int nextIndex = this->currentProcessIndex;
    std::size_t attempts = 0;
    bool found = false;

    do
    {
        nextIndex = (nextIndex + 1) % this->processList.size();
        attempts++;
        if (this->processList[nextIndex]->getState() == ProcessState::READY)
        {
            found = true;
            break;
        }
    } while (attempts <= this->processList.size());

    if (!found)
    {
        // Check if everyone is terminated
        bool allDead = true;
        for (auto* p : processList)
        {
            if (p->getState() != ProcessState::TERMINATED) allDead = false;
        }

        if (allDead)
        {
            std::cout << "[Scheduler] All processes terminated.\n";
            this->cpu.halt();
        }
        return;
    }

    // Perform Switch
    Process* nextProcess = this->processList[nextIndex];
    this->contextSwitch(nextProcess);
    this->currentProcessIndex = nextIndex;
}

void Kernel::run()
{
    // ADMISSION: Move NEW -> READY
    bool hasReady = false;
    for (auto* proc : processList)
    {
        if (proc->getState() == ProcessState::NEW)
        {
            proc->setState(ProcessState::READY);
            hasReady = true;
        }
    }
    if (!hasReady)
    {
        std::cout << "[Kernel] No READY processes.\n";
        return;
    }

    // start the first process
    this->currentProcessIndex = 0;
    Process* first = this->processList[0];
    this->cpu.getRegs() = first->getRegs();
    this->cpu.setPC(first->getPC());

    this->cpu.setPageTable(first->getPageTable());
    this->cpu.enableVM(true);
    first->setState(ProcessState::RUNNING);

    std::cout << "Simulation started..." << std::endl;
    uint64_t instructions = 0;

    while (!this->cpu.isHalted())
    {
        try
        {
            this->cpu.step();
            this->cpu.advancePC();
            instructions++;
        }
        catch (SyscallException& sys)
        {
            // will handle pc increment individually
            this->handleSyscall(sys.getSyscallID());
        }
        catch (PageFaultException& pf)
        {
            this->handlePageFault(pf.getFaultAddr());
        }
        catch (std::exception& e)
        {
            std::cerr << "Unexpected exception: " << e.what() << std::endl;
            break;
        }

        if (instructions % TIME_QUANTUM == 0) this->schedule();
    }

    std::cout << "--------------------------------\n";
    std::cout << "Simulation finished in " << std::dec << instructions << " instructions.\n";
    std::cout << "--------------------------------\n";
}