#pragma once
#include <memory>
#include <thread>
#include <SDL2/SDL.h>
#include "../core/gbGameBoy.hpp"

namespace Giffi
{
	class appGui;

	class appWindow
	{
	public:
		appWindow(const char* openRom, int width = SCREEN_WIDTH, int height = SCREEN_HEIGHT);
		~appWindow();
		void Run();
		inline bool ShouldWindowClose() { return mClosing; }

	private:
		void DoEvents();

	private:
		SDL_Renderer * mRenderer = nullptr;
		SDL_Window* mWindow = nullptr;
		
		std::unique_ptr<appGui> mGui = nullptr;
		std::shared_ptr<gbGameBoy> mGameBoy = nullptr;
		bool mClosing = false;
	};

}
