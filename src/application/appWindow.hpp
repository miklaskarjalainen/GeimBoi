#pragma once
#include <memory>
#include <SDL2/SDL.h>

#include "../core/gbGameBoy.hpp"

#include "appGui.hpp"

namespace Giffi
{

	class appWindow
	{
	public:
		appWindow(const char* openRom, int width = SCREEN_WIDTH, int height = SCREEN_HEIGHT);
		~appWindow();
		void Run();
		inline bool ShouldWindowClose() const { return mClosing; }

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
