#pragma once
#include <memory>
#include <string>
#include "../core/gbGameBoy.hpp"

namespace GeimBoi
{

class appGui
{
public:
	appGui(struct SDL_Renderer* renderer, std::shared_ptr<gbGameBoy>& emulator, int width, int height);
	~appGui();

	void Update();
	void Draw();

	bool IsPaused();
private:
	void OpenRomDialog();

	void UpdateTopbar();
	void UpdateDebug();

private:
	std::shared_ptr<gbGameBoy> mGameBoy = nullptr;
	bool mDrawDebug = false;
	bool mEmuPaused = false;
};

} // Namespace