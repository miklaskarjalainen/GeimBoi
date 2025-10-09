#pragma once

#include <cstdint>
#include <string>

struct SDL_Window;
struct SDL_GLContextState;
typedef struct gb_emu gb_emu_t;

namespace GeimBoi {
int main();

class App {
private:
	SDL_Window* m_Window = nullptr;
	SDL_GLContextState* m_GL = nullptr;
	gb_emu_t* m_Emulator = nullptr;
	uint16_t m_LastExecutedOpcode = 0;
	std::string m_RomPath = "";

private:
	friend int main();
	App();
	~App();

	void reset();
	void run();
};

} // namespace GeimBoi
