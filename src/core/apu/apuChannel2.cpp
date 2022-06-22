#include <cassert>
#include "apuChannel2.hpp"
#include "apuDefenitions.hpp"
#include "../gbGameBoy.hpp"
#include "soundWaves.hpp"

using namespace GeimBoi;

apuChannel2::apuChannel2(gbGameBoy* gb)
    : mGameBoy(gb) {}

apuChannel2::~apuChannel2() {}

void apuChannel2::DoEnvelope(uint16_t cycles)
{
    uint8_t NR22 = mGameBoy->ReadByte(0xFF17);
    uint8_t num_sweep = NR22 & 0b111;
    if (num_sweep == 0)
        return;

    constexpr double SECOND = ((double)CPU_CYCLES_PER_FRAME * 60.0);
    constexpr double STEP   = SECOND / (SECOND*64);
    mEnvelopeTimer += (double)cycles / SECOND;

    if (mEnvelopeTimer > (STEP * num_sweep))
    {
        mEnvelopeTimer = 0.0;
        const bool Adds = (NR22 >> 3) & 0b1;
        uint8_t volume = (NR22 & 0b11110000) >> 4;
        if (Adds)
        {
            volume++;
        }
        else
        {
            volume--;
        }
        NR22 &= ~(0b11110000);
        NR22 |= (volume << 4);

        // volume not in range, set volume to 0 and disable envelope.
        if (volume > 15)
        {   
            NR22 &= ~(0b11110111);
        }
        mGameBoy->WriteByte(0xFF12, NR22);
    }
}

void apuChannel2::UpdateTimers(uint16_t cycles)
{
    DoEnvelope(cycles);
    mSoundLengthTimer += (double)cycles / ((double)CPU_CYCLES_PER_FRAME * 60.0);
    
    const uint8_t NR21 = mGameBoy->ReadByte(0xFF16);
    const bool use_length = (NR21 >> 6) & 0b1;
    if (use_length && mSoundLengthTimer >= mSoundLength)
    {
        mEnabled = false;
    }

    // handle volume (NR22)
    const uint8_t NR22 = mGameBoy->ReadByte(0xFF17);
    const uint8_t VOLUME = (NR22 & 0b11110000) >> 4;
    mVolume = VOLUME == 0 ? 0.0 : 1.0 * VOLUME / 15;
}

void apuChannel2::WriteByte(uint16_t addr, uint8_t data)
{
    // Sound/Wave pattern duty
    if (addr == 0xFF16)
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
            default: assert(false);
        }

        mGameBoy->mRom[addr] = data;
    }
    // Frequency low (lower 8 bits of freq (which is 11 bits)
    else if (addr == 0xFF18) 
    {
        mFreq &= ~(0b11111111);
        mFreq |= data;

        mGameBoy->mRom[addr] = data;
    }
    // Frequency high
    else if (addr == 0xFF19)
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

double apuChannel2::GetAmplitude(double dt)
{
    if (!mEnabled)
        return 0.0;

    double f = 131072.0/(2048.0-(double)mFreq);
    return HarmonicSquareWave(f, mCycleDuty, 25.0, dt) * mVolume;
}
