#pragma once 
#include <cstdint>
#include "gbZ80.hpp"
#include "gbPPU.hpp"
#include "gbCart.hpp"
#include "gbBootRom.hpp"

#include "opcode/debugger.hpp"

#include "reg.hpp"

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

        void SetPalette   (gbColor white, gbColor lgray, gbColor dgray, gbColor black);
        void PressButton  (gbButton key);
        void ReleaseButton(gbButton key);

        uint8_t  ReadByte(uint16_t addr) const;
        uint16_t ReadWord() const;
        uint16_t ReadWord(uint16_t addr) const;

        inline bool LoadRom(const std::string& path) {
            return mCart.LoadRom(path);
        }
        inline bool LoadBios(const std::string& path) {
            return mBootRom.LoadBios(path);
        }
        inline bool IsBiosLoaded() {
            return mBootRom.IsBiosLoaded();
        }

    public:
        gbPPU  mPpu;
        gbZ80  mCpu;
        gbCart mCart;
        gbBootRom mBootRom;

    private:
        void WriteByte(uint16_t addr, uint8_t data);

    private:
        uint8_t mRom[0x10000];
        uint8_t mBtsPressed = 0x00; // 0 = pressed

        friend gbZ80;
        friend gbPPU;
        friend gbCart;
    };



}
