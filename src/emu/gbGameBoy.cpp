#include <string.h>
#include <string>
#include "gbGameBoy.hpp"

namespace Giffi
{

gbGameBoy::gbGameBoy()
    : mCpu(this), mPpu(this)
{
    Reset();
}

void gbGameBoy::Run()
{
    uint16_t addr = 0x100;

    while ( !WindowShouldClose() )
    {
        // Run
        
        if ( IsKeyDown(KEY_SPACE) ) // run for a frame (hold)
        {
        }
        mCpu.FrameAdvance();
        
        // Run
        if ( IsKeyReleased(KEY_W) ) // 100 ticks (once)
        {
            for (int i = 0; i < 100; i++)
            {
                mCpu.Clock();
            }
        }
        if ( IsKeyReleased(KEY_F) ) // 1 tick (once)
            mCpu.Clock();
        
        
        if (IsKeyPressed(KEY_ENTER))
        {
            PressButton(gbButton::START);
        }
        if (IsKeyReleased(KEY_ENTER))
        {
            ReleaseButton(gbButton::START);
        }
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            PressButton(gbButton::SELECT);
        }
        if (IsKeyReleased(KEY_BACKSPACE))
        {
            ReleaseButton(gbButton::SELECT);
        }

        if (IsKeyPressed(KEY_A))
        {
            PressButton(gbButton::A);
        }
        if (IsKeyReleased(KEY_A))
        {
            ReleaseButton(gbButton::A);
        }
        if (IsKeyPressed(KEY_B))
        {
            PressButton(gbButton::B);
        }
        if (IsKeyReleased(KEY_B))
        {
            ReleaseButton(gbButton::B);
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            PressButton(gbButton::LEFT);
        }
        if (IsKeyReleased(KEY_LEFT))
        {
            ReleaseButton(gbButton::LEFT);
        }
        if (IsKeyPressed(KEY_RIGHT))
        {
            PressButton(gbButton::RIGHT);
        }
        if (IsKeyReleased(KEY_RIGHT))
        {
            ReleaseButton(gbButton::RIGHT);
        }
        if (IsKeyPressed(KEY_UP))
        {
            PressButton(gbButton::UP);
        }
        if (IsKeyReleased(KEY_UP))
        {
            ReleaseButton(gbButton::UP);
        }
        if (IsKeyPressed(KEY_DOWN))
        {
            PressButton(gbButton::DOWN);
        }
        if (IsKeyReleased(KEY_DOWN))
        {
            ReleaseButton(gbButton::DOWN);
        }


        BeginDrawing();
            ClearBackground(BLUE);

            // Render Screen
            Texture2D text_screen = LoadTextureFromImage(mPpu.mCanvasBuffer);
            DrawTextureEx(text_screen, {200, 200}, 0, 3.0f, WHITE);
            
            {
                // Print Registers
               DrawText( FormatText("AF: 0x%04X\n BC: 0x%04X\n HL: 0x%04X\n DE:0x%04X\n PC: 0x%04X\n SP: 0x%04X\n CONTROL: 0x%02X\n STATUS: 0x%02X\n", mCpu.mRegAF.val, mCpu.mRegBC.val, mCpu.mRegHL.val, mCpu.mRegDE.val, mCpu.mRegPC.val, mCpu.mRegSP.val, mRom[0xFF40], mRom[0xFF41]), 20, 20, 18, RED );
                DrawText( FormatText("CartType: %u RomBanks: %u CurBank: %u", mCart.mMBC, mCart.ReadByte(0x148) * 4, mCart.mCurRomBank), 300, 20, 18, RED );
                DrawText( FormatText("Next OP: 0x%02X", ReadByte(mCpu.mRegPC.val + 1) ), 300, 120, 18, RED );
                DrawText( FormatText("TimerEnabled: %u Timer: 0x%02X", (mRom[0xFF07] >> 2) & 1, ReadByte(0xFF05) ), 300, 140, 18, RED );
                DrawText( FormatText("LY: %u", ReadByte(0xFF44) ), 300, 160, 18, RED );

                // Print Registers  
                DrawText( FormatText("Z"), 160, 80, 18, (mCpu.mRegAF.low & ( 1 << FLAG_Z) ? GREEN : RED) );
                DrawText( FormatText("N"), 180, 80, 18, (mCpu.mRegAF.low & ( 1 << FLAG_N) ? GREEN : RED) );
                DrawText( FormatText("H"), 200, 80, 18, (mCpu.mRegAF.low & ( 1 << FLAG_H) ? GREEN : RED) );
                DrawText( FormatText("C"), 220, 80, 18, (mCpu.mRegAF.low & ( 1 << FLAG_C) ? GREEN : RED) );

                // Requested Interrupt
                DrawText( FormatText("V"), 160, 100, 18, (mRom[0xFF0F] & ( 1 << INTERUPT_VBLANK) ? GREEN : RED) );
                DrawText( FormatText("L"), 180, 100, 18, (mRom[0xFF0F] & ( 1 << INTERUPT_LCD)    ? GREEN : RED) );
                DrawText( FormatText("T"), 200, 100, 18, (mRom[0xFF0F] & ( 1 << INTERUPT_TIMER)  ? GREEN : RED) );
                DrawText( FormatText("J"), 220, 100, 18, (mRom[0xFF0F] & ( 1 << INTERUPT_JOYPAD) ? GREEN : RED) );

                DrawText( FormatText("LH"), 140, 120, 18, (mRom[0xFF41] & ( 1 << 3) ? GREEN : RED) );
                DrawText( FormatText("LV"), 170, 120, 18, (mRom[0xFF41] & ( 1 << 4) ? GREEN : RED) );
                DrawText( FormatText("LO"), 200, 120, 18, (mRom[0xFF41] & ( 1 << 5) ? GREEN : RED) );
                DrawText( FormatText("LY"), 230, 120, 18, (mRom[0xFF41] & ( 1 << 6) ? GREEN : RED) );

                // Enabled Interrupt
                DrawText( FormatText("V"), 160, 140, 18, (mRom[0xFFFF] & ( 1 << INTERUPT_VBLANK) ? GREEN : RED) );
                DrawText( FormatText("L"), 180, 140, 18, (mRom[0xFFFF] & ( 1 << INTERUPT_LCD)    ? GREEN : RED) );
                DrawText( FormatText("T"), 200, 140, 18, (mRom[0xFFFF] & ( 1 << INTERUPT_TIMER)  ? GREEN : RED) );
                DrawText( FormatText("J"), 220, 140, 18, (mRom[0xFFFF] & ( 1 << INTERUPT_JOYPAD) ? GREEN : RED) );

                // Inputs
                DrawText( FormatText("A"), 160, 200, 18, (mBtsPressed >> gbButton::A     ) & 1 ? GREEN : RED );
                DrawText( FormatText("B"), 180, 200, 18, (mBtsPressed >> gbButton::B     ) & 1 ? GREEN : RED );
                DrawText( FormatText("T"), 200, 200, 18, (mBtsPressed >> gbButton::START ) & 1 ? GREEN : RED );
                DrawText( FormatText("S"), 220, 200, 18, (mBtsPressed >> gbButton::SELECT) & 1 ? GREEN : RED );

                DrawText( FormatText("L"), 240, 200, 18, (mBtsPressed >> gbButton::LEFT  ) & 1 ? GREEN : RED );
                DrawText( FormatText("R"), 260, 200, 18, (mBtsPressed >> gbButton::RIGHT ) & 1 ? GREEN : RED );
                DrawText( FormatText("D"), 280, 200, 18, (mBtsPressed >> gbButton::DOWN  ) & 1 ? GREEN : RED );
                DrawText( FormatText("U"), 300, 200, 18, (mBtsPressed >> gbButton::UP    ) & 1 ? GREEN : RED );

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

                DrawText( FormatText("mCyclesDone: %u mEnableInterrupts: %i mIsHalted: %i ScreenEnabled: %i",
                    mCpu.mCyclesDone, mCpu.mEnableInterrupts, mCpu.mIsHalted, (mRom[0xFF40] >> 7) & 1), 500, 110, 18, RED );

                // Print Name
                DrawText( FormatText(" Game: %s", mCart.GetGameName().c_str() ), 300, 60, 18, RED );
                DrawText( FormatText(" Btns: 0x%02x Btns: 0x%02x", mBtsPressed, ReadByte(0xFF00)), 300, 40, 18, RED );
            }


        EndDrawing();

        UnloadTexture(text_screen);
    }

}

void gbGameBoy::PressButton(gbButton _key)
{
    bool was_pressed = ((mBtsPressed >> _key) & 0b1);
    mBtsPressed |= 1 << _key; // In gameboy's memory "0 = pressed", but this makes more sense for more normal people xd.

    // Request an interupt?
    if (!was_pressed) {return;}       // Button was already held
    uint8_t btn_opts = mRom[0xFF00];  // Programmer wants direction or action buttons?
    if ( (_key > 3) && !((btn_opts >> 5) & 0b1) )       // Is action and wants action
    {
        mCpu.RequestInterupt(INTERUPT_JOYPAD);
    }
    else if ( (_key <= 3) && !((btn_opts >> 4) & 0b1) ) // Is direction and wants direction
    {
        mCpu.RequestInterupt(INTERUPT_JOYPAD);
    }
}

void gbGameBoy::ReleaseButton(gbButton _key)
{
    mBtsPressed &= ~(1 << _key);
}

void gbGameBoy::Reset()
{
    mCpu.Reset();
    mCart.Reset();

    // Rom
    mBtsPressed = 0x00;
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

uint8_t gbGameBoy::ReadByte(uint16_t _addr)
{
    // are we reading from the rom memory bank?
    if ((_addr>=0x0000) && (_addr <= 0x7FFF))
    {
        return mCart.ReadByte(_addr);
    }

    // are we reading from ram memory bank?
    else if ((_addr >= 0xA000) && (_addr <= 0xBFFF))
    {
        uint16_t new_addr = _addr - 0xA000 ;
        return 0xFF;//mRam[new_addr + (mCurRamBank*0x2000)] ; 
    }
    else if (_addr == 0xFF00) // Input
    {
        uint8_t buttons = mRom[0xFF00]; // All 0's expect bit 4,5 which (might) be set

        if ( !((buttons >> 4) & 1)) // Wants Directions keys
        {
            buttons |= (mBtsPressed & 0x0F);
        }
        if ( !((buttons >> 5) & 1)) // Wants Actions keys
        {
            buttons |= (mBtsPressed >> 4);
        }
        buttons ^= 0xFF;
        buttons ^= 0b00110000;

        return buttons;
    }
    // else return memory
    return mRom[_addr];
}

// Uses the program counter (without incrementing it)
uint16_t gbGameBoy::ReadWord()
{
    uint16_t res = ReadByte( mCpu.mRegPC.val +1);
	res = res << 8;
	res |= ReadByte( mCpu.mRegPC.val );
	return res;
}

void gbGameBoy::WriteByte(uint16_t _addr, uint8_t _data)
{
    if (_addr < 0x8000) // Cartridge is read only, writing here can be detect by on board hardware mostly for rom banking.
    {
        mCart.WriteByte(_addr, _data);
    } 
    else if ( ( _addr >= 0xE000 ) && (_addr < 0xFE00) ) // Echo ram
    {
        mRom[_addr] = _data;
        WriteByte(_addr-0x2000, _data);
    } 
    else if ( ( _addr >= 0xFEA0 ) && (_addr < 0xFEFF) ) {}                         // this area is restricted
    else if ( _addr == 0xFF04 ) { mRom[0xFF04] = 0x00; mCpu.mDividerCounter = 0; } // Writing to Divier   counter resets it
    else if ( _addr == 0xFF44 ) { mRom[0xFF44] = 0x00; }                           // Writing to Scanline counter resets it
    else if ( _addr == 0xFF07) // Set new counter frequency
    {
        mRom[_addr] = _data;   // Put in the data normally

        // Get a new frequency for the counter
        unsigned int new_freq = 0;
        switch(_data & 0b11)
		{
			case 0b00: new_freq = 1024; break;
			case 0b01: new_freq = 16;   break;
			case 0b10: new_freq = 64;   break;
			case 0b11: new_freq = 256;  break;
		}
        if (new_freq != mCpu.mCounterFreq)
        {
            mCpu.mTimerCounter = 0;
            mCpu.mCounterFreq = new_freq;
        }
    }
    else if ( _addr == 0xFF46 ) // DMA transfer
	{
	    uint16_t newAddress = (_data << 8);
		for (uint16_t i = 0; i < 0xA0; i++)
		{
			mRom[0xFE00 + i] = ReadByte(newAddress + i);
		}
	}
    else 
    {
        mRom[_addr] = _data;
    }
}

} // Namespace
