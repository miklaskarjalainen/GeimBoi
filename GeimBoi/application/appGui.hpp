#pragma once
#include <SDL.h>
#include <memory>
#include "../core/gbGameBoy.hpp"

namespace Giffi
{

class appGui
{
public:
	static void Init(SDL_Renderer* _renderer, std::shared_ptr<gbGameBoy>& _emu);
	static void Update();

	static void Draw();
private:
	static void UpdateTopbar();
	static void UpdateDebug();

private:
	static std::shared_ptr<gbGameBoy> mGameBoy;
	static bool mDrawDebug;
	static bool mEmuPaused;


	appGui() = delete;
	~appGui() = delete;
};

} // Namespace