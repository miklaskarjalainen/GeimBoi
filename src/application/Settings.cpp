#include "Settings.hpp"
#include "log.h"

#define TOML_EXCEPTIONS 0
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

#define LOAD_FROM_FILE(setting)                                                \
	get().setting = table.at_path(#setting).value_or(get().setting)

	LOAD_FROM_FILE(general.last_rompath);

#undef LOAD_FROM_FILE

	GB_INFO("Successfully loaded settings from '%s'", load_path.c_str());
}

void Settings::save(const std::string& save_path)
{
	toml::table table{
		{"general", toml::table{{"last_rompath", get().general.last_rompath}}},
		{"controls",
		 toml::table{
			 {"up", get().controls.up},
			 {"down", get().controls.down},
			 {"right", get().controls.right},
			 {"left", get().controls.left},
			 {"select", get().controls.select},
			 {"start", get().controls.start},
			 {"b", get().controls.b},
			 {"a", get().controls.a},
		 }}
	};

	auto file = std::ofstream{save_path};
	file << table;

	GB_INFO("Successfully saved settings to '%s'", save_path.c_str());
}

Settings::Settings() {}

Settings::~Settings() {}

Settings& Settings::get() { return m_Singleton; }
