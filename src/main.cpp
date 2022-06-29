#include <SDL2/SDL.h> //! gives a linking error if not included in visual studio, and only in "release" configuration.
#include "application/appWindow.hpp"
using namespace GeimBoi;

int main(int argc, char* argv[])
{
    const char* loadRom = argc > 1 ? argv[1] : nullptr;
    appWindow window(loadRom);
    window.Run();
    return 0;
}
