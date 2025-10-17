/* SPDX-License-Identifier: GPL-2.0-only */

#pragma once

#include "Window.hpp"

#include <SDL3/SDL_opengl.h>
#include <cstdint>
#include <memory>

typedef struct gb_emu gb_emu_t;

namespace GeimBoi {
int main();

class App {
private:
	Window m_Window;
	std::unique_ptr<gb_emu_t> m_Emulator;
	bool m_IsLoaded = false, m_Paused = false, m_LimitFPS = true;
	uint16_t m_LastExecutedOpcode = 0;
	GLuint m_PpuTexture = 0;

private:
	friend int main();
	App();
	~App();

	void process_event(const SDL_Event& ev) noexcept;
	void open_rom(const char* fpath);
	void reset();
	void run();

	void draw_display();
	void draw_control();
};

} // namespace GeimBoi
