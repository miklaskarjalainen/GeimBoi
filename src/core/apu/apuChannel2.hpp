#pragma once
#include <cstdint>

namespace GeimBoi
{

    class gbGameBoy;

    class apuChannel2
    {
    public:
        apuChannel2(gbGameBoy* gb);
        ~apuChannel2();

        void UpdateTimers(uint16_t cycles);
        void WriteByte(uint16_t addr, uint8_t data);
        
        double GetAmplitude(double time);
    private:
        bool mEnabled = true;
        double mVolume = .0;
        float mCycleDuty = 0.5f; 
        uint16_t mFreq = 0b0;
        int16_t mSweepTimer = 0;

        double mSoundLength = 0.0; // seconds
        double mSoundLengthTimer = 0.0; // seconds

        double mEnvelopeTimer = 0.0;

    private:
        void DoEnvelope(uint16_t cycles);

        gbGameBoy* mGameBoy;
    };

}
