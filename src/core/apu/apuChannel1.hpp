#pragma once
#include <cstdint>

namespace GeimBoi
{

    class gbGameBoy;

    class apuBaseChannel
    {
    public:
        virtual double GetAmplitude(double) = 0;
    };

    class apuChannel1 : public apuBaseChannel
    {
    public:
        apuChannel1(gbGameBoy* gb);
        ~apuChannel1();

        void UpdateTimers(uint16_t cycles);
        
        virtual double GetAmplitude(double time);
    private:
        float mCycleDuty = 0.5f; 
        uint16_t mFreq = 0b0;
        int16_t mSweepTimer = 0;

    private:
        void DoSweep(uint16_t cycles);

        gbGameBoy* mGameBoy;
    };

}
