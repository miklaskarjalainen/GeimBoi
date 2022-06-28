#include <cstring>
#include <cassert>
#include <time.h>
#include "../gbCart.hpp"
#include "gbMBC3.hpp"

using namespace GeimBoi;

gbMBC3::gbMBC3(gbCart* cart)
    : gbMBC(cart)
{
    Reset();
    printf("MBC3 Created\n");
}

gbMBC3::~gbMBC3()
{ 
    printf("MBC3 Destroyed\n"); 
}

bool gbMBC3::SaveBattery(const std::string& path)
{
    if (mCart->HasBattery())
    {
        return SaveBatteryImpl(mCart->GetGameName() + ".sav", (uint8_t*)&mRam, sizeof(mRam));
    }
    return false;
}

bool gbMBC3::LoadBattery(const std::string& path)
{
    if (mCart->HasBattery())
    {
        return LoadBatteryImpl(mCart->GetGameName() + ".sav", (uint8_t*)&mRam, sizeof(mRam));
    }
    return false;
}

uint8_t gbMBC3::ReadByte(uint16_t addr) const
{
    if (addr < 0x4000) // Cartridge
    {
        return mCart->mCart[addr];
    }
    else if (addr < 0x8000)   // Rom bank
    {
        uint8_t bank = mRomBank;
        bank &= mCart->GetRomBankCount() - 1;
        uint32_t new_addr = (addr - 0x4000) + (bank * 0x4000);
        return mCart->mCart[new_addr];
    }
    else if ((addr >= 0xA000) && (addr < 0xC000) && mRamEnable) // Ram banks
    {
        if (mRamBank <= 0b11)
        {
            uint16_t new_addr = addr - 0xA000;
            uint16_t bank = mRamBank;
            bank &= mCart->GetRamBankCount() - 1;
            return mRam[new_addr + (bank * 0x2000)];
        }
        else
        {
            switch(mRamBank & 0b1111)
            {
                case 0x8:
                    return mRtc.Sec;
                case 0x9:
                    return mRtc.Min;
                case 0xA:
                    return mRtc.Hour;
                case 0xB:
                    return mRtc.Day & 0xFF;
                case 0xC:
                {
                    uint8_t DH = 0;
                    DH |= ((mRtc.Day >> 8) & 0b1);
                    DH |= (mRtc.Halt << 6);
                    DH |= (mRtc.DayCarry << 7);
                    return DH;
                }
                default:
                    assert(false);
                    break;
            }
        }
    }
    else
    {
        return 0xFF;
    }
}

void gbMBC3::WriteByte(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000)     // Ram Enable
    {
        mRamEnable = (data & 0xF) == 0b1010;
    }
    else if (addr < 0x4000) // ROM Change
    {
        mRomBank = data & 0b01111111;
        if (mRomBank == 0) { mRomBank++; }
    }
    else if (addr < 0x6000) // RAM / RTS Reg change
    {
        if (data <= 0b1111)
            mRamBank = data;
    }
    else if (addr < 0x8000) // Latch Clock Data
    {
        // Latch if 0 and 1 were written back to back to this memory address..
        if (data == 1 && mRtc.LatchData == 0)
        {
            LatchRTC();
        }
        mRtc.LatchData = data;
    }
    else if ((addr >= 0xA000) && (addr < 0xC000)) // Ram Bank & RTC
    {
        if (!mRamEnable) { return; }

        if (mRamBank <= 0b11) // RAM
        {
            uint16_t new_addr = addr - 0xA000;
            uint16_t bank = mRamBank;
            bank &= mCart->GetRamBankCount() - 1;
            mRam[new_addr + (bank * 0x2000)] = data;
        }
        else if (mRtc.Halt)// RTC
        {
            switch(mRamBank & 0b1111)
            {
                case 0x8:
                    mRtc.Sec = data;
                    break;
                case 0x9:
                    mRtc.Min = data;
                    break;
                case 0xA:
                    mRtc.Hour = data;
                    break;
                case 0xB:
                    mRtc.Day = (mRtc.Day & (0b1 << 8)) | data;
                    break;
                case 0xC:
                {
                    // Most significant bit of Day Counter
                    mRtc.Day &= ~(0x100);
                    mRtc.Day |= ((data & 0b1) << 8);

                    mRtc.Halt = data & (0b1 << 6);
                    mRtc.DayCarry = data & (0b1 << 7);
                    break;
                }
                default:
                    assert(false);
                    break;
            }
        }
    }
}

void gbMBC3::Reset()
{
    mRamEnable = false;
    mRomBank = 0x01;
    mRamBank = 0x00;
    if (!mCart->HasBattery())
    {
        memset(&mRam, 0xFF, sizeof(mRam));
    }
    mRtc = Rtc();
}

void gbMBC3::LatchRTC()
{
    time_t now = time(NULL);
    time_t difference = now - mRtc.LastLatch;
    mRtc.LastLatch = now;

    if (!mRtc.Halt && difference > 0)
    {
        mRtc.Sec = static_cast<uint8_t>(difference % 60);
        if (mRtc.Sec > 59)
        {
            mRtc.Sec -= 60;
            mRtc.Min++;
        } 
        difference /= 60;

        mRtc.Min += static_cast<uint8_t>(difference % 60); 
        if (mRtc.Min > 59)
        {
            mRtc.Min -= 60;
            mRtc.Hour++;
        } 
        difference /= 60;

        mRtc.Hour += static_cast<uint8_t>(difference % 24);
        if (mRtc.Hour > 23)
        {
            mRtc.Hour -= 24;
            mRtc.Day++;
        } 
        difference /= 24;

        mRtc.Day += static_cast<uint16_t>(difference);
        /*
        ! After 511 days the daycarry should be set, the problem now is that upon startup the emulator resets mRtc.LastLatch to 0
        ! and the difference is years so the flag is always set on startup.
        if (mRtc.Day > 511) 
        {
            mRtc.Day %= 512;
            mRtc.DayCarry = true;
        }
        */
    }

}

uint16_t gbMBC3::GetCurRomBank() const
{
    return mRomBank & mCart->GetRomBankCount() - 1;
}

uint16_t gbMBC3::GetCurRamBank() const
{
    return mRamBank;
}
