#include "apuChannel1.hpp"
#include "../gbGameBoy.hpp"

using namespace GeimBoi;

apuChannel1::apuChannel1(gbGameBoy* gb)
    : apuSquare(gb, 0xFF11, 0xFF12, 0xFF13, 0xFF14) 
{
    
}

void apuChannel1::ClockSweep()
{
    uint8_t NR10 = mGameBoy->ReadByte(0xFF10);
    uint8_t SweepPeriod = (NR10 & 0b01110000) >> 4; 

    if (mSweepEnabled && --mSweepTimer <= 0)
    {
        const uint8_t SweepShift = (NR10 & 0b111);
        mSweepTimer = SweepPeriod ? SweepPeriod : 8;
        mSweepEnabled = SweepPeriod != 0 || SweepShift != 0;

        if (SweepPeriod == 0 && SweepShift != 0)
        {
            const bool Adds = !((NR10 >> 3) & 0b1);
            uint16_t hold_freq = mFreq;

            if (Adds)
                mFreq = mFreq + (mFreq / (uint16_t)(SweepShift * SweepShift));
            else
                mFreq = mFreq - (mFreq / (uint16_t)(SweepShift * SweepShift));

            if (mFreq > 2047)
            {
                mFreq = hold_freq;
                mEnabled = false;
            }
        }

        SweepPeriod <<= 4;
        NR10 &= ~(0b01110000);
        NR10 |= SweepPeriod;
        mGameBoy->WriteByte(0xFF10, NR10); 
    }
}
