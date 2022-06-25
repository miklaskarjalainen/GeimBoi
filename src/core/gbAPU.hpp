#pragma once
#include <cstdint>
#include "apu/apuChannel1.hpp"
#include "apu/apuChannel2.hpp"

#define CPU_CYCLES_PER_FRAME 70221

namespace GeimBoi
{

    class gbGameBoy;

    /* Audio Processing Unit */
    // TODO: Frame sequencer, might get rid of all the desyncing issues.
    class gbAPU {
    public:
        gbAPU(gbGameBoy* gb);
        ~gbAPU();

        void Reset();
        void UpdateTimers(uint16_t cycles);
        void WriteByte(uint16_t addr, uint8_t data);

        float masterVolume = 1.0f;
        apuChannel1 channel1;
        apuChannel2 channel2;
    private:
        uint32_t mSequenceCounter = 0;
        double timeElapsed = 0.0;
        gbGameBoy* mGameBoy = nullptr;
    
    private:
        static void sdl2_callback(void* userdata, uint8_t *stream, int len);
    
    };

}
