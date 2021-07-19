#include "gbZ80.hpp"
#include "gbPPU.hpp"
#include "gbGameBoy.hpp"

using namespace Giffi;

// Advances the emulator by a single CYCLE
void gbZ80::Clock()
{
    int cur_cycle = mCyclesDone;

    if (!mIsHalted) { ExecuteNextOpcode(); }
    else            { mCyclesDone += 4; }
    
    int delta_cycles = mCyclesDone - cur_cycle;

    UpdateTimers(delta_cycles);
    mGameBoy->mPpu.UpdateGraphics(delta_cycles);
    DoInterupts();
}

// Advance by a frame
void gbZ80::FrameAdvance()
{
    while (mCyclesDone < 70221)
    {
        Clock();
    }

    if (mCyclesDone >= 70221)
    {
        mCyclesDone -= 70221;
    }
}

uint8_t gbZ80::ReadByte(uint16_t _addr) const
{
    return mGameBoy->ReadByte(_addr);
}

void  gbZ80::WriteByte(uint16_t _addr, uint8_t _data)
{
    mGameBoy->WriteByte(_addr, _data);
}

uint16_t gbZ80::ReadWord() const
{
    return mGameBoy->ReadWord();
}

void gbZ80::PushWordOntoStack(uint16_t _word)
{
	uint8_t high = _word >> 8 ;
	uint8_t low = _word & 0xFF;
	mRegSP.val--;
	mGameBoy->WriteByte(mRegSP.val, high);
	mRegSP.val--;
	mGameBoy->WriteByte(mRegSP.val, low);
}

uint16_t gbZ80::PopWordOffStack()
{
	uint16_t word = mGameBoy->ReadByte(mRegSP.val+1) << 8;
	word |= mGameBoy->ReadByte(mRegSP.val);
	mRegSP.val+=2;
	return word;
}

void gbZ80::UpdateTimers( uint16_t cycles )
{
    uint8_t& timer      = mGameBoy->mRom[0xFF05];
    uint8_t  timer_set  = mGameBoy->mRom[0xFF06];

    if ((mGameBoy->mRom[0xFF07] >> 2) & 1) // Timer Enabled
    {
        mTimerCounter += cycles;

        if (mTimerCounter >= mCounterFreq)
        {
            mTimerCounter -= mCounterFreq;
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
		mGameBoy->mRom[0xFF04]++;
	}
}

void gbZ80::RequestInterupt(uint8_t _id)
{
    uint8_t IF = mGameBoy->mRom[0xFF0F];
    IF |= 1 << _id;
    mGameBoy->mRom[0xFF0F] = IF;
} 
   
void gbZ80::DoInterupts()
{
    uint8_t requested = mGameBoy->mRom[0xFF0F];
    uint8_t enabled   = mGameBoy->mRom[0xFFFF];
    
    for (uint8_t i = 0 ; i < 5; i++)
    {
        if ( ((enabled >> i) & 1) && ((requested >> i) & 1))
        {
            mIsHalted = false;
            if (mEnableInterrupts)
            {
                ServiceInterupt(i);
            }
        }
    }  
} 

void gbZ80::ServiceInterupt(uint8_t _interrupt)
{
    mEnableInterrupts = false;

    mGameBoy->mRom[0xFF0F] &= ~(1 << _interrupt);

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
}

void gbZ80::Reset()
{
    mCounterFreq = 0;
    mDividerCounter = 0; 
    mTimerCounter = 0; 
    mCyclesDone = 0;

    mIsHalted = false;
    mEnableInterrupts = false;

    mRegAF.val = 0x01B0;
    mRegBC.val = 0x0013;
    mRegDE.val = 0x00D8;
    mRegHL.val = 0x014D;
    mRegPC.val = 0x0000;
    mRegSP.val = 0xFFFE;
}
