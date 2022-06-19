#pragma once
#include <memory>
#include <SDL2/SDL.h>
#include "appGui.hpp"
#include "../core/gbGameBoy.hpp"

namespace GeimBoi
{

	class appWindow
	{
	public:
		appWindow(const char* openRom);
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
	
	private:
		const std::string SettingsPath = "./settings.ini"; 
	};

}
