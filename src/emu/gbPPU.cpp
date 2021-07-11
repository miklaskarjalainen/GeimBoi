#include "gbPPU.hpp"
#include "gbZ80.hpp"
#include "gbGameBoy.hpp"

namespace Giffi
{

Color gbPPU::GetPixelColor(uint8_t col, uint16_t _addr)
{
    switch (col)
    {
        case 0b00:
            return {248,248,248,255};
        case 0b10:
            return {186,186,186,255};
        case 0b01:
            return {96, 96, 96,255};
        case 0b11:
            return {18,18,18,255};
        default:
            return {255,0,0,255};
    }
}

void gbPPU::UpdateGraphics(uint16_t _cycles)
{
    SetLCDStatus();

    uint8_t LCD_CONTROL = mGameBoy->mRom[0xFF40]; // Settings for the lcd

    if (LCD_CONTROL >> 7) // LCD_ENABLE
    {
        mScanlineCounter -= _cycles;
    }
    
    if (mScanlineCounter <= 0)
    {
        mScanlineCounter = 456;
        RenderScanline();
    }
}

void gbPPU::SetLCDStatus()
{
    uint8_t LCD_CONTROL = mGameBoy->mRom[0xFF40]; 
    uint8_t LCD_STATUS  = mGameBoy->mRom[0xFF41]; 
    uint8_t LY  = mGameBoy->mRom[0xFF44]; // Current scanline
    uint8_t LYC = mGameBoy->mRom[0xFF45]; // This is set by a the game, if same as LY then a interrupt is requested

    if (!((LCD_CONTROL >> 7) & 1)) // If screen is disabled
    {
        mScanlineCounter = 456;
        mGameBoy->mRom[0xFF44] = 0; // Set cur scanline to 0

        LCD_STATUS &= 252;
        LCD_STATUS |= 0b1;
        mGameBoy->mRom[0xFF41] = LCD_STATUS;
        return;
    }


    // SET MODE
    bool    mode_interupt = false;
    uint8_t old_mode = LCD_STATUS & 0b11;      // Last 2 bits contains the current mode
    if (LY >= 144)                             // VBlank (0b01)
    {
        // mode 1
        LCD_STATUS &= ~(0b11);
        LCD_STATUS |= 0b01;
        mode_interupt = (LCD_STATUS >> 4) & 1; // Vblank interrupt enable
    }
    else
    {
        int mode2Bounds = (456 - 80) ;
        int mode3Bounds = (mode2Bounds - 172) ;

        if (mScanlineCounter >= mode2Bounds)            // Searching Oam (0b10)
        {
            LCD_STATUS &= ~(0b11);
            LCD_STATUS |= 0b10;
            mode_interupt = (LCD_STATUS >> 5) & 1; // Oam interrupt enable
        }
        else if (mScanlineCounter >= mode3Bounds)       // Transferring Data (0b11)
        {   
            LCD_STATUS |= 0b11;
        }
        else                                   //Hblank (0b00)
        {
            LCD_STATUS &= ~(0b11);
            mode_interupt = (LCD_STATUS >> 5) & 3; // Hblank interrupt enable
        }
    }       
    // just entered a new mode. Request interupt
    uint8_t new_mode = LCD_STATUS & 0b11;
    if (mode_interupt && (old_mode != new_mode))
    {
        mGameBoy->mCpu.RequestInterupt(INTERUPT_LCD);
    }

    if (LY == LYC)
    {    
        LCD_STATUS |= 1 << 2;      // Set LY=LYC flag on the lcd
        if ((LCD_STATUS >> 6) & 1) // Is LY=LYC interrupt enabled.
        {
            mGameBoy->mCpu.RequestInterupt(INTERUPT_LCD);
        }
    }
    else
    {
        LCD_STATUS &= ~(1 << 2);   // Reset LY=LYC flag on the lcd
    }
    
    mGameBoy->mRom[0xFF41] = LCD_STATUS;
}

void gbPPU::RenderScanline()
{
    uint8_t LCD_CONTROL = mGameBoy->mRom[0xFF40]; // Settings for the lcd
    if (!(LCD_CONTROL >> 7))
        return;
    
    uint8_t curScanline = mGameBoy->mRom[0xFF44]; // 0-153
    curScanline++;
    
    if ( curScanline == 144U ) // VBLANK
    {
        mGameBoy->mCpu.RequestInterupt(INTERUPT_VBLANK);
    }
    else if ( curScanline > 153U ) // GOTO NEXT LINE
    {
        curScanline = 0;
    }
    mGameBoy->mRom[0xFF44] = curScanline;

    if ( curScanline < 144U) // 0-144 is visible to the viewport
    {
        RenderBackground();
        RenderSprites();
    }
    
}

void gbPPU::RenderBackground()
{   
    uint8_t lcdControl = mGameBoy->mRom[0xFF40];
    uint8_t scanline   = mGameBoy->mRom[0xFF44];

    if ( (lcdControl >> 0) & 1) // LCD Enabled?
    {
        uint16_t tileData = (lcdControl >> 4) & 1 ? 0x8000 : 0x8800; // 
        bool unsig        = (lcdControl >> 4) & 1;                   // Use signed with 0x8800

        // Using Window
        uint8_t windowY = mGameBoy->ReadByte(0xFF4A);
        uint8_t windowX = mGameBoy->ReadByte(0xFF4B) - 7;
        bool usingWindow = ((lcdControl >> 5) & 1) &&  windowY <= mGameBoy->ReadByte(0xFF44);
        
        // Background Memory
        uint16_t backgroundMemory;
        if (usingWindow)
            backgroundMemory = (lcdControl >> 6) & 1 ? 0x9C00 : 0x9800;
        else
            backgroundMemory = (lcdControl >> 3) & 1 ? 0x9C00 : 0x9800;

        uint8_t scrollY = mGameBoy->ReadByte(0xFF42); // Viewport y + current y scanline
        uint8_t scrollX = mGameBoy->ReadByte(0xFF43);
        
        uint8_t yPos = scrollY + scanline;
        uint16_t tileRow = (((uint8_t)(yPos/8))*32) ;
        for (int pixel = 0 ; pixel < 160; pixel++)
        {
            uint8_t xPos     = pixel+scrollX;
            uint16_t tileCol = (xPos/8);

            int16_t tileNum;
            uint16_t tileLocation;
            if (unsig)
            {
                tileNum = (uint8_t)mGameBoy->ReadByte(backgroundMemory + tileRow + tileCol);
                tileLocation = tileData + (tileNum * 16);
            }
            else
            {
                tileNum = (int8_t)mGameBoy->ReadByte(backgroundMemory + tileRow + tileCol);
                tileLocation = tileData + ((tileNum+128)*16);
            }


            uint8_t line = yPos % 8 ;
            uint8_t data1 = mGameBoy->ReadByte(tileLocation + (line * 2));
            uint8_t data2 = mGameBoy->ReadByte(tileLocation + (line * 2) + 1);

            int colourBit = xPos % 8 ;
            colourBit -= 7 ;
            colourBit *= -1 ;

            int colourNum = ((data1 >> colourBit) & 0b1);
            colourNum   <<= 1;
            colourNum    |= ((data2 >> colourBit) & 0b1);

            Color col = GetPixelColor(colourNum, 0xFF47);

            if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
            {
                continue;
            }
    
            ImageDrawPixel(&mCanvasBuffer, pixel, scanline, col);
        }
    }
}

void gbPPU::RenderSprites()
{
    uint8_t lcdControl = mGameBoy->mRom[0xFF40];
    if ( (lcdControl >> 1) & 1)
    {
        for (int sprite = 0; sprite < 40; sprite++)
        {
            uint8_t index = sprite*4;
            uint8_t yPos = mGameBoy->ReadByte(0xFE00+index) - 16;
            uint8_t xPos = mGameBoy->ReadByte(0xFE00+index+1) - 8;
            uint8_t tileLocation = mGameBoy->ReadByte(0xFE00+index+2);
            uint8_t attributes   = mGameBoy->ReadByte(0xFE00+index+3);

            bool yFlip = (attributes >> 6) & 1;
            bool xFlip = (attributes >> 5) & 1;

            int scanline = mGameBoy->ReadByte(0xFF44);
            int y_size = 8; // Sprites can be either 8x8 or 8x16 (lcdControl >> 2) & 1 ? 16 : 8

            if ((scanline >= yPos) && (scanline < (yPos+y_size)))
            {
                int line = scanline - yPos ;

                if (yFlip)
                {
                    line -= y_size ;
                    line *= -1 ;
                }

                uint8_t data1 = mGameBoy->ReadByte( (0x8000 + (tileLocation * 16)) + (line * 2) ) ;
                uint8_t data2 = mGameBoy->ReadByte( (0x8000 + (tileLocation * 16)) + (line * 2)+1 ) ;

                for (int tilePixel = 7; tilePixel >= 0; tilePixel--)
                {
                    int colourbit = tilePixel ;
                    if (xFlip)
                    {
                        colourbit -= 7 ;
                        colourbit *= -1 ;
                    }
                    int colourNum = ((data2 >> colourbit) & 0b1);
                    colourNum <<= 1;
                    colourNum |= ((data1 >> colourbit) & 0b1);

                    uint16_t colourAddress = (attributes >> 4 ) & 1?0xFF49:0xFF48;
                    Color col = GetPixelColor(colourNum, colourAddress); // 0xFF48 || 0xFF49;
                    /*if (col.r == 18)  // White = transparent
                        continue;*/

                    int xPix = 0 - tilePixel ;
                    xPix += 7 ;

                    int pixel = xPos+xPix ;

                    if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
                    {
                        continue ;
                    }

                    ImageDrawPixel(&mCanvasBuffer, pixel, scanline, col);
                }
            }
        }
    }
}



} // Namespace
