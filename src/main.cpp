#include <utils/Benchmark.hpp>
#include <fstream>
#include "application/appWindow.hpp"

using namespace Giffi;

int main(int argc, char* argv[])
{
    PROFILE_START();

    appWindow window;
    if (argc > 1) 
        window.mGameBoy->LoadRom(argv[1]);
    window.Run();
    
    PROFILE_END();
    return 0;
}
