#include <stdint.h>
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
            CPU_RLC(mRegBC.high);
            break;
        case 0x01: // RLC C
            CPU_RLC(mRegBC.low);
            break;
        case 0x02: // RLC D
            CPU_RLC(mRegDE.high);
            break;
        case 0x03: // RLC E
            CPU_RLC(mRegDE.low);
            break;
        case 0x04: // RLC H
            CPU_RLC(mRegHL.high);
            break;
        case 0x05: // RLC L
            CPU_RLC(mRegHL.low);
            break;
        case 0x06: // RLC (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_RLC(byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        };
        case 0x07: // RRC A
            CPU_RLC(mRegAF.high);
            break;

        case 0x08: // RRC B
            CPU_RRC(mRegBC.high);
            break;
        case 0x09: // RRC C
            CPU_RRC(mRegBC.low);
            break;
        case 0x0A: // RRC D
            CPU_RRC(mRegDE.high);
            break;
        case 0x0B: // RRC E
            CPU_RRC(mRegDE.low);
            break;
        case 0x0C: // RRC H
            CPU_RRC(mRegHL.high);
            break;
        case 0x0D: // RRC L
            CPU_RRC(mRegHL.low);
            break;
        case 0x0E: // RRC (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_RRC(byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        };
        case 0x0F: // RRC A
            CPU_RRC(mRegAF.high);
            break;

        // GET BIT
        case 0x40: // BIT 0, B
            CPUGetBit(mRegBC.high, 0);
            break;
        case 0x48: // BIT 1, B
            CPUGetBit(mRegBC.high, 1);
            break;
        case 0x50: // BIT 2, B
            CPUGetBit(mRegBC.high, 2);
            break;
        case 0x58: // BIT 3, B
            CPUGetBit(mRegBC.high, 3);
            break;
        case 0x60: // BIT 4, B
            CPUGetBit(mRegBC.high, 4);
            break;
        case 0x68: // BIT 5, B
            CPUGetBit(mRegBC.high, 5);
            break;
        case 0x70: // BIT 6, B
            CPUGetBit(mRegBC.high, 6);
            break;
        case 0x78: // BIT 7, B
            CPUGetBit(mRegBC.high, 7);
            break;
        
        case 0x41: // BIT 0, C
            CPUGetBit(mRegBC.low, 0);
            break;
        case 0x49: // BIT 1, C
            CPUGetBit(mRegBC.low, 1);
            break;
        case 0x51: // BIT 2, C
            CPUGetBit(mRegBC.low, 2);
            break;
        case 0x59: // BIT 3, C
            CPUGetBit(mRegBC.low, 3);
            break;
        case 0x61: // BIT 4, C
            CPUGetBit(mRegBC.low, 4);
            break;
        case 0x69: // BIT 5, C
            CPUGetBit(mRegBC.low, 5);
            break;
        case 0x71: // BIT 6, C
            CPUGetBit(mRegBC.low, 6);
            break;
        case 0x79: // BIT 7, C
            CPUGetBit(mRegBC.low, 7);
            break;

        case 0x42: // BIT 0, D
            CPUGetBit(mRegDE.high, 0);
            break;
        case 0x4A: // BIT 1, D
            CPUGetBit(mRegDE.high, 1);
            break;
        case 0x52: // BIT 2, D
            CPUGetBit(mRegDE.high, 2);
            break;
        case 0x5A: // BIT 3, D
            CPUGetBit(mRegDE.high, 3);
            break;
        case 0x62: // BIT 4, D
            CPUGetBit(mRegDE.high, 4);
            break;
        case 0x6A: // BIT 5, D
            CPUGetBit(mRegDE.high, 5);
            break;
        case 0x72: // BIT 6, D
            CPUGetBit(mRegDE.high, 6);
            break;
        case 0x7A: // BIT 7, D
            CPUGetBit(mRegDE.high, 7);
            break;

        case 0x43: // BIT 0, E
            CPUGetBit(mRegDE.low, 0);
            break;
        case 0x4B: // BIT 1, E
            CPUGetBit(mRegDE.low, 1);
            break;
        case 0x53: // BIT 2, E
            CPUGetBit(mRegDE.low, 2);
            break;
        case 0x5B: // BIT 3, E
            CPUGetBit(mRegDE.low, 3);
            break;
        case 0x63: // BIT 4, E
            CPUGetBit(mRegDE.low, 4);
            break;
        case 0x6B: // BIT 5, E
            CPUGetBit(mRegDE.low, 5);
            break;
        case 0x73: // BIT 6, E
            CPUGetBit(mRegDE.low, 6);
            break;
        case 0x7B: // BIT 7, E
            CPUGetBit(mRegDE.low, 7);
            break;

        case 0x44: // BIT 0, H
            CPUGetBit(mRegHL.high, 0);
            break;
        case 0x4C: // BIT 1, H
            CPUGetBit(mRegHL.high, 1);
            break;
        case 0x54: // BIT 2, H
            CPUGetBit(mRegHL.high, 2);
            break;
        case 0x5C: // BIT 3, H
            CPUGetBit(mRegHL.high, 3);
            break;
        case 0x64: // BIT 4, H 
            CPUGetBit(mRegHL.high, 4);
            break;
        case 0x6C: // BIT 5, H
            CPUGetBit(mRegHL.high, 5);
            break;
        case 0x74: // BIT 6, H
            CPUGetBit(mRegHL.high, 6);
            break;
        case 0x7C: // BIT 7, H
            CPUGetBit(mRegHL.high, 7);
            break;

        case 0x45: // BIT 0, L
            CPUGetBit(mRegHL.low, 0);
            break;
        case 0x4D: // BIT 1, L
            CPUGetBit(mRegHL.low, 1);
            break;
        case 0x55: // BIT 2, L
            CPUGetBit(mRegHL.low, 2);
            break;
        case 0x5D: // BIT 3, L
            CPUGetBit(mRegHL.low, 3);
            break;
        case 0x65: // BIT 4, L
            CPUGetBit(mRegHL.low, 4);
            break;
        case 0x6D: // BIT 5, L
            CPUGetBit(mRegHL.low, 5);
            break;
        case 0x75: // BIT 6, L
            CPUGetBit(mRegHL.low, 6);
            break;
        case 0x7D: // BIT 7, L
            CPUGetBit(mRegHL.low, 7);
            break;

        case 0x46: // BIT 0, (HL)
            CPUGetBit(ReadByte(mRegHL.val), 0);
            mCyclesDone += 8;
            break;
        case 0x4E: // BIT 1, (HL)
            CPUGetBit(ReadByte(mRegHL.val), 1);
            mCyclesDone += 8;
            break;
        case 0x56: // BIT 2, (HL)
            CPUGetBit(ReadByte(mRegHL.val), 2);
            mCyclesDone += 8;
            break;
        case 0x5E: // BIT 3, (HL)
            CPUGetBit(ReadByte(mRegHL.val), 3);
            mCyclesDone += 8;
            break;
        case 0x66: // BIT 4, (HL)
            CPUGetBit(ReadByte(mRegHL.val), 4);
            mCyclesDone += 8;
            break;
        case 0x6E: // BIT 5, (HL)
            CPUGetBit(ReadByte(mRegHL.val), 5);
            mCyclesDone += 8;
            break;
        case 0x76: // BIT 6, (HL)
            CPUGetBit(ReadByte(mRegHL.val), 6);
            mCyclesDone += 8;
            break;
        case 0x7E: // BIT 7, (HL)
            CPUGetBit(ReadByte(mRegHL.val), 7);
            mCyclesDone += 8;
            break;

        case 0x47: // BIT 0, A
            CPUGetBit(mRegAF.high, 0);
            break;
        case 0x4F: // BIT 1, A
            CPUGetBit(mRegAF.high, 1);
            break;
        case 0x57: // BIT 2, A
            CPUGetBit(mRegAF.high, 2);
            break;
        case 0x5F: // BIT 3, A
            CPUGetBit(mRegAF.high, 3);
            break;
        case 0x67: // BIT 4, A
            CPUGetBit(mRegAF.high, 4);
            break;
        case 0x6F: // BIT 5, A
            CPUGetBit(mRegAF.high, 5);
            break;
        case 0x77: // BIT 6, A
            CPUGetBit(mRegAF.high, 6);
            break;
        case 0x7F: // BIT 7, A
            CPUGetBit(mRegAF.high, 7);
            break;
        
        // SET //

        case 0xC0: // SET 0, B
            CPUSetBit(mRegBC.high, 0);
            break;
        case 0xC8: // SET 1, B
            CPUSetBit(mRegBC.high, 1);
            break;
        case 0xD0: // SET 2, B
            CPUSetBit(mRegBC.high, 2);
            break;
        case 0xD8: // SET 3, B
            CPUSetBit(mRegBC.high, 3);
            break;
        case 0xE0: // SET 4, B
            CPUSetBit(mRegBC.high, 4);
            break;
        case 0xE8: // SET 5, B
            CPUSetBit(mRegBC.high, 5);
            break;
        case 0xF0: // SET 6, B
            CPUSetBit(mRegBC.high, 6);
            break;
        case 0xF8: // SET 7, B
            CPUSetBit(mRegBC.high, 7);
            break;
        
        case 0xC1: // SET 0, C
            CPUSetBit(mRegBC.low, 0);
            break;
        case 0xC9: // SET 1, C
            CPUSetBit(mRegBC.low, 1);
            break;
        case 0xD1: // SET 2, C
            CPUSetBit(mRegBC.low, 2);
            break;
        case 0xD9: // SET 3, C
            CPUSetBit(mRegBC.low, 3);
            break;
        case 0xE1: // SET 4, C
            CPUSetBit(mRegBC.low, 4);
            break;
        case 0xE9: // SET 5, C
            CPUSetBit(mRegBC.low, 5);
            break;
        case 0xF1: // SET 6, C
            CPUSetBit(mRegBC.low, 6);
            break;
        case 0xF9: // SET 7, C
            CPUSetBit(mRegBC.low, 7);
            break;
        
        case 0xC2: // SET 0, D
            CPUSetBit(mRegDE.high, 0);
            break;
        case 0xCA: // SET 1, D
            CPUSetBit(mRegDE.high, 1);
            break;
        case 0xD2: // SET 2, D
            CPUSetBit(mRegDE.high, 2);
            break;
        case 0xDA: // SET 3, D
            CPUSetBit(mRegDE.high, 3);
            break;
        case 0xE2: // SET 4, D
            CPUSetBit(mRegDE.high, 4);
            break;
        case 0xEA: // SET 5, D
            CPUSetBit(mRegDE.high, 5);
            break;
        case 0xF2: // SET 6, D
            CPUSetBit(mRegDE.high, 6);
            break;
        case 0xFA: // SET 7, D
            CPUSetBit(mRegDE.high, 7);
            break;
        
        case 0xC3: // SET 0, E
            CPUSetBit(mRegDE.low, 0);
            break;
        case 0xCB: // SET 1, E
            CPUSetBit(mRegDE.low, 1);
            break;
        case 0xD3: // SET 2, E
            CPUSetBit(mRegDE.low, 2);
            break;
        case 0xDB: // SET 3, E
            CPUSetBit(mRegDE.low, 3);
            break;
        case 0xE3: // SET 4, E
            CPUSetBit(mRegDE.low, 4);
            break;
        case 0xEB: // SET 5, E
            CPUSetBit(mRegDE.low, 5);
            break;
        case 0xF3: // SET 6, E
            CPUSetBit(mRegDE.low, 6);
            break;
        case 0xFB: // SET 7, E
            CPUSetBit(mRegDE.low, 7);
            break;

        case 0xC4: // SET 0, H
            CPUSetBit(mRegHL.high, 0);
            break;
        case 0xCC: // SET 1, H
            CPUSetBit(mRegHL.high, 1);
            break;
        case 0xD4: // SET 2, H
            CPUSetBit(mRegHL.high, 2);
            break;
        case 0xDC: // SET 3, H
            CPUSetBit(mRegHL.high, 3);
            break;
        case 0xE4: // SET 4, H
            CPUSetBit(mRegHL.high, 4);
            break;
        case 0xEC: // SET 5, H
            CPUSetBit(mRegHL.high, 5);
            break;
        case 0xF4: // SET 6, H
            CPUSetBit(mRegHL.high, 6);
            break;
        case 0xFC: // SET 7, H
            CPUSetBit(mRegHL.high, 7);
            break;

        case 0xC5: // SET 0, L
            CPUSetBit(mRegHL.low, 0);
            break;
        case 0xCD: // SET 1, L
            CPUSetBit(mRegHL.low, 1);
            break;
        case 0xD5: // SET 2, L
            CPUSetBit(mRegHL.low, 2);
            break;
        case 0xDD: // SET 3, L
            CPUSetBit(mRegHL.low, 3);
            break;
        case 0xE5: // SET 4, L
            CPUSetBit(mRegHL.low, 4);
            break;
        case 0xED: // SET 5, L
            CPUSetBit(mRegHL.low, 5);
            break;
        case 0xF5: // SET 6, L
            CPUSetBit(mRegHL.low, 6);
            break;
        case 0xFD: // SET 7, L
            CPUSetBit(mRegHL.low, 7);
            break;

        case 0xC6: // SET 0, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUSetBit(byte, 0);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xCE: // SET 1, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUSetBit(byte, 1);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xD6: // SET 2, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUSetBit(byte, 2);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xDE: // SET 3, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUSetBit(byte, 3);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xE6: // SET 4, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUSetBit(byte, 4);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xEE: // SET 5, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUSetBit(byte, 5);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xF6: // SET 6, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUSetBit(byte, 6);
            WriteByte(mRegHL.val, byte);
            break;
        };
        case 0xFE: // SET 7, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUSetBit(byte, 7);
            WriteByte(mRegHL.val, byte);
            break;
        };
        
        case 0xC7: // SET 0, A
            CPUSetBit(mRegAF.high, 0);
            break;
        case 0xCF: // SET 1, A
            CPUSetBit(mRegAF.high, 1);
            break;
        case 0xD7: // SET 2, A
            CPUSetBit(mRegAF.high, 2);
            break;
        case 0xDF: // SET 3, A
            CPUSetBit(mRegAF.high, 3);
            break;
        case 0xE7: // SET 4, A
            CPUSetBit(mRegAF.high, 4);
            break;
        case 0xEF: // SET 5, A
            CPUSetBit(mRegAF.high, 5);
            break;
        case 0xF7: // SET 6, A
            CPUSetBit(mRegAF.high, 6);
            break;
        case 0xFF: // SET 7, A
            CPUSetBit(mRegAF.high, 7);
            break;
        
        // Reset Bit
        case 0x80: // RES 0, B
            CPUResetBit(mRegBC.high, 0);
            break;
        case 0x88: // RES 1, B
            CPUResetBit(mRegBC.high, 1);
            break;
        case 0x90: // RES 2, B
            CPUResetBit(mRegBC.high, 2);
            break;
        case 0x98: // RES 3, B
            CPUResetBit(mRegBC.high, 3);
            break;
        case 0xA0: // RES 4, B
            CPUResetBit(mRegBC.high, 4);
            break;
        case 0xA8: // RES 5, B
            CPUResetBit(mRegBC.high, 5);
            break;
        case 0xB0: // RES 6, B
            CPUResetBit(mRegBC.high, 6);
            break;
        case 0xB8: // RES 7, B
            CPUResetBit(mRegBC.high, 7);
            break;
        
        case 0x81: // RES 0, C
            CPUResetBit(mRegBC.low, 0);
            break;
        case 0x89: // RES 1, C
            CPUResetBit(mRegBC.low, 1);
            break;
        case 0x91: // RES 2, C
            CPUResetBit(mRegBC.low, 2);
            break;
        case 0x99: // RES 3, C
            CPUResetBit(mRegBC.low, 3);
            break;
        case 0xA1: // RES 4, C
            CPUResetBit(mRegBC.low, 4);
            break;
        case 0xA9: // RES 5, C
            CPUResetBit(mRegBC.low, 5);
            break;
        case 0xB1: // RES 6, C
            CPUResetBit(mRegBC.low, 6);
            break;
        case 0xB9: // RES 7, C
            CPUResetBit(mRegBC.low, 7);
            break;

        case 0x82: // RES 0, D
            CPUResetBit(mRegDE.high, 0);
            break;
        case 0x8A: // RES 1, D
            CPUResetBit(mRegDE.high, 1);
            break;
        case 0x92: // RES 2, D
            CPUResetBit(mRegDE.high, 2);
            break;
        case 0x9A: // RES 3, D
            CPUResetBit(mRegDE.high, 3);
            break;
        case 0xA2: // RES 4, D
            CPUResetBit(mRegDE.high, 4);
            break;
        case 0xAA: // RES 5, D
            CPUResetBit(mRegDE.high, 5);
            break;
        case 0xB2: // RES 6, D
            CPUResetBit(mRegDE.high, 6);
            break;
        case 0xBA: // RES 7, D
            CPUResetBit(mRegDE.high, 7);
            break;
        
        case 0x83: // RES 0, E
            CPUResetBit(mRegDE.low, 0);
            break;
        case 0x8B: // RES 1, E
            CPUResetBit(mRegDE.low, 1);
            break;
        case 0x93: // RES 2, E
            CPUResetBit(mRegDE.low, 2);
            break;
        case 0x9B: // RES 3, E
            CPUResetBit(mRegDE.low, 3);
            break;
        case 0xA3: // RES 4, E
            CPUResetBit(mRegDE.low, 4);
            break;
        case 0xAB: // RES 5, E
            CPUResetBit(mRegDE.low, 5);
            break;
        case 0xB3: // RES 6, E
            CPUResetBit(mRegDE.low, 6);
            break;
        case 0xBB: // RES 7, E
            CPUResetBit(mRegDE.low, 7);
            break;

        case 0x84: // RES 0, H
            CPUResetBit(mRegHL.high, 0);
            break;
        case 0x8C: // RES 1, H
            CPUResetBit(mRegHL.high, 1);
            break;
        case 0x94: // RES 2, H
            CPUResetBit(mRegHL.high, 2);
            break;
        case 0x9C: // RES 3, H
            CPUResetBit(mRegHL.high, 3);
            break;
        case 0xA4: // RES 4, H
            CPUResetBit(mRegHL.high, 4);
            break;
        case 0xAC: // RES 5, H
            CPUResetBit(mRegHL.high, 5);
            break;
        case 0xB4: // RES 6, H
            CPUResetBit(mRegHL.high, 6);
            break;
        case 0xBC: // RES 7, H
            CPUResetBit(mRegHL.high, 7);
            break;

        case 0x85: // RES 0, L
            CPUResetBit(mRegHL.low, 0);
            break;
        case 0x8D: // RES 1, L
            CPUResetBit(mRegHL.low, 1);
            break;
        case 0x95: // RES 2, L
            CPUResetBit(mRegHL.low, 2);
            break;
        case 0x9D: // RES 3, L
            CPUResetBit(mRegHL.low, 3);
            break;
        case 0xA5: // RES 4, L
            CPUResetBit(mRegHL.low, 4);
            break;
        case 0xAD: // RES 5, L
            CPUResetBit(mRegHL.low, 5);
            break;
        case 0xB5: // RES 6, L
            CPUResetBit(mRegHL.low, 6);
            break;
        case 0xBD: // RES 7, L
            CPUResetBit(mRegHL.low, 7);
            break;

        case 0x86: // RES 0, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUResetBit(byte, 0);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0x8E: // RES 1, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUResetBit(byte, 1);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0x96: // RES 2, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUResetBit(byte, 2);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0x9E: // RES 3, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUResetBit(byte, 3);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0xA6: // RES 4, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUResetBit(byte, 4);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0xAE: // RES 5, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUResetBit(byte, 5);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0xB6: // RES 6, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUResetBit(byte, 6);
            WriteByte(mRegHL.val, byte);
            break;
        }
        case 0xBE: // RES 7, (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUResetBit(byte, 7);
            WriteByte(mRegHL.val, byte);
            break;
        }
        
        case 0x87: // RES 0, A
            CPUResetBit(mRegAF.high, 0);
            break;
        case 0x8F: // RES 1, A
            CPUResetBit(mRegAF.high, 1);
            break;
        case 0x97: // RES 2, A
            CPUResetBit(mRegAF.high, 2);
            break;
        case 0x9F: // RES 3, A
            CPUResetBit(mRegAF.high, 3);
            break;
        case 0xA7: // RES 4, A
            CPUResetBit(mRegAF.high, 4);
            break;
        case 0xAF: // RES 5, A
            CPUResetBit(mRegAF.high, 5);
            break;
        case 0xB7: // RES 6, A
            CPUResetBit(mRegAF.high, 6);
            break;
        case 0xBF: // RES 7, A
            CPUResetBit(mRegAF.high, 7);
            break;

        // SWAP
        case 0x30: // SWAP B
            CPUSwap(mRegBC.high);
            break;
        case 0x31: // SWAP C
            CPUSwap(mRegBC.low);
            break;
        case 0x32: // SWAP D
            CPUSwap(mRegDE.high);
            break;
        case 0x33: // SWAP E
            CPUSwap(mRegDE.low);
            break;
        case 0x34: // SWAP H
            CPUSwap(mRegHL.high);
            break;
        case 0x35: // SWAP L
            CPUSwap(mRegHL.low);
            break;
        case 0x36: // SWAP (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPUSwap(byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        case 0x37: // SWAP A
            CPUSwap(mRegAF.high);
            break;

        case 0x38: // SRL B
            CPU_SRL(mRegBC.high);
            break;
        case 0x39: // SRL C
            CPU_SRL(mRegBC.low);
            break;
        case 0x3A: // SRL D
            CPU_SRL(mRegDE.high);
            break;
        case 0x3B: // SRL E
            CPU_SRL(mRegDE.low);
            break;
        case 0x3C: // SRL H
            CPU_SRL(mRegHL.high);
            break;
        case 0x3D: // SRL L
            CPU_SRL(mRegHL.low);
            break;
        case 0x3F: // SRL A
            CPU_SRL(mRegAF.high);
            break;
        case 0x3E: // SRL (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_SRL(byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        
        case 0x10: // RL B
            CPU_RL(mRegBC.high);
            break;
        case 0x11: // RL C
            CPU_RL(mRegBC.low);
            break;
        case 0x12: // RL D
            CPU_RL(mRegDE.high);
            break; 
        case 0x13: // RL E
            CPU_RL(mRegDE.low);
            break;
        case 0x14: // RL H
            CPU_RL(mRegHL.high);
            break; 
        case 0x15: // RL  L
            CPU_RL(mRegHL.low);
            break;
        case 0x16: // RL (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_RL(byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        case 0x17: // RL A
            CPU_RL(mRegAF.high);
            break;

        case 0x18: // RR B
            CPU_RR(mRegBC.high);
            break;
        case 0x19: // RR C
            CPU_RR(mRegBC.low);
            break;
        case 0x1A: // RR D
            CPU_RR(mRegDE.high);
            break; 
        case 0x1B: // RR E
            CPU_RR(mRegDE.low);
            break;
        case 0x1C: // RR H
            CPU_RR(mRegHL.high);
            break; 
        case 0x1D: // RR  L
            CPU_RR(mRegHL.low);
            break;
        case 0x1E: // RR (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_RR(byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        case 0x1F: // RR A
            CPU_RR(mRegAF.high);
            break;

        case 0x20: // SLA B
            CPU_SLA(mRegBC.high);
            break;
        case 0x21: // SLA C
            CPU_SLA(mRegBC.low);
            break;
        case 0x22: // SLA D
            CPU_SLA(mRegDE.high);
            break;
        case 0x23: // SLA E
            CPU_SLA(mRegDE.low);
            break;
        case 0x24: // SLA H
            CPU_SLA(mRegHL.high);
            break;
        case 0x25: // SLA L
            CPU_SLA(mRegHL.low);
            break;
        case 0x26: // SLA (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_SLA(byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        case 0x27: // SLA A
            CPU_SLA(mRegAF.high);
            break;
        
        case 0x28: // SRA B
            CPU_SRA(mRegBC.high);
            break;
        case 0x29: // SRA C
            CPU_SRA(mRegBC.low);
            break;
        case 0x2A: // SRA D
            CPU_SRA(mRegDE.high);
            break;
        case 0x2B: // SRA E
            CPU_SRA(mRegDE.low);
            break;
        case 0x2C: // SRA H
            CPU_SRA(mRegHL.high);
            break;
        case 0x2D: // SRA L
            CPU_SRA(mRegHL.low);
            break;
        case 0x2E: // SRA (HL)
        {
            uint8_t byte = ReadByte(mRegHL.val);
            CPU_SRA(byte);
            WriteByte(mRegHL.val, byte);
            mCyclesDone += 8;
            break;
        }
        case 0x2F: // SRA A
            CPU_SRA(mRegAF.high);
            break;
        default:
            break;
    }
}

namespace Giffi
{

// Swap low and high nibbles of a register (8BIT)
void gbZ80::CPUSwap(uint8_t& reg)
{
    // Operation
    uint8_t old_high = (reg & 0xF0) >> 4;
    reg <<= 4;
    reg |= old_high;

    // Flags
    mRegAF.low = 0x00; // Reset all
    if (reg == 0x00)  // Set zero if result is zero
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
}

// Set zeroflag if wanted bit is zero.
void gbZ80::CPUGetBit(uint8_t reg, uint8_t bit)
{
    mRegAF.low |= 1 << gbFlag::HalfCarry;  // Set Half Carry flag
    mRegAF.low &= ~(1 << gbFlag::Substract); // Reset Substract flag
    mRegAF.low &= ~(1 << gbFlag::Zero);      // Reset Zero Flag

    if (!((reg >> bit) & 1)) // If wanted bit is zero
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
}

// Set wanted bit
void gbZ80::CPUSetBit(uint8_t& reg, uint8_t bit)
{
    reg |= 1 << bit;
}

// Set wanted bit
void gbZ80::CPUResetBit(uint8_t& reg, uint8_t bit)
{
    reg &= ~(1 << bit);
}

// SHIFTS

void gbZ80::CPU_RR(uint8_t& reg)
{
    uint8_t carry = (mRegAF.low >> gbFlag::Carry) & 0b1;
    bool will_carry = reg & 0b1;

    mRegAF.low = 0x00;
    reg >>= 1;
    reg |= (carry << 7);

    if (will_carry)
    {
        mRegAF.low |= 1 << gbFlag::Carry;
    }

    if (reg == 0U)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void gbZ80::CPU_RL(uint8_t& _reg)
{
    uint8_t carry = (mRegAF.low >> gbFlag::Carry) & 0b1;
    bool will_carry = (_reg >> 7) & 0b1;

    _reg <<= 1;
    _reg |= carry;

    mRegAF.low = 0x00;
    if (will_carry)
    {
        mRegAF.low |= 1 << gbFlag::Carry;
    }

    if (_reg == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void gbZ80::CPU_SLA(uint8_t& _reg)
{
    bool isMSBSet = (_reg >> 7) & 0b1;
    _reg <<= 1;
    mRegAF.low = 0x00;

    if (isMSBSet)
    {
        mRegAF.low |= 1 << gbFlag::Carry;
    }

    if (_reg == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void gbZ80::CPU_SRA(uint8_t& _reg)
{
    bool isLSBSet = (_reg >> 0) & 0b1;
    bool isMSBSet = (_reg >> 7) & 0b1;

    mRegAF.low = 0x00; // Reset Flags
    _reg >>= 1;

    if (isMSBSet)
    {
        _reg |= 1 << 7;
    }
    if (isLSBSet)
    {
        mRegAF.low |= 1 << gbFlag::Carry;
    }

    if (_reg == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void gbZ80::CPU_SRL(uint8_t& reg)
{
    bool isLSBSet = (reg >> 0) & 0b1;

    mRegAF.low = 0x00;
    reg >>= 1;

    if (isLSBSet)
    {
        mRegAF.low |= 1 << gbFlag::Carry;
    }
    if (reg == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void gbZ80::CPU_RLC(uint8_t& reg)
{
    bool will_carry = (reg >> 7) & 0b1;
    reg <<= 1;

    mRegAF.low = 0x00;
    if (will_carry)
    {
        mRegAF.low |= 1 << gbFlag::Carry;
        reg |= 1;
    }
    if (reg == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
}

void gbZ80::CPU_RRC(uint8_t& _reg)
{
    bool will_carry = _reg & 0b1;
    _reg >>= 1;

    mRegAF.low = 0x00;
    if (will_carry)
    {
        mRegAF.low |= 1 << gbFlag::Carry;
        _reg |= 1 << 7;
    }

    if (_reg == 0)
    {
        mRegAF.low |= 1 << gbFlag::Zero;
    }
}

}