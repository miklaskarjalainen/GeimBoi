#pragma once
#include <cstdint>

#define HZ(hz) (44100/hz)

namespace GeimBoi
{
    class gbGameBoy;
    class gbAPU;

    class apuChannel3
    {
    public:
        apuChannel3(gbGameBoy* gb);
        ~apuChannel3() = default;
    
    private:
        void Restart();
        void Clock();
        void ClockLength();
        double GetAmplitude(float dt);
        void WriteByte(uint16_t addr, uint8_t data);

    private:
        bool mEnabled = false;
        bool mLengthEnable = false;
        uint8_t mLengthTimer = 0;
        uint8_t mVolume = 0; // 2 bit
        uint16_t mFreq = 0;  // 11 bit

        uint8_t mTableIdx = 0; // 0-31
        uint8_t mSampleBuffer = 0; // 4 bit
        
        gbGameBoy* mGameBoy = nullptr;

        friend gbAPU;
    };
}
