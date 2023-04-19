#include <cstring>
#include <fstream>

#include "../gbCart.hpp"
#include "gbMBC1.hpp"

using namespace GeimBoi;

gbMBC1::gbMBC1(gbCart* cart)
    : gbMBC(cart)
{
    Reset();
    printf("MBC1 Created\n");
}

gbMBC1::~gbMBC1()
{
    printf("MBC1 Destroyed\n");
}

bool gbMBC1::WriteState(std::ofstream& wf)
{
    wf.write((char*)&mMode,      sizeof(mMode));
    wf.write((char*)&mRamEnable, sizeof(mRamEnable));
    wf.write((char*)&mCurBank,   sizeof(mCurBank));
    wf.write((char*)&mRam,       sizeof(mRam));
    return wf.bad();
}

bool gbMBC1::ReadState(std::ifstream& rf)
{
    rf.read((char*)&mMode,      sizeof(mMode));
    rf.read((char*)&mRamEnable, sizeof(mRamEnable));
    rf.read((char*)&mCurBank,   sizeof(mCurBank));
    rf.read((char*)&mRam,       sizeof(mRam));
    return rf.bad();
}

bool gbMBC1::SaveBatteryImpl(std::ofstream& wf)
{
    if (mCart->HasBattery())
    {
        wf.write((char*)&mRam, sizeof(mRam));
        return !wf.bad();
    }
    return false;
}

bool gbMBC1::LoadBatteryImpl(std::ifstream& rf)
{
    if (mCart->HasBattery())
    {
        rf.read((char*)&mRam, sizeof(mRam));
        return !rf.bad();
    }
    return false;
}

uint8_t gbMBC1::ReadByte(uint16_t addr) const      
{
    if (addr < 0x4000) // Cartridge
    {
        
        if (!mMode) { return mCart->mCart[addr]; }
        uint8_t bank = mCurBank & (0b11 << 5);
        bank &= mCart->GetRomBankCount() - 1;
        uint32_t new_addr = (0x4000 * bank) + addr;
        return mCart->mCart[new_addr];
    }
    else if (addr < 0x8000)   // Rom bank
    {
        uint8_t bank = mCurBank & 0b01111111;
        bank &= mCart->GetRomBankCount() - 1;
        uint32_t new_addr = (addr - 0x4000) + (bank * 0x4000);
        return mCart->mCart[new_addr];
    }
    else if ((addr >= 0xA000) && (addr < 0xC000) && mRamEnable) // Ram banks
    {
        uint16_t new_addr = addr - 0xA000;
        uint16_t bank = mMode ? ((mCurBank >> 5) & 0b11) : 0;
        bank &= mCart->GetRamBankCount() - 1;
        return mRam[(new_addr + (bank * 0x2000))];
    }
    else
    {
        return 0xFF;
    }
}

void gbMBC1::WriteByte(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000)     // Ram Enable
    {
        if (mCart->GetRamSize() <= 0) { return; }
        mRamEnable = (data & 0xF) == 0b1010;
    }
    else if (addr < 0x4000) // ROM Change
    {
        uint8_t bank1 = data & 0x1F;
        if (bank1 == 0) { bank1++; }
        mCurBank &= (0b11 << 5);
        mCurBank |= bank1;
    }
    else if (addr < 0x6000) // Set Bank2 (RAM/ROM)
    {
        uint8_t bank2 = data & 0b11;
        mCurBank &= ~(0b11 << 5);
        mCurBank |= (bank2 << 5);
    }
    else if (addr < 0x8000) // Set RAM/ROM Mode
    {
        mMode = (data & 0b1);
    }
    else if ((addr >= 0xA000) && (addr < 0xC000)) // Ram Bank
    {
        if (!mRamEnable) { return; }
        uint16_t new_addr = addr - 0xA000;
        uint16_t bank = mMode ? ((mCurBank >> 5) & 0b11) : 0;
        bank &= mCart->GetRamBankCount() - 1;
        mRam[new_addr + (bank * 0x2000)] = data;
    }
}

void gbMBC1::Reset()
{
    mCurBank = 0x01;
    mRamEnable = false;
    mMode = false;
    if (!mCart->HasBattery())
    {
        memset(&mRam, 0xFF, sizeof(mRam));
    }
}

uint16_t gbMBC1::GetCurRomBank() const
{
    return mCurBank & (mCart->GetRomBankCount() - 1);
}

uint16_t gbMBC1::GetCurRamBank() const
{
    return 0;
}
