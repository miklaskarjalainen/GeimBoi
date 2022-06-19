#include <fstream>
#include <iostream>
#include "application/appWindow.hpp"

using namespace Giffi;

int main(int argc, char* argv[])
{
    const char* loadRom = argc > 1 ? argv[1] : nullptr;
    appWindow window(loadRom, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);
    window.Run();
    return 0;
}
