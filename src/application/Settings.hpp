#pragma once

#include <SDL3/SDL_scancode.h>

#include <cstdint>
#include <string>

namespace GeimBoi {

struct Settings {
public:
	struct {
		// not fs::path, because toml++ doesn't use it.
		std::string last_rompath = "";
	} general;

	struct {
		uint16_t up = SDL_SCANCODE_W;
		uint16_t down = SDL_SCANCODE_S;
		uint16_t right = SDL_SCANCODE_D;
		uint16_t left = SDL_SCANCODE_A;
		uint16_t select = SDL_SCANCODE_BACKSPACE;
		uint16_t start = SDL_SCANCODE_RETURN;
		uint16_t b = SDL_SCANCODE_J;
		uint16_t a = SDL_SCANCODE_K;
	} controls;

	static Settings& get();

	static void load(const std::string& load_path);
	static void save(const std::string& save_path);

private:
	Settings();
	~Settings();

	static Settings m_Singleton;
};

} // namespace GeimBoi
