#include "apuChannel1.hpp"
#include "apuDefenitions.hpp"
#include "../gbGameBoy.hpp"
#include "soundWaves.hpp"

using namespace GeimBoi;

#define CPU_CYCLES_PER_SWEEP 549 // (70221 / 128)

apuChannel1::apuChannel1(gbGameBoy* gb)
    : mGameBoy(gb) {}

apuChannel1::~apuChannel1() {}

void apuChannel1::DoSweep(uint16_t cycles)
{
    uint8_t NR10 = mGameBoy->ReadByte(0xFF10);
    uint8_t SweepTime = (NR10 & 0b01110000) >> 4; 
    if (SweepTime > 0)
    {
        mSweepTimer += cycles;
        if (mSweepTimer > CPU_CYCLES_PER_SWEEP)
        {
            mSweepTimer = 0;
            SweepTime--;
        } 

        const uint8_t SweepCount = (NR10 & 0b111);
        if (SweepTime == 0 && SweepCount != 0)
        {
            const bool Adds = !(NR10 & 0b1000);

            if (Adds)
            {
                mFreq = mFreq + (mFreq / (uint16_t)(SweepCount * SweepCount));
            }
            else
            {
                mFreq = mFreq - (mFreq / (uint16_t)(SweepCount * SweepCount));
            }
        }

        // store new sweep time
        SweepTime <<= 4;
        NR10 &= ~(0b01110000);
        NR10 |= SweepTime;
        mGameBoy->WriteByte(0xFF10, NR10);
    }
}

void apuChannel1::DoEnvelope(uint16_t cycles)
{
    uint8_t NR12 = mGameBoy->ReadByte(0xFF12);
    uint8_t num_sweep = NR12 & 0b111;
    if (num_sweep == 0)
        return;

    constexpr double SECOND = ((double)CPU_CYCLES_PER_FRAME * 60.0);
    constexpr double STEP   = SECOND / (SECOND*64);
    mEnvelopeTimer += (double)cycles / SECOND;

    if (mEnvelopeTimer > (STEP * num_sweep))
    {
        mEnvelopeTimer = 0.0;
        bool increase = (NR12 >> 3) & 0b1;
        uint8_t volume = (NR12 & 0b11110000) >> 4;
        if (increase)
        {
            volume++;
        }
        else
        {
            volume--;
        }
        volume <<= 4;
        NR12 &= ~(0b11110000);
        NR12 |= volume;
        mGameBoy->WriteByte(0xFF12, NR12);
    }
}

void apuChannel1::UpdateTimers(uint16_t cycles)
{
    DoSweep(cycles);
    DoEnvelope(cycles);

    const uint8_t NR14 = mGameBoy->ReadByte(0xFF14);
    mSoundLengthTimer += (double)cycles / ((double)CPU_CYCLES_PER_FRAME * 60.0);
    
    bool use_length = (NR14 >> 6) & 0b1;
    if (use_length && mSoundLengthTimer >= mSoundLength)
    {
        mEnabled = false;
    }

    // handle volume (NR12)
    const uint8_t NR12 = mGameBoy->ReadByte(0xFF12);
    const uint8_t VOLUME = (NR12 & 0b11110000) >> 4;
    mVolume = VOLUME == 0 ? 0.0 : 1.0 * VOLUME / 15;
}

void apuChannel1::WriteByte(uint16_t addr, uint8_t data)
{
    if (addr == 0xFF11)
    {
        // length
        const uint8_t sound_length = (data & 0b00111111);
        mSoundLength = (64.0-(double)sound_length) * (1.0 / 256.0);

        // duty
        switch (data >> 6)
        {
            case 0b00:
                mCycleDuty = 0.125f;
                break;
            case 0b01:
                mCycleDuty = 0.25f;
                break;
            case 0b10:
                mCycleDuty = 0.50f;
                break;
            case 0b11:
                mCycleDuty = 0.75f;
                break;
            default: printf("something went wrong lmao\n");
        }

        mGameBoy->mRom[addr] = data;
    }
    else if (addr == 0xFF13) 
    {
        // lower 8 bits of freq (which is 11 bits)
        mFreq &= ~(0b11111111);
        mFreq |= data;

        mGameBoy->mRom[addr] = data;
    }
    else if (addr == 0xFF14)
    {
        // higher 3 bits of freq
        mFreq &= ~((0b111) << 8);
        mFreq |= (data & 0b111) << 8;

        // restarting current sound
        bool restart = data & (0b1 << 7);
        if (restart)
        {
            mEnabled = true;
            mSoundLengthTimer = 0.0;
        }

        mGameBoy->mRom[addr] = data;
    }
}

double apuChannel1::GetAmplitude(double dt)
{
    if (!mEnabled)
        return 0.0;
        
    double f = 131072.0/(2048.0-(double)mFreq);
    return HarmonicSquareWave(f, mCycleDuty, 20.0, dt) * mVolume;
}
