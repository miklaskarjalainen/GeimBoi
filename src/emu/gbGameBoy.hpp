#pragma once 
#include <cstdint>
#include <raylib.h>
#include "gbZ80.hpp"
#include "gbPPU.hpp"
#include "gbCart.hpp"
#include "reg.hpp"

// Include the emulator //

namespace Giffi
{
enum gbButton
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

    void Run  ();
    void Reset();
    void LoadRom(const char* _path) { mCart.LoadRom(_path); };

    void PressButton  (gbButton _key);
    void ReleaseButton(gbButton _key);
private:
    uint8_t  ReadByte(uint16_t _addr);
    uint16_t ReadWord();
    void WriteByte(uint16_t _addr, uint8_t _data);

private:
    uint8_t mBtsPressed = 0xFF; // 0 = pressed
    uint8_t mRom[0x10000];
    gbZ80  mCpu;
    gbPPU  mPpu;
    gbCart mCart;

    friend gbZ80;
    friend gbPPU;
    friend gbCart;
};



}

