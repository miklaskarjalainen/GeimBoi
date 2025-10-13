#include "App.hpp"
#include "Settings.hpp"
#include "gui/GuiDebugger.hpp"

extern "C" {
#include "gbCore.h"
#include "gbReg.h"
#include "log.h"
}

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <imgui_memory_editor.h>

static bool
LoadTextureFromMemory(size_t width, size_t height, GLuint* out_texture)
{
	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		width,
		height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		nullptr
	);

	*out_texture = image_texture;
	return true;
}

void UpdateTexture(GLuint tex, int width, int height, const unsigned char* data)
{
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexSubImage2D(
		GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data
	);
}

void GeimBoi::App::process_event(const SDL_Event& ev) noexcept
{
	switch (ev.type) {
		case SDL_EVENT_DROP_FILE: {
			const char* fpath = ev.drop.data;
			open_rom(fpath);
			break;
		}

		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP: {
			auto fn = ev.type == SDL_EVENT_KEY_UP ? gb_emu_release_key
												  : gb_emu_press_key;

			if (ev.key.scancode == SDL_SCANCODE_W) {
				fn(m_Emulator.get(), GB_INPUT_UP);
			}
			if (ev.key.scancode == SDL_SCANCODE_S) {
				fn(m_Emulator.get(), GB_INPUT_DOWN);
			}
			if (ev.key.scancode == SDL_SCANCODE_A) {
				fn(m_Emulator.get(), GB_INPUT_LEFT);
			}
			if (ev.key.scancode == SDL_SCANCODE_D) {
				fn(m_Emulator.get(), GB_INPUT_RIGHT);
			}

			if (ev.key.key == SDLK_J) {
				fn(m_Emulator.get(), GB_INPUT_B);
			}
			if (ev.key.key == SDLK_K) {
				fn(m_Emulator.get(), GB_INPUT_A);
			}
			if (ev.key.key == SDLK_RETURN) {
				fn(m_Emulator.get(), GB_INPUT_START);
			}
			if (ev.key.key == SDLK_BACKSPACE) {
				fn(m_Emulator.get(), GB_INPUT_SELECT);
			}
		}

		default: {
			break;
		}
	}
}

void GeimBoi::App::run()
{
	auto text_opcode = [&](uint16_t addr) -> int {
		uint8_t opcode = gb_mmu_read_u8(&m_Emulator->mmu, addr);
		uint8_t opcode_size = gb_opcode_size(opcode);

		switch (opcode_size) {
			case 3: {
				ImGui::Text(
					"[0x%04X] %s (0x%04X)",
					addr,
					gb_opcode_asm(opcode),
					gb_mmu_read_u16(&m_Emulator->mmu, addr + 1)
				);
				break;
			}
			case 2: {
				ImGui::Text(
					"[0x%04X] %s (0x%02X)",
					addr,
					gb_opcode_asm(opcode),
					gb_mmu_read_u8(&m_Emulator->mmu, addr + 1)
				);
				break;
			}

			default: {
				ImGui::Text("[0x%04X] %s", addr, gb_opcode_asm(opcode));
				break;
			}
		}
		return opcode_size;
	};

	GLuint my_image_texture = 0;
	bool ret =
		LoadTextureFromMemory(GB_LCD_WIDTH, GB_LCD_HEIGHT, &my_image_texture);
	IM_ASSERT(ret);

	bool paused = false;

	auto debugger = GuiDebugger(m_Emulator.get());

	while (!m_Window.should_close()) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			m_Window.process_event(event);
			process_event(event);
		}

		m_Window.render_begin();

		if (!paused && m_IsLoaded) {
			gb_emu_advance_frame(m_Emulator.get());
		}

		UpdateTexture(
			my_image_texture,
			GB_LCD_WIDTH,
			GB_LCD_HEIGHT,
			(unsigned char*)m_Emulator->ppu.frame
		);

		// GUIs
		ImGui::Begin("Control");
		ImGui::Text("Is Game Loaded: %s", m_IsLoaded ? "true" : "false");
		ImGui::Checkbox("Is paused?`", &paused);
		if (ImGui::Button("reset")) {
			reset();
		}

		if (ImGui::Button("Execute opcode")) {
			uint16_t addr = GB_REG_PC(m_Emulator->cpu.regs);
			m_LastExecutedOpcode = addr;
			gb_emu_advance_opcode(m_Emulator.get());
		}

		ImGui::Text("Last Executed Opcode: ");
		ImGui::SameLine();
		text_opcode(m_LastExecutedOpcode);

		ImGui::SeparatorText("Upcoming instructions");
		int offset = 0;
		for (int i = 0; i < 8; i++) {
			uint16_t addr = GB_REG_PC(m_Emulator->cpu.regs) + offset;
			offset += text_opcode(addr);
		}

		ImGui::End();

		ImGui::Begin("Display");
		ImGui::Image(
			(ImTextureID)(intptr_t)my_image_texture,
			ImVec2(GB_LCD_WIDTH * 4, GB_LCD_HEIGHT * 4)
		);
		ImGui::End();

		debugger.draw();

		static MemoryEditor rom_memory = [&]() {
			MemoryEditor mem;
			mem.UserData = reinterpret_cast<void*>(this);
			mem.ReadFn = [](auto, size_t addr, void* void_emu) -> ImU8 {
				const auto* emu =
					reinterpret_cast<App*>(void_emu)->m_Emulator.get();
				return gb_mmu_read_u8(&emu->mmu, (uint16_t)addr);
			};
			mem.WriteFn = [](auto, size_t addr, ImU8 byte, void* void_emu) {
				auto* emu = reinterpret_cast<App*>(void_emu)->m_Emulator.get();
				gb_mmu_write_u8(&emu->mmu, (uint16_t)addr, (u8)byte);
			};
			return mem;
		}();
		rom_memory.DrawWindow("GameBoy memory", nullptr, 0x10000);

		m_Window.render_present();
	}
}

GeimBoi::App::App() : m_Emulator(std::make_unique<gb_emu_t>()) { reset(); }

void GeimBoi::App::open_rom(const char* fpath)
{
	GeimBoi::Settings::get().general.last_rompath = std::string(fpath);
	reset();
}

void GeimBoi::App::reset()
{
	if (m_Emulator) {
		gb_emu_deinit(m_Emulator.get());
	}

	gb_emu_init(m_Emulator.get());
	const auto& rompath = Settings::get().general.last_rompath;
	m_IsLoaded = false;
	if (!rompath.empty()) {
		m_IsLoaded = gb_emu_load_rom_file(m_Emulator.get(), rompath.c_str());
	}
}

GeimBoi::App::~App() = default;
