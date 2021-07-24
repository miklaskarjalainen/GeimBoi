#include "../gbCart.hpp"
#include "gbMBC3.hpp"

using namespace Giffi;

gbMBC3::gbMBC3(gbCart* _cart)
    : gbMBC(_cart)
{
    Reset();
    printf("MBC3 Created\n");
    if (mCart->HasBattery())
    {
        LoadRam(mCart->GetGameName() + ".sav", (uint8_t*)&mRam, sizeof(mRam));
    }
}

gbMBC3::~gbMBC3()
{ 
    printf("MBC3 Destroyed\n"); 
    if (mCart->HasBattery())
    {
        SaveRam(mCart->GetGameName() + ".sav", (uint8_t*)&mRam, sizeof(mRam));
    }
}

uint8_t gbMBC3::ReadByte(uint16_t _addr) const
{
    if (_addr < 0x4000) // Cartridge
    {
        return mCart->mCart[_addr];
    }
    else if (_addr < 0x8000)   // Rom bank
    {
        uint8_t bank = mRomBank;
        bank &= mCart->GetRomBankCount() - 1;
        uint32_t new_addr = (_addr - 0x4000) + (bank * 0x4000);
        return mCart->mCart[new_addr];
    }
    else if ((_addr >= 0xA000) && (_addr < 0xC000) && mRamEnable) // Ram banks
    {
        uint16_t new_addr = _addr - 0xA000;
        uint16_t bank = mRamBank;
        bank &= mCart->GetRamBankCount() - 1;
        return mRam[new_addr + (bank * 0x2000)];
    }
    else
    {
        return 0xFF;
    }
}

void gbMBC3::WriteByte(uint16_t _addr, uint8_t _data)
{
    if (_addr < 0x2000)     // Ram Enable
    {
        mRamEnable = (_data & 0xF) == 0b1010;
    }
    else if (_addr < 0x4000) // ROM Change
    {
        mRomBank = _data & 0b01111111;
        if (mRomBank == 0) { mRomBank++; }
    }
    else if (_addr < 0x6000) // RAM / RTS Reg change
    {
        _data &= 0b11;
        mRamBank = _data;
    }
    else if (_addr < 0x8000) // Set RAM/ROM Mode
    {
        mMode = (_data & 0b1);
    }
    else if ((_addr >= 0xA000) && (_addr < 0xC000)) // Ram Bank
    {
        if (!mRamEnable) { return; }
        uint16_t new_addr = _addr - 0xA000;
        uint16_t bank = mRamBank;
        bank &= mCart->GetRamBankCount() - 1;
        mRam[new_addr + (bank * 0x2000)] = _data;
    }
}

void gbMBC3::Reset()
{
    mRomBank = 0x01;
    mRamEnable = false;
    mMode = false;
    memset(&mRam, 0xFF, sizeof(mRam));
}

uint16_t gbMBC3::GetCurRomBank() const
{
    return mRomBank & mCart->GetRomBankCount() - 1;
}

uint16_t gbMBC3::GetCurRamBank() const
{
    return 0;
}
