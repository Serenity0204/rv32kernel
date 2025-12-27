#pragma once
#include "Common.hpp"
#include "Utils.hpp"

class Decoder
{
public:
    Decoder() = delete;
    ~Decoder() = delete;
    Decoder(const Decoder& other) = delete;
    Decoder& operator=(const Decoder& other) = delete;

    static uint32_t opcode(Word instr);
    static uint32_t rd(Word instr);
    static uint32_t rs1(Word instr);
    static uint32_t rs2(Word instr);
    static uint32_t func3(Word instr);
    static uint32_t func7(Word instr);
    static uint32_t immI(Word instr);
    static uint32_t immS(Word instr);
    static uint32_t immB(Word instr);
    static uint32_t immU(Word instr);
    static uint32_t immJ(Word instr);
};