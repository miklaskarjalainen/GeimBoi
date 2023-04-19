#pragma once
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <array>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

namespace GeimBoi
{
class gbGameBoy;
class gbZ80;

struct gbColor
{
    gbColor(const gbColor& other)
        :r(other.r), g(other.g), b(other.b) {}
    //constexpr gbColor(const gbColor& other)
    //    :r(other.r), g(other.g), b(other.b) {}
    gbColor(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0)
        :r(r), g(g), b(b) {}
    gbColor(const int hex_code)
        :r((hex_code >> 16) & 0xFF), g((hex_code >> 8) & 0xFF), b(hex_code & 0xFF) {}
    uint8_t r, g, b;

    constexpr gbColor& operator=(const gbColor& other)
    {
        r = other.r; g = other.g; b = other.b;
        return *this;
    }
};

class gbPPU
{
public:
    gbPPU(gbGameBoy* _gameboy);
    ~gbPPU();
    void Reset();

    void WriteState(std::ofstream& file);
    void ReadState(std::ifstream& file);
    using gbBuffer = gbColor[SCREEN_HEIGHT * SCREEN_WIDTH];
    
public:
    std::array<gbColor, 4> dmgPalette;
    gbBuffer* frontBuffer = nullptr;

    void CheckCoinsidenceFlag(); // Aka LY=LYC

    enum ppuMode : uint8_t {
        HBLANK = 0b00,
        VBLANK = 0b01,
        OAM_SEARCH = 0b10,
        TRANSFERRING_DATA = 0b11,
    };

    uint8_t& GetLY()   const;
    uint8_t& GetLYC()  const;
    uint8_t& GetSTAT() const;
    uint8_t& GetLCDC() const;
    ppuMode GetMode() const;
    
    bool LCD_Enable()         const;
    bool LCD_WindowEnable()   const;
    bool LCD_ObjEnable()      const;
    bool LCD_BgWindowEnable() const;

private:
    enum gbColorId 
    {
        WHITE = 0b00,
        LIGHTGRAY = 0b01,
        DARKGRAY = 0b10,
        BLACK = 0b11,
    };
    gbColorId mFifo[SCREEN_WIDTH]; // FIXME: kinda hackky, resolves bg and sprite priority.

    gbColorId GetPixelColor(uint8_t col, uint16_t addr);
    struct OamEntry {
        uint8_t pos_y;
        uint8_t pos_x;
        uint8_t tile_index;
        struct {
            bool bg_priority;
            bool y_flip;
            bool x_flip;
            bool dmg_palette;
        } flags;
    };
    OamEntry GetOamForSprite(int sprite_idx) const;

    void UpdateGraphics(uint16_t cycles);
    void SetLCDStatus    ();
    void RenderScanline  ();
    void RenderBackground();
    void RenderWindow    ();
    void RenderSprites	 ();
    
private:
    int  mScanlineCounter = 456; // 1 scanline is drawn every 456 cpu cycles, this keeps track of it.
    gbGameBoy* mGameBoy = nullptr;

    friend gbZ80;
};


}
