#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include "mbc/gbMBC.hpp"

namespace Giffi
{

class gbGameBoy;
class gbCart
{
public:
    gbCart()  = default;
    ~gbCart() = default;

    std::string GetGameName() const;
    uint8_t     GetGameVersion() const;
    uint16_t    GetRomSize() const;
    uint16_t    GetCurRomBank() const;
    uint16_t    GetRomBankCount() const;
    uint16_t    GetRamSize() const;
    uint16_t    GetRamBankCount() const;
    uint16_t    GetCurRamBank() const;
    gbCartType  GetCartType() const;
    std::string GetCartTypeText() const;
    uint8_t     ReadByte(uint16_t _addr) const;
    bool        IsGameLoaded() const;
private:
    void Reset();
    void LoadRom(const std::string& _path);
    void WriteByte(uint16_t _addr, uint8_t _data);
private:
    std::unique_ptr<gbMBC> mMBC = nullptr;
public:
    uint8_t mCart[0x200000]; 
    bool mGameLoaded = false;

    friend gbGameBoy;
    friend gbMBC;
};


}

