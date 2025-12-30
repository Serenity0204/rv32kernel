#include "machine/Bus.hpp"
#include "machine/CPU.hpp"
#include "machine/Memory.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::cout << "RISCV Emulator\n";
    if (argc < 2)
    {
        std::cout << "Usage: ./rv32sysemu <bin_file>\n";
        return 1;
    }

    // init
    Memory memory;
    std::string filename = std::string(argv[1]);
    bool good = loadBinary(filename, memory);

    if (!good)
    {
        std::cout << "Failed to load binary.\n";
        return 1;
    }
    Bus bus(memory);
    CPU cpu(bus);
    cpu.reset();

    std::cout << "Simulation started..." << std::endl;

    uint64_t instructions = 0;
    while (!cpu.isHalted())
    {
        try
        {
            cpu.step();
            instructions++;
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
            break;
        }
    }

    // x10 (a0)
    Word result = cpu.readReg(10);
    std::cout << "--------------------------------\n";
    std::cout << "Simulation finished in " << std::dec << instructions << " instructions.\n";
    std::cout << "Exit Code (x10): " << result << "\n";
    std::cout << "--------------------------------\n";
    return 1;
}