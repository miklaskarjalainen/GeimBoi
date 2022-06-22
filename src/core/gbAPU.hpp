#pragma once
#include <functional>
#include <cstdint>
#include <array>
#include "apu/apuChannel1.hpp"
#include "apu/apuChannel2.hpp"

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

        apuChannel1 channel1;
        apuChannel2 channel2;
    private:
        double timeElapsed = 0.0;
        gbGameBoy* mGameBoy;
    
    private:
        static void sdl2_callback(void* userdata, uint8_t *stream, int len);
    
    };

}
