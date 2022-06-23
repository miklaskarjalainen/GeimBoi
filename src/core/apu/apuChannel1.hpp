#pragma once
#include <cstdint>
#include "apuSquare.hpp"

namespace GeimBoi
{

    class gbGameBoy;
    class gbAPU;

    class apuChannel1 : public apuSquare
    {
    public:
        apuChannel1(gbGameBoy* gb);
        ~apuChannel1() = default;

        void Restart();
    private:
        void ClockSweep();
    
    private:
        uint8_t mSweepTimer = 0;

        friend gbAPU;
    };

}
