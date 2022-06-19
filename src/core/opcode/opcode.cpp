#include <iostream>
#include <stdint.h>
#include "../gbZ80.hpp"

namespace GeimBoi
{
    
void gbZ80::ExecuteNextOpcode()
{
    // Every instrucitons take atleast 4 cycles, some take longer than that.
    mCyclesDone += 4;
    if (mIsHalted) {
        return;
    }

    uint8_t opcode = ReadByte(mRegPC.val);
    mLastExecutedOpcode = opcode;
    mRegPC.val++;

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
            CPU8BitInc(mRegBC.high);
            break;
        case 0x14: // INC D
            CPU8BitInc(mRegDE.high);
            break;
        case 0x24: // INC H
            CPU8BitInc(mRegHL.high);
            break;
        case 0x0C: // INC C
            CPU8BitInc(mRegBC.low);
            break;
        case 0x1C: // INC E
            CPU8BitInc(mRegDE.low);
            break;
        case 0x2C: // INC L
            CPU8BitInc(mRegHL.low);
            break;
        case 0x3C: // INC A 
            CPU8BitInc(mRegAF.high);
            break;
        case 0x34: // INC (HL)
        {
            CPU8BitMemInc(mRegHL.val);
            mCyclesDone+=8;
            break;
        }
        
        case 0x05: // DEC B
            CPU8BitDec(mRegBC.high);
            break;
        case 0x15: // DEC D
            CPU8BitDec(mRegDE.high);
            break;
        case 0x25: // DEC H
            CPU8BitDec(mRegHL.high);
            break;
        case 0x0D: // DEC C
            CPU8BitDec(mRegBC.low);
            break;
        case 0x1D: // DEC E
            CPU8BitDec(mRegDE.low);
            break;
        case 0x2D: // DEC L
            CPU8BitDec(mRegHL.low);
            break;
        case 0x3D: // DEC A
            CPU8BitDec(mRegAF.high);
            break;
        case 0x35: // DEC (HL)
            CPU8BitMemDec(mRegHL.val);
            mCyclesDone+=8;
            break;

        // 8BIT LOAD
        case 0x40: // LD B,B
            mRegBC.high = mRegBC.high;
            break;
        case 0x41: // LD B,C
            mRegBC.high = mRegBC.low;
            break;
        case 0x42: // LD B,D
            mRegBC.high = mRegDE.high;
            break;
        case 0x43: // LD B,E
            mRegBC.high = mRegDE.low;
            break;
        case 0x44: // LD B,H
            mRegBC.high = mRegHL.high;
            break;
        case 0x45: // LD B,L
            mRegBC.high = mRegHL.low;
            break;
        case 0x46: // LD B,(HL)
            mRegBC.high = ReadByte(mRegHL.val);
            mCyclesDone += 4;
            break;
        case 0x47: // LD B,A
            mRegBC.high = mRegAF.high;
            break;
        
        case 0x48: // LD C,B
            mRegBC.low = mRegBC.high;
            break;
        case 0x49: // LD C,C
            mRegBC.low = mRegBC.low;
            break;
        case 0x4A: // LD C,D
            mRegBC.low = mRegDE.high;
            break;
        case 0x4B: // LD C,E
            mRegBC.low = mRegDE.low;
            break;
        case 0x4C: // LD C,H
            mRegBC.low = mRegHL.high;
            break;
        case 0x4D: // LD C,L
            mRegBC.low = mRegHL.low;
            break;
        case 0x4E: // LD C,(HL)
            mRegBC.low = ReadByte(mRegHL.val);
            mCyclesDone += 4;
            break;
        case 0x4F: // LD C,A
            mRegBC.low = mRegAF.high;
            break;

        case 0x50: // LD D,B
            mRegDE.high = mRegBC.high;
            break;
        case 0x51: // LD D,C
            mRegDE.high = mRegBC.low;
            break;
        case 0x52: // LD D,D
            mRegDE.high = mRegDE.high;
            break;
        case 0x53: // LD D,E
            mRegDE.high = mRegDE.low;
            break;
        case 0x54: // LD D,H
            mRegDE.high = mRegHL.high;
            break;
        case 0x55: // LD D,L
            mRegDE.high = mRegHL.low;
            break;
        case 0x56: // LD D,(HL)
            mRegDE.high = ReadByte(mRegHL.val);
            mCyclesDone += 4;
            break;
        case 0x57: // LD D,A
            mRegDE.high = mRegAF.high;
            break;
        
        case 0x58: // LD E,B
            mRegDE.low = mRegBC.high;
            break;
        case 0x59: // LD E,C
            mRegDE.low = mRegBC.low;
            break;
        case 0x5A: // LD E,D
            mRegDE.low = mRegDE.high;
            break;
        case 0x5B: // LD E,E
            mRegDE.low = mRegDE.low;
            break;
        case 0x5C: // LD E,H
            mRegDE.low = mRegHL.high;
            break;
        case 0x5D: // LD E,L
            mRegDE.low = mRegHL.low;
            break;
        case 0x5E: // LD E,(HL)
            mRegDE.low = ReadByte(mRegHL.val);
            mCyclesDone += 4;
            break;
        case 0x5F: // LD E,A
            mRegDE.low = mRegAF.high;
            break;
        
        case 0x60: // LD H,B
            mRegHL.high = mRegBC.high;
            break;
        case 0x61: // LD H,C
            mRegHL.high = mRegBC.low;
            break;
        case 0x62: // LD H,D
            mRegHL.high = mRegDE.high;
            break;
        case 0x63: // LD H,E
            mRegHL.high = mRegDE.low;
            break;
        case 0x64: // LD H,H
            mRegHL.high = mRegHL.high;
            break;
        case 0x65: // LD H,L
            mRegHL.high = mRegHL.low;
            break;
        case 0x66: // LD H,(HL)
            mRegHL.high = ReadByte(mRegHL.val);
            mCyclesDone += 4;
            break;
        case 0x67: // LD H,A
            mRegHL.high = mRegAF.high;
            break;

        case 0x68: // LD L,B
            mRegHL.low = mRegBC.high;
            break;
        case 0x69: // LD L,C
            mRegHL.low = mRegBC.low;
            break;
        case 0x6A: // LD L,D
            mRegHL.low = mRegDE.high;
            break;
        case 0x6B: // LD L,E
            mRegHL.low = mRegDE.low;
            break;
        case 0x6C: // LD L,H
            mRegHL.low = mRegHL.high;
            break;
        case 0x6D: // LD L,L
            mRegHL.low = mRegHL.low;
            break;
        case 0x6E: // LD L,(HL)
            mRegHL.low = ReadByte(mRegHL.val);
            mCyclesDone += 4;
            break;
        case 0x6F: // LD L,A
            mRegHL.low = mRegAF.high;
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
            mRegAF.high = mRegBC.high;
            break;
        case 0x79: // LD A, C
            mRegAF.high = mRegBC.low;
            break;
        case 0x7A: // LD A, D
            mRegAF.high = mRegDE.high;
            break;
        case 0x7B: // LD A, E
            mRegAF.high = mRegDE.low;
            break;
        case 0x7C: // LD A, H
            mRegAF.high = mRegHL.high;
            break;
        case 0x7D: // LD A, L
            mRegAF.high = mRegHL.low;
            break;
        case 0x7E: // LD A,(HL)
            mRegAF.high = ReadByte(mRegHL.val);
            mCyclesDone += 4;
            break;
        case 0x7F: // LD A, A
            mRegAF.high = mRegAF.high;
            break;
        case 0xEA: // LD (a16), A
            WriteByte(ReadWord(), mRegAF.high);
            mRegPC.val  += 2;
            mCyclesDone += 12;
            break;
        case 0xFA: // LD A, (a16)
            mRegAF.high = ReadByte( ReadWord() );
            mRegPC.val  += 2;
            mCyclesDone += 12;
            break;

        case 0x02: // LD (BC), A
            WriteByte(mRegBC.val, mRegAF.high);
            mCyclesDone += 8;
            break;
        case 0x12: // LD (DE), A
            WriteByte(mRegDE.val, mRegAF.high);
            mCyclesDone += 8;
            break;
        case 0x22: // LD (HL+), A
            WriteByte(mRegHL.val++, mRegAF.high);
            mCyclesDone += 8;
            break;
        case 0x32: // LD (HL-), A
            WriteByte(mRegHL.val--, mRegAF.high);
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
            mRegBC.high = ReadByte(mRegPC.val++);
            mCyclesDone += 4;
            break;
        case 0x16: // LD D, d8
            mRegDE.high = ReadByte(mRegPC.val++);
            mCyclesDone += 4;
            break;
        case 0x26: // LD H, d8
            mRegHL.high = ReadByte(mRegPC.val++);
            mCyclesDone += 4;
            break;
        case 0x36: // LD (HL), d8
            WriteByte(mRegHL.val, ReadByte(mRegPC.val++));
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
                uint8_t data = ReadByte( 0xFF00 + mRegBC.low );
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
            mRegAF.high = ReadByte(mRegBC.val);
            mCyclesDone += 4;
            break;
        case 0x1A: // LD A, (DE)
            mRegAF.high = ReadByte(mRegDE.val);
            mCyclesDone += 4;
            break;
        case 0x2A: // LD A, (HL+) (post increment HL)
            mRegAF.high = ReadByte(mRegHL.val++);
            mCyclesDone += 4;
            break;
        case 0x3A: // LD A, (HL-) (post decrement HL)
            mRegAF.high = ReadByte(mRegHL.val--);
            mCyclesDone += 4;
            break;

        case 0x0E: // LD C, d8
            mRegBC.low = ReadByte(mRegPC.val++);
            mCyclesDone += 4;
            break;
        case 0x1E: // LD E, d8
            mRegDE.low = ReadByte(mRegPC.val++);
            mCyclesDone += 4;
            break;
        case 0x2E: // LD L, d8
            mRegHL.low = ReadByte(mRegPC.val++);
            mCyclesDone += 4;
            break;
        case 0x3E: // LD A, d8
            mRegAF.high = ReadByte(mRegPC.val++);
            mCyclesDone += 4;
            break;

        // 8BIT SUB
        case 0x90: // SUB A, B
            CPU8BitSub(mRegAF.high, mRegBC.high);
            break;
        case 0x91: // SUB A, C
            CPU8BitSub(mRegAF.high, mRegBC.low);
            break;
        case 0x92: // SUB A, D
            CPU8BitSub(mRegAF.high, mRegDE.high);
            break;
        case 0x93: // SUB A, E
            CPU8BitSub(mRegAF.high, mRegDE.low);
            break;
        case 0x94: // SUB A, H
            CPU8BitSub(mRegAF.high, mRegHL.high);
            break;
        case 0x95: // SUB A, L
            CPU8BitSub(mRegAF.high, mRegHL.low);
            break;
        case 0x96: // SUB A, (HL)
            CPU8BitSub(mRegAF.high, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x97: // SUB A, A
            CPU8BitSub(mRegAF.high, mRegAF.high);
            break;
        case 0xD6: // SUB A, d8
            CPU8BitSub(mRegAF.high, ReadByte(mRegPC.val++) );
            mCyclesDone += 4;
            break;

        case 0x98: // SBC A, B
            CPU8BitSub(mRegAF.high, mRegBC.high, true);
            break;
        case 0x99: // SBC A, C
            CPU8BitSub(mRegAF.high, mRegBC.low, true);
            break;
        case 0x9A: // SBC A, D
            CPU8BitSub(mRegAF.high, mRegDE.high, true);
            break;
        case 0x9B: // SBC A, E
            CPU8BitSub(mRegAF.high, mRegDE.low, true);
            break;
        case 0x9C: // SBC A, H
            CPU8BitSub(mRegAF.high, mRegHL.high, true);
            break;
        case 0x9D: // SBC A, L
            CPU8BitSub(mRegAF.high, mRegHL.low, true);
            break;
        case 0x9E: // SBC A, (HL)
            CPU8BitSub(mRegAF.high, ReadByte(mRegHL.val), true);
            mCyclesDone += 4;
            break;
        case 0x9F: // SBC A, A
            CPU8BitSub(mRegAF.high, mRegAF.high, true);
            break;
        case 0xDE: // SBC A, d8
            CPU8BitSub(mRegAF.high, ReadByte(mRegPC.val++), true);
            mCyclesDone += 4;
            break;

        // 8BIT ADD
        case 0x80: // ADD A, B
            CPU8BitAdd(mRegAF.high, mRegBC.high );
            break;
        case 0x81: // ADD A, C
            CPU8BitAdd(mRegAF.high, mRegBC.low );
            break;
        case 0x82: // ADD A, D
            CPU8BitAdd(mRegAF.high, mRegDE.high );
            break;
        case 0x83: // ADD A, E
            CPU8BitAdd(mRegAF.high, mRegDE.low );
            break;
        case 0x84: // ADD A, H
            CPU8BitAdd(mRegAF.high, mRegHL.high );
            break;
        case 0x85: // ADD A, L
            CPU8BitAdd(mRegAF.high, mRegHL.low );
            break;
        case 0x86: // ADD A, (HL)
            CPU8BitAdd(mRegAF.high, ReadByte(mRegHL.val) );
            mCyclesDone += 4;
            break;
        case 0x87: // ADD A, A
            CPU8BitAdd(mRegAF.high, mRegAF.high );
            break;
        case 0xC6: // ADD A, d8
            CPU8BitAdd(mRegAF.high, ReadByte(mRegPC.val++) );
            mCyclesDone += 4;
            break;
        
        case 0x88: // ADC A, B
            CPU8BitAdd(mRegAF.high, mRegBC.high, true);
            break;
        case 0x89: // ADC A, C
            CPU8BitAdd(mRegAF.high, mRegBC.low, true);
            break;
        case 0x8A: // ADC A, D
            CPU8BitAdd(mRegAF.high, mRegDE.high, true);
            break;
        case 0x8B: // ADC A, E
            CPU8BitAdd(mRegAF.high, mRegDE.low, true);
            break;
        case 0x8C: // ADC A, H
            CPU8BitAdd(mRegAF.high, mRegHL.high, true);
            break;
        case 0x8D: // ADC A, L
            CPU8BitAdd(mRegAF.high, mRegHL.low, true);
            break;
        case 0x8E: // ADC A, (HL)
            CPU8BitAdd(mRegAF.high, ReadByte(mRegHL.val), true);
            mCyclesDone += 4;
            break;
        case 0x8F: // ADC A, A
            CPU8BitAdd(mRegAF.high, mRegAF.high, true);
            break;
        case 0xCE: // ADC A, d8
            CPU8BitAdd(mRegAF.high, ReadByte(mRegPC.val++), true);
            mCyclesDone += 4;
            break;


        // Comparisions
        case 0xA0: // AND B
            CPU8BitAnd(mRegBC.high);
            break;
        case 0xA1: // AND C
            CPU8BitAnd(mRegBC.low);
            break;
        case 0xA2: // AND D
            CPU8BitAnd(mRegDE.high);
            break;
        case 0xA3: // AND E
            CPU8BitAnd(mRegDE.low);
            break;
        case 0xA4: // AND H
            CPU8BitAnd(mRegHL.high);
            break;
        case 0xA5: // AND L
            CPU8BitAnd(mRegHL.low);
            break;
        case 0xA6: // AND (HL)
            CPU8BitAnd(ReadByte(mRegHL.val) );
            break;
        case 0xA7: // AND A
            CPU8BitAnd(mRegAF.high);
            break;
        case 0xE6: // AND d8
            CPU8BitAnd(ReadByte(mRegPC.val++));
            break;

        case 0xB0: // OR B
            CPU8BitOr(mRegBC.high);
            break;
        case 0xB1: // OR C
            CPU8BitOr(mRegBC.low);
            break;
        case 0xB2: // OR D
            CPU8BitOr(mRegDE.high);
            break;
        case 0xB3: // OR E
            CPU8BitOr(mRegDE.low);
            break;
        case 0xB4: // OR H
            CPU8BitOr(mRegHL.high);
            break;
        case 0xB5: // OR L
            CPU8BitOr(mRegHL.low);
            break;
        case 0xB6: // OR (HL)
            CPU8BitOr(ReadByte(mRegHL.val) );
            break;
        case 0xB7: // OR A
            CPU8BitOr(mRegAF.high);
            break;
        case 0xF6: // OR d8
            CPU8BitOr(ReadByte(mRegPC.val++) );
            break;

        case 0xA8: // XOR B
            CPU8BitXor(mRegBC.high);
            break;
        case 0xA9: // XOR C
            CPU8BitXor(mRegBC.low);
            break;
        case 0xAA: // XOR D
            CPU8BitXor(mRegDE.high);
            break;
        case 0xAB: // XOR E
            CPU8BitXor(mRegDE.low);
            break;
        case 0xAC: // XOR H
            CPU8BitXor(mRegHL.high);
            break;
        case 0xAD: // XOR L
            CPU8BitXor(mRegHL.low);
            break;
        case 0xAE: // XOR (HL)
            CPU8BitXor(ReadByte(mRegHL.val) );
            break;
        case 0xAF: // XOR A
            CPU8BitXor(mRegAF.high);
            break;
        case 0xEE: // XOR d8
            CPU8BitXor(ReadByte(mRegPC.val++) );
            break;

        case 0xB8: // CP B
            CPU8BitCompare(mRegBC.high);
            break;
        case 0xB9: // CP C
            CPU8BitCompare(mRegBC.low);
            break;
        case 0xBA: // CP D
            CPU8BitCompare(mRegDE.high);
            break;
        case 0xBB: // CP E
            CPU8BitCompare(mRegDE.low);
            break;
        case 0xBC: // CP H
            CPU8BitCompare(mRegHL.high);
            break;
        case 0xBD: // CP L
            CPU8BitCompare(mRegHL.low);
            break;
        case 0xBE: // CP (HL)
            CPU8BitCompare(ReadByte(mRegHL.val));
            mCyclesDone += 4;
            break;
        case 0xBF: // CP A
            CPU8BitCompare(mRegAF.high);
            break;
        case 0xFE: // CP d8
            CPU8BitCompare(ReadByte(mRegPC.val++));
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
            mRegBC.val--;
            mCyclesDone += 4;
            break;
        case 0x1B: // DEC DE
            mRegDE.val--;
            mCyclesDone += 4;
            break;
        case 0x2B: // DEC HL
            mRegHL.val--;
            mCyclesDone += 4;
            break;
        case 0x3B: // DEC SP
            mRegSP.val--;
            mCyclesDone += 4;
            break;

        // 16BIT LOAD
        case 0x01: // LD BC, d16
            mRegBC.val = ReadWord();
            mRegPC.val += 2;
            mCyclesDone += 8;
            break;
        case 0x11: // LD DE, d16
            mRegDE.val = ReadWord();
            mRegPC.val += 2;
            mCyclesDone += 8;
            break;
        case 0x21: // LD HL, d16
            mRegHL.val = ReadWord();
            mRegPC.val += 2;
            mCyclesDone += 8;
            break;
        case 0x31: // LD SP, d16
            mRegSP.val = ReadWord();
            mRegPC.val += 2;
            mCyclesDone += 8;
            break;
        case 0xF9: // LD SP, HL
            mRegSP.val = mRegHL.val;
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
		} break;
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
            CPU16BitAdd(mRegHL.val, mRegBC.val);
            mCyclesDone += 4;
            break;
		case 0x19: // ADD HL, DE
            CPU16BitAdd(mRegHL.val, mRegDE.val);
            mCyclesDone += 4;
            break;
		case 0x29: // ADD HL, HL
            CPU16BitAdd(mRegHL.val, mRegHL.val);
            mCyclesDone += 4;
            break;
		case 0x39: // ADD HL, SP
            CPU16BitAdd(mRegHL.val, mRegSP.val);
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
            CPU_RLC(mRegAF.high);
            mRegAF.low &= ~(1 << gbFlag::Zero);
            break;
        case 0x0F: // RRCA
            CPU_RRC(mRegAF.high);
            mRegAF.low &= ~(1 << gbFlag::Zero);
            break;
        case 0x17: // RLA
            CPU_RL(mRegAF.high);
            mRegAF.low &= ~(1 << gbFlag::Zero);
            break;
        case 0x1F: // RRA
            CPU_RR(mRegAF.high);
            mRegAF.low &= ~(1 << gbFlag::Zero);
            break;

        // Jumps
		case 0xE9: // JP (HL)
            mRegPC.val = mRegHL.val;
            break;
		case 0xC3: // JP a16
            CPUJump(false, 0, false);
            break;
		case 0xC2: // JP NZ, a16
            CPUJump(true, gbFlag::Zero, false);
            break;
		case 0xCA: // JP Z, a16
            CPUJump(true, gbFlag::Zero, true);
            break;
		case 0xD2: // JP NC, a16
            CPUJump(true, gbFlag::Carry, false);
            break;
		case 0xDA: // JP C, a16
            CPUJump(true, gbFlag::Carry, true);
            break;
        
        case 0x18: // JR r8
            CPUJumpImmediate(false, 0, false);
            break;
		case 0x20: // JR NZ, r8
            CPUJumpImmediate(true, gbFlag::Zero, false);
            break;
		case 0x28: // JR  Z, r8
            CPUJumpImmediate(true, gbFlag::Zero, true);
            break;
		case 0x30: // JR NC, r8
            CPUJumpImmediate(true, gbFlag::Carry, false);
            break;
		case 0x38: // JR  C, r8
            CPUJumpImmediate(true, gbFlag::Carry, true);
            break;

        case 0xC9: // RET
            CPURet(false, 0, false);
            break;
        case 0xC0: // RET NZ
            CPURet(true, gbFlag::Zero, false);
            break;
        case 0xC8: // RET Z
            CPURet(true, gbFlag::Zero, true);
            break;
        case 0xD0: // RET NC
            CPURet(true, gbFlag::Carry, false);
            break;
        case 0xD8: // RET C
            CPURet(true, gbFlag::Carry, true);
            break;
        case 0xD9: // RETI
            CPURet(false, 0, false);
            mEnableInterrupts = true;
            break;

        case 0xCD: // CALL a16
            CPUCall(false, 0, false);
            break;
        case 0xC4: // CALL NZ, a16
            CPUCall(true, gbFlag::Zero, false);
            break;
		case 0xCC: // CALL  Z, a16
            CPUCall(true, gbFlag::Zero, true);
            break;
		case 0xD4: // CALL NC, a16
            CPUCall(true, gbFlag::Carry, false);
            break;
		case 0xDC: // CALL  C, a16
            CPUCall(true, gbFlag::Carry, true);
            break;

        case 0xC7: // RST 00H
            CPURestart(0x00);
            break;
        case 0xCF: // RST 08H
            CPURestart(0x08);
            break;
        case 0xD7: // RST 10H
            CPURestart(0x10);
            break;
        case 0xDF: // RST 18H
            CPURestart(0x18);
            break;
        case 0xE7: // RST 20H
            CPURestart(0x20);
            break;
        case 0xEF: // RST 28H
            CPURestart(0x28);
            break;
        case 0xF7: // RST 30H
            CPURestart(0x30);
            break;
        case 0xFF: // RST 38H
            CPURestart(0x38);
            break;
        
        case 0x27: // DAA
            CPU_DAA();
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

void gbZ80::CPU8BitInc(uint8_t& _reg)
{
    uint8_t before = _reg;
    _reg++;

    mRegAF.low &= ~(1 << gbFlag::Zero);      // Reset zero flag
    mRegAF.low &= ~(1 << gbFlag::Substract); // Reset substract flag
    mRegAF.low &= ~(1 << gbFlag::HalfCarry); // Reset halfcarry flag

    if (_reg == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
    if ((before & 0xF) == 0xF)
    {
        mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
}

void gbZ80::CPU8BitDec(uint8_t& _reg)
{
    uint8_t before = _reg;
    _reg--;

    mRegAF.low &= ~(1 << gbFlag::Zero);      // Reset zero flag
    mRegAF.low |= (1 << gbFlag::Substract); // Set substract flag
    mRegAF.low &= ~(1 << gbFlag::HalfCarry); // Reset halfcarry flag

    if (_reg == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
    if ((before & 0xF) == 0)
    {
        mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
}

void gbZ80::CPU8BitMemInc(uint16_t _addr)
{
    uint8_t before = ReadByte(_addr);
    WriteByte(_addr, before + 1);

    mRegAF.low &= ~(1 << gbFlag::Zero);      // Reset zero flag
    mRegAF.low &= ~(1 << gbFlag::Substract); // Reset substract flag
    mRegAF.low &= ~(1 << gbFlag::HalfCarry); // Reset halfcarry flag

    if ((uint8_t)(before + 1) == 0U)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
    if ((before & 0xF) == 0xF)
    {
        mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
}

void gbZ80::CPU8BitMemDec(uint16_t _addr)
{
    uint8_t before = ReadByte(_addr);
    WriteByte(_addr, before-1);

    mRegAF.low &= ~(1 << gbFlag::Zero);      // Reset zero flag
    mRegAF.low |=   1 << gbFlag::Substract;  // Set substract flag
    mRegAF.low &= ~(1 << gbFlag::HalfCarry); // Reset halfcarry flag

    if ((before - 1) == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
    if ((before & 0xF) == 0)
    {
        mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
}

void gbZ80::CPU8BitAdd(uint8_t& _reg, uint8_t _amount, bool _add_carry)
{
    uint8_t carry = 0x00;

    // If add carry and carry flag is enabled
    if (_add_carry)
    {
        if ((mRegAF.low >> gbFlag::Carry) & 0b1)
        {
            carry = 1;
        }
    }
    uint16_t result_full = (uint16_t)(_reg + _amount + carry);
    uint8_t result = (uint8_t)result_full;

    // Reset all flags
    mRegAF.low = 0x00;

    if  (result == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
	if (result_full > 0xFF)
    {
		mRegAF.low |= 1 << gbFlag::Carry;
    }
	if (((_reg & 0xF) + (_amount & 0xF) + carry) > 0xF)
    {
		mRegAF.low |= 1 << gbFlag::HalfCarry;
    }

    _reg = result;
}

void gbZ80::CPU8BitSub(uint8_t& reg, uint8_t amount, bool addCarry)
{
    uint8_t carry = 0x00;

    // If add carry and carry flag is enabled
    if (addCarry)
    {
        if ((mRegAF.low >> gbFlag::Carry) & 0b1)
        {
            carry = 1;
        }
    }
    int result_full = reg - amount - carry;
    uint8_t result = (uint8_t)result_full;

    // Reset all flags
    mRegAF.low = 0x00;
    mRegAF.low |= 1 << gbFlag::Substract;

    if  (result == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
	if (result_full < 0)
    {
		mRegAF.low |= 1 << gbFlag::Carry;
    }
	if (((reg & 0xF) - (amount & 0xF) - carry) < 0)
    {
		mRegAF.low |= 1 << gbFlag::HalfCarry;
    }

    reg = result;
}

void gbZ80::CPU16BitAdd(uint16_t& reg, uint16_t value)
{
	unsigned int result = reg + value;

    // Reset Flags
	mRegAF.low &= ~(1 << gbFlag::Substract);
    mRegAF.low &= ~(1 << gbFlag::Carry);
    mRegAF.low &= ~(1 << gbFlag::HalfCarry);

	if ( (result & 0x10000) != 0)
    {
		mRegAF.low |= 1 << gbFlag::Carry;
    }
	if ( (reg & 0xFFF) + (value & 0xFFF) > 0xFFF)
    {
        mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
    reg = (uint16_t)result;	
}

// Bitwise operations
void gbZ80::CPU8BitCompare(uint8_t bits)
{
    // Substraction flag set, others reset
    uint8_t before = mRegAF.high;
    mRegAF.low = 0b01000000;

    if (mRegAF.high == bits)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
    if (mRegAF.high < bits)
    {
        mRegAF.low |= 1 << gbFlag::Carry;
    }
    
    int16_t htest = before & 0xF;
    htest -= (bits & 0xF);
    if (htest < 0)
    {
	    mRegAF.low |= 1 << gbFlag::HalfCarry;
    }
}

void gbZ80::CPU8BitAnd(uint8_t mask)
{
    // Set Halfcarry, reset everything else
    mRegAF.low = 0b0010 << 4; 

    // Do the operation, set zeroflag if result was zero
    mRegAF.high &= mask;
    if (mRegAF.high == 0x00)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void gbZ80::CPU8BitXor(uint8_t mask)
{
    // Reset Flags
    mRegAF.low = 0x00;

    // Do the operation, set zeroflag if result was zero
    mRegAF.high ^= mask;
    if (mRegAF.high == 0x00)
        mRegAF.low |= 1 << gbFlag::Zero;
}

void gbZ80::CPU8BitOr(uint8_t mask)
{
    // Reset Flags
    mRegAF.low = 0x00;

    // Do the operation, set zeroflag if result was zero
    mRegAF.high |= mask;
    if (mRegAF.high == 0x00)
        mRegAF.low |= 1 << gbFlag::Zero;
}

// Jumps
void gbZ80::CPUJump(bool doComprision, uint8_t flag, bool state)
{
	uint16_t word = ReadWord();
	mRegPC.val += 2;
    mCyclesDone += 8; // Failed comparision takes 12 cycles (4 added before), on successful jump this takes 16cycles

	if (!doComprision)
	{
		mRegPC.val = word;
        mCyclesDone += 4;
	}
	else if ( ((mRegAF.low >> flag) & 1) == state)
	{
		mRegPC.val = word;
        mCyclesDone += 4;
	}

}

void gbZ80::CPUJumpImmediate(bool doComprision, uint8_t flag, bool state)
{
	int8_t byte = (int8_t)ReadByte(mRegPC.val);
    mCyclesDone += 4; // Failed comparision takes 8 cycles (4 added before), on successful jump this takes 12 cycles

	if (!doComprision)
	{
		mRegPC.val += byte;
        mCyclesDone += 4;
	}
	else if ( ((mRegAF.low >> flag ) & 1) == state)
	{
		mRegPC.val += byte;
        mCyclesDone += 4;
	}

    mRegPC.val++;
}

void gbZ80::CPURet(bool doComprision, uint8_t flag, bool state)
{
    mCyclesDone += 4; // Failed comparision takes 8 cycles (4 added before), on successful jump this takes 20 cycles

    if (!doComprision)
	{
		mRegPC.val = PopWordOffStack();
        mCyclesDone += 12;
	}
    else if ( ((mRegAF.low >> flag ) & 1) == state)
	{
		mRegPC.val = PopWordOffStack();
        mCyclesDone += 12;
	}
}

void gbZ80::CPUCall(bool doComprision, uint8_t flag, bool state)
{
    uint16_t word = ReadWord();
    mCyclesDone += 8; // Failed comparision takes 12 cycles (4 added before), on successful jump this takes 24 cycles
    mRegPC.val += 2;

    if (!doComprision)
	{
        PushWordOntoStack(mRegPC.val);
        mCyclesDone += 12;
        mRegPC.val = word;
	} else if ( ((mRegAF.low >> flag) & 1) == state)
	{
		PushWordOntoStack(mRegPC.val);
        mCyclesDone += 12;
		mRegPC.val = word;
	}
}

void gbZ80::CPURestart(uint8_t addr)
{
    PushWordOntoStack(mRegPC.val);
    mRegPC.val = addr;
    mCyclesDone += 12;
}

void gbZ80::CPU_DAA()
{
    // DAA
    uint16_t RegA = (uint16_t)mRegAF.high;
    uint8_t& RegF = mRegAF.low;

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

    mRegAF.high = (uint8_t)RegA;
}


} // Namespace

