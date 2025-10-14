#include "Settings.hpp"
#include "log.h"

#define TOML_EXCEPTIONS 0
#include <SDL3/SDL.h>
#include <fstream>
#include <toml++/toml.hpp>

using namespace GeimBoi;

Settings Settings::m_Singleton;

void Settings::load(const std::string& load_path)
{
	// Loads settings from 'settings.toml' file, if it exists.
	// Otherwise, uses the defaults.
	toml::parse_result result = toml::parse_file(load_path);
	if (!result) {
		GB_WARN(
			"Could not open settings from '%s'! Using defaults!",
			load_path.c_str()
		);
		return;
	}

	toml::table table = std::move(result).table();

	auto table_get = [&table](
						 std::string_view path, auto default_value
					 ) -> decltype(default_value) {
		return table.at_path(path).value_or(default_value);
	};

#define GET_SETTING_KEY(setting)                                               \
	SDL_GetScancodeFromName(                                                   \
		table_get(#setting, SDL_GetScancodeName((SDL_Scancode)get().setting))  \
	)
#define GET_SETTING(setting) table_get(#setting, get().setting)

	get().general.last_rompath = GET_SETTING(general.last_rompath);

	get().controls.up = GET_SETTING_KEY(controls.up);
	get().controls.down = GET_SETTING_KEY(controls.down);
	get().controls.right = GET_SETTING_KEY(controls.right);
	get().controls.left = GET_SETTING_KEY(controls.left);
	get().controls.select = GET_SETTING_KEY(controls.select);
	get().controls.start = GET_SETTING_KEY(controls.start);
	get().controls.b = GET_SETTING_KEY(controls.b);
	get().controls.a = GET_SETTING_KEY(controls.a);

#undef GET_SETTING
#undef GET_SETTING_KEY

	GB_INFO("Successfully loaded settings from '%s'", load_path.c_str());
}

void Settings::save(const std::string& save_path)
{
	toml::table table{
		{"general", toml::table{{"last_rompath", get().general.last_rompath}}},
		{"controls",
		 toml::table{
			 {"up", SDL_GetScancodeName((SDL_Scancode)get().controls.up)},
			 {"down", SDL_GetScancodeName((SDL_Scancode)get().controls.down)},
			 {"right", SDL_GetScancodeName((SDL_Scancode)get().controls.right)},
			 {"left", SDL_GetScancodeName((SDL_Scancode)get().controls.left)},
			 {"select",
			  SDL_GetScancodeName((SDL_Scancode)get().controls.select)},
			 {"start", SDL_GetScancodeName((SDL_Scancode)get().controls.start)},
			 {"b", SDL_GetScancodeName((SDL_Scancode)get().controls.b)},
			 {"a", SDL_GetScancodeName((SDL_Scancode)get().controls.a)},
		 }}
	};

	auto file = std::ofstream{save_path};
	file << table;

	GB_INFO("Successfully saved settings to '%s'", save_path.c_str());
}

Settings::Settings() {}

Settings::~Settings() {}

Settings& Settings::get() { return m_Singleton; }
