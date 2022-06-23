#include "apuChannel1.hpp"
#include "../gbGameBoy.hpp"

using namespace GeimBoi;

apuChannel1::apuChannel1(gbGameBoy* gb)
    : apuSquare(gb, 0xFF11, 0xFF12, 0xFF13, 0xFF14) 
{
    Restart();   
}

void apuChannel1::Restart()
{
    apuSquare::Restart();
    mSweepTimer = 8;
}

void apuChannel1::ClockSweep()
{
    uint8_t NR10 = mGameBoy->ReadByte(0xFF10);
    uint8_t SweepTime = (NR10 & 0b01110000) >> 4; 

    if (--mSweepTimer <= 0)
    {
        mSweepTimer = SweepTime;
        if (!SweepTime)
            mSweepTimer = 8;

        const uint8_t SweepCount = (NR10 & 0b111);
        if (SweepTime == 0 && SweepCount != 0)
        {
            const bool Adds = !(NR10 >> 3) & 0b1;
            uint16_t hold_freq = mFreq;

            if (Adds)
                mFreq = mFreq + (mFreq / (uint16_t)(SweepCount * SweepCount));
            else
                mFreq = mFreq - (mFreq / (uint16_t)(SweepCount * SweepCount));

            if (mFreq > 0x7FF)
            {
                mFreq = hold_freq;
                mEnabled = false;
            }
        }

        // store new sweep time
        SweepTime <<= 4;
        NR10 &= ~(0b01110000);
        NR10 |= SweepTime;
        mGameBoy->WriteByte(0xFF10, NR10);
    }
}
