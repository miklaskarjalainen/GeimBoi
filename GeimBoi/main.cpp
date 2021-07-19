#include "application/appWindow.hpp"

using namespace Giffi;

int main(int argc, char* argv[])
{
    if (!appWindow::Init())
    {
        printf("Couldn't initalize a window\n");
        return 1;
    }

    if (argc > 1)
    {
        appWindow::mGameBoy->LoadRom(argv[1]);
    }
    else
    {
        appWindow::mGameBoy->LoadRom("tetris.gb");
    }

    appWindow::Run();
    appWindow::CleanUp();

    return 0;
}
