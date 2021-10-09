#pragma once
#include <memory>
#include <string>
#include "../core/gbGameBoy.hpp"

namespace Giffi
{

class appGui
{
public:
	static void Init(struct SDL_Renderer* _renderer, std::shared_ptr<gbGameBoy>& _emu, int _width, int _height);
	static void Update();
	static void Draw();

	static bool IsPaused();
private:
	static void OpenRomDialog();

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