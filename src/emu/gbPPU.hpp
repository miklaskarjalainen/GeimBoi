#pragma once
#include <cstdint>
#include <raylib.h>

namespace Giffi
{

    class gbGameBoy;
    class gbZ80;

    class gbPPU
    {
    public:
        gbPPU(gbGameBoy* _gameboy)
            : mGameBoy(_gameboy) { mCanvasBuffer = GenImageColor(160, 144, WHITE); };
        ~gbPPU() = default;
    public:
        Image mCanvasBuffer;
    
    private:
        Color   GetPixelColor(uint8_t col, uint16_t _addr);
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
