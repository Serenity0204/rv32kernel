#pragma once
#include "Types.hpp"
#include <cstdint>
#include <iostream>

// Memory
const Addr MEMORY_BASE = 0x00000000;
// 128MB
const Addr MEMORY_SIZE = 1024 * 1024 * 128;

// 1kb
const Addr PAGE_SIZE = 4096;

// 0x08000000
const Addr STACK_TOP = MEMORY_BASE + MEMORY_SIZE;

// 8MB below top
const Addr STACK_LIMIT = STACK_TOP - (8 * 1024 * 1024);

const int MAX_PROCESS = 8;

// Switch every 100 instructions
const int TIME_QUANTUM = 100;