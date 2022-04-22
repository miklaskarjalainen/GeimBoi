#include <utils/Benchmark.hpp>
#include "gbPPU.hpp"
#include "gbZ80.hpp"
#include "gbGameBoy.hpp"

using namespace Giffi;

gbPPU::gbColorId gbPPU::GetPixelColor(uint8_t col, uint16_t addr)
{
    uint8_t palette = mGameBoy->ReadByte(addr);
    uint8_t high = 0;
    uint8_t low = 0;

    switch (col)
    {
        case 0b00: high = 1; low = 0; break;
        case 0b01: high = 3; low = 2; break;
        case 0b10: high = 5; low = 4; break;
        case 0b11: high = 7; low = 6; break;
    }

    uint8_t color = 0;
    color = ((palette >> high) & 0b1) << 1;
    color |= ((palette >> low) & 0b1);
    
    return (gbColorId)color;
}

void gbPPU::Reset()
{
    mScanlineCounter = 456;
    memset(frontBuffer, 0, sizeof(gbColor[144 * 160]));
}

void gbPPU::UpdateGraphics(uint16_t cycles)
{
    SetLCDStatus();

    if (LCD_Enable()) // LCD_ENABLE
    {
        mScanlineCounter -= cycles;
    }

    if (mScanlineCounter <= 0)
    {
        mScanlineCounter = 456;   
        RenderScanline();
    }
}

void gbPPU::SetLCDStatus()
{
    uint8_t LCD_CONTROL = GetLCDC();
    uint8_t LCD_STATUS  = GetSTAT();
    uint8_t LY  = mGameBoy->mRom[0xFF44]; // Current scanline

    if (!LCD_Enable())
    {
        mScanlineCounter = 456;
        mGameBoy->mRom[0xFF41] &= 0b11111100; // LCD Status to 0x00
        mGameBoy->mRom[0xFF44] = 0x0;         // Set cur scanline to 0
        return;
    }

    // SET MODE
    bool    modeInterrupt = false;
    uint8_t oldMode = LCD_STATUS & 0b11;       // Last 2 bits contains the current mode
    if (LY >= 144)                             // VBlank (0b01)
    {
        // mode 1
        LCD_STATUS &= ~(0b11);
        LCD_STATUS |= 0b01;
        modeInterrupt = (LCD_STATUS >> 4) & 1; // Vblank interrupt enable
    }
    else
    {
        const int mode2Bounds = (456 - 80);
        const int mode3Bounds = (mode2Bounds - 172);

        if (mScanlineCounter >= mode2Bounds)            // Searching Oam (0b10)
        {
            LCD_STATUS &= ~(0b11);
            LCD_STATUS |= 0b10;
            modeInterrupt = (LCD_STATUS >> 5) & 1;      // Oam interrupt enable
        }
        else if (mScanlineCounter >= mode3Bounds)       // Transferring Data (0b11)
        {   
            LCD_STATUS |= 0b11;
        }
        else                                            //Hblank (0b00)
        {
            LCD_STATUS &= ~(0b11);
            modeInterrupt = (LCD_STATUS >> 3) & 1;      // Hblank interrupt enable
        }
    }       
    // just entered a new mode. Request interupt
    uint8_t newMode = LCD_STATUS & 0b11;
    mGameBoy->mRom[0xFF41] = LCD_STATUS;

    if (modeInterrupt && (oldMode != newMode))
    {
        mGameBoy->mCpu.RequestInterrupt(gbInterrupt::LCD);
    }
}

void gbPPU::RenderScanline()
{
    if (!LCD_Enable()) { return; }
    
    uint8_t& curScanline = mGameBoy->mRom[0xFF44]; // 0-153
    curScanline++;
    
    if ( curScanline == 144U )     // VBLANK
    {
        mGameBoy->mCpu.RequestInterrupt(gbInterrupt::VBlank);
    }
    else if ( curScanline > 153U ) // RESET SCANLINE
    {
        curScanline = 0;
    }
    CheckCoinsidenceFlag();

    if ( curScanline < 144U) // 0-144 is visible to the viewport
    {
        RenderBackground();
        RenderSprites();
    }    
}

void gbPPU::RenderBackground()
{  
    uint8_t LCDControl = mGameBoy->mRom[0xFF40];

    if (!LCD_BgWindowEnable()) // Background enable
        return;
    
    const uint8_t  LY   = mGameBoy->mRom[0xFF44]; // Scanline
    
    uint16_t tileData = (LCDControl >> 4) & 1 ? 0x8000 : 0x8800;
    bool     unsig    = (LCDControl >> 4) & 1;  // Use signed with 0x8800
    
    // Using Window
    uint8_t windowY = mGameBoy->ReadByte(0xFF4A);
    uint8_t windowX = mGameBoy->ReadByte(0xFF4B) - 7;
    bool windowEnable = ((LCDControl >> 5) & 1) &&  windowY <= LY;
    
    // Background Memory
    uint16_t backgroundMemory;
    if (windowEnable)
        backgroundMemory = (LCDControl >> 6) & 1 ? 0x9C00 : 0x9800;
    else
        backgroundMemory = (LCDControl >> 3) & 1 ? 0x9C00 : 0x9800;

    uint8_t scrollY = mGameBoy->ReadByte(0xFF42);
    uint8_t scrollX = mGameBoy->ReadByte(0xFF43);
        
    uint8_t yPos = 0; 
    if (!windowEnable)
		yPos = scrollY + LY;
	else
		yPos = LY - windowY;


    uint16_t tileRow = (((uint8_t)(yPos/8))*32);
    for (int pixel = 0; pixel < 160; pixel++)
    {
        uint8_t xPos = pixel + scrollX;

        if (windowEnable)
        {
            if (pixel >= windowX)
            {
                xPos = pixel - windowX;
            }
        }

        uint16_t tileCol = (xPos / 8);
        int16_t tileNum;

        uint16_t tileLocation = tileData;
        if (unsig)
        {
            tileNum = (uint8_t)mGameBoy->ReadByte(backgroundMemory + tileRow + tileCol);
            tileLocation += (tileNum * 16);
        }
        else
        {
            tileNum = (int8_t)mGameBoy->ReadByte(backgroundMemory + tileRow + tileCol);
            tileLocation += ((tileNum + 128) * 16);
        }

        uint8_t line = yPos % 8;
        uint8_t data1 = mGameBoy->ReadByte(tileLocation + (line * 2));
        uint8_t data2 = mGameBoy->ReadByte(tileLocation + (line * 2) + 1);

        int colourBit = xPos % 8;
        colourBit -= 7;
        colourBit *= -1;

        int colourNum = ((data2 >> colourBit) & 0b1);
        colourNum <<= 1;
        colourNum |= ((data1 >> colourBit) & 0b1);

        gbColorId col   = GetPixelColor(colourNum, 0xFF47);
        gbColor color   = dmgPalette[col];

        if ((LY < 0) || (LY > 143) || (pixel < 0) || (pixel > 159))
        {
            continue;
        }
        (*frontBuffer)[SCREEN_WIDTH * LY +pixel] = color;
    }
}

void gbPPU::RenderSprites()
{
    if ( !LCD_ObjEnable() ) { return; }

    uint8_t lcdControl = mGameBoy->mRom[0xFF40];
    uint8_t spritesPerScanline = 10;
    int ySize = (lcdControl >> 2) & 1 ? 16 : 8;

    // Loop through all the available sprites
    for (int sprite = 0; sprite < 40; sprite++)
    {
        // 10 sprites per scanline
        if (!spritesPerScanline) { break; }

        uint8_t index = sprite*4;
        uint8_t yPos = mGameBoy->ReadByte(0xFE00+index)   - 16;
        uint8_t xPos = mGameBoy->ReadByte(0xFE00+index+1) - 8;
        uint8_t tileLocation = mGameBoy->ReadByte(0xFE00 + index + 2);
        if (ySize == 16)
            tileLocation &= ~(0b1);
        uint8_t attributes   = mGameBoy->ReadByte(0xFE00+index+3);

        bool yFlip = (attributes >> 6) & 1;
        bool xFlip = (attributes >> 5) & 1;

        int scanline = mGameBoy->ReadByte(0xFF44);
        // Is in the current scanline?.
        if ((scanline >= yPos) && (scanline < ( yPos + ySize)))
        {
            spritesPerScanline--;

            int line = scanline - yPos ;
            if (yFlip)
            {
                line -= ySize ;
                line *= -1 ;
            }

            line *= 2;
            uint16_t dataAddress = (0x8000 + (tileLocation * 16)) + line;
            uint8_t data1 = mGameBoy->ReadByte( dataAddress );
            uint8_t data2 = mGameBoy->ReadByte( dataAddress+1 );

            // Loop through a row of pixels in the sprite (for 1 scanline)
            for (int tilePixel = 7; tilePixel >= 0; tilePixel--)
            {
                int colourbit = tilePixel ;
                if (xFlip)
                {
                    colourbit -= 7 ;
                    colourbit *= -1 ;
                }

                // Color Index with in a palette
                int ColorIdx = ((data2 >> colourbit) & 0b1);
                ColorIdx <<= 1;
                ColorIdx |= ((data1 >> colourbit) & 0b1);
                // Index 0 is transparent 
                if (ColorIdx == 0b00) { continue; }
                // Color palette address
                uint16_t ColorAddress = (attributes >> 4 ) & 1 ? 0xFF49 : 0xFF48;

                gbColorId col = GetPixelColor(ColorIdx, ColorAddress);
                gbColor color = dmgPalette[col];

                int xPix = 0 - tilePixel;
                xPix += 7;

                int pixel = xPos+xPix;

                if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
                {
                    continue ;
                }

                (*frontBuffer)[SCREEN_WIDTH * scanline + pixel] = color;
            }
        }
    }
}

// Aka LY=LYC
void gbPPU::CheckCoinsidenceFlag()
{
    uint8_t& lcdStatus = mGameBoy->mRom[0xFF41]; 
    
    if (GetLY() == GetLYC())
    {    
        lcdStatus |= 1 << 2;      // Set LY=LYC flag on the lcd
        if ((lcdStatus >> 6) & 1) // Is LY=LYC interrupt enabled.
        {
            mGameBoy->mCpu.RequestInterrupt(gbInterrupt::LCD);
        }
    }
    else
    {
        lcdStatus &= ~(1 << 2);   // Reset LY=LYC flag on the lcd
    }
}

inline uint8_t gbPPU::GetLY() const 
{
    return mGameBoy->mRom[0xFF44];
}

inline uint8_t gbPPU::GetLYC() const
{
    return mGameBoy->mRom[0xFF45];
}

inline uint8_t gbPPU::GetSTAT() const 
{
    return mGameBoy->mRom[0xFF41]; 
};

inline uint8_t gbPPU::GetLCDC() const 
{
    return mGameBoy->mRom[0xFF40];
};

inline gbPPU::ppuMode gbPPU::GetMode() const
{ 
    return (ppuMode)(GetSTAT() & 0b11); 
}

inline bool gbPPU::LCD_Enable() const
{
    return (mGameBoy->mRom[0xFF40] >> 7) & 0b1;
}

inline bool gbPPU::LCD_WindowEnable() const
{
    return (mGameBoy->mRom[0xFF40] >> 5) & 0b1;
}

inline bool gbPPU::LCD_ObjEnable() const 
{
    return (mGameBoy->mRom[0xFF40] >> 1) & 0b1;
}

inline bool gbPPU::LCD_BgWindowEnable() const
{
    return (mGameBoy->mRom[0xFF40] >> 0) & 0b1;
}

