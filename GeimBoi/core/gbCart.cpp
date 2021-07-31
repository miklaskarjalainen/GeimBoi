#include <string.h>
#include <filesystem>
#include <fstream>
#include "gbCart.hpp"

using namespace Giffi;

std::string gbCart::GetGameName() const
{
    char name[12] = "";
    memcpy( &name, mCart + 0x134, sizeof(char) * 11 );
    return std::string(name);
}

uint8_t gbCart::GetGameVersion() const
{
    return mCart[0x14C];
}

// In Kb (KiloBytes), 1 rom bank is 16kb
uint16_t gbCart::GetRomSize() const
{
    return (32 << mCart[0x148]);
}

uint16_t gbCart::GetCurRomBank() const
{
    return mMBC->GetCurRomBank();
}

// 1 rom bank is 16kb mCart[0x148] * 4
uint16_t gbCart::GetRomBankCount() const
{
    return GetRomSize() / 16;
}

// In Kb (KiloBytes), 1 ram bank is 8kb
uint16_t gbCart::GetRamSize() const
{
    switch (mCart[0x149])
    {
        case 0x1: return 2;
        case 0x2: return 8;
        case 0x3: return 64;
        case 0x4: return 256;
        case 0x5: return 512;
        default: return 0;
    }
}

uint16_t gbCart::GetRamBankCount() const
{
    uint16_t ram = GetRamSize();
    if (ram == 2) { return 1; }
    return ram / 8;
}

uint16_t gbCart::GetCurRamBank() const
{
    return mMBC->GetCurRamBank();
}

gbCartType gbCart::GetCartType() const
{
    switch (mCart[0x147])
    {
        case 0x01: // MBC1
        case 0x02: // MBC1+RAM
        case 0x03: // MBC1+RAM+BATTERY
            return gbCartType::MBC1;

        case 0x04: // MBC2
        case 0x05: // MBC2+BATTERY
        case 0x06: // MBC2+RAM+BATTERY
            return gbCartType::MBC2;
        
        case 0x09: // MBC3+TIMER+BATTERY
        case 0x10: // MBC3+TIMER+RAM+BATTERY
        case 0x11: // MBC3
        case 0x12: // MBC3+RAM
        case 0x13: // MBC3+RAM+BATTERY
            return gbCartType::MBC3;

        default: // None or unsupported
            return gbCartType::None;
    } 
}

std::string gbCart::GetCartTypeText() const
{
    switch ( GetCartType() )
    {
        case gbCartType::None:
            return "NONE";
        case gbCartType::MBC1:
            return "MBC1";
        case gbCartType::MBC2:
            return "MBC2";
        case gbCartType::MBC3:
            return "MBC3";
        default:
            return "Unknown";
    }
}

bool gbCart::IsGameLoaded() const
{
    return mGameLoaded;
}

bool gbCart::HasBattery() const
{
    switch (mCart[0x147])
    {
        case 0x03:
        case 0x06:
        case 0x09:
        case 0x0D:
        case 0x0F:
        case 0x10:
        case 0x13:
            return true;
        default:
            return false;
    }
}

void gbCart::Reset()
{
    if (mMBC != nullptr) { mMBC->Reset(); }
}

bool gbCart::LoadRom(const std::string& _path)
{
    if (!std::filesystem::exists(_path))
    {
        printf("Can't find a file at %s\n", _path.c_str());
        mGameLoaded = false;
        return false;
    }

    // Load Rom
    std::ifstream rf(_path, std::ios::binary);
    rf.read((char*)mCart, sizeof(mCart));
    if (rf.bad())
    {    
        printf("An error occurred when trying to read a romfile at %s!\n", _path.c_str());
        rf.close();
        mGameLoaded = false;
        return false;
    }
    rf.close();
    printf("Rom %s loaded\n", _path.c_str());

    // Get MBC
    mMBC = std::unique_ptr<gbMBC>(gbMBC::CreateMBC(this));

    mGameLoaded = true;
    return true;
}

uint8_t gbCart::ReadByte(uint16_t _addr) const
{
    return mMBC->ReadByte(_addr);
}

void gbCart::WriteByte(uint16_t _addr, uint8_t _data)
{
    mMBC->WriteByte(_addr, _data);
}
