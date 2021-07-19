#pragma once
#include <memory>
#include <SDL.h>
#include "../core/gbGameBoy.hpp"

namespace Giffi
{

class appWindow
{
public:
	appWindow()  = delete;
	~appWindow() = delete;

	// Retruns true on success
	static bool Init();
	static void Run();
	static void CleanUp();
private:
	static bool ShouldWindowClose();

public:
	static std::shared_ptr<gbGameBoy> mGameBoy;
private:
	static SDL_Renderer* mRenderer;
	static SDL_Window* mWindow;
};

} // Namespace


