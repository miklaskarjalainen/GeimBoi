#pragma once
#include <cstdint>

namespace Giffi
{
class gbGameBoy;
class gbZ80;

struct gbColor
{
    gbColor(uint8_t _r = 0, uint8_t _g = 0, uint8_t _b = 0)
        :r(_r), g(_g), b(_b) {}
    gbColor(const gbColor& other)
        :r(other.r), g(other.g), b(other.b) {}
    gbColor(const int hex_code)
        :r((hex_code >> 16) & 0xFF), g((hex_code >> 8) & 0xFF), b(hex_code & 0xFF) {}
    uint8_t r, g, b;
};

class gbPPU
{
public:
    gbPPU(gbGameBoy* _gameboy)
        : mGameBoy(_gameboy)
    {
        dmgPalette[0] = { 217,217,217 };
        dmgPalette[1] = { 128,128,128 };
        dmgPalette[2] = {  97, 97, 97 };
        dmgPalette[3] = {  12, 12, 12 };
    };
    ~gbPPU() = default;
public:
    gbColor dmgPalette[4];
    gbColor front_buffer[144][160];

    void Reset();
    void CheckCoinsidenceFlag(); // Aka LY=LYC
private:
    enum gbColorId // Color ids used by the gameboy.
    {
        WHITE = 0b00,
        LIGHTGRAY = 0b01,
        DARKGRAY = 0b10,
        BLACK = 0b11,
    };

    gbColorId GetPixelColor(uint8_t col, uint16_t _addr);

    void UpdateGraphics(uint16_t _cycles);
    void SetLCDStatus    ();
    void RenderScanline  ();
    void RenderBackground();
    void RenderSprites	 ();
    
private:
    int  mScanlineCounter = 456;// 1 scanline is drawn every 456 cpu cycles, this keeps track of it.
    gbGameBoy* mGameBoy = nullptr;

    friend gbZ80;
};


}
