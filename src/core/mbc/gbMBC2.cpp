#include <cstring>
#include <fstream>

#include "../gbCart.hpp"
#include "gbMBC2.hpp"

using namespace GeimBoi;

gbMBC2::gbMBC2(gbCart* cart)
    : gbMBC(cart)
{
    Reset();
    printf("MBC2 Created\n");
}

gbMBC2::~gbMBC2()
{
    printf("MBC2 Destroyed\n");
}

bool gbMBC2::SaveBatteryImpl(std::ofstream& wf)
{
    if (mCart->HasBattery())
    {
        wf.write((char*)&mRam, sizeof(mRam));
        wf.close();
        return !wf.bad();
    }
    return false;
}

bool gbMBC2::LoadBatteryImpl(std::ifstream& rf)
{
    if (mCart->HasBattery())
    {
        rf.read((char*)&mRam, sizeof(mRam));
        rf.close();
        return !rf.bad();
    }
    return false;
}

uint8_t gbMBC2::ReadByte(uint16_t addr) const      
{
    if      (addr < 0x4000) // Static Rom
    {
        return mCart->mCart[addr];
    }
    else if (addr < 0x8000) // Banked Rom
    {
        uint32_t new_addr = (addr - 0x4000) + (mCurBank * 0x4000);
        return mCart->mCart[new_addr];
    }
    else if (addr >= 0xA000 && addr < 0xC000 && mRamEnable) // RAM
    {
        return mRam[addr % 0x200];
    }

    return 0xFF;
}

void gbMBC2::WriteByte(uint16_t addr, uint8_t data)
{
    if (addr < 0x4000) // ROM Change
    {
        data &= 0xF;
        
        if (((addr >>  8) & 1)) // ROMB
        {
            if (data == 0x00) { data++; } // can't be 0
            mCurBank = data;
            mCurBank &= mCart->GetRomBankCount() - 1;
        }
        else                    // RAMG
        {
            mRamEnable = data == 0b1010;
        }
    }
    else if ((addr >= 0xA000) && (addr < 0xC000)) // Ram
    {
        if (!mRamEnable) { return; }
        mRam[addr % 0x200] = 0xF0 | (data & 0xF); // Unused bits are high (bits: 7-4)
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
    return mCurBank & (mCart->GetRomBankCount() - 1);
}
