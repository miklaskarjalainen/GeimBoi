#pragma once
#include <cstdint>

/* Contains half of all the cpu instructions for the gameboy's processor */
/* The other half is in the cb_opcode.cpp/.hpp */

namespace Giffi
{
    class gbZ80;

    // 8 Bit Operations
    void CPU_8BIT_INC(gbZ80* _emu, uint8_t& _reg);
    void CPU_8BIT_DEC(gbZ80* _emu, uint8_t& _reg);
    void CPU_8BIT_SUB(gbZ80* _emu, uint8_t& _reg, uint8_t _amount, bool _add_carry = false);
    void CPU_8BIT_ADD(gbZ80* _emu, uint8_t& _reg, uint8_t _amount, bool _add_carry = false);
    void CPU_REG_LOAD(gbZ80* _emu, uint8_t& _reg, uint8_t _data);

    void CPU_8BIT_MEMORY_INC(gbZ80* _emu, uint16_t _addr);
    void CPU_8BIT_MEMORY_DEC(gbZ80* _emu, uint16_t _addr);

    // 16 Bit Operations
    void CPU_16BIT_INC (gbZ80* _emu, uint16_t& _reg);
    void CPU_16BIT_DEC (gbZ80* _emu, uint16_t& _reg);
    void CPU_16BIT_LOAD(gbZ80* _emu, uint16_t& _reg, uint16_t _data);
    void CPU_16BIT_ADD (gbZ80* _emu, uint16_t& _reg, uint16_t _value);

    // Bitwise operations
    void CPU_RRC(gbZ80* _emu, uint8_t& _reg);
    void CPU_RR (gbZ80* _emu, uint8_t& _reg);
    void CPU_ROTATE_RIGHT(gbZ80* _emu, uint8_t& _reg, uint8_t _bit);
    void CPU_ROTATE_LEFT (gbZ80* _emu, uint8_t& _reg, uint8_t _bit);

    void CPU_8BIT_COMPARE(gbZ80* _emu, uint8_t _bits);
    void CPU_8BIT_AND(gbZ80* _emu, uint8_t _mask);
    void CPU_8BIT_XOR(gbZ80* _emu, uint8_t _mask);
    void CPU_8BIT_OR (gbZ80* _emu, uint8_t _mask);
    
    // Jumps
    void CPU_JUMP_IMMEDIATE(gbZ80* _emu, bool _do_comprision = false, uint8_t _flag = 0, bool _state = false);
    void CPU_JUMP(gbZ80* _emu, bool _do_comprision = false, uint8_t _flag = 0, bool _state = false);
    void CPU_RET (gbZ80* _emu, bool _do_comprision = false, uint8_t _flag = 0, bool _state = false);
    void CPU_CALL(gbZ80* _emu, bool _do_comprision = false, uint8_t _flag = 0, bool _state = false);
    void CPU_RESTART(gbZ80* _emu, uint8_t _addr);

    void CPU_DAA(gbZ80* _emu);
}
