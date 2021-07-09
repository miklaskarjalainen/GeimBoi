#pragma once
#include <iostream>
#include "../base.hpp"


namespace Giffi
{
    class GBEmu;

    void REG_SWAP(GBEmu* _emu, uint8_t& _reg);
    void GET_BIT(GBEmu* _emu, uint8_t _reg, uint8_t _bit);
    void SET_BIT(GBEmu* _emu, uint8_t& _reg, uint8_t _bit);
    void RESET_BIT(GBEmu* _emu, uint8_t& _reg, uint8_t _bit);

    void CPU_RR(GBEmu* _emu, uint8_t& _reg);
    void CPU_RL(GBEmu* _emu, uint8_t& _reg);
    void CPU_SLA(GBEmu* _emu, uint8_t& _reg);
    void CPU_SRA(GBEmu* _emu, uint8_t& _reg);
    void CPU_SRL(GBEmu* _emu, uint8_t& _reg);
    void CPU_RLC(GBEmu* _emu, uint8_t& _reg);
    void CPU_RRC(GBEmu* _emu, uint8_t& _reg);
}



