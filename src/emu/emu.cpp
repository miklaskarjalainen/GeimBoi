#include <string.h>
#include <string>
#include "emu.hpp"

namespace Giffi
{

GBEmu::GBEmu()
{
    mCanvasBuffer = GenImageColor(256, 256, WHITE);
}

GBEmu::~GBEmu()
{
    
}


void GBEmu::Run()
{
    uint16_t addr = 0x100;

    while ( !WindowShouldClose() )
    {
        // Run
        if ( IsKeyDown(KEY_SPACE) ) // 10 ticks (hold)
        {
            AdvanceFrame();
        }
        // Run
        if ( IsKeyReleased(KEY_W) ) // 10 ticks (hold)
        {
            for (int i = 0; i < 100; i++)
            {
                AdvanceClock();
            }
        }
        if ( IsKeyReleased(KEY_F) ) // 1 tick (once)
            AdvanceClock();
        
        if ( IsKeyReleased(KEY_R))
        {
            Reset();
            LoadRom("tetris.gb");
        }


        BeginDrawing();
            ClearBackground(BLUE);

            // Render Screen
            Texture2D text_screen = LoadTextureFromImage(mCanvasBuffer);
            DrawTextureEx(text_screen, {200, 160}, 0, 2.0f, WHITE);
            
            {
                // Print Registers
                DrawText( FormatText(" AF: 0x%04X\n BC: 0x%04X\n HL: 0x%04X\n DE:0x%04X\n PC: 0x%04X\n SP: 0x%04X\n CONTROL: 0x%02X\n STATUS: 0x%02X\n", mRegAF.val, mRegBC.val, mRegHL.val, mRegDE.val, mRegPC.val, mRegSP.val, mRom[0xFF40], mRom[0xFF41]), 20, 20, 18, RED );
                DrawText( FormatText(" MBC1: %i MBC2: %i", mMBC1, mMBC2), 300, 20, 18, RED );
                DrawText( FormatText(" Last OP: 0x%02X", mLastOpcode ), 300, 100, 18, RED );
                DrawText( FormatText(" Next OP: 0x%02X", ReadByte(mRegPC.val + 1) ), 300, 120, 18, RED );
                DrawText( FormatText(" TimerEnabled: %u Timer: 0x%02X", (mRom[0xFF07] >> 2) & 1, ReadByte(0xFF05) ), 300, 140, 18, RED );
                DrawText( FormatText(" LY: %u", ReadByte(0xFF44) ), 300, 160, 18, RED );
                DrawText( FormatText(" LastInterrupt: 0x%02x", mLastInterupt ), 300, 180, 18, RED );

                // Print Registers  
                DrawText( FormatText("Z"), 160, 80, 18, (mRegAF.low & ( 1 << FLAG_Z) ? GREEN : RED) );
                DrawText( FormatText("N"), 180, 80, 18, (mRegAF.low & ( 1 << FLAG_N) ? GREEN : RED) );
                DrawText( FormatText("H"), 200, 80, 18, (mRegAF.low & ( 1 << FLAG_H) ? GREEN : RED) );
                DrawText( FormatText("C"), 220, 80, 18, (mRegAF.low & ( 1 << FLAG_C) ? GREEN : RED) );

                // Requested Interrupt
                DrawText( FormatText("V"), 160, 100, 18, (mRom[0xFF0F] & ( 1 << INTERUPT_VBLANK) ? GREEN : RED) );
                DrawText( FormatText("L"), 180, 100, 18, (mRom[0xFF0F] & ( 1 << INTERUPT_LCD)    ? GREEN : RED) );
                DrawText( FormatText("T"), 200, 100, 18, (mRom[0xFF0F] & ( 1 << INTERUPT_TIMER)  ? GREEN : RED) );
                DrawText( FormatText("J"), 220, 100, 18, (mRom[0xFF0F] & ( 1 << INTERUPT_JOYPAD) ? GREEN : RED) );

                DrawText( FormatText("LH"), 140, 120, 18, (mRom[0xFF41] & ( 1 << 3) ? GREEN : RED) );
                DrawText( FormatText("LV"), 170, 120, 18, (mRom[0xFF41] & ( 1 << 4) ? GREEN : RED) );
                DrawText( FormatText("LO"), 200, 120, 18, (mRom[0xFF41] & ( 1 << 5) ? GREEN : RED) );
                DrawText( FormatText("LY"), 230, 120, 18, (mRom[0xFF41] & ( 1 << 6) ? GREEN : RED) );
                DrawText( FormatText("0x%02X", mRom[0xFF41] & 0b11), 255, 120, 18, RED );

                // Enabled Interrupt
                DrawText( FormatText("V"), 160, 140, 18, (mRom[0xFFFF] & ( 1 << INTERUPT_VBLANK) ? GREEN : RED) );
                DrawText( FormatText("L"), 180, 140, 18, (mRom[0xFFFF] & ( 1 << INTERUPT_LCD)    ? GREEN : RED) );
                DrawText( FormatText("T"), 200, 140, 18, (mRom[0xFFFF] & ( 1 << INTERUPT_TIMER)  ? GREEN : RED) );
                DrawText( FormatText("J"), 220, 140, 18, (mRom[0xFFFF] & ( 1 << INTERUPT_JOYPAD) ? GREEN : RED) );

                // Print Things
                auto redbyte = [&](uint16_t offset) 
                {
                    return ReadByte(addr + offset);
                };
                
                addr += GetMouseWheelMove() * 0x10;
                if (IsKeyPressed( KEY_UP ))
                    addr += 0x1000;
                if (IsKeyPressed( KEY_DOWN ))
                    addr -= 0x1000;

                DrawText( FormatText("0x%04x: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", 
                    addr, redbyte(0x0), redbyte(0x1), redbyte(0x2), redbyte(0x3), redbyte(0x4), redbyte(0x5), redbyte(0x6), redbyte(0x7), redbyte(0x8), redbyte(0x9), redbyte(0xa), redbyte(0xb), redbyte(0xc), redbyte(0xd), redbyte(0xe), redbyte(0xf)), 500, 150, 18, RED );
                DrawText( FormatText("Last unkown: 0x%02x", last_unkown_opcode), 500, 170, 18, RED );
                DrawText( FormatText("mCyclesDone: %u mEnableInterrupts: %i mIsHalted: %i ScreenEnabled: %i",
                    mCyclesDone, mEnableInterrupts, mIsHalted, (mRom[0xFF40] >> 7) & 1), 500, 110, 18, RED );

                // Print Name
                char name[11] = "";
                memcpy( &name, mCart + 0x134, sizeof(char) * 11 );
                DrawText( FormatText(" Game: %s", &name ), 300, 60, 18, RED );
            }


        EndDrawing();

        UnloadTexture(text_screen);
    }

}

// Advances the emulator by a single FRAME
void GBEmu::AdvanceFrame()
{
    while (mCyclesDone < 70221)
    {
        AdvanceClock();
    }

    if (mCyclesDone >= 70221)
        mCyclesDone -= 70221;
}

// Advances the emulator by a single CYCLE
void GBEmu::AdvanceClock()
{
    int cur_cycle = mCyclesDone;

    if (!mIsHalted)
        ExecuteNextOpcode();
    else
        mCyclesDone += 4;
    
    int delta_cycles = mCyclesDone - cur_cycle;

    UpdateTimers(delta_cycles);
    UpdateGraphics(delta_cycles);
    DoInterupts();
}

uint8_t GBEmu::ReadByte(uint16_t _addr)
{
    // are we reading from the rom memory bank?
    if ((_addr>=0x4000) && (_addr <= 0x7FFF))
    {
        //uint16_t new_addr = _addr - 0x4000 ;
        return mCart[_addr];
    }

    // are we reading from ram memory bank?
    else if ((_addr >= 0xA000) && (_addr <= 0xBFFF))
    {
        uint16_t new_addr = _addr - 0xA000 ;
        return mRam[new_addr + (mCurRamBank*0x2000)] ;
    }

    // else return memory
    return mRom[_addr];
}

// Uses the program counter (without incrementing it)
uint16_t GBEmu::ReadWord()
{
    uint16_t res = ReadByte(mRegPC.val+1);
	res = res << 8;
	res |= ReadByte(mRegPC.val);
	return res;
}

void GBEmu::WriteByte(uint16_t _addr, uint8_t _data)
{
    // Read only
    if (_addr < 0x8000) {} 

    // Echo
    else if ( ( _addr >= 0xE000 ) && (_addr < 0xFE00) )
    {
        mRom[_addr] = _data;
        WriteByte(_addr-0x2000, _data);
    } 
    else if ( _addr == 0xFF00) {}
    else if ( ( _addr >= 0xFEA0 ) && (_addr < 0xFEFF) ) {}                      // this area is restricted
    else if ( _addr == 0xFF04 ) { mRom[0xFF04] = 0x00; mDividerCounter = 0; } // Writing any value to this register resets it to $00
    else if ( _addr == 0xFF46 ) // DMA transfer
	{
	    uint8_t newAddress = (_data << 8);
		for (uint8_t i = 0; i < 0xA0; i++)
		{
			mRom[0xFE00 + i] = ReadByte(newAddress + i);
		}
	}
    else 
    {
        mRom[_addr] = _data;
    }
}

void GBEmu::PushWordOntoStack(uint16_t _word)
{
	uint8_t high = _word >> 8 ;
	uint8_t low = _word & 0xFF;
	mRegSP.val--;
	WriteByte(mRegSP.val, high);
	mRegSP.val--;
	WriteByte(mRegSP.val, low);
}

uint16_t GBEmu::PopWordOffStack()
{
	uint16_t word = ReadByte(mRegSP.val+1) << 8;
	word |= ReadByte(mRegSP.val);
	mRegSP.val+=2;
	return word;
}

void GBEmu::UpdateTimers( uint16_t cycles )
{
    uint8_t& timer     = mRom[0xFF05];
    uint8_t timer_set  = mRom[0xFF06];

    unsigned int timer_freq = 0;
    switch (mRom[0xFF07] & 0b11)
    {
        case 0b00:
            timer_freq = 4096;
            break;
        case 0b01:
            timer_freq = 262144;
            break;
        case 0b10:
            timer_freq = 65536;
            break;
        case 0b11:
            timer_freq = 16384;
            break;
        default:
            timer_freq = 4096;
            break;
    }

    if ((mRom[0xFF07] >> 2) & 1) // Timer Enabled
    {
        mTimerCounter += cycles;

        if (mTimerCounter >= timer_freq)
        {
            mTimerCounter -= timer_freq;
            timer++;

            if (timer == 0xFF) // Overflow
            {
                timer = timer_set;
                RequestInterupt(INTERUPT_TIMER);
            }
        }
    }

    // do divider register
    mDividerCounter += cycles;
	if (mDividerCounter >= 256)
	{
		mDividerCounter -= 256;
		mRom[0xFF04]++;
	}
}

Color GBGetColor(uint8_t col, uint16_t _addr)
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

void GBEmu::UpdateGraphics(uint16_t _cycles)
{
    static int retrace = 456;   // how many cycles it takes to do 
    SetLCDStatus(retrace);

    uint8_t LCD_CONTROL = mRom[0xFF40]; // Settings for the lcd

    if (LCD_CONTROL >> 7) // LCD_ENABLE
        retrace -= _cycles;
    
	if (retrace <= 0)
    {
        retrace = 456;
        RenderScanline();
    }
}

void GBEmu::SetLCDStatus(int& retrace)
{
    uint8_t LCD_CONTROL = mRom[0xFF40]; 
    uint8_t LCD_STATUS  = mRom[0xFF41]; 
    uint8_t LY  = mRom[0xFF44]; // Current scanline
    uint8_t LYC = mRom[0xFF45]; // This is set by a the game, if same as LY then a interrupt is requested

    if (!((LCD_CONTROL >> 7) & 1)) // If screen is disabled
    {
        retrace = 456;
        mRom[0xFF44] = 0; // Set cur scanline to 0

        LCD_STATUS &= 252;
        LCD_STATUS |= 0b1;
        mRom[0xFF41] = LCD_STATUS;
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

		if (retrace >= mode2Bounds)            // Searching Oam (0b10)
		{
            LCD_STATUS &= ~(0b11);
			LCD_STATUS |= 0b10;
            mode_interupt = (LCD_STATUS >> 5) & 1; // Oam interrupt enable
		}
		else if (retrace >= mode3Bounds)       // Transferring Data (0b11)
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
		RequestInterupt(INTERUPT_LCD);

    if (LY == LYC)
    {    
        LCD_STATUS |= 1 << 2;      // Set LY=LYC flag on the lcd
        if ((LCD_STATUS >> 6) & 1) // Is LY=LYC interrupt enabled.
            RequestInterupt(INTERUPT_LCD);
    }
    else
    {
        LCD_STATUS &= ~(1 << 2);   // Reset LY=LYC flag on the lcd
    }
    
    mRom[0xFF41] = LCD_STATUS;
}

void GBEmu::RenderScanline()
{
    uint8_t LCD_CONTROL = mRom[0xFF40]; // Settings for the lcd
    if (!(LCD_CONTROL >> 7))
        return;
    
    uint8_t curScanline = mRom[0xFF44]; // 0-153
    curScanline++;
    
    if ( curScanline == 144U ) // VBLANK
    {
        RequestInterupt(INTERUPT_VBLANK);
    }
    else if ( curScanline > 153U ) // GOTO NEXT LINE
    {
        curScanline = 0;
    }
    mRom[0xFF44] = curScanline;

    if ( curScanline < 144U) // 0-144 is visible to the viewport
    {
        RenderBackground();
		RenderSprites();
    }
    
}

void GBEmu::RenderBackground()
{   
    uint8_t lcdControl = mRom[0xFF40];
    uint8_t scanline   = mRom[0xFF44];

	if ( (lcdControl >> 0) & 1) // LCD Enabled?
	{
		uint16_t tileData = (lcdControl >> 4) & 1 ? 0x8000 : 0x8800; // 
        bool unsig        = (lcdControl >> 4) & 1;                   // Use signed with 0x8800

        // Using Window
        uint8_t windowY = ReadByte(0xFF4A);
		uint8_t windowX = ReadByte(0xFF4B) - 7;
        bool usingWindow = ((lcdControl >> 5) & 1) &&  windowY <= ReadByte(0xFF44);
        
        // Background Memory
		uint16_t backgroundMemory;
        if (usingWindow)
            backgroundMemory = (lcdControl >> 6) & 1 ? 0x9C00 : 0x9800;
        else
            backgroundMemory = (lcdControl >> 3) & 1 ? 0x9C00 : 0x9800;

		uint8_t scrollY = ReadByte(0xFF42); // Viewport y + current y scanline
		uint8_t scrollX = ReadByte(0xFF43);
        
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
                tileNum = (uint8_t)ReadByte(backgroundMemory + tileRow + tileCol);
                tileLocation = tileData + (tileNum * 16);
            }
            else
            {
                tileNum = (int8_t) ReadByte(backgroundMemory + tileRow + tileCol);
                tileLocation = tileData + ((tileNum+128)*16);
            }


			uint8_t line = yPos % 8 ;
			uint8_t data1 = ReadByte(tileLocation + (line * 2));
			uint8_t data2 = ReadByte(tileLocation + (line * 2) + 1);

			int colourBit = xPos % 8 ;
			colourBit -= 7 ;
			colourBit *= -1 ;

			int colourNum = ((data1 >> colourBit) & 0b1);
 			colourNum   <<= 1;
 			colourNum    |= ((data2 >> colourBit) & 0b1);

			Color col = GBGetColor(colourNum, 0xFF47);

            if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
			{
				continue;
			}
    
			ImageDrawPixel(&mCanvasBuffer, pixel, scanline, col);
		}
	}
}

void GBEmu::RenderSprites()
{
    uint8_t lcdControl = mRom[0xFF40];
    if ( (lcdControl >> 1) & 1)
	{
		for (int sprite = 0; sprite < 40; sprite++)
		{
 			uint8_t index = sprite*4;
 			uint8_t yPos = ReadByte(0xFE00+index) - 16;
 			uint8_t xPos = ReadByte(0xFE00+index+1) - 8;
 			uint8_t tileLocation = ReadByte(0xFE00+index+2);
 			uint8_t attributes   = ReadByte(0xFE00+index+3);

			bool yFlip = (attributes >> 6) & 1;
			bool xFlip = (attributes >> 5) & 1;

			int scanline = ReadByte(0xFF44);
            int y_size = 8; // Sprites can be either 8x8 or 8x16 (lcdControl >> 2) & 1 ? 16 : 8

 			if ((scanline >= yPos) && (scanline < (yPos+y_size)))
 			{
 				int line = scanline - yPos ;

 				if (yFlip)
 				{
 					line -= y_size ;
 					line *= -1 ;
 				}

 				uint8_t data1 = ReadByte( (0x8000 + (tileLocation * 16)) + (line * 2) ) ;
 				uint8_t data2 = ReadByte( (0x8000 + (tileLocation * 16)) + (line * 2)+1 ) ;

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
					Color col = GBGetColor(colourNum, colourAddress); // 0xFF48 || 0xFF49;
                    /*if (col.r == 18)  // White = transparent
                        continue;*/

 					int xPix = 0 - tilePixel ;
 					xPix += 7 ;

					int pixel = xPos+xPix ;

					if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
					{
					//	assert(false) ;
						continue ;
					}

 					ImageDrawPixel(&mCanvasBuffer, pixel, scanline, col);
 				}
 			}
		}
	}
}

void GBEmu::RequestInterupt(uint8_t _id)
{
    uint8_t wut = mRom[0xFF0F];
    wut |= 1 << _id;
    mRom[0xFF0F] = wut;
} 
   
void GBEmu::DoInterupts()
{
    if (mEnableInterrupts)
    {
        uint8_t req     = mRom[0xFF0F];
        uint8_t enabled = mRom[0xFFFF];
        
        if (req > 0)
        {
            for (uint8_t i = 0 ; i < 5; i++)
            {
                if ( (req >> i) & 1)
                {
                    if ( (enabled >> i) & 1 )
                    {
                        ServiceInterupt(i);
                    }
                }
            }
        }
       
    }

} 

void GBEmu::ServiceInterupt(uint8_t _interrupt)
{
    mEnableInterrupts = false;
    mIsHalted = false;

    mRom[0xFF0F] &= ~(1 << _interrupt);

    /// we must save the current execution address by pushing it onto the stack
    PushWordOntoStack(mRegPC.val);

    switch (_interrupt)
    {
        case INTERUPT_VBLANK: mRegPC.val = 0x40; break;
        case INTERUPT_LCD   : mRegPC.val = 0x48; break;
        case INTERUPT_TIMER : mRegPC.val = 0x50; break;
        case INTERUPT_SERIAL: mRegPC.val = 0x58; break;
        case INTERUPT_JOYPAD: mRegPC.val = 0x60; break;
    }   
    mLastInterupt = mRegPC.val;
}
                       
void GBEmu::LoadRom(const char* _file_path)
{
    // Init Cart
    memset(mCart,0,sizeof(mCart)) ;
    FILE *in;
    in = fopen( _file_path, "rb" );
    fread(mCart, 1, 0x200000, in);
    fclose(in); 

    memcpy(&mRom[0x0], &mCart[0x0], 0x8000) ; // this is read only and never changes
	mCurRomBank = 1;

    // Init Ram
    memset( &mRam,0,sizeof(mRam) );
    mCurRamBank = 0;
}

void GBEmu::Reset()
{
    
    // Setup rom banking
    mMBC1 = false;
    mMBC2 = false;

    mEnableInterrupts = false;
    mRegAF.val = 0x01B0;
    mRegBC.val = 0x0013;
    mRegDE.val = 0x00D8;
    mRegHL.val = 0x014D;
    mRegPC.val = 0x0100;
    mRegSP.val = 0xFFFE;

    // Init Regs
    mRom[0xFF00] = 0xFF;

    mRom[0xFF05] = 0x00;
    mRom[0xFF06] = 0x00;
    mRom[0xFF07] = 0x00;
    mRom[0xFF10] = 0x80;
    mRom[0xFF11] = 0xBF;
    mRom[0xFF12] = 0xF3;
    mRom[0xFF14] = 0xBF;
    mRom[0xFF16] = 0x3F;
    mRom[0xFF17] = 0x00;
    mRom[0xFF19] = 0xBF;
    mRom[0xFF1A] = 0x7F;
    mRom[0xFF1B] = 0xFF;
    mRom[0xFF1C] = 0x9F;
    mRom[0xFF1E] = 0xBF;
    mRom[0xFF20] = 0xFF;
    mRom[0xFF21] = 0x00;
    mRom[0xFF22] = 0x00;
    mRom[0xFF23] = 0xBF;
    mRom[0xFF24] = 0x77;
    mRom[0xFF25] = 0xF3;
    mRom[0xFF26] = 0xF1;
    mRom[0xFF40] = 0x91;
    mRom[0xFF42] = 0x00;
    mRom[0xFF43] = 0x00;
    mRom[0xFF45] = 0x00;
    mRom[0xFF47] = 0xFC;
    mRom[0xFF48] = 0xFF;
    mRom[0xFF49] = 0xFF;
    mRom[0xFF4A] = 0x00;
    mRom[0xFF4B] = 0x00;

    mRom[0xFFFF] = 0x00; 
}

} // Namespace


/*
for (int i = 0; i < 10; i++)
    {
        uint16_t block1 = 0x8000 + (i * 0x10); // Each tile is 16bytes long

        for (int y = 0; y < 8;  y++)
        {
            uint8_t color1 = ReadByte(block1 + (y * 2 + 1));
            uint8_t color2 = ReadByte(block1 + (y * 2 + 0));
            for (int x = 0; x < 8; x++)
            {
                uint8_t indx = 0x00;
                indx |= ((color1 >> (7-x)) & 0b1);
                indx <<= 1;
                indx |= ((color2 >> (7-x)) & 0b1);

                Color col = GBGetColor( indx );
                ImageDrawPixel(&mCanvasBuffer, x + (i*8), y, col);
            }
        }
    }
*/