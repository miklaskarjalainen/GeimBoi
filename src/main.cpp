#include <fstream>
#include <iostream>
#include "application/appWindow.hpp"

using namespace GeimBoi;

int main(int argc, char* argv[])
{
    const char* loadRom = argc > 1 ? argv[1] : nullptr;
    appWindow window(loadRom);
    window.Run();
    return 0;
}
