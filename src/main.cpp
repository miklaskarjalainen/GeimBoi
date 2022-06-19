#include <utils/Benchmark.hpp>
#include <fstream>
#include <iostream>
#include "application/appWindow.hpp"

using namespace Giffi;

int main(int argc, char* argv[])
{
    PROFILE_START();

    const char* loadRom = argc > 1 ? argv[1] : nullptr;
    appWindow window(loadRom, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);
    window.Run();
    
    PROFILE_END();
    return 0;
}
