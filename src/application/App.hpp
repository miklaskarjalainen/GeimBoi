#pragma once

#include "Window.hpp"

#include <cstdint>
#include <memory>

typedef struct gb_emu gb_emu_t;

namespace GeimBoi {
int main();

class App {
private:
	Window m_Window;
	std::unique_ptr<gb_emu_t> m_Emulator;
	bool m_IsLoaded = false;
	uint16_t m_LastExecutedOpcode = 0;

private:
	friend int main();
	App();
	~App();

	void process_event(const SDL_Event& ev) noexcept;
	void open_rom(const char* fpath);
	void reset();
	void run();
};

} // namespace GeimBoi
