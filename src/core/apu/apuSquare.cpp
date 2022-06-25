#include <cassert>
#include "apuSquare.hpp"
#include "soundWaves.hpp"
#include "../gbGameBoy.hpp"

using namespace GeimBoi;

apuSquare::apuSquare(gbGameBoy* gb, uint16_t length_addr, uint16_t envelope_addr, uint16_t freq_lo_addr, uint16_t freq_hi_addr)
    : mGameBoy(gb), mLengthAddr(length_addr), mEnvelopeAddr(envelope_addr), mFreqLoAddr(freq_lo_addr), mFreqHiAddr(freq_hi_addr)
{ 
    Restart(); 
}

apuSquare::~apuSquare() {}

void apuSquare::Restart()
{
    mEnabled = false;
    mVolume = (mGameBoy->ReadByte(mEnvelopeAddr) & 0b11110000) >> 4;;
    mEnvelopeTimer = (mGameBoy->ReadByte(mEnvelopeAddr) & 0b111);
    mEnvelopeEnabled = mEnvelopeTimer != 0;
    
    // length
    const uint8_t LengthData = mGameBoy->ReadByte(mLengthAddr);
    mLengthTimer = 64 - (LengthData & 0b00111111);

    // duty
    switch (LengthData >> 6)
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

    mSweepTimer = 8;
    const uint8_t sound_length = (mGameBoy->ReadByte(mLengthAddr) & 0b00111111);
    mLengthTimer = 0b00111111 - sound_length;

    mFreq = mGameBoy->ReadByte(mFreqLoAddr);
    mFreq |= (mGameBoy->ReadByte(mFreqHiAddr) & 0b111) << 8;
}

void apuSquare::ClockLength()
{
    if (mLengthEnable && mLengthTimer > 0)
    {
        if (--mLengthTimer == 0)
            mEnabled = false;
    }
}

void apuSquare::ClockEnvelope()
{
    uint8_t NR12 = mGameBoy->ReadByte(mEnvelopeAddr);

    if (mEnvelopeEnabled && --mEnvelopeTimer <= 0)
    {
        uint8_t num_sweep = NR12 & 0b111;
        mEnvelopeTimer = num_sweep ? num_sweep : 8;

        const bool Adds = (NR12 >> 3) & 0b1;
        uint8_t hold_volume = mVolume;
        if (Adds)
            hold_volume++;
        else
            hold_volume--;
        
        if (hold_volume <= 15)
            mVolume = hold_volume;
        else
            mEnvelopeEnabled = false;
    }
}

void apuSquare::WriteByte(uint16_t addr, uint8_t data)
{
    // Sound/Wave pattern duty
    if (addr == mLengthAddr)
    {
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
        mLengthTimer = 64 - (data & 0b00111111);

        mGameBoy->mRom[addr] = data;
    }
    // Envelope
    else if (addr == mEnvelopeAddr) 
    {
        mGameBoy->mRom[addr] = data;
    }
    // Frequency low (lower 8 bits of freq (which is 11 bits)
    else if (addr == mFreqLoAddr) 
    {
        mFreq &= ~(0xFF);
        mFreq |= data;
        mGameBoy->mRom[addr] = data;
    }
    // Frequency high
    else if (addr == mFreqHiAddr)
    {
        mFreq &= ~((0b111) << 8);
        mFreq |= (data & 0b111) << 8; 

        // restarting current sound
        mEnabled = (data & (1 << 7)) != 0;
        mLengthEnable = (data & (1 << 6)) != 0;
        mVolume = mGameBoy->mRom[mEnvelopeAddr] >> 4;
        if (mEnabled && mLengthTimer == 0)
        {
            const uint8_t sound_length = (mGameBoy->ReadByte(mLengthAddr) & 0b00111111);
            mLengthTimer = 64 - sound_length;
        }
        mEnvelopeEnabled = true;


        mGameBoy->mRom[addr] = data;
    }
}

double apuSquare::GetAmplitude(double dt)
{
    if (!mEnabled)
        return 0.0;

    double v = mVolume == 0 ? 0.0 : (double)mVolume / 15.0;
    double f = 131072.0/(2048.0-(double)mFreq);
    return HarmonicSquareWave(f, mCycleDuty, 25.0, dt) * v;
}
