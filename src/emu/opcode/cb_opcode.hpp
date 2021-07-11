#pragma once
#include <cstdint>

namespace Giffi
{
    class gbZ80;

    void REG_SWAP(gbZ80* _emu, uint8_t& _reg);
    void GET_BIT(gbZ80* _emu, uint8_t _reg, uint8_t _bit);
    void SET_BIT(gbZ80* _emu, uint8_t& _reg, uint8_t _bit);
    void RESET_BIT(gbZ80* _emu, uint8_t& _reg, uint8_t _bit);

    void CPU_RR(gbZ80* _emu, uint8_t& _reg);
    void CPU_RL(gbZ80* _emu, uint8_t& _reg);
    void CPU_SLA(gbZ80* _emu, uint8_t& _reg);
    void CPU_SRA(gbZ80* _emu, uint8_t& _reg); 
    void CPU_SRL(gbZ80* _emu, uint8_t& _reg);
    void CPU_RLC(gbZ80* _emu, uint8_t& _reg);
    void CPU_RRC(gbZ80* _emu, uint8_t& _reg);
}



