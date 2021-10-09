#include <utils/Benchmark.hpp>
#include <fstream>
#include "application/appWindow.hpp"

using namespace Giffi;

int main(int argc, char* argv[])
{
    PROFILE_START();

    appWindow::Init();
    if (argc > 1) 
        appWindow::mGameBoy->LoadRom(argv[1]);
    appWindow::Run();
    appWindow::CleanUp();

    PROFILE_END();
    return 0;
}
