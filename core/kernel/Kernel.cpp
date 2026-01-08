#include "Kernel.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>

Kernel::Kernel()
{
    this->pmm.init();
    this->currentTable = nullptr;
}

Kernel::~Kernel()
{
    if (this->currentTable != nullptr) delete this->currentTable;
}

void Kernel::handleSyscall(SyscallID syscallID)
{
    switch (syscallID)
    {
    case SyscallID::SYS_EXIT:
    {
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
            return;
        }
        // other fd, not supported yet
        this->cpu.writeReg(10, static_cast<Word>(-1));
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
            return;
        }

        this->cpu.writeReg(10, static_cast<Word>(-1));
        return;
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
        (*this->currentTable)[vpn] = (paddr & 0xFFFFF000) | 0x1;
        return;
    }

    // If it's not stack, it's a real crash (SegFault)
    std::cerr << "Segmentation Fault: Access to invalid address 0x" << std::hex << faultAddr << "\n";
    this->cpu.halt();
}

void Kernel::run()
{
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
            this->handleSyscall(sys.getSyscallID());
            this->cpu.advancePC();
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
    }

    // x10 (a0)
    Word result = cpu.readReg(10);
    std::cout << "--------------------------------\n";
    std::cout << "Simulation finished in " << std::dec << instructions << " instructions.\n";
    std::cout << "Exit Code (x10): " << result << "\n";
    std::cout << "--------------------------------\n";
}

bool loadBinary(const std::string& filename, Kernel* kernel)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    Addr vaddr = MEMORY_BASE;

    char buffer[PAGE_SIZE];
    PageTable* table = new PageTable();
    kernel->currentTable = table;

    while (file.read(buffer, 4096) || file.gcount() > 0)
    {
        size_t bytesRead = file.gcount();

        Addr paddr = kernel->pmm.allocateFrame();

        Addr vpn = vaddr >> 12;
        (*kernel->currentTable)[vpn] = (paddr & 0xFFFFF000) | 0x1;

        // 3. Write data to RAM
        for (size_t i = 0; i < bytesRead; i++) kernel->cpu.storePhysicalMemory(paddr + i, 1, buffer[i]);
        vaddr += PAGE_SIZE;
    }

    kernel->cpu.reset();
    kernel->cpu.enableVM(true);
    kernel->cpu.setPageTable(kernel->currentTable);

    if (file.bad()) return false;

    return true;
}