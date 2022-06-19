#include "../gbCart.hpp"
#include "gbMBC2.hpp"

using namespace GeimBoi;

gbMBC2::gbMBC2(gbCart* _cart)
    : gbMBC(_cart)
{
    Reset();
    printf("MBC2 Created\n");
}

gbMBC2::~gbMBC2()
{
    printf("MBC2 Destroyed\n");
}

bool gbMBC2::SaveBattery(const std::string& _path)
{
    if (mCart->HasBattery())
    {
        return SaveBatteryImpl(mCart->GetGameName() + ".sav", (uint8_t*)&mRam, sizeof(mRam));
    }
    return false;
}

bool gbMBC2::LoadBattery(const std::string& _path)
{
    if (mCart->HasBattery())
    {
        return LoadBatteryImpl(mCart->GetGameName() + ".sav", (uint8_t*)&mRam, sizeof(mRam));
    }
    return false;
}

uint8_t gbMBC2::ReadByte(uint16_t _addr) const      
{
    if      (_addr < 0x4000) // Static Rom
    {
        return mCart->mCart[_addr];
    }
    else if (_addr < 0x8000) // Banked Rom
    {
        uint32_t new_addr = (_addr - 0x4000) + (mCurBank * 0x4000);
        return mCart->mCart[new_addr];
    }
    else if (_addr >= 0xA000 && _addr < 0xC000 && mRamEnable) // RAM
    {
        return mRam[_addr % 0x200];
    }

    return 0xFF;
}

void gbMBC2::WriteByte(uint16_t _addr, uint8_t _data)
{
    if (_addr < 0x4000) // ROM Change
    {
        _data &= 0xF;
        
        if (((_addr >>  8) & 1)) // ROMB
        {
            if (_data == 0x00) { _data++; } // can't be 0
            mCurBank = _data;
            mCurBank &= mCart->GetRomBankCount() - 1;
        }
        else                    // RAMG
        {
            mRamEnable = _data == 0b1010;
        }
    }
    else if ((_addr >= 0xA000) && (_addr < 0xC000)) // Ram
    {
        if (!mRamEnable) { return; }
        mRam[_addr % 0x200] = 0xF0 | (_data & 0xF); // Unused bits are high (bits: 7-4)
    }
}

void gbMBC2::Reset()
{
    mCurBank = 0x01;
    mRamEnable = false;
    if (!mCart->HasBattery())
    {
        memset(&mRam, 0xFF, sizeof(mRam));
    }
}

uint16_t gbMBC2::GetCurRomBank() const
{
    return mCurBank & mCart->GetRomBankCount() - 1;
}
