#include "opcode.hpp"
#include "cb_opcode.hpp"
#include "../gbZ80.hpp"

#include <iostream>

namespace Giffi
{
    
void gbZ80::ExecuteNextOpcode()
{
    uint8_t opcode = ReadByte(mRegPC.val);
    mLastExecutedOpcode = opcode;

    mRegPC.val++;

    // Every instrucitons take atleast 4 cycles, some take longer than that.
    mCyclesDone += 4;
    switch ( opcode )
    {
        case 0x00: // NOP
            break;
        
        case 0x10: // STOP, halts after hence fallthrough
            WriteByte(0xFF04, 0); // Reset Divider Counter
            mDividerCounter = 0;
            mRegPC.val++;
            [[fallthrough]];
        case 0x76: // HALT
        {
            uint8_t IE = ReadByte(0xFFFF);
            if (IE || mEnableInterrupts)
            {
                mIsHalted = true;
            }
            break;
        }

        // 8BIT INC & DEC      
        case 0x04: // INC B
            CPU_8BIT_INC(this, mRegBC.high);
            break;
        case 0x14: // INC D
            CPU_8BIT_INC(this, mRegDE.high);
            break;
        case 0x24: // INC H
            CPU_8BIT_INC(this, mRegHL.high);
            break;
        case 0x0C: // INC C
            CPU_8BIT_INC(this, mRegBC.low);
            break;
        case 0x1C: // INC E
            CPU_8BIT_INC(this, mRegDE.low);
            break;
        case 0x2C: // INC L
            CPU_8BIT_INC(this, mRegHL.low);
            break;
        case 0x3C: // INC A 
            CPU_8BIT_INC(this, mRegAF.high);
            break;
        case 0x34: // INC (HL)
        {
            CPU_8BIT_MEMORY_INC(this, mRegHL.val);
            mCyclesDone+=8;
            break;
        }
        
        case 0x05: // DEC B
            CPU_8BIT_DEC(this, mRegBC.high);
            break;
        case 0x15: // DEC D
            CPU_8BIT_DEC(this, mRegDE.high);
            break;
        case 0x25: // DEC H
            CPU_8BIT_DEC(this, mRegHL.high);
            break;
        case 0x0D: // DEC C
            CPU_8BIT_DEC(this, mRegBC.low);
            break;
        case 0x1D: // DEC E
            CPU_8BIT_DEC(this, mRegDE.low);
            break;
        case 0x2D: // DEC L
            CPU_8BIT_DEC(this, mRegHL.low);
            break;
        case 0x3D: // DEC A
            CPU_8BIT_DEC(this, mRegAF.high);
            break;
        case 0x35: // DEC (HL)
            CPU_8BIT_MEMORY_DEC(this, mRegHL.val);
            mCyclesDone+=8;
            break;

        // 8BIT LOAD
        case 0x40: // LD B,B
            CPU_REG_LOAD(this, mRegBC.high, mRegBC.high);
            break;
        case 0x41: // LD B,C
            CPU_REG_LOAD(this, mRegBC.high, mRegBC.low);
            break;
        case 0x42: // LD B,D
            CPU_REG_LOAD(this, mRegBC.high, mRegDE.high);
            break;
        case 0x43: // LD B,E
            CPU_REG_LOAD(this, mRegBC.high, mRegDE.low);
            break;
        case 0x44: // LD B,H
            CPU_REG_LOAD(this, mRegBC.high, mRegHL.high);
            break;
        case 0x45: // LD B,L
            CPU_REG_LOAD(this, mRegBC.high, mRegHL.low);
            break;
        case 0x46: // LD B,(HL)
            CPU_REG_LOAD(this, mRegBC.high, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x47: // LD B,A
            CPU_REG_LOAD(this, mRegBC.high, mRegAF.high);
            break;
        
        case 0x48: // LD C,B
            CPU_REG_LOAD(this, mRegBC.low, mRegBC.high);
            break;
        case 0x49: // LD C,C
            CPU_REG_LOAD(this, mRegBC.low, mRegBC.low);
            break;
        case 0x4A: // LD C,D
            CPU_REG_LOAD(this, mRegBC.low, mRegDE.high);
            break;
        case 0x4B: // LD C,E
            CPU_REG_LOAD(this, mRegBC.low, mRegDE.low);
            break;
        case 0x4C: // LD C,H
            CPU_REG_LOAD(this, mRegBC.low, mRegHL.high);
            break;
        case 0x4D: // LD C,L
            CPU_REG_LOAD(this, mRegBC.low, mRegHL.low);
            break;
        case 0x4E: // LD C,(HL)
            CPU_REG_LOAD(this, mRegBC.low, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x4F: // LD C,A
            CPU_REG_LOAD(this, mRegBC.low, mRegAF.high);
            break;

        case 0x50: // LD D,B
            CPU_REG_LOAD(this, mRegDE.high, mRegBC.high);
            break;
        case 0x51: // LD D,C
            CPU_REG_LOAD(this, mRegDE.high, mRegBC.low);
            break;
        case 0x52: // LD D,D
            CPU_REG_LOAD(this, mRegDE.high, mRegDE.high);
            break;
        case 0x53: // LD D,E
            CPU_REG_LOAD(this, mRegDE.high, mRegDE.low);
            break;
        case 0x54: // LD D,H
            CPU_REG_LOAD(this, mRegDE.high, mRegHL.high);
            break;
        case 0x55: // LD D,L
            CPU_REG_LOAD(this, mRegDE.high, mRegHL.low);
            break;
        case 0x56: // LD D,(HL)
            CPU_REG_LOAD(this, mRegDE.high, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x57: // LD D,A
            CPU_REG_LOAD(this, mRegDE.high, mRegAF.high);
            break;
        
        case 0x58: // LD E,B
            CPU_REG_LOAD(this, mRegDE.low, mRegBC.high);
            break;
        case 0x59: // LD E,C
            CPU_REG_LOAD(this, mRegDE.low, mRegBC.low);
            break;
        case 0x5A: // LD E,D
            CPU_REG_LOAD(this, mRegDE.low, mRegDE.high);
            break;
        case 0x5B: // LD E,E
            CPU_REG_LOAD(this, mRegDE.low, mRegDE.low);
            break;
        case 0x5C: // LD E,H
            CPU_REG_LOAD(this, mRegDE.low, mRegHL.high);
            break;
        case 0x5D: // LD E,L
            CPU_REG_LOAD(this, mRegDE.low, mRegHL.low);
            break;
        case 0x5E: // LD E,(HL)
            CPU_REG_LOAD(this, mRegDE.low, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x5F: // LD E,A
            CPU_REG_LOAD(this, mRegDE.low, mRegAF.high);
            break;
        
        case 0x60: // LD H,B
            CPU_REG_LOAD(this, mRegHL.high, mRegBC.high);
            break;
        case 0x61: // LD H,C
            CPU_REG_LOAD(this, mRegHL.high, mRegBC.low);
            break;
        case 0x62: // LD H,D
            CPU_REG_LOAD(this, mRegHL.high, mRegDE.high);
            break;
        case 0x63: // LD H,E
            CPU_REG_LOAD(this, mRegHL.high, mRegDE.low);
            break;
        case 0x64: // LD H,H
            CPU_REG_LOAD(this, mRegHL.high, mRegHL.high);
            break;
        case 0x65: // LD H,L
            CPU_REG_LOAD(this, mRegHL.high, mRegHL.low);
            break;
        case 0x66: // LD H,(HL)
            CPU_REG_LOAD(this, mRegHL.high, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x67: // LD H,A
            CPU_REG_LOAD(this, mRegHL.high, mRegAF.high);
            break;

        case 0x68: // LD L,B
            CPU_REG_LOAD(this, mRegHL.low, mRegBC.high);
            break;
        case 0x69: // LD L,C
            CPU_REG_LOAD(this, mRegHL.low, mRegBC.low);
            break;
        case 0x6A: // LD L,D
            CPU_REG_LOAD(this, mRegHL.low, mRegDE.high);
            break;
        case 0x6B: // LD L,E
            CPU_REG_LOAD(this, mRegHL.low, mRegDE.low);
            break;
        case 0x6C: // LD L,H
            CPU_REG_LOAD(this, mRegHL.low, mRegHL.high);
            break;
        case 0x6D: // LD L,L
            CPU_REG_LOAD(this, mRegHL.low, mRegHL.low);
            break;
        case 0x6E: // LD L,(HL)
            CPU_REG_LOAD(this, mRegHL.low, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x6F: // LD L,A
            CPU_REG_LOAD(this, mRegHL.low, mRegAF.high);
            break;

        case 0x70: // LD (HL),B
            WriteByte(mRegHL.val, mRegBC.high);
            mCyclesDone += 4;
            break;
        case 0x71: // LD (HL),C
            WriteByte(mRegHL.val, mRegBC.low);
            mCyclesDone += 4;
            break;
        case 0x72: // LD (HL),D
            WriteByte(mRegHL.val, mRegDE.high);
            mCyclesDone += 4;
            break;
        case 0x73: // LD (HL),E
            WriteByte(mRegHL.val, mRegDE.low);
            mCyclesDone += 4;
            break;
        case 0x74: // LD (HL),H
            WriteByte(mRegHL.val, mRegHL.high);
            mCyclesDone += 4;
            break;
        case 0x75: // LD (HL),L
            WriteByte(mRegHL.val, mRegHL.low);
            mCyclesDone += 4;
            break;
        case 0x77: // LD (HL),A
            WriteByte(mRegHL.val, mRegAF.high);
            mCyclesDone += 4;
            break;

        case 0x78: // LD A, B
            CPU_REG_LOAD(this, mRegAF.high, mRegBC.high);
            break;
        case 0x79: // LD A, C
            CPU_REG_LOAD(this, mRegAF.high, mRegBC.low);
            break;
        case 0x7A: // LD A, D
            CPU_REG_LOAD(this, mRegAF.high, mRegDE.high);
            break;
        case 0x7B: // LD A, E
            CPU_REG_LOAD(this, mRegAF.high, mRegDE.low);
            break;
        case 0x7C: // LD A, H
            CPU_REG_LOAD(this, mRegAF.high, mRegHL.high);
            break;
        case 0x7D: // LD A, L
            CPU_REG_LOAD(this, mRegAF.high, mRegHL.low);
            break;
        case 0x7E: // LD A,(HL)
            CPU_REG_LOAD(this, mRegAF.high, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x7F: // LD A, A
            CPU_REG_LOAD(this, mRegAF.high, mRegAF.high);
            break;
        case 0xEA: // LD (a16), A
            WriteByte( ReadWord(), mRegAF.high);
            mRegPC.val  += 2;
            mCyclesDone += 12;
            break;
        case 0xFA: // LD A, (a16)
            CPU_REG_LOAD(this, mRegAF.high, ReadByte( ReadWord() ));
            mRegPC.val  += 2;
            mCyclesDone += 12;
            break;

        case 0x02: // LD (BC), A
            WriteByte( mRegBC.val, mRegAF.high);
            mCyclesDone += 8;
            break;
        case 0x12: // LD (DE), A
            WriteByte( mRegDE.val, mRegAF.high);
            mCyclesDone += 8;
            break;
        case 0x22: // LD (HL+), A
            WriteByte( mRegHL.val++, mRegAF.high);
            mCyclesDone += 8;
            break;
        case 0x32: // LD (HL-), A
            WriteByte( mRegHL.val--, mRegAF.high);
            mCyclesDone += 8;
            break;
        
        case 0xE0: // LD (0xFF00 + a8), A
            {
                uint16_t addr = 0xFF00 + ReadByte(mRegPC.val++);
                WriteByte(addr, mRegAF.high);
                mCyclesDone += 8;
                break;   
            }
        case 0xF0: // LDH A, (0xFF00 + a8)
            {
                uint16_t addr = 0xFF00 + ReadByte(mRegPC.val++);
                mRegAF.high = ReadByte(addr);
                mCyclesDone += 8;
                break;
            }

        case 0x06: // LD B, d8
            CPU_REG_LOAD(this, mRegBC.high, ReadByte(mRegPC.val++));
            mCyclesDone += 4;
            break;
        case 0x16: // LD D, d8
            CPU_REG_LOAD(this, mRegDE.high, ReadByte(mRegPC.val++));
            mCyclesDone += 4;
            break;
        case 0x26: // LD H, d8
            CPU_REG_LOAD(this, mRegHL.high, ReadByte(mRegPC.val++));
            mCyclesDone += 4;
            break;
        case 0x36: // LD (HL), d8
            WriteByte( mRegHL.val, ReadByte(mRegPC.val++));
            mCyclesDone += 8;
            break;
        
        case 0xE2: // LD (0xFF00 + C), A
            {
                uint16_t addr = 0xFF00 + mRegBC.low;
                WriteByte(addr, mRegAF.high);
                mCyclesDone += 4;
                break;
            }
        case 0xF2: // LD A, (0xFF00 + C)
            {
                uint8_t  data = ReadByte( 0xFF00 + mRegBC.low );
                mRegAF.high = data;
                mCyclesDone += 4;
                break;
            }
        case 0xE8: // ADD SP, r8
            {
                // Operation
                uint16_t reg  = mRegSP.val;
                int8_t value  = (int8_t)ReadByte(mRegPC.val++);
                int    result = (int)(reg + value);
                mRegSP.val    = (uint16_t)result;

                // Flags
                mRegAF.low = 0x00; // Reset flags
                if ( (reg & 0xF) + (value & 0xF) > 0xF)    // Half carry
                {
                    mRegAF.low |= 1 << gbFlag::HalfCarry;
                }
                if ( (reg & 0xFF) + (value & 0xFF) > 0xFF) // Carry
                {
                    mRegAF.low |= 1 << gbFlag::Carry;
                }
                mCyclesDone += 12;
                break;
            }

        case 0x0A: // LD A, (BC)
            CPU_REG_LOAD(this, mRegAF.high, ReadByte(mRegBC.val));
            mCyclesDone += 4;
            break;
        case 0x1A: // LD A, (DE)
            CPU_REG_LOAD(this, mRegAF.high, ReadByte(mRegDE.val));
            mCyclesDone += 4;
            break;
        case 0x2A: // LD A, (HL+) (post increment HL)
            CPU_REG_LOAD(this, mRegAF.high, ReadByte(mRegHL.val++));
            mCyclesDone += 4;
            break;
        case 0x3A: // LD A, (HL-) (post decrement HL)
            CPU_REG_LOAD(this, mRegAF.high, ReadByte(mRegHL.val--));
            mCyclesDone += 4;
            break;

        case 0x0E: // LD C, d8
            CPU_REG_LOAD(this, mRegBC.low, ReadByte(mRegPC.val++));
            mCyclesDone += 4;
            break;
        case 0x1E: // LD E, d8
            CPU_REG_LOAD(this, mRegDE.low, ReadByte(mRegPC.val++));
            mCyclesDone += 4;
            break;
        case 0x2E: // LD L, d8
            CPU_REG_LOAD(this, mRegHL.low, ReadByte(mRegPC.val++));
            mCyclesDone += 4;
            break;
        case 0x3E: // LD A, d8
            CPU_REG_LOAD(this, mRegAF.high, ReadByte(mRegPC.val++));
            mCyclesDone += 4;
            break;

        // 8BIT SUB
        case 0x90: // SUB A, B
            CPU_8BIT_SUB(this, mRegAF.high, mRegBC.high);
            break;
        case 0x91: // SUB A, C
            CPU_8BIT_SUB(this, mRegAF.high, mRegBC.low);
            break;
        case 0x92: // SUB A, D
            CPU_8BIT_SUB(this, mRegAF.high, mRegDE.high);
            break;
        case 0x93: // SUB A, E
            CPU_8BIT_SUB(this, mRegAF.high, mRegDE.low);
            break;
        case 0x94: // SUB A, H
            CPU_8BIT_SUB(this, mRegAF.high, mRegHL.high);
            break;
        case 0x95: // SUB A, L
            CPU_8BIT_SUB(this, mRegAF.high, mRegHL.low);
            break;
        case 0x96: // SUB A, (HL)
            CPU_8BIT_SUB(this, mRegAF.high, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x97: // SUB A, A
            CPU_8BIT_SUB(this, mRegAF.high, mRegAF.high);
            break;
        case 0xD6: // SUB A, d8
            CPU_8BIT_SUB(this, mRegAF.high, ReadByte(mRegPC.val++) );
            mCyclesDone += 4;
            break;

        case 0x98: // SBC A, B
            CPU_8BIT_SUB(this, mRegAF.high, mRegBC.high, true);
            break;
        case 0x99: // SBC A, C
            CPU_8BIT_SUB(this, mRegAF.high, mRegBC.low, true);
            break;
        case 0x9A: // SBC A, D
            CPU_8BIT_SUB(this, mRegAF.high, mRegDE.high, true);
            break;
        case 0x9B: // SBC A, E
            CPU_8BIT_SUB(this, mRegAF.high, mRegDE.low, true);
            break;
        case 0x9C: // SBC A, H
            CPU_8BIT_SUB(this, mRegAF.high, mRegHL.high, true);
            break;
        case 0x9D: // SBC A, L
            CPU_8BIT_SUB(this, mRegAF.high, mRegHL.low, true);
            break;
        case 0x9E: // SBC A, (HL)
            CPU_8BIT_SUB(this, mRegAF.high, ReadByte(mRegHL.val), true);
            mCyclesDone += 4;
            break;
        case 0x9F: // SBC A, A
            CPU_8BIT_SUB(this, mRegAF.high, mRegAF.high, true);
            break;
        case 0xDE: // SBC A, d8
            CPU_8BIT_SUB(this, mRegAF.high, ReadByte(mRegPC.val++), true);
            mCyclesDone += 4;
            break;

        // 8BIT ADD
        case 0x80: // ADD A, B
            CPU_8BIT_ADD(this, mRegAF.high, mRegBC.high );
            break;
        case 0x81: // ADD A, C
            CPU_8BIT_ADD(this, mRegAF.high, mRegBC.low );
            break;
        case 0x82: // ADD A, D
            CPU_8BIT_ADD(this, mRegAF.high, mRegDE.high );
            break;
        case 0x83: // ADD A, E
            CPU_8BIT_ADD(this, mRegAF.high, mRegDE.low );
            break;
        case 0x84: // ADD A, H
            CPU_8BIT_ADD(this, mRegAF.high, mRegHL.high );
            break;
        case 0x85: // ADD A, L
            CPU_8BIT_ADD(this, mRegAF.high, mRegHL.low );
            break;
        case 0x86: // ADD A, (HL)
            CPU_8BIT_ADD(this, mRegAF.high, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x87: // ADD A, A
            CPU_8BIT_ADD(this, mRegAF.high, mRegAF.high );
            break;
        case 0xC6: // ADD A, d8
            CPU_8BIT_ADD(this, mRegAF.high, ReadByte(mRegPC.val++) );
            mCyclesDone += 4;
            break;
        
        case 0x88: // ADC A, B
            CPU_8BIT_ADD(this, mRegAF.high, mRegBC.high, true);
            break;
        case 0x89: // ADC A, C
            CPU_8BIT_ADD(this, mRegAF.high, mRegBC.low, true);
            break;
        case 0x8A: // ADC A, D
            CPU_8BIT_ADD(this, mRegAF.high, mRegDE.high, true);
            break;
        case 0x8B: // ADC A, E
            CPU_8BIT_ADD(this, mRegAF.high, mRegDE.low, true);
            break;
        case 0x8C: // ADC A, H
            CPU_8BIT_ADD(this, mRegAF.high, mRegHL.high, true);
            break;
        case 0x8D: // ADC A, L
            CPU_8BIT_ADD(this, mRegAF.high, mRegHL.low, true);
            break;
        case 0x8E: // ADC A, (HL)
            CPU_8BIT_ADD(this, mRegAF.high, ReadByte(mRegHL.val), true);
            mCyclesDone += 4;
            break;
        case 0x8F: // ADC A, A
            CPU_8BIT_ADD(this, mRegAF.high, mRegAF.high, true);
            break;
        case 0xCE: // ADC A, d8
            CPU_8BIT_ADD(this, mRegAF.high, ReadByte(mRegPC.val++), true);
            mCyclesDone += 4;
            break;


        // Comparisions
        case 0xA0: // AND B
            CPU_8BIT_AND(this, mRegBC.high);
            break;
        case 0xA1: // AND C
            CPU_8BIT_AND(this, mRegBC.low);
            break;
        case 0xA2: // AND D
            CPU_8BIT_AND(this, mRegDE.high);
            break;
        case 0xA3: // AND E
            CPU_8BIT_AND(this, mRegDE.low);
            break;
        case 0xA4: // AND H
            CPU_8BIT_AND(this, mRegHL.high);
            break;
        case 0xA5: // AND L
            CPU_8BIT_AND(this, mRegHL.low);
            break;
        case 0xA6: // AND (HL)
            CPU_8BIT_AND(this, ReadByte(mRegHL.val) );
            break;
        case 0xA7: // AND A
            CPU_8BIT_AND(this, mRegAF.high);
            break;
        case 0xE6: // AND d8
            CPU_8BIT_AND(this, ReadByte(mRegPC.val++) );
            break;

        case 0xB0: // OR B
            CPU_8BIT_OR(this, mRegBC.high);
            break;
        case 0xB1: // OR C
            CPU_8BIT_OR(this, mRegBC.low);
            break;
        case 0xB2: // OR D
            CPU_8BIT_OR(this, mRegDE.high);
            break;
        case 0xB3: // OR E
            CPU_8BIT_OR(this, mRegDE.low);
            break;
        case 0xB4: // OR H
            CPU_8BIT_OR(this, mRegHL.high);
            break;
        case 0xB5: // OR L
            CPU_8BIT_OR(this, mRegHL.low);
            break;
        case 0xB6: // OR (HL)
            CPU_8BIT_OR(this, ReadByte(mRegHL.val) );
            break;
        case 0xB7: // OR A
            CPU_8BIT_OR(this, mRegAF.high);
            break;
        case 0xF6: // OR d8
            CPU_8BIT_OR(this, ReadByte(mRegPC.val++) );
            break;

        case 0xA8: // XOR B
            CPU_8BIT_XOR(this, mRegBC.high);
            break;
        case 0xA9: // XOR C
            CPU_8BIT_XOR(this, mRegBC.low);
            break;
        case 0xAA: // XOR D
            CPU_8BIT_XOR(this, mRegDE.high);
            break;
        case 0xAB: // XOR E
            CPU_8BIT_XOR(this, mRegDE.low);
            break;
        case 0xAC: // XOR H
            CPU_8BIT_XOR(this, mRegHL.high);
            break;
        case 0xAD: // XOR L
            CPU_8BIT_XOR(this, mRegHL.low);
            break;
        case 0xAE: // XOR (HL)
            CPU_8BIT_XOR(this, ReadByte(mRegHL.val) );
            break;
        case 0xAF: // XOR A
            CPU_8BIT_XOR(this, mRegAF.high);
            break;
        case 0xEE: // XOR d8
            CPU_8BIT_XOR(this, ReadByte(mRegPC.val++) );
            break;

        case 0xB8: // CP B
            CPU_8BIT_COMPARE(this, mRegBC.high);
            break;
        case 0xB9: // CP C
            CPU_8BIT_COMPARE(this, mRegBC.low);
            break;
        case 0xBA: // CP D
            CPU_8BIT_COMPARE(this, mRegDE.high);
            break;
        case 0xBB: // CP E
            CPU_8BIT_COMPARE(this, mRegDE.low);
            break;
        case 0xBC: // CP H
            CPU_8BIT_COMPARE(this, mRegHL.high);
            break;
        case 0xBD: // CP L
            CPU_8BIT_COMPARE(this, mRegHL.low);
            break;
        case 0xBE: // CP (HL)
            CPU_8BIT_COMPARE(this, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0xBF: // CP A
            CPU_8BIT_COMPARE(this, mRegAF.high);
            break;
        case 0xFE: // CP d8
            CPU_8BIT_COMPARE(this, ReadByte(mRegPC.val++) );
            mCyclesDone += 4;
            break;

        // 16BIT INC        
        case 0x03: // INC BC
            mRegBC.val++;
            mCyclesDone += 4;
            break;
        case 0x13: // INC DE
            mRegDE.val++;
            mCyclesDone += 4;
            break;
        case 0x23: // INC HL
            mRegHL.val++;
            mCyclesDone += 4;
            break;
        case 0x33: // INC SP
            mRegSP.val++;
            mCyclesDone += 4;
            break;

        // 16BIT DEC  
        case 0x0B: // DEC BC
            CPU_16BIT_DEC(this, mRegBC.val);
            mCyclesDone += 4;
            break;
        case 0x1B: // DEC DE
            CPU_16BIT_DEC(this, mRegDE.val);
            mCyclesDone += 4;
            break;
        case 0x2B: // DEC HL
            CPU_16BIT_DEC(this, mRegHL.val);
            mCyclesDone += 4;
            break;
        case 0x3B: // DEC SP
            CPU_16BIT_DEC(this, mRegSP.val);
            mCyclesDone += 4;
            break;

        // 16BIT LOAD
        case 0x01: // LD BC, d16
            CPU_16BIT_LOAD(this, mRegBC.val, ReadWord());
            mRegPC.val += 2;
            mCyclesDone += 8;
            break;
        case 0x11: // LD DE, d16
            CPU_16BIT_LOAD(this, mRegDE.val, ReadWord());
            mRegPC.val += 2;
            mCyclesDone += 8;
            break;
        case 0x21: // LD HL, d16
            CPU_16BIT_LOAD(this, mRegHL.val, ReadWord());
            mRegPC.val += 2;
            mCyclesDone += 8;
            break;
        case 0x31: // LD SP, d16
            CPU_16BIT_LOAD(this, mRegSP.val, ReadWord());
            mRegPC.val += 2;
            mCyclesDone += 8;
            break;
        case 0xF9: // LD SP, HL
            CPU_16BIT_LOAD(this, mRegSP.val, mRegHL.val);
            mCyclesDone += 4;
            break;
        case 0xF8: // LD HL, SP+r8
		{
            // Operation
			uint16_t reg = mRegSP.val;
            int8_t value = (int8_t)ReadByte(mRegPC.val++);
            int result = (int)(reg + value);
            mRegHL.val = (uint16_t)result;

            // Flags
            mRegAF.low = 0x00; // Reset flags
            if ( (reg & 0xF) + (value & 0xF) > 0xF)    // Half carry
            {
                mRegAF.low |= 1 << gbFlag::HalfCarry;
            }
            if ( (reg & 0xFF) + (value & 0xFF) > 0xFF) // Carry
            {
                mRegAF.low |= 1 << gbFlag::Carry;
            }            
		}break ;
        case 0x08: // LD (a16),SP
        {
            uint16_t word = ReadWord();
            WriteByte(word, mRegSP.low);
			word++;
			WriteByte(word, mRegSP.high);
            mRegPC.val  += 2;
            mCyclesDone += 16;
            break;
        }

        // 16-bit add
		case 0x09: // ADD HL, BC
            CPU_16BIT_ADD(this, mRegHL.val, mRegBC.val);
            mCyclesDone += 4;
            break;
		case 0x19: // ADD HL, DE
            CPU_16BIT_ADD(this, mRegHL.val, mRegDE.val);
            mCyclesDone += 4;
            break;
		case 0x29: // ADD HL, HL
            CPU_16BIT_ADD(this, mRegHL.val, mRegHL.val);
            mCyclesDone += 4;
            break;
		case 0x39: // ADD HL, SP
            CPU_16BIT_ADD(this, mRegHL.val, mRegSP.val);
            mCyclesDone += 4;
            break;

        case 0xC1: // POP BC
            mRegBC.val = PopWordOffStack();   
            mCyclesDone += 8;
            break;
        case 0xD1: // POP DE
            mRegDE.val = PopWordOffStack();    
            mCyclesDone += 8;
            break;
        case 0xE1: // POP HL
            mRegHL.val = PopWordOffStack();    
            mCyclesDone += 8;
            break;
        case 0xF1: // POP AF
            mRegAF.val = PopWordOffStack();  
            mRegAF.low &= 0xF0; // bottom 4 bits of the f register are ALWAYS 0.
            mCyclesDone += 8;
            break;
        
        case 0xC5: // PUSH BC
            PushWordOntoStack( mRegBC.val );
            mCyclesDone += 12;
            break;
        case 0xD5: // PUSH DE
            PushWordOntoStack( mRegDE.val );
            mCyclesDone += 12;
            break;
        case 0xE5: // PUSH HL
            PushWordOntoStack( mRegHL.val );
            mCyclesDone += 12;
            break;
        case 0xF5: // PUSH AF
            PushWordOntoStack( mRegAF.val );
            mCyclesDone += 12;
            break;

        // Shifts (Reset Zero flag)
        case 0x07: // RLCA
            CPU_RLC(this, mRegAF.high);
            mRegAF.low &= ~(1 << gbFlag::Zero);
            break;
        case 0x0F: // RRCA
            CPU_RRC(this, mRegAF.high);
            mRegAF.low &= ~(1 << gbFlag::Zero);
            break;
        case 0x17: // RLA
            CPU_RL(this, mRegAF.high);
            mRegAF.low &= ~(1 << gbFlag::Zero);
            break;
        case 0x1F: // RRA
            CPU_RR(this, mRegAF.high);
            mRegAF.low &= ~(1 << gbFlag::Zero);
            break;

        // Jumps
		case 0xE9: // JP (HL)
            mRegPC.val = mRegHL.val;
            break;
		case 0xC3: // JP a16
            CPU_JUMP(this, false, 0, false);
            break;
		case 0xC2: // JP NZ, a16
            CPU_JUMP(this, true, gbFlag::Zero, false);
            break;
		case 0xCA: // JP Z, a16
            CPU_JUMP(this, true, gbFlag::Zero, true);
            break;
		case 0xD2: // JP NC, a16
            CPU_JUMP(this, true, gbFlag::Carry, false);
            break;
		case 0xDA: // JP C, a16
            CPU_JUMP(this, true, gbFlag::Carry, true);
            break;
        
        case 0x18: // JR r8
            CPU_JUMP_IMMEDIATE(this);
            break;
		case 0x20: // JR NZ, r8
            CPU_JUMP_IMMEDIATE(this, true, gbFlag::Zero, false);
            break;
		case 0x28: // JR  Z, r8
            CPU_JUMP_IMMEDIATE(this, true, gbFlag::Zero, true);
            break;
		case 0x30: // JR NC, r8
            CPU_JUMP_IMMEDIATE(this, true, gbFlag::Carry, false);
            break;
		case 0x38: // JR  C, r8
            CPU_JUMP_IMMEDIATE(this, true, gbFlag::Carry, true);
            break;

        case 0xC9: // RET
            CPU_RET(this);
            break;
        case 0xC0: // RET NZ
            CPU_RET(this, true, gbFlag::Zero, false);
            break;
        case 0xC8: // RET Z
            CPU_RET(this, true, gbFlag::Zero, true);
            break;
        case 0xD0: // RET NC
            CPU_RET(this, true, gbFlag::Carry, false);
            break;
        case 0xD8: // RET C
            CPU_RET(this, true, gbFlag::Carry, true);
            break;
        case 0xD9: // RETI
            CPU_RET(this);
            mEnableInterrupts = true;
            break;

        case 0xCD: // CALL a16
            CPU_CALL(this);
            break;
        case 0xC4: // CALL NZ, a16
            CPU_CALL(this, true, gbFlag::Zero, false);
            break;
		case 0xCC: // CALL  Z, a16
            CPU_CALL(this, true, gbFlag::Zero, true);
            break;
		case 0xD4: // CALL NC, a16
            CPU_CALL(this, true, gbFlag::Carry, false);
            break;
		case 0xDC: // CALL  C, a16
            CPU_CALL(this, true, gbFlag::Carry, true);
            break;

        case 0xC7: // RST 00H
            CPU_RESTART(this, 0x00);
            break;
        case 0xCF: // RST 08H
            CPU_RESTART(this, 0x08);
            break;
        case 0xD7: // RST 10H
            CPU_RESTART(this, 0x10);
            break;
        case 0xDF: // RST 18H
            CPU_RESTART(this, 0x18);
            break;
        case 0xE7: // RST 20H
            CPU_RESTART(this, 0x20);
            break;
        case 0xEF: // RST 28H
            CPU_RESTART(this, 0x28);
            break;
        case 0xF7: // RST 30H
            CPU_RESTART(this, 0x30);
            break;
        case 0xFF: // RST 38H
            CPU_RESTART(this, 0x38);
            break;
        
        case 0x27: // DAA
            CPU_DAA(this);
            break;
        case 0x2F: // CPL (flip A register's bits)
        {
            // Set Substract & Half Carry -flag
            mRegAF.low |= 1 << gbFlag::Substract;
            mRegAF.low |= 1 << gbFlag::HalfCarry;

            // flip A register's bits
            mRegAF.high ^= 0b11111111;
            break;
        }
        case 0x3F: // CCF (flip carry)
        {
            // Reset Substract and halfcarry flag
            mRegAF.low &= ~( 1 << gbFlag::Substract);
            mRegAF.low &= ~( 1 << gbFlag::HalfCarry);
            // Flip Carry
            mRegAF.low ^= 1 << gbFlag::Carry;
            break;
        }
        case 0x37: // SCF (set carry)
        {
            // Reset Substract and halfcarry flag
            mRegAF.low &= ~( 1 << gbFlag::Substract);
            mRegAF.low &= ~( 1 << gbFlag::HalfCarry);
            // Set Carry
            mRegAF.low |= 1 << gbFlag::Carry;
            break;
        }

        case 0xF3: // DI
            mEnableInterrupts = false;
            break;
        case 0xFB: // EI
            mEnableInterrupts = true;
            break;

        case 0xCB: // Extended op codes
            ExecuteExtendedOpcode();
            break;
        default:
            //last_unkown_opcode = opcode;
            break;
    }
}

// 8 Bit Operations

void CPU_8BIT_INC(gbZ80* _emu, uint8_t& _reg)
{
    uint8_t before = _reg;
    _reg++;

    _emu->mRegAF.low &= ~(1 << gbFlag::Zero);      // Reset zero flag
    _emu->mRegAF.low &= ~(1 << gbFlag::Substract); // Reset substract flag
    _emu->mRegAF.low &= ~(1 << gbFlag::HalfCarry); // Reset halfcarry flag

    if (_reg == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
    if ((before & 0xF) == 0xF)
    {
        _emu->mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
}

void CPU_8BIT_MEMORY_INC(gbZ80* _emu, uint16_t _addr)
{
    uint8_t before = _emu->ReadByte(_addr);
    _emu->WriteByte(_addr, before + 1);

    _emu->mRegAF.low &= ~(1 << gbFlag::Zero);      // Reset zero flag
    _emu->mRegAF.low &= ~(1 << gbFlag::Substract); // Reset substract flag
    _emu->mRegAF.low &= ~(1 << gbFlag::HalfCarry); // Reset halfcarry flag

    if ((uint8_t)(before + 1) == 0U)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
    if ((before & 0xF) == 0xF)
    {
        _emu->mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
}

void CPU_8BIT_DEC(gbZ80* _emu, uint8_t& _reg)
{
    uint8_t before = _reg;
    _reg--;

    _emu->mRegAF.low &= ~(1 << gbFlag::Zero);      // Reset zero flag
    _emu->mRegAF.low |=  (1 << gbFlag::Substract); // Set substract flag
    _emu->mRegAF.low &= ~(1 << gbFlag::HalfCarry); // Reset halfcarry flag

    if (_reg == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
    if ((before & 0xF) == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
}

void CPU_8BIT_MEMORY_DEC(gbZ80* _emu, uint16_t _addr)
{
    uint8_t before = _emu->ReadByte(_addr);
    _emu->WriteByte(_addr, before-1);

    _emu->mRegAF.low &= ~(1 << gbFlag::Zero);      // Reset zero flag
    _emu->mRegAF.low |=   1 << gbFlag::Substract;  // Set substract flag
    _emu->mRegAF.low &= ~(1 << gbFlag::HalfCarry); // Reset halfcarry flag

    if ((before - 1) == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
    if ((before & 0xF) == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
}

void CPU_8BIT_ADD(gbZ80* _emu, uint8_t& _reg, uint8_t _amount, bool _add_carry)
{
    uint8_t carry = 0x00;

    // If add carry and carry flag is enabled
    if (_add_carry)
    {
        if ((_emu->mRegAF.low >> gbFlag::Carry) & 0b1)
        {
            carry = 1;
        }
    }
    uint16_t result_full = (uint16_t)(_reg + _amount + carry);
    uint8_t result = (uint8_t)result_full;

    // Reset all flags
    _emu->mRegAF.low = 0x00;

    if  (result == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
	if (result_full > 0xFF)
    {
		_emu->mRegAF.low |= 1 << gbFlag::Carry;
    }
	if (((_reg & 0xF) + (_amount & 0xF) + carry) > 0xF)
    {
		_emu->mRegAF.low |= 1 << gbFlag::HalfCarry;
    }

    _reg = result;
}

void CPU_8BIT_SUB(gbZ80* _emu, uint8_t& _reg, uint8_t _amount, bool _add_carry)
{
    uint8_t carry = 0x00;

    // If add carry and carry flag is enabled
    if (_add_carry)
    {
        if ((_emu->mRegAF.low >> gbFlag::Carry) & 0b1)
        {
            carry = 1;
        }
    }
    int result_full = _reg - _amount - carry;
    uint8_t result = (uint8_t)result_full;

    // Reset all flags
    _emu->mRegAF.low = 0x00;
    _emu->mRegAF.low |= 1 << gbFlag::Substract;

    if  (result == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
	if (result_full < 0)
    {
		_emu->mRegAF.low |= 1 << gbFlag::Carry;
    }
	if (((_reg & 0xF) - (_amount & 0xF) - carry) < 0)
    {
		_emu->mRegAF.low |= 1 << gbFlag::HalfCarry;
    }

    _reg = result;
}

void CPU_REG_LOAD(gbZ80* _emu, uint8_t& _reg, uint8_t _data)
{
    _reg = _data;
}

// 16 Bit Operations
void CPU_16BIT_INC(gbZ80* _emu, uint16_t& _reg)
{
    _reg++;
}

void CPU_16BIT_DEC(gbZ80* _emu, uint16_t& _reg)
{
    _reg--;
}

// Loads the next "argument"
void CPU_16BIT_LOAD(gbZ80* _emu, uint16_t& _reg, uint16_t _data) 
{
    _reg = _data;
}

void CPU_16BIT_ADD(gbZ80* _emu, uint16_t& _reg, uint16_t _value)
{
	unsigned int result = _reg + _value;

    // Reset Flags
	_emu->mRegAF.low &= ~(1 << gbFlag::Substract);
    _emu->mRegAF.low &= ~(1 << gbFlag::Carry);
    _emu->mRegAF.low &= ~(1 << gbFlag::HalfCarry);

	if ( (result & 0x10000) != 0)
    {
		_emu->mRegAF.low |= 1 << gbFlag::Carry;
    }
	if ( (_reg & 0xFFF) + (_value & 0xFFF) > 0xFFF)
    {
        _emu->mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
    _reg = (uint16_t)result;	
}

// Bitwise operations
void CPU_8BIT_COMPARE(gbZ80* _emu, uint8_t _bits)
{
    // Substraction flag set, others reset
    uint8_t before = _emu->mRegAF.high;
    _emu->mRegAF.low = 0b01000000;

    if (_emu->mRegAF.high == _bits)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
    if (_emu->mRegAF.high < _bits)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Carry;
    }
    
    int16_t htest = before & 0xF;
    htest -= (_bits & 0xF);
    if (htest < 0)
    {
	    _emu->mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
}

void CPU_8BIT_AND(gbZ80* _emu, uint8_t _mask)
{
    // Set Halfcarry, reset everything else
    _emu->mRegAF.low = 0b0010 << 4; 

    // Do the operation, set zeroflag if result was zero
    _emu->mRegAF.high &= _mask;
    if (_emu->mRegAF.high == 0x00)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void CPU_8BIT_XOR(gbZ80* _emu, uint8_t _mask)
{
    // Reset Flags
    _emu->mRegAF.low = 0x00;

    // Do the operation, set zeroflag if result was zero
    _emu->mRegAF.high ^= _mask;
    if (_emu->mRegAF.high == 0x00)
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
}

void CPU_8BIT_OR(gbZ80* _emu, uint8_t _mask)
{
    // Reset Flags
    _emu->mRegAF.low = 0x00;

    // Do the operation, set zeroflag if result was zero
    _emu->mRegAF.high |= _mask;
    if (_emu->mRegAF.high == 0x00)
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
}

// Jumps
void CPU_JUMP(gbZ80* _emu, bool _do_comprision, uint8_t _flag, bool _state)
{
	uint16_t word = _emu->ReadWord( );
	_emu->mRegPC.val += 2;
    _emu->mCyclesDone += 8; // Failed comparision takes 12 cycles (4 added before), on successful jump this takes 16cycles

	if (!_do_comprision)
	{
		_emu->mRegPC.val = word;
        _emu->mCyclesDone += 4;
	}
	else if ( ((_emu->mRegAF.low >> _flag ) & 1) == _state)
	{
		_emu->mRegPC.val = word;
        _emu->mCyclesDone += 4;
	}

}

void CPU_JUMP_IMMEDIATE(gbZ80* _emu, bool _do_comprision, uint8_t _flag, bool _state)
{
	int8_t byte = (int8_t)_emu->ReadByte(_emu->mRegPC.val);
    _emu->mCyclesDone += 4; // Failed comparision takes 8 cycles (4 added before), on successful jump this takes 12 cycles

	if (!_do_comprision)
	{
		_emu->mRegPC.val += byte;
        _emu->mCyclesDone += 4;
	}
	else if ( ((_emu->mRegAF.low >> _flag ) & 1) == _state)
	{
		_emu->mRegPC.val += byte;
        _emu->mCyclesDone += 4;
	}

    _emu->mRegPC.val++;
}

void CPU_RET(gbZ80* _emu, bool _do_comprision, uint8_t _flag, bool _state)
{
    _emu->mCyclesDone += 4; // Failed comparision takes 8 cycles (4 added before), on successful jump this takes 20 cycles

    if (!_do_comprision)
	{
		_emu->mRegPC.val = _emu->PopWordOffStack();
        _emu->mCyclesDone += 12;
	}
    else if ( ((_emu->mRegAF.low >> _flag ) & 1) == _state)
	{
		_emu->mRegPC.val = _emu->PopWordOffStack();
        _emu->mCyclesDone += 12;
	}
}

void CPU_CALL(gbZ80* _emu, bool _do_comprision, uint8_t _flag, bool _state)
{
    uint16_t word = _emu->ReadWord();
    _emu->mCyclesDone += 8; // Failed comparision takes 12 cycles (4 added before), on successful jump this takes 24 cycles
    _emu->mRegPC.val += 2;

    if (!_do_comprision)
	{
		_emu->PushWordOntoStack(_emu->mRegPC.val);
        _emu->mCyclesDone += 12;
		_emu->mRegPC.val = word;
	} else if ( ((_emu->mRegAF.low >> _flag ) & 1) == _state)
	{
		_emu->PushWordOntoStack(_emu->mRegPC.val);
        _emu->mCyclesDone += 12;
		_emu->mRegPC.val = word;
	}
}

void CPU_RESTART(gbZ80* _emu, uint8_t _addr)
{
    _emu->PushWordOntoStack( _emu->mRegPC.val );
    _emu->mRegPC.val = _addr;
    _emu->mCyclesDone += 12;
}

// FUCKING WORKS FINALLY! xd
void CPU_DAA(gbZ80* _emu)
{
    // DAA
    uint16_t RegA = (uint16_t)_emu->mRegAF.high;
    uint8_t&  RegF = _emu->mRegAF.low;

    if ( !((RegF >> gbFlag::Substract) & 1) )
    {
        if ( ((RegF >> gbFlag::HalfCarry) & 1) || ((RegA & 0xF) > 9))
        {
            RegA += 0x6;
        }

        if ( ((RegF >> gbFlag::Carry) & 1) || (RegA > 0x9F))
        {
            RegA += 0x60;
            RegF |= 1 << gbFlag::Carry;
        }
    }
    else
    {
        if ( (RegF >> gbFlag::HalfCarry) & 1 )
        {
            RegA -= 0x6;
        }

        if ( (RegF >> gbFlag::Carry) & 1)
        {
            RegA -= 0x60;
        }
    }

    // Reset Flags
    RegF &= ~(1 << gbFlag::Zero);
    RegF &= ~(1 << gbFlag::HalfCarry);

    if ( (RegA & 0xFF) == 0U)
    {
       RegF |= 1 << gbFlag::Zero;
    }

    _emu->mRegAF.high = (uint8_t)RegA;
}


} // Namespace

