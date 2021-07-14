#include "gbCart.hpp"
#include <cstdio>   // fopen
#include <string.h>

namespace Giffi
{   

std::string gbCart::GetGameName()
{
    char name[12] = "";
    memcpy( &name, mCart + 0x134, sizeof(char) * 11 );
    return std::string(name);
}

// 1 rom bank is 16kb
uint16_t gbCart::GetRomBankCount()
{
    return mCart[0x148] * 4;
}

// In Kb (KiloBytes), 1 ram bank is 8kb
uint16_t gbCart::GetRamSize() 
{
    switch (mCart[0x149])
    {
        case 0x1: return 2;
        case 0x2: return 8;
        case 0x3: return 32;
        case 0x4: return 128;
        case 0x5: return 256;
        default: return 0;
    }
}

gbCartType gbCart::GetCartType()
{
    switch (mCart[0x147])
    {
        case 0x01: // MBC1
        case 0x02: // MBC1+RAM
        case 0x03: // MBC1+RAM+BATTERY
            return gbCartType::MBC1;

        case 0x04: // MBC2
        case 0x05: // MBC2+BATTERY
            return gbCartType::MBC2;
        
        //case 0x0F: // MBC3+TIMER+BATTERY
        //case 0x10: // MBC3+TIMER+BATTERY+RAM
        case 0x11: // MBC3
        case 0x12: // MBC3+RAM
        case 0x13: // MBC3+RAM+BATTERY
            return gbCartType::MBC3;

        default: // None or unsupported
            return gbCartType::None;
    } 
}

void gbCart::Reset()
{
    mCurRomBank = 1;
    mCurRamBank = 0;
    memset(&mRam, 0xFF, sizeof(mRam));
    mRamEnable = false;
    mMBC = GetCartType();
}

void gbCart::LoadRom(std::string _path)
{
    // Init Cart
    memset(mCart,0,sizeof(mCart));
    FILE *in;
    in = fopen( _path.c_str(), "rb" );
    fread(mCart, 1, 0x200000, in);
    fclose(in); 
    
    // Reset cart variables to defaults
    Reset();
}

uint8_t gbCart::ReadByte(uint16_t _addr)
{
    if ( _addr < 0x4000) // Cartridge
    {
        return mCart[_addr];
    }
    else if ((_addr>=0x4000) && (_addr < 0x8000))   // Rom bank
    {
        uint16_t new_addr = (_addr - 0x4000) + (0x4000 * mCurRomBank);
        return mCart[new_addr];
    }
    else if ((_addr >= 0xA000) && (_addr < 0xC000)) // Ram banks
    {
        uint16_t new_addr = _addr - 0xA000;
        return mRam[new_addr + (mCurRamBank*0x2000)]; 
    }
    else
    {
        return 0xFF;
    }
}

void gbCart::WriteByte(uint16_t _addr, uint8_t _data)
{
    switch (mMBC)
    {
        case gbCartType::MBC1:
        {
            if ( _addr < 0x2000)                         // Ram Enable
            {
                mRamEnable = _data > 0;
            }
            else if (_addr < 0x4000) // ROM Change
            {
                mCurRomBank &= 224;
                mCurRomBank |= _data & 0x1F;
                if (mCurRomBank == 0x00 || mCurRomBank == 0x20 || // Cant be any of these.
                    mCurRomBank == 0x40 || mCurRomBank == 0x60) { mCurRomBank++; }
            }
            else if (_addr < 0x6000) // Set upper bits of rom bank number
            {
                if (mRomMode)
                {
                    mCurRomBank &= ~(0b11 << 5);
                    mCurRomBank |= (_data & 3) << 5;
                }
                else
                {
                    mCurRamBank = _data & 3;
                }
            }
            else if (_addr < 0x8000) // Ram / Rom Change
            {
                mRomMode = _data > 0;
            }
            else if ( ( _addr >= 0xA000 ) && ( _addr < 0xC000 ) ) // Ram Bank
            {
                if (!mRamEnable) { return; }
                uint16_t new_addr = _addr - 0xA000;
                mRam[new_addr + (mCurRamBank * 0x2000)] = _data;
            }
            break;
        }
        case gbCartType::MBC2:
        {
            if (_addr < 0x2000)
            {
                mRamEnable = _data > 0;
            }
            else if (_addr < 0x4000)
            {
                mCurRomBank = _data & 0x1F;
                if (mCurRomBank == 0x00 || mCurRomBank == 0x20 || // Cant be any of these.
                    mCurRomBank == 0x40 || mCurRomBank == 0x60) { mCurRomBank++; }
            }
            break;
        }
        default: break;
    }
    
}



} // Namespace 
