#pragma once 
#include <cstdint>
#include "gbZ80.hpp"
#include "gbPPU.hpp"
#include "gbCart.hpp"
#include "gbBootRom.hpp"

#include "opcode/decompiler.hpp"

#include "reg.hpp"

// Include the emulator //

namespace Giffi
{

    enum gbButton : uint8_t
    {
        RIGHT = 0,
        LEFT  = 1,
        UP    = 2,
        DOWN  = 3,
        A     = 4,
        B     = 5,
        SELECT= 6,
        START = 7,
    };


    class gbGameBoy
    {
    public:
        gbGameBoy();
        ~gbGameBoy() = default;

        void Clock();
        void FrameAdvance();
        void Reset();
        bool LoadRom(const std::string& _path);

        void SetPalette   (gbColor _white, gbColor _lgray, gbColor _dgray, gbColor _black);
        void PressButton  (gbButton _key);
        void ReleaseButton(gbButton _key);

        uint8_t  ReadByte(uint16_t _addr) const;
        uint16_t ReadWord() const;
    public:
        gbPPU  mPpu;
        gbZ80  mCpu;
        gbCart mCart;
        gbBootRom mBootRom;

    private:
        void WriteByte(uint16_t _addr, uint8_t _data);

    private:
        uint8_t mRom[0x10000];
        uint8_t mBtsPressed = 0x00; // 0 = pressed

        friend gbZ80;
        friend gbPPU;
        friend gbCart;
    };



} // Namespace
