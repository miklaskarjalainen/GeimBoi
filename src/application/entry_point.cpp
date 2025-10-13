#include <SDL3/SDL.h>
#include <imgui.h>

#include "App.hpp"
#include "Settings.hpp"

static const std::string s_SettingsFilePath = "./settings.toml";

namespace GeimBoi {
int main();
}

int GeimBoi::main()
{
	GeimBoi::Settings::load(s_SettingsFilePath);
	GeimBoi::App app;
	app.run();
	GeimBoi::Settings::save(s_SettingsFilePath);
	return 0;
}
