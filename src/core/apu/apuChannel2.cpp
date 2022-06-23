#include "apuChannel2.hpp"

using namespace GeimBoi;

apuChannel2::apuChannel2(gbGameBoy* gb)
    : apuSquare(gb, 0xFF16, 0xFF17, 0xFF18, 0xFF19)
{
    
}
