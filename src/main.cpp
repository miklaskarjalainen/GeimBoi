#include <memory>
#include "emu/gbGameBoy.hpp"

using namespace Giffi;

int main(int argc, char* argv[])
{
    // Setup Window //
    InitWindow(1280, 720, "GeimBoy");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(60);

    // Run Editor //
    {
        std::unique_ptr<gbGameBoy> emulator = std::make_unique<gbGameBoy>();
        emulator->Reset();
        if (argc > 1)
        {
            emulator->LoadRom(argv[1]);
        }
        else
        {
            emulator->LoadRom("tetris.gb");
        }
        emulator->Run();
    }

    return 0;
}
