#pragma once
#include <iostream>
#include "../base.hpp"

/* Contains half of all the cpu instructions for the gameboy's processor */
/* The other half is in the cb_opcode.cpp/.hpp */

namespace Giffi
{
    class GBEmu;

    // 8 Bit Operations
    void CPU_8BIT_INC(GBEmu* _emu, uint8_t& _reg);
    void CPU_8BIT_DEC(GBEmu* _emu, uint8_t& _reg);
    void CPU_8BIT_SUB(GBEmu* _emu, uint8_t& _reg, uint8_t _amount, bool _add_carry = false);
    void CPU_8BIT_ADD(GBEmu* _emu, uint8_t& _reg, uint8_t _amount, bool _add_carry = false);
    void CPU_REG_LOAD(GBEmu* _emu, uint8_t& _reg, uint8_t _data);

    void CPU_8BIT_MEMORY_INC(GBEmu* _emu, uint16_t _addr);
    void CPU_8BIT_MEMORY_DEC(GBEmu* _emu, uint16_t _addr);

    // 16 Bit Operations
    void CPU_16BIT_INC (GBEmu* _emu, uint16_t& _reg);
    void CPU_16BIT_DEC (GBEmu* _emu, uint16_t& _reg);
    void CPU_16BIT_LOAD(GBEmu* _emu, uint16_t& _reg, uint16_t _data);
    void CPU_16BIT_ADD (GBEmu* _emu, uint16_t& _reg, uint16_t _value);

    // Bitwise operations
    void CPU_RRC(GBEmu* _emu, uint8_t& _reg);
    void CPU_RR (GBEmu* _emu, uint8_t& _reg);
    void CPU_ROTATE_RIGHT(GBEmu* _emu, uint8_t& _reg, uint8_t _bit);
    void CPU_ROTATE_LEFT(GBEmu* _emu, uint8_t& _reg, uint8_t _bit);

    void CPU_8BIT_COMPARE(GBEmu* _emu, uint8_t _bits);
    void CPU_8BIT_AND(GBEmu* _emu, uint8_t _mask);
    void CPU_8BIT_XOR(GBEmu* _emu, uint8_t _mask);
    void CPU_8BIT_OR (GBEmu* _emu, uint8_t _mask);
    
    // Jumps
    void CPU_JUMP_IMMEDIATE(GBEmu* _emu, bool _do_comprision = false, uint8_t _flag = 0, bool _state = false);
    void CPU_JUMP(GBEmu* _emu, bool _do_comprision = false, uint8_t _flag = 0, bool _state = false);
    void CPU_RET (GBEmu* _emu, bool _do_comprision = false, uint8_t _flag = 0, bool _state = false);
    void CPU_CALL(GBEmu* _emu, bool _do_comprision = false, uint8_t _flag = 0, bool _state = false);
    void CPU_RESTART(GBEmu* _emu, uint8_t _addr);

    void CPU_DAA(GBEmu* _emu);
}
