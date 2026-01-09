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
