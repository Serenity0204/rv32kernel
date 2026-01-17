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

// for context switch
const int TIME_QUANTUM = 1000;

// constants for timer modeling
const std::size_t USER_MODE_TICK_TIME = 1;
const std::size_t ENTER_KERNEL_MODE_TIME = 20;
const std::size_t CONTEXT_SWITCH_TIME = 50;
const std::size_t SYSCALL_BASE_TIME = 10;
const std::size_t MEMORY_ALLOCATION_TIME = 100;
const std::size_t DISK_IO_TIME = 5000;
