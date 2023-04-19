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

bool apuSquare::WriteState(std::ofstream& wf)
{
    wf.write((char*)&mEnabled,          sizeof(mEnabled));
    wf.write((char*)&mCycleDuty,        sizeof(mCycleDuty));
    wf.write((char*)&mFreq,             sizeof(mFreq));
    wf.write((char*)&mVolume,           sizeof(mVolume));
    wf.write((char*)&mSweepTimer,       sizeof(mSweepTimer));
    wf.write((char*)&mEnvelopeTimer,    sizeof(mEnvelopeTimer));
    wf.write((char*)&mLengthTimer,      sizeof(mLengthTimer));
    wf.write((char*)&mLengthEnable,     sizeof(mLengthEnable));
    wf.write((char*)&mEnvelopeEnabled,  sizeof(mEnvelopeEnabled));
    wf.write((char*)&mSweepEnabled,     sizeof(mSweepEnabled));
    return !wf.bad();
}

bool apuSquare::ReadState(std::ifstream& rf)
{
    rf.read((char*)&mEnabled,           sizeof(mEnabled));
    rf.read((char*)&mCycleDuty,         sizeof(mCycleDuty));
    rf.read((char*)&mFreq,              sizeof(mFreq));
    rf.read((char*)&mVolume,            sizeof(mVolume));
    rf.read((char*)&mSweepTimer,        sizeof(mSweepTimer));
    rf.read((char*)&mEnvelopeTimer,     sizeof(mEnvelopeTimer));
    rf.read((char*)&mLengthTimer,       sizeof(mLengthTimer));
    rf.read((char*)&mLengthEnable,      sizeof(mLengthEnable));
    rf.read((char*)&mEnvelopeEnabled,   sizeof(mEnvelopeEnabled));
    rf.read((char*)&mSweepEnabled,      sizeof(mSweepEnabled));
    return !rf.bad();
}

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
        const uint8_t SweepCount = NR12 & 0b111;
        mEnvelopeTimer = SweepCount ? SweepCount : 8;
        if (!SweepCount)
            return;

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
        // higher 3 bits of freq
        mFreq &= ~((0b111) << 8);
        mFreq |= (data & 0b111) << 8;

        // restarting current sound
        const bool Restart = data & (0b1 << 7);
        
        if (Restart)
        {
            mEnabled = true;
            mLengthEnable = data & (0b1 << 6);
            mEnvelopeEnabled = true;
            mSweepEnabled = true;
            mVolume = mGameBoy->ReadByte(mEnvelopeAddr) >> 4;

            const uint8_t sound_length = (mGameBoy->ReadByte(mLengthAddr) & 0b00111111);
            mLengthTimer = 64 - sound_length;
            if (!mLengthTimer)
                mLengthTimer = 64;

            mEnvelopeTimer = mGameBoy->ReadByte(mEnvelopeAddr) & 0b111;
            if (!mEnvelopeTimer)
                mEnvelopeTimer = 8;

            mSweepTimer = (mGameBoy->ReadByte(0xFF10) & 0b01110000) >> 4;
            if (!mSweepTimer)
                mSweepTimer = 8;
        }
        mGameBoy->mRom[addr] = data;
    }
}

double apuSquare::GetAmplitude(double dt) const
{
    if (!mEnabled)
        return 0.0;

    const double v = mVolume == 0 ? 0.0 : (double)mVolume / 15.0;
    const double f = 131072.0/(2048.0-(double)mFreq);
    return HarmonicSquareWave(f, mCycleDuty, 25.0, dt) * v;
}
