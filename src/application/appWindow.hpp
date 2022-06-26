#pragma once
#include <memory>
#include "appGui.hpp"
#include "../core/gbGameBoy.hpp"

struct SDL_Window;

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
		void* mGLContext;
		SDL_Window* mWindow = nullptr;
		
		std::unique_ptr<appGui> mGui = nullptr;
		std::shared_ptr<gbGameBoy> mGameBoy = nullptr;
		bool mClosing = false;
	
	private:
		const std::string SettingsPath = "./settings.ini"; 
	};

}
