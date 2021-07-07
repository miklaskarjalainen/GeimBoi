#include <memory>
#include "emu/emu.hpp"

using namespace Giffi;

int main(void)
{
    // Setup Window //
    InitWindow(1280, 720, "GiffiPaint");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(60);

    // Run Editor //
    {
        std::unique_ptr<GBEmu> editor = std::make_unique<GBEmu>();
        editor->Reset();
        editor->LoadRom("tetris.gb");
        editor->Run();
    }

    return 0;
}
