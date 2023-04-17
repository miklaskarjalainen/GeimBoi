#pragma once
#include <memory>
#include "../core/gbGameBoy.hpp"

namespace GeimBoi
{

class appGui
{
public:
	appGui(struct SDL_Window* window, void* context, std::shared_ptr<gbGameBoy>& emulator);
	~appGui();

	void Draw();
	void Render();   

private:
	void OpenRomDialog();

	void DrawTopbar();
	void DrawOptions();
	void DrawDebug();
	void DrawAuthors();
	void DrawLicences();
	void DrawInfo();

private:
	std::shared_ptr<gbGameBoy> mGameBoy = nullptr;
	bool mDrawOptions = false;
	bool mDrawDebug = false;
	bool mDrawAuthors = false;
	bool mDrawLicences = false;
	bool mDrawInfo = false;
};

} // Namespace