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
		appWindow();
		~appWindow();
		void Run();

	private:
		bool ShouldWindowClose();
		void DoEvents();

	public:
		std::unique_ptr<appGui> mGui = nullptr;
		std::shared_ptr<gbGameBoy> mGameBoy = nullptr;
		bool mClosing = false;

	private:
		SDL_Renderer* mRenderer = nullptr;
		SDL_Window* mWindow = nullptr;
	};

}
