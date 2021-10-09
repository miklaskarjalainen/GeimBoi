#pragma once
#include <memory>
#include <thread>
#include <SDL2/SDL.h>
#include "../core/gbGameBoy.hpp"

namespace Giffi
{

class appWindow
{
public:
	// Retruns true on success
	static void Init();
	static void Run();
	static void CleanUp();
private:
	static bool ShouldWindowClose();
	static void DoEvents();

public:
	static std::shared_ptr<gbGameBoy> mGameBoy;
	static bool mClosing;

private:
	static SDL_Renderer* mRenderer;
	static SDL_Window* mWindow;

	appWindow() = delete;
	~appWindow() = delete;
};

} // Namespace


