#include "gbPPU.hpp"
#include "gbZ80.hpp"
#include "gbGameBoy.hpp"

namespace Giffi
{

gbColor gbPPU::GetPixelColor(uint8_t col, uint16_t _addr)
{
    uint8_t palette = mGameBoy->ReadByte(_addr);
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
    
    return (gbColor)color;
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
        mGameBoy->mRom[0xFF41] = 0x00; // LCD Status to 0x00
        mGameBoy->mRom[0xFF44] = 0;    // Set cur scanline to 0
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
            mode_interupt = (LCD_STATUS >> 5) & 1;      // Oam interrupt enable
        }
        else if (mScanlineCounter >= mode3Bounds)       // Transferring Data (0b11)
        {   
            LCD_STATUS |= 0b11;
        }
        else                                            //Hblank (0b00)
        {
            LCD_STATUS &= ~(0b11);
            mode_interupt = (LCD_STATUS >> 3) & 1;      // Hblank interrupt enable
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
        SwapBuffers();
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
        uint16_t tileData = (lcdControl >> 4) & 1 ? 0x8000 : 0x8800;
        bool     unsig    = (lcdControl >> 4) & 1;                   // Use signed with 0x8800

        // Using Window
        uint8_t windowY = mGameBoy->ReadByte(0xFF4A);
        uint8_t windowX = mGameBoy->ReadByte(0xFF4B) - 7;
        bool usingWindow = ((lcdControl >> 5) & 1) &&  windowY <= mGameBoy->ReadByte(0xFF44);
        
        // Background Memory
        uint16_t backgroundMemory;
        if (usingWindow) 
            { backgroundMemory = (lcdControl >> 6) & 1 ? 0x9C00 : 0x9800; }
        else
            { backgroundMemory = (lcdControl >> 3) & 1 ? 0x9C00 : 0x9800; }

        uint8_t scrollY = mGameBoy->ReadByte(0xFF42);
        uint8_t scrollX = mGameBoy->ReadByte(0xFF43);
        
        uint8_t yPos = 0; 
        if (!usingWindow)
			yPos = scrollY + scanline;
		else
			yPos = scanline - windowY;


        uint16_t tileRow = (((uint8_t)(yPos/8))*32);
        for (int pixel = 0; pixel < 160; pixel++)
        {
            uint8_t xPos = pixel+scrollX;

            if (usingWindow)
			{
				if (pixel >= windowX)
				{
					xPos = pixel - windowX;
				}
			}

            uint16_t tileCol = (xPos/8);
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
                tileLocation += ((tileNum+128)*16);
            }

            uint8_t line = yPos % 8;
            uint8_t data1 = mGameBoy->ReadByte(tileLocation + (line * 2));
            uint8_t data2 = mGameBoy->ReadByte(tileLocation + (line * 2) + 1);

            int colourBit = xPos % 8;
            colourBit -=  7;
            colourBit *= -1;

            int colourNum = ((data2 >> colourBit) & 0b1);
            colourNum   <<= 1;
            colourNum    |= ((data1 >> colourBit) & 0b1);

            gbColor col = GetPixelColor(colourNum, 0xFF47);
            Color ray_color = WHITE;
            switch (col)
            {
                case gbColor::White:     ray_color = RAYWHITE;  break;
                case gbColor::LightGray: ray_color = LIGHTGRAY; break;
                case gbColor::DarkGray:  ray_color = DARKGRAY;  break;
                case gbColor::Black:     ray_color = BLACK;     break;
            }

            if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
            {
                continue;
            }
    
            ImageDrawPixel(back_buffer, pixel, scanline, ray_color);
        }
    }
}

void gbPPU::RenderSprites()
{
    uint8_t lcdControl = mGameBoy->mRom[0xFF40];
    int y_size = (lcdControl >> 2) & 1 ? 16 : 8;
    if ( (lcdControl >> 1) & 1)
    {
        for (int sprite = 0; sprite < 40; sprite++)
        {
            uint8_t index = sprite*4;
            uint8_t yPos = mGameBoy->ReadByte(0xFE00+index)   - 16;
            uint8_t xPos = mGameBoy->ReadByte(0xFE00+index+1) - 8;
            uint8_t tileLocation = mGameBoy->ReadByte(0xFE00+index+2);
            uint8_t attributes   = mGameBoy->ReadByte(0xFE00+index+3);

            bool yFlip = (attributes >> 6) & 1;
            bool xFlip = (attributes >> 5) & 1;

            int scanline = mGameBoy->ReadByte(0xFF44);

            if ((scanline >= yPos) && (scanline < (yPos+y_size)))
            {
                int line = scanline - yPos ;

                if (yFlip)
                {
                    line -= y_size ;
                    line *= -1 ;
                }

                line *= 2;
                uint16_t dataAddress = (0x8000 + (tileLocation * 16)) + line;
                uint8_t data1 = mGameBoy->ReadByte( dataAddress ) ;
                uint8_t data2 = mGameBoy->ReadByte( dataAddress+1 ) ;

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

                    uint16_t colourAddress = (attributes >> 4 ) & 1 ? 0xFF49 : 0xFF48;
                    gbColor col = GetPixelColor(colourNum, colourAddress);
                    if (col == gbColor::White)  // White is transparent
                        continue;

                    Color ray_color = WHITE;
                    switch (col)
                    {
                        case gbColor::White:     ray_color = RAYWHITE;  break;
                        case gbColor::LightGray: ray_color = LIGHTGRAY; break;
                        case gbColor::DarkGray:  ray_color = DARKGRAY;  break;
                        case gbColor::Black:     ray_color = BLACK;     break;
                    }

                    int xPix = 0 - tilePixel ;
                    xPix += 7 ;

                    int pixel = xPos+xPix ;

                    if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
                    {
                        continue ;
                    }

                    ImageDrawPixel(back_buffer, pixel, scanline, ray_color);
                }
            }
        }
    }
}



} // Namespace
