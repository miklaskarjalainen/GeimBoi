#include <cassert>
#include "apuChannel3.hpp"
#include "../gbAPU.hpp"
#include "../gbGameBoy.hpp"

using namespace GeimBoi;

apuChannel3::apuChannel3(gbGameBoy* gb)
    : mGameBoy(gb)
{

}

void apuChannel3::Restart()
{
    mEnabled = true;
    mLengthEnable = mGameBoy->ReadByte(0xFF1E) & (0b1 << 6);
    mLengthTimer = 256 - mGameBoy->ReadByte(0xFF1B);
    mTableIdx = 0;
}

void apuChannel3::Clock()
{

}

void apuChannel3::ClockLength()
{
    if (mLengthEnable && mLengthTimer > 0)
    {
        if (--mLengthTimer == 0)
            mEnabled = false;
    }
}

double apuChannel3::GetAmplitude(float dt)
{
    if (!mEnabled)
        return 0.0;

    const int16_t Hz = (2048-mFreq)*2;
    static int32_t freqCounter = HZ(Hz);
    freqCounter--;
    if (freqCounter <= 0)
    {
        freqCounter += HZ(Hz);
        mTableIdx = (mTableIdx + 1) &31;
    }

    if (mVolume == 0)
        return 0.0;
    
    mSampleBuffer = mGameBoy->ReadByte((0xFF30 + mTableIdx) / 2);
    if (mTableIdx & 1)
        mSampleBuffer &= 0xF;
    else
        mSampleBuffer >>= 4;

    uint8_t v = (mSampleBuffer >> (mVolume - 1));
    return 0.0;
}

void apuChannel3::WriteByte(uint16_t addr, uint8_t data)
{
    if (addr == 0xFF1A) // DAC power
    {
        const uint8_t Masked = data & (0b1 << 7);
        mGameBoy->mRom[addr] = Masked;
    }
    else if (addr == 0xFF1B) // Length Load (256-val)
    {
        mLengthTimer = 256 - data;
    }
    else if (addr == 0xFF1C) // Volume Code
    {
        const uint8_t Masked = (data & 0b01100000);
        mVolume = Masked >> 5;
        mGameBoy->mRom[addr] = Masked;
    }
    else if (addr == 0xFF1D) // Freq LSB
    {
        mFreq &= ~(0xFF);
        mFreq |= data;
    }
    else if (addr == 0xFF1E) // Trigger, Length Enable, Freq MSB
    {
        mGameBoy->mRom[addr] = data;

        const uint8_t FreqMsb = data & 0b111;
        mFreq &= ~(0x700);
        mFreq |= (FreqMsb << 8);

        const bool Trigger = data & (0b1 << 7);
        if (Trigger)
        {
            Restart();
        }
    }
}

