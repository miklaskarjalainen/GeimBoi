#pragma once
#include "apuSquare.hpp"

namespace GeimBoi
{
    class gbGameBoy;

    class apuChannel2 : public apuSquare
    {
    public:
        apuChannel2(gbGameBoy* gb);
        ~apuChannel2() = default;
    };
}
