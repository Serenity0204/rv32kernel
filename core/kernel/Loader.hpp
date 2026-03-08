#pragma once
#include "KernelContext.hpp"
#include <string>

class Loader
{
public:
    Loader(SystemContext* ctx);
    bool loadELF(const std::string& filename);

private:
    SystemContext* systemCtx;
};