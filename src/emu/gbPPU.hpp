#pragma once
#include <cstdint>
#include <raylib.h>

namespace Giffi
{

enum gbColor
{
    White     = 0b00, // Not capitalized because, they're are reserved by raylib.
    LightGray = 0b01,
    DarkGray  = 0b10,
    Black     = 0b11,
};

class gbGameBoy;
class gbZ80;

class gbPPU
{
public:
    gbPPU(gbGameBoy* _gameboy)
        : mGameBoy(_gameboy)
    {
        back_buffer = new Image;
        *back_buffer = GenImageColor(160, 144, WHITE);

        front_buffer = new Image;
        *front_buffer = GenImageColor(160, 144, WHITE);
    };
    ~gbPPU() { delete front_buffer; delete back_buffer; }
public:
    Image* front_buffer = nullptr;

private:
    Image* back_buffer  = nullptr;
    void SwapBuffers() // Removing all unnecessary screen tearing, so i actually know what is caused by the emulation
    {
        Image* hold = back_buffer;
        back_buffer = front_buffer;
        front_buffer = hold;
    }

    gbColor GetPixelColor(uint8_t col, uint16_t _addr);
    void    UpdateGraphics(uint16_t _cycles);
    void    SetLCDStatus    ();
    void    RenderScanline  ();
    void	RenderBackground();
    void	RenderSprites	();

private:
    int mScanlineCounter = 456; // 1 scanline is drawn every 456 cpu cycles, this keeps track of it.
    gbGameBoy* mGameBoy = nullptr;

    friend gbZ80;
};


}
