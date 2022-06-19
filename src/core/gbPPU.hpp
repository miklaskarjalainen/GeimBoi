#pragma once
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <array>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

namespace GeimBoi
{
class gbGameBoy;
class gbZ80;

struct gbColor
{
    gbColor(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0)
        :r(r), g(g), b(b) {}
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
        frontBuffer = reinterpret_cast<gbBuffer*>(new gbBuffer);
        if (frontBuffer == nullptr) {
            printf("could not allocate memory for frontBuffer! (malloc returned nullptr)\n");
            exit(-1);
        }
        memset(frontBuffer, 0, sizeof(gbBuffer));

        dmgPalette[0] = { 217,217,217 };
        dmgPalette[1] = { 128,128,128 };
        dmgPalette[2] = {  97, 97, 97 };
        dmgPalette[3] = {  12, 12, 12 };
    };
    ~gbPPU()
    {
        delete[] frontBuffer;
    }
public:
    using gbBuffer = gbColor[SCREEN_HEIGHT * SCREEN_WIDTH];

    std::array<gbColor, 4> dmgPalette;
    gbBuffer* frontBuffer = nullptr;

    void Reset();
    void CheckCoinsidenceFlag(); // Aka LY=LYC

    enum ppuMode : uint8_t {
        HBLANK = 0b00,
        VBLANK = 0b01,
        OAM_SEARCH = 0b10,
        TRANSFERRING_DATA = 0b11,
    };

    uint8_t GetLY()   const;
    uint8_t GetLYC()  const;
    uint8_t GetSTAT() const;
    uint8_t GetLCDC() const;
    ppuMode GetMode() const;
    
    bool LCD_Enable()         const;
    bool LCD_WindowEnable()   const;
    bool LCD_ObjEnable()      const;
    bool LCD_BgWindowEnable() const;

private:
    // Color ids used by the gameboy.
    enum gbColorId 
    {
        WHITE = 0b00,
        LIGHTGRAY = 0b01,
        DARKGRAY = 0b10,
        BLACK = 0b11,
    };

    gbColorId GetPixelColor(uint8_t col, uint16_t addr);

    void UpdateGraphics(uint16_t cycles);
    void SetLCDStatus    ();
    void RenderScanline  ();
    void RenderBackground();
    void RenderSprites	 ();
    
private:
    int  mScanlineCounter = 456; // 1 scanline is drawn every 456 cpu cycles, this keeps track of it.
    gbGameBoy* mGameBoy = nullptr;

    friend gbZ80;
};


}
