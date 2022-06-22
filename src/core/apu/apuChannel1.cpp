#include "apuChannel1.hpp"
#include "../gbGameBoy.hpp"
#include "soundWaves.hpp"

using namespace GeimBoi;

static double volume = 1.0f;
#define CPU_CYCLES_PER_FRAME 70221
#define CPU_CYCLES_PER_SWEEP 549

#define SOUND_LENGTH(t1) ((64.0-t1)*(1.0/256.0)) // seconds

apuChannel1::apuChannel1(gbGameBoy* gb)
    : mGameBoy(gb)
{

}

apuChannel1::~apuChannel1()
{
    
}

void apuChannel1::DoSweep(uint16_t cycles)
{
    const uint8_t NR10 = mGameBoy->ReadByte(0xFF10);
    const uint8_t SweepTime = (NR10 & 0b01110000) >> 4; 
    if (SweepTime > 0)
    {
        mSweepTimer += cycles;
        if (mSweepTimer > CPU_CYCLES_PER_SWEEP)
        {
            const bool Adds = !(NR10 & 0b1000);
            const uint8_t SweepCount = (NR10 & 0b111);
            if (SweepCount == 0) return;

            if (Adds)
            {
                mFreq = mFreq + (mFreq / (uint16_t)(SweepCount * SweepCount));
            }
            else
            {
                mFreq = mFreq - (mFreq / (uint16_t)(SweepCount * SweepCount));
            }
        }

    }
}

void apuChannel1::UpdateTimers(uint16_t cycles)
{
    DoSweep(cycles);

    // square wave length
    const uint8_t NR11 = mGameBoy->ReadByte(0xFF11);
    const uint8_t WAVE_DUTY = (NR11 >> 6); 
    switch (WAVE_DUTY)
    {
        // these are opposite than in the docs, because of the squarewave function. 
        case 0b00:
            mCycleDuty = 0.75f;
            break;
        case 0b01:
            mCycleDuty = 0.50f;
            break;
        case 0b10:
            mCycleDuty = 0.25f;
            break;
        case 0b11:
            mCycleDuty = 0.125f;
            break;
        default: printf("something went wrong lmao\n");
    }

    // handle volume (NR12)
    const uint8_t NR12 = mGameBoy->ReadByte(0xFF12);
    const uint8_t VOLUME = (NR12 & 0b11110000) >> 4;
    volume = VOLUME == 0 ? volume = 0.0 : volume = 1.0 * VOLUME / 15;

    // calculate frequency (NR13 & NR14)
    const uint16_t NR14 = mGameBoy->ReadByte(0xFF14);
    uint16_t frequency = mGameBoy->ReadByte(0xFF13);
    mFreq = ((NR14 & 0b111) << 8) | frequency;
}

double apuChannel1::GetAmplitude(double dt)
{
    double f = 131072.0/(2048.0-(double)mFreq);
    return HarmonicSquareWave(f, mCycleDuty, 20.0, dt) * volume;
}
