#pragma once
#include <cstdint>

namespace GeimBoi
{

    class gbGameBoy;
    class gbAPU;

    class apuSquare
    {
    public:
        apuSquare(gbGameBoy* gb, uint16_t length_addr, uint16_t envelope_addr, uint16_t freq_lo_addr, uint16_t freq_hi_addr);
        ~apuSquare();

        void Restart();

    private:
        void WriteByte(uint16_t addr, uint8_t data);
        void ClockEnvelope();
        void ClockLength();
        double GetAmplitude(double time);
    
    protected:
        bool mEnabled = true;
        float mCycleDuty = 0.5f; 
        uint16_t mFreq = 0b0;
        uint8_t mVolume = 0;
        uint8_t mSweepTimer = 0;
        uint8_t mEnvelopeTimer = 0;
        uint8_t mLengthTimer = 0;
        bool mLengthEnable = false;
        bool mEnvelopeEnabled = false;

        gbGameBoy* mGameBoy;

    private:
        uint16_t mLengthAddr, mEnvelopeAddr, mFreqLoAddr, mFreqHiAddr;

        friend gbAPU;
    };

}
