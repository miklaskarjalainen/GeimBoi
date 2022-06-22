#include <cstring>
#include <string>
#include <filesystem>
#include "gbGameBoy.hpp"

using namespace GeimBoi;

gbGameBoy::gbGameBoy()
    : mApu(this), mCpu(this), mPpu(this), mCart(this)
{
    Reset();
}

void gbGameBoy::Clock()
{
    if (mCart.IsGameLoaded())
    {
        mCpu.Clock();
    }
}

void gbGameBoy::FrameAdvance()
{
    if (mCart.IsGameLoaded())
    {
        mCpu.FrameAdvance();
    }
}

void gbGameBoy::SetPalette(gbColor white, gbColor lgray, gbColor dgray, gbColor black)
{
    mPpu.dmgPalette[0] = white;
    mPpu.dmgPalette[1] = lgray;
    mPpu.dmgPalette[2] = dgray;
    mPpu.dmgPalette[3] = black;
}

void gbGameBoy::PressButton(gbButton key)
{
    bool was_pressed = ((mBtsPressed >> key) & 0b1);
    mBtsPressed |= 1 << key; // In gameboy's memory "0 = pressed", but this makes more sense for more normal people xd.

    // Request an interupt?
    if (!was_pressed) { return; }     // Button was already held
    uint8_t btn_opts = mRom[0xFF00];  // Programmer wants direction or action buttons?
    if ( (key > 3) && !((btn_opts >> 5) & 0b1) )       // Is action and wants action
    {
        mCpu.RequestInterrupt(gbInterrupt::Joypad);
    }
    else if ( (key <= 3) && !((btn_opts >> 4) & 0b1) ) // Is direction and wants direction
    {
        mCpu.RequestInterrupt(gbInterrupt::Joypad);
    }
}

void gbGameBoy::ReleaseButton(gbButton key)
{
    mBtsPressed &= ~(1 << key);
}

void gbGameBoy::Reset()
{
    mApu.Reset();
    mCpu.Reset();
    mCart.Reset();
    mPpu.Reset();
    mBootRom.Reset();

    // Rom
    mBtsPressed = 0x00;
    memset(&mRom, 0x00, sizeof(mRom));
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

uint8_t gbGameBoy::ReadByte(uint16_t addr) const
{
    // Bios
    if (addr < 0x100 && mBootRom.IsBiosLoaded())
    {
        return mBootRom.ReadByte(addr);
    }
    // are we reading from the rom memory bank?
    else if (addr < 0x8000)
    {
        return mCart.ReadByte(addr);
    }
    // are we reading from ram memory bank?
    else if ((addr >= 0xA000) && (addr < 0xC000))
    {
        return mCart.ReadByte(addr);
    }
    else if (addr == 0xFF00) // Input
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
    return mRom[addr];
}

// Uses the program counter (without incrementing it)
uint16_t gbGameBoy::ReadWord() const
{
    uint16_t res = ReadByte( mCpu.mRegPC.val +1);
	res = res << 8;
	res |= ReadByte( mCpu.mRegPC.val );
	return res;
}

uint16_t gbGameBoy::ReadWord(uint16_t addr) const
{
    uint16_t res = ReadByte(addr + 1);
    res = res << 8;
    res |= ReadByte(addr);
    return res;
}

void gbGameBoy::WriteByte(uint16_t addr, uint8_t data)
{
    if (addr < 0x8000) // Cartridge is read only, writing here can be detect by on board hardware mostly for rom banking.
    {
        mCart.WriteByte(addr, data);
    } 
    else if ( ( addr >= 0xA000 ) && ( addr < 0xC000 ) ) // Ram Bank
    {
        mCart.WriteByte(addr, data);
    }
    else if ( ( addr >= 0xE000 ) && ( addr < 0xFE00 ) ) // Echo ram
    {
        mRom[addr] = data;
        WriteByte(addr-0x2000, data);
    } 
    else if ( ( addr >= 0xFEA0 ) && (addr < 0xFEFF) ) {}                         // this area is restricted
    else if ( addr == 0xFF04 ) { mRom[0xFF04] = 0x00; mCpu.mDividerCounter = 0; } // Writing to Divier   counter resets it
    else if ( addr == 0xFF44 ) { mRom[0xFF44] = 0x00; }                           // Writing to Scanline counter resets it
    else if ( addr == 0xFF07) // Set new counter frequency
    {
        mRom[addr] = data;   // Put in the data normally

        // Get a new frequency for the counter
        unsigned int new_freq = 0;
        switch(data & 0b11)
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
    else if (addr == 0xFF11 || addr == 0xFF13 || addr == 0xFF14) // Apu (channel1)
    {
        mApu.WriteByte(addr, data);
    } 
    else if (addr == 0xFF16 || addr == 0xFF18 || addr == 0xFF19) // Apu (channel2)
    {
        mApu.WriteByte(addr, data);
    }
    else if ( addr == 0xFF40 ) // Control
    {
        bool lcd_enabled  = (mRom[0xFF40] >> 7) & 1;
        bool would_enable = (data >> 7) & 1;
        mRom[addr] = data;
        if (!lcd_enabled && would_enable) // LCD got enabled.
        {
            mPpu.CheckCoinsidenceFlag();
        }
    } 
    else if ( addr == 0xFF41 ) // STAT
    {
        mRom[addr] = data;
        mCpu.RequestInterrupt(gbInterrupt::LCD); // A bug that occurs only on the dmg model.
    }
    else if ( addr == 0xFF46 ) // DMA transfer
	{
	    uint16_t newAddress = (data << 8);
		for (uint16_t i = 0; i < 0xA0; i++)
		{
			mRom[0xFE00 + i] = ReadByte(newAddress + i);
		}
	}   
    else 
    {
        mRom[addr] = data;
    }
}
