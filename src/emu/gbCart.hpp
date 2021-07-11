#pragma once
#include <cstdint>
#include <string>

namespace Giffi
{

    enum CartType
    {
        None = 0,
        MBC1 = 1,
        MBC2 = 2,
    };

    class gbGameBoy;
    class gbCart
    {
    public:
        gbCart()  = default;
        ~gbCart() = default;

        std::string GetGameName();
    private:
        void Reset();
        void LoadRom(std::string _path);
        uint8_t& ReadByte(uint16_t _addr);
        void     WriteByte(uint16_t _addr, uint8_t _data);
    
    private:
        CartType mMBC = CartType::None;
        uint8_t  mCart[0x200000]; 
        uint8_t  mRam[0x8000];
        uint8_t  mCurRamBank = 0;
        uint16_t mCurRomBank = 1;

        friend gbGameBoy;
    };


}

