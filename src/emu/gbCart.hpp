#pragma once
#include <cstdint>
#include <string>

namespace Giffi
{

enum gbCartType // meaby turn this into a class instead? Probably would clean up these else ifs and switches a bit.
{
    None = 0,
    MBC1 = 1,
    MBC2 = 2,
    MBC3 = 3,
};

class gbGameBoy;
class gbCart
{
public:
    gbCart()  = default;
    ~gbCart() = default;

    std::string GetGameName();
    uint16_t    GetRomBankCount();
    uint16_t    GetRamSize();
    gbCartType  GetCartType();
private:
    void Reset();
    void LoadRom(std::string _path);
    uint8_t ReadByte(uint16_t _addr);
    void    WriteByte(uint16_t _addr, uint8_t _data);
private:
    gbCartType mMBC = gbCartType::None;
    uint8_t  mCart[0x200000]; 
    uint8_t  mRam[0x8000];
    uint16_t  mCurRomBank = 1;
    uint8_t  mCurRamBank = 0;
    bool     mRamEnable = false;
    bool     mRomMode   = true;

    friend gbGameBoy;
};


}

