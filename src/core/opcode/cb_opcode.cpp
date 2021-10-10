#include "cb_opcode.hpp"
#include "../gbZ80.hpp"

using namespace Giffi;
    
void gbZ80::ExecuteExtendedOpcode()
{
    // All of these instructions take atleast 8 cycles
    uint8_t opcode = ReadByte( mRegPC.val );
    mLastExecutedOpcode = 0xCB00 + opcode;

    mRegPC.val++;
    mCyclesDone += 8; 
    switch (opcode)
    {
        case 0x00: // RLC B
            CPU_RLC(this, mRegBC.high);
            break;
        case 0x01: // RLC C
            CPU_RLC(this, mRegBC.low);
            break;
        case 0x02: // RLC D
            CPU_RLC(this, mRegDE.high);
            break;
        case 0x03: // RLC E
            CPU_RLC(this, mRegDE.low);
            break;
        case 0x04: // RLC H
            CPU_RLC(this, mRegHL.high);
            break;
        case 0x05: // RLC L
            CPU_RLC(this, mRegHL.low);
            break;
        case 0x06: // RLC (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_RLC(this, byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        };
        case 0x07: // RRC A
            CPU_RLC(this, mRegAF.high);
            break;

        case 0x08: // RRC B
            CPU_RRC(this, mRegBC.high);
            break;
        case 0x09: // RRC C
            CPU_RRC(this, mRegBC.low);
            break;
        case 0x0A: // RRC D
            CPU_RRC(this, mRegDE.high);
            break;
        case 0x0B: // RRC E
            CPU_RRC(this, mRegDE.low);
            break;
        case 0x0C: // RRC H
            CPU_RRC(this, mRegHL.high);
            break;
        case 0x0D: // RRC L
            CPU_RRC(this, mRegHL.low);
            break;
        case 0x0E: // RRC (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_RRC(this, byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        };
        case 0x0F: // RRC A
            CPU_RRC(this, mRegAF.high);
            break;

        // GET BIT
        case 0x40: // BIT 0, B
            GET_BIT(this, mRegBC.high, 0);
            break;
        case 0x48: // BIT 1, B
            GET_BIT(this, mRegBC.high, 1);
            break;
        case 0x50: // BIT 2, B
            GET_BIT(this, mRegBC.high, 2);
            break;
        case 0x58: // BIT 3, B
            GET_BIT(this, mRegBC.high, 3);
            break;
        case 0x60: // BIT 4, B
            GET_BIT(this, mRegBC.high, 4);
            break;
        case 0x68: // BIT 5, B
            GET_BIT(this, mRegBC.high, 5);
            break;
        case 0x70: // BIT 6, B
            GET_BIT(this, mRegBC.high, 6);
            break;
        case 0x78: // BIT 7, B
            GET_BIT(this, mRegBC.high, 7);
            break;
        
        case 0x41: // BIT 0, C
            GET_BIT(this, mRegBC.low, 0);
            break;
        case 0x49: // BIT 1, C
            GET_BIT(this, mRegBC.low, 1);
            break;
        case 0x51: // BIT 2, C
            GET_BIT(this, mRegBC.low, 2);
            break;
        case 0x59: // BIT 3, C
            GET_BIT(this, mRegBC.low, 3);
            break;
        case 0x61: // BIT 4, C
            GET_BIT(this, mRegBC.low, 4);
            break;
        case 0x69: // BIT 5, C
            GET_BIT(this, mRegBC.low, 5);
            break;
        case 0x71: // BIT 6, C
            GET_BIT(this, mRegBC.low, 6);
            break;
        case 0x79: // BIT 7, C
            GET_BIT(this, mRegBC.low, 7);
            break;

        case 0x42: // BIT 0, D
            GET_BIT(this, mRegDE.high, 0);
            break;
        case 0x4A: // BIT 1, D
            GET_BIT(this, mRegDE.high, 1);
            break;
        case 0x52: // BIT 2, D
            GET_BIT(this, mRegDE.high, 2);
            break;
        case 0x5A: // BIT 3, D
            GET_BIT(this, mRegDE.high, 3);
            break;
        case 0x62: // BIT 4, D
            GET_BIT(this, mRegDE.high, 4);
            break;
        case 0x6A: // BIT 5, D
            GET_BIT(this, mRegDE.high, 5);
            break;
        case 0x72: // BIT 6, D
            GET_BIT(this, mRegDE.high, 6);
            break;
        case 0x7A: // BIT 7, D
            GET_BIT(this, mRegDE.high, 7);
            break;

        case 0x43: // BIT 0, E
            GET_BIT(this, mRegDE.low, 0);
            break;
        case 0x4B: // BIT 1, E
            GET_BIT(this, mRegDE.low, 1);
            break;
        case 0x53: // BIT 2, E
            GET_BIT(this, mRegDE.low, 2);
            break;
        case 0x5B: // BIT 3, E
            GET_BIT(this, mRegDE.low, 3);
            break;
        case 0x63: // BIT 4, E
            GET_BIT(this, mRegDE.low, 4);
            break;
        case 0x6B: // BIT 5, E
            GET_BIT(this, mRegDE.low, 5);
            break;
        case 0x73: // BIT 6, E
            GET_BIT(this, mRegDE.low, 6);
            break;
        case 0x7B: // BIT 7, E
            GET_BIT(this, mRegDE.low, 7);
            break;

        case 0x44: // BIT 0, H
            GET_BIT(this, mRegHL.high, 0);
            break;
        case 0x4C: // BIT 1, H
            GET_BIT(this, mRegHL.high, 1);
            break;
        case 0x54: // BIT 2, H
            GET_BIT(this, mRegHL.high, 2);
            break;
        case 0x5C: // BIT 3, H
            GET_BIT(this, mRegHL.high, 3);
            break;
        case 0x64: // BIT 4, H 
            GET_BIT(this, mRegHL.high, 4);
            break;
        case 0x6C: // BIT 5, H
            GET_BIT(this, mRegHL.high, 5);
            break;
        case 0x74: // BIT 6, H
            GET_BIT(this, mRegHL.high, 6);
            break;
        case 0x7C: // BIT 7, H
            GET_BIT(this, mRegHL.high, 7);
            break;

        case 0x45: // BIT 0, L
            GET_BIT(this, mRegHL.low, 0);
            break;
        case 0x4D: // BIT 1, L
            GET_BIT(this, mRegHL.low, 1);
            break;
        case 0x55: // BIT 2, L
            GET_BIT(this, mRegHL.low, 2);
            break;
        case 0x5D: // BIT 3, L
            GET_BIT(this, mRegHL.low, 3);
            break;
        case 0x65: // BIT 4, L
            GET_BIT(this, mRegHL.low, 4);
            break;
        case 0x6D: // BIT 5, L
            GET_BIT(this, mRegHL.low, 5);
            break;
        case 0x75: // BIT 6, L
            GET_BIT(this, mRegHL.low, 6);
            break;
        case 0x7D: // BIT 7, L
            GET_BIT(this, mRegHL.low, 7);
            break;

        case 0x46: // BIT 0, (HL)
            GET_BIT(this, ReadByte(mRegHL.val), 0);
            mCyclesDone += 8;
            break;
        case 0x4E: // BIT 1, (HL)
            GET_BIT(this, ReadByte(mRegHL.val), 1);
            mCyclesDone += 8;
            break;
        case 0x56: // BIT 2, (HL)
            GET_BIT(this, ReadByte(mRegHL.val), 2);
            mCyclesDone += 8;
            break;
        case 0x5E: // BIT 3, (HL)
            GET_BIT(this, ReadByte(mRegHL.val), 3);
            mCyclesDone += 8;
            break;
        case 0x66: // BIT 4, (HL)
            GET_BIT(this, ReadByte(mRegHL.val), 4);
            mCyclesDone += 8;
            break;
        case 0x6E: // BIT 5, (HL)
            GET_BIT(this, ReadByte(mRegHL.val), 5);
            mCyclesDone += 8;
            break;
        case 0x76: // BIT 6, (HL)
            GET_BIT(this, ReadByte(mRegHL.val), 6);
            mCyclesDone += 8;
            break;
        case 0x7E: // BIT 7, (HL)
            GET_BIT(this, ReadByte(mRegHL.val), 7);
            mCyclesDone += 8;
            break;

        case 0x47: // BIT 0, A
            GET_BIT(this, mRegAF.high, 0);
            break;
        case 0x4F: // BIT 1, A
            GET_BIT(this, mRegAF.high, 1);
            break;
        case 0x57: // BIT 2, A
            GET_BIT(this, mRegAF.high, 2);
            break;
        case 0x5F: // BIT 3, A
            GET_BIT(this, mRegAF.high, 3);
            break;
        case 0x67: // BIT 4, A
            GET_BIT(this, mRegAF.high, 4);
            break;
        case 0x6F: // BIT 5, A
            GET_BIT(this, mRegAF.high, 5);
            break;
        case 0x77: // BIT 6, A
            GET_BIT(this, mRegAF.high, 6);
            break;
        case 0x7F: // BIT 7, A
            GET_BIT(this, mRegAF.high, 7);
            break;
        
        // SET //

        case 0xC0: // SET 0, B
            SET_BIT(this, mRegBC.high, 0);
            break;
        case 0xC8: // SET 1, B
            SET_BIT(this, mRegBC.high, 1);
            break;
        case 0xD0: // SET 2, B
            SET_BIT(this, mRegBC.high, 2);
            break;
        case 0xD8: // SET 3, B
            SET_BIT(this, mRegBC.high, 3);
            break;
        case 0xE0: // SET 4, B
            SET_BIT(this, mRegBC.high, 4);
            break;
        case 0xE8: // SET 5, B
            SET_BIT(this, mRegBC.high, 5);
            break;
        case 0xF0: // SET 6, B
            SET_BIT(this, mRegBC.high, 6);
            break;
        case 0xF8: // SET 7, B
            SET_BIT(this, mRegBC.high, 7);
            break;
        
        case 0xC1: // SET 0, C
            SET_BIT(this, mRegBC.low, 0);
            break;
        case 0xC9: // SET 1, C
            SET_BIT(this, mRegBC.low, 1);
            break;
        case 0xD1: // SET 2, C
            SET_BIT(this, mRegBC.low, 2);
            break;
        case 0xD9: // SET 3, C
            SET_BIT(this, mRegBC.low, 3);
            break;
        case 0xE1: // SET 4, C
            SET_BIT(this, mRegBC.low, 4);
            break;
        case 0xE9: // SET 5, C
            SET_BIT(this, mRegBC.low, 5);
            break;
        case 0xF1: // SET 6, C
            SET_BIT(this, mRegBC.low, 6);
            break;
        case 0xF9: // SET 7, C
            SET_BIT(this, mRegBC.low, 7);
            break;
        
        case 0xC2: // SET 0, D
            SET_BIT(this, mRegDE.high, 0);
            break;
        case 0xCA: // SET 1, D
            SET_BIT(this, mRegDE.high, 1);
            break;
        case 0xD2: // SET 2, D
            SET_BIT(this, mRegDE.high, 2);
            break;
        case 0xDA: // SET 3, D
            SET_BIT(this, mRegDE.high, 3);
            break;
        case 0xE2: // SET 4, D
            SET_BIT(this, mRegDE.high, 4);
            break;
        case 0xEA: // SET 5, D
            SET_BIT(this, mRegDE.high, 5);
            break;
        case 0xF2: // SET 6, D
            SET_BIT(this, mRegDE.high, 6);
            break;
        case 0xFA: // SET 7, D
            SET_BIT(this, mRegDE.high, 7);
            break;
        
        case 0xC3: // SET 0, E
            SET_BIT(this, mRegDE.low, 0);
            break;
        case 0xCB: // SET 1, E
            SET_BIT(this, mRegDE.low, 1);
            break;
        case 0xD3: // SET 2, E
            SET_BIT(this, mRegDE.low, 2);
            break;
        case 0xDB: // SET 3, E
            SET_BIT(this, mRegDE.low, 3);
            break;
        case 0xE3: // SET 4, E
            SET_BIT(this, mRegDE.low, 4);
            break;
        case 0xEB: // SET 5, E
            SET_BIT(this, mRegDE.low, 5);
            break;
        case 0xF3: // SET 6, E
            SET_BIT(this, mRegDE.low, 6);
            break;
        case 0xFB: // SET 7, E
            SET_BIT(this, mRegDE.low, 7);
            break;

        case 0xC4: // SET 0, H
            SET_BIT(this, mRegHL.high, 0);
            break;
        case 0xCC: // SET 1, H
            SET_BIT(this, mRegHL.high, 1);
            break;
        case 0xD4: // SET 2, H
            SET_BIT(this, mRegHL.high, 2);
            break;
        case 0xDC: // SET 3, H
            SET_BIT(this, mRegHL.high, 3);
            break;
        case 0xE4: // SET 4, H
            SET_BIT(this, mRegHL.high, 4);
            break;
        case 0xEC: // SET 5, H
            SET_BIT(this, mRegHL.high, 5);
            break;
        case 0xF4: // SET 6, H
            SET_BIT(this, mRegHL.high, 6);
            break;
        case 0xFC: // SET 7, H
            SET_BIT(this, mRegHL.high, 7);
            break;

        case 0xC5: // SET 0, L
            SET_BIT(this, mRegHL.low, 0);
            break;
        case 0xCD: // SET 1, L
            SET_BIT(this, mRegHL.low, 1);
            break;
        case 0xD5: // SET 2, L
            SET_BIT(this, mRegHL.low, 2);
            break;
        case 0xDD: // SET 3, L
            SET_BIT(this, mRegHL.low, 3);
            break;
        case 0xE5: // SET 4, L
            SET_BIT(this, mRegHL.low, 4);
            break;
        case 0xED: // SET 5, L
            SET_BIT(this, mRegHL.low, 5);
            break;
        case 0xF5: // SET 6, L
            SET_BIT(this, mRegHL.low, 6);
            break;
        case 0xFD: // SET 7, L
            SET_BIT(this, mRegHL.low, 7);
            break;

        case 0xC6: // SET 0, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            SET_BIT(this, byte, 0);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xCE: // SET 1, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            SET_BIT(this, byte, 1);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xD6: // SET 2, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            SET_BIT(this, byte, 2);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xDE: // SET 3, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            SET_BIT(this, byte, 3);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xE6: // SET 4, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            SET_BIT(this, byte, 4);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xEE: // SET 5, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            SET_BIT(this, byte, 5);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xF6: // SET 6, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            SET_BIT(this, byte, 6);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xFE: // SET 7, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            SET_BIT(this, byte, 7);
            WriteByte(mRegHL.val, byte);
            break;
        };
        
        case 0xC7: // SET 0, A
            SET_BIT(this, mRegAF.high, 0);
            break;
        case 0xCF: // SET 1, A
            SET_BIT(this, mRegAF.high, 1);
            break;
        case 0xD7: // SET 2, A
            SET_BIT(this, mRegAF.high, 2);
            break;
        case 0xDF: // SET 3, A
            SET_BIT(this, mRegAF.high, 3);
            break;
        case 0xE7: // SET 4, A
            SET_BIT(this, mRegAF.high, 4);
            break;
        case 0xEF: // SET 5, A
            SET_BIT(this, mRegAF.high, 5);
            break;
        case 0xF7: // SET 6, A
            SET_BIT(this, mRegAF.high, 6);
            break;
        case 0xFF: // SET 7, A
            SET_BIT(this, mRegAF.high, 7);
            break;
        
        // Reset Bit
        case 0x80: // RES 0, B
            RESET_BIT(this, mRegBC.high, 0);
            break;
        case 0x88: // RES 1, B
            RESET_BIT(this, mRegBC.high, 1);
            break;
        case 0x90: // RES 2, B
            RESET_BIT(this, mRegBC.high, 2);
            break;
        case 0x98: // RES 3, B
            RESET_BIT(this, mRegBC.high, 3);
            break;
        case 0xA0: // RES 4, B
            RESET_BIT(this, mRegBC.high, 4);
            break;
        case 0xA8: // RES 5, B
            RESET_BIT(this, mRegBC.high, 5);
            break;
        case 0xB0: // RES 6, B
            RESET_BIT(this, mRegBC.high, 6);
            break;
        case 0xB8: // RES 7, B
            RESET_BIT(this, mRegBC.high, 7);
            break;
        
        case 0x81: // RES 0, C
            RESET_BIT(this, mRegBC.low, 0);
            break;
        case 0x89: // RES 1, C
            RESET_BIT(this, mRegBC.low, 1);
            break;
        case 0x91: // RES 2, C
            RESET_BIT(this, mRegBC.low, 2);
            break;
        case 0x99: // RES 3, C
            RESET_BIT(this, mRegBC.low, 3);
            break;
        case 0xA1: // RES 4, C
            RESET_BIT(this, mRegBC.low, 4);
            break;
        case 0xA9: // RES 5, C
            RESET_BIT(this, mRegBC.low, 5);
            break;
        case 0xB1: // RES 6, C
            RESET_BIT(this, mRegBC.low, 6);
            break;
        case 0xB9: // RES 7, C
            RESET_BIT(this, mRegBC.low, 7);
            break;

        case 0x82: // RES 0, D
            RESET_BIT(this, mRegDE.high, 0);
            break;
        case 0x8A: // RES 1, D
            RESET_BIT(this, mRegDE.high, 1);
            break;
        case 0x92: // RES 2, D
            RESET_BIT(this, mRegDE.high, 2);
            break;
        case 0x9A: // RES 3, D
            RESET_BIT(this, mRegDE.high, 3);
            break;
        case 0xA2: // RES 4, D
            RESET_BIT(this, mRegDE.high, 4);
            break;
        case 0xAA: // RES 5, D
            RESET_BIT(this, mRegDE.high, 5);
            break;
        case 0xB2: // RES 6, D
            RESET_BIT(this, mRegDE.high, 6);
            break;
        case 0xBA: // RES 7, D
            RESET_BIT(this, mRegDE.high, 7);
            break;
        
        case 0x83: // RES 0, E
            RESET_BIT(this, mRegDE.low, 0);
            break;
        case 0x8B: // RES 1, E
            RESET_BIT(this, mRegDE.low, 1);
            break;
        case 0x93: // RES 2, E
            RESET_BIT(this, mRegDE.low, 2);
            break;
        case 0x9B: // RES 3, E
            RESET_BIT(this, mRegDE.low, 3);
            break;
        case 0xA3: // RES 4, E
            RESET_BIT(this, mRegDE.low, 4);
            break;
        case 0xAB: // RES 5, E
            RESET_BIT(this, mRegDE.low, 5);
            break;
        case 0xB3: // RES 6, E
            RESET_BIT(this, mRegDE.low, 6);
            break;
        case 0xBB: // RES 7, E
            RESET_BIT(this, mRegDE.low, 7);
            break;

        case 0x84: // RES 0, H
            RESET_BIT(this, mRegHL.high, 0);
            break;
        case 0x8C: // RES 1, H
            RESET_BIT(this, mRegHL.high, 1);
            break;
        case 0x94: // RES 2, H
            RESET_BIT(this, mRegHL.high, 2);
            break;
        case 0x9C: // RES 3, H
            RESET_BIT(this, mRegHL.high, 3);
            break;
        case 0xA4: // RES 4, H
            RESET_BIT(this, mRegHL.high, 4);
            break;
        case 0xAC: // RES 5, H
            RESET_BIT(this, mRegHL.high, 5);
            break;
        case 0xB4: // RES 6, H
            RESET_BIT(this, mRegHL.high, 6);
            break;
        case 0xBC: // RES 7, H
            RESET_BIT(this, mRegHL.high, 7);
            break;

        case 0x85: // RES 0, L
            RESET_BIT(this, mRegHL.low, 0);
            break;
        case 0x8D: // RES 1, L
            RESET_BIT(this, mRegHL.low, 1);
            break;
        case 0x95: // RES 2, L
            RESET_BIT(this, mRegHL.low, 2);
            break;
        case 0x9D: // RES 3, L
            RESET_BIT(this, mRegHL.low, 3);
            break;
        case 0xA5: // RES 4, L
            RESET_BIT(this, mRegHL.low, 4);
            break;
        case 0xAD: // RES 5, L
            RESET_BIT(this, mRegHL.low, 5);
            break;
        case 0xB5: // RES 6, L
            RESET_BIT(this, mRegHL.low, 6);
            break;
        case 0xBD: // RES 7, L
            RESET_BIT(this, mRegHL.low, 7);
            break;

        case 0x86: // RES 0, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            RESET_BIT(this, byte, 0);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0x8E: // RES 1, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            RESET_BIT(this, byte, 1);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0x96: // RES 2, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            RESET_BIT(this, byte, 2);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0x9E: // RES 3, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            RESET_BIT(this, byte, 3);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0xA6: // RES 4, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            RESET_BIT(this, byte, 4);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0xAE: // RES 5, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            RESET_BIT(this, byte, 5);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0xB6: // RES 6, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            RESET_BIT(this, byte, 6);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0xBE: // RES 7, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            RESET_BIT(this, byte, 7);
            WriteByte(mRegHL.val, byte);
            break;
        }
        
        case 0x87: // RES 0, A
            RESET_BIT(this, mRegAF.high, 0);
            break;
        case 0x8F: // RES 1, A
            RESET_BIT(this, mRegAF.high, 1);
            break;
        case 0x97: // RES 2, A
            RESET_BIT(this, mRegAF.high, 2);
            break;
        case 0x9F: // RES 3, A
            RESET_BIT(this, mRegAF.high, 3);
            break;
        case 0xA7: // RES 4, A
            RESET_BIT(this, mRegAF.high, 4);
            break;
        case 0xAF: // RES 5, A
            RESET_BIT(this, mRegAF.high, 5);
            break;
        case 0xB7: // RES 6, A
            RESET_BIT(this, mRegAF.high, 6);
            break;
        case 0xBF: // RES 7, A
            RESET_BIT(this, mRegAF.high, 7);
            break;

        // SWAP
        case 0x30: // SWAP B
            REG_SWAP(this, mRegBC.high);
            break;
        case 0x31: // SWAP C
            REG_SWAP(this, mRegBC.low);
            break;
        case 0x32: // SWAP D
            REG_SWAP(this, mRegDE.high);
            break;
        case 0x33: // SWAP E
            REG_SWAP(this, mRegDE.low);
            break;
        case 0x34: // SWAP H
            REG_SWAP(this, mRegHL.high);
            break;
        case 0x35: // SWAP L
            REG_SWAP(this, mRegHL.low);
            break;
        case 0x36: // SWAP (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            REG_SWAP(this, byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        case 0x37: // SWAP A
            REG_SWAP(this, mRegAF.high);
            break;

        case 0x38: // SRL B
            CPU_SRL(this, mRegBC.high);
            break;
        case 0x39: // SRL C
            CPU_SRL(this, mRegBC.low);
            break;
        case 0x3A: // SRL D
            CPU_SRL(this, mRegDE.high);
            break;
        case 0x3B: // SRL E
            CPU_SRL(this, mRegDE.low);
            break;
        case 0x3C: // SRL H
            CPU_SRL(this, mRegHL.high);
            break;
        case 0x3D: // SRL L
            CPU_SRL(this, mRegHL.low);
            break;
        case 0x3F: // SRL A
            CPU_SRL(this, mRegAF.high);
            break;
        case 0x3E: // SRL (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_SRL(this, byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        
        case 0x10: // RL B
            CPU_RL(this, mRegBC.high);
            break;
        case 0x11: // RL C
            CPU_RL(this, mRegBC.low);
            break;
        case 0x12: // RL D
            CPU_RL(this, mRegDE.high);
            break; 
        case 0x13: // RL E
            CPU_RL(this, mRegDE.low);
            break;
        case 0x14: // RL H
            CPU_RL(this, mRegHL.high);
            break; 
        case 0x15: // RL  L
            CPU_RL(this, mRegHL.low);
            break;
        case 0x16: // RL (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_RL(this, byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        case 0x17: // RL A
            CPU_RL(this, mRegAF.high);
            break;

        case 0x18: // RR B
            CPU_RR(this, mRegBC.high);
            break;
        case 0x19: // RR C
            CPU_RR(this, mRegBC.low);
            break;
        case 0x1A: // RR D
            CPU_RR(this, mRegDE.high);
            break; 
        case 0x1B: // RR E
            CPU_RR(this, mRegDE.low);
            break;
        case 0x1C: // RR H
            CPU_RR(this, mRegHL.high);
            break; 
        case 0x1D: // RR  L
            CPU_RR(this, mRegHL.low);
            break;
        case 0x1E: // RR (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_RR(this, byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        case 0x1F: // RR A
            CPU_RR(this, mRegAF.high);
            break;
        

        case 0x20: // SLA B
            CPU_SLA(this, mRegBC.high);
            break;
        case 0x21: // SLA C
            CPU_SLA(this, mRegBC.low);
            break;
        case 0x22: // SLA D
            CPU_SLA(this, mRegDE.high);
            break;
        case 0x23: // SLA E
            CPU_SLA(this, mRegDE.low);
            break;
        case 0x24: // SLA H
            CPU_SLA(this, mRegHL.high);
            break;
        case 0x25: // SLA L
            CPU_SLA(this, mRegHL.low);
            break;
        case 0x26: // SLA (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_SLA(this, byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        case 0x27: // SLA A
            CPU_SLA(this, mRegAF.high);
            break;
        
        case 0x28: // SRA B
            CPU_SRA(this, mRegBC.high);
            break;
        case 0x29: // SRA C
            CPU_SRA(this, mRegBC.low);
            break;
        case 0x2A: // SRA D
            CPU_SRA(this, mRegDE.high);
            break;
        case 0x2B: // SRA E
            CPU_SRA(this, mRegDE.low);
            break;
        case 0x2C: // SRA H
            CPU_SRA(this, mRegHL.high);
            break;
        case 0x2D: // SRA L
            CPU_SRA(this, mRegHL.low);
            break;
        case 0x2E: // SRA (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_SRA(this, byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        case 0x2F: // SRA A
            CPU_SRA(this, mRegAF.high);
            break;
        default:
            //last_unkown_opcode = (0xCB00) + opcode; 
            break;
    }
}

namespace Giffi
{

// Swap low and high nibbles of a register (8BIT)
void REG_SWAP(gbZ80* _emu, uint8_t& _reg)
{
    // Operation
    uint8_t old_high = (_reg & 0xF0) >> 4;
    _reg <<= 4;
    _reg |= old_high;

    // Flags
    _emu->mRegAF.low = 0x00; // Reset all
    if (_reg == 0x00)  // Set zero if result is zero
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
}

// Set zeroflag if wanted bit is zero.
void GET_BIT(gbZ80* _emu, uint8_t _reg, uint8_t _bit)
{
    _emu->mRegAF.low |= 1 << gbFlag::HalfCarry;  // Set Half Carry flag
    _emu->mRegAF.low &= ~(1 << gbFlag::Substract); // Reset Substract flag
    _emu->mRegAF.low &= ~(1 << gbFlag::Zero);      // Reset Zero Flag

    if (!((_reg >> _bit) & 1)) // If wanted bit is zero
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
}

// Set wanted bit
void SET_BIT(gbZ80* _emu, uint8_t& _reg, uint8_t _bit)
{
    _reg |= 1 << _bit;
}

// Set wanted bit
void RESET_BIT(gbZ80* _emu, uint8_t& _reg, uint8_t _bit)
{
    _reg &= ~(1 << _bit);
}

// SHIFTS

void CPU_RR(gbZ80* _emu, uint8_t& _reg)
{
    uint8_t carry = (_emu->mRegAF.low >> gbFlag::Carry) & 0b1;
    bool will_carry = _reg & 0b1;

    _emu->mRegAF.low = 0x00;
    _reg >>= 1;
    _reg |= (carry << 7);

    if (will_carry)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Carry;
    }

    if (_reg == 0U)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void CPU_RL(gbZ80* _emu, uint8_t& _reg)
{
    uint8_t carry = (_emu->mRegAF.low >> gbFlag::Carry) & 0b1;
    bool will_carry = (_reg >> 7) & 0b1;

    _reg <<= 1;
    _reg |= carry;

    _emu->mRegAF.low = 0x00;
    if (will_carry)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Carry;
    }

    if (_reg == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void CPU_SLA(gbZ80* _emu, uint8_t& _reg)
{
    bool isMSBSet = (_reg >> 7) & 0b1;
    _reg <<= 1;
    _emu->mRegAF.low = 0x00;

    if (isMSBSet)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Carry;
    }

    if (_reg == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void CPU_SRA(gbZ80* _emu, uint8_t& _reg)
{
    bool isLSBSet = (_reg >> 0) & 0b1;
    bool isMSBSet = (_reg >> 7) & 0b1;

    _emu->mRegAF.low = 0x00; // Reset Flags
    _reg >>= 1;

    if (isMSBSet)
    {
        _reg |= 1 << 7;
    }
    if (isLSBSet)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Carry;
    }

    if (_reg == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void CPU_SRL(gbZ80* _emu, uint8_t& _reg)
{
    bool isLSBSet = (_reg >> 0) & 0b1;

    _emu->mRegAF.low = 0x00;

    _reg >>= 1;

    if (isLSBSet)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Carry;
    }
    if (_reg == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void CPU_RLC(gbZ80* _emu, uint8_t& _reg)
{
    bool will_carry = (_reg >> 7) & 0b1;
    _reg <<= 1;

    _emu->mRegAF.low = 0x00;
    if (will_carry)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Carry;
        _reg |= 1;
    }
    if (_reg == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void CPU_RRC(gbZ80* _emu, uint8_t& _reg)
{
    bool will_carry = _reg & 0b1;
    _reg >>= 1;

    _emu->mRegAF.low = 0x00;
    if (will_carry)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Carry;
        _reg |= 1 << 7;
    }

    if (_reg == 0)
    {
        _emu->mRegAF.low |= 1 << gbFlag::Zero;
    }
}

}