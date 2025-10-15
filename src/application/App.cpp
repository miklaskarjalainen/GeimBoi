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
#include <chrono>
#include <fstream>
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

			if (ev.key.scancode == Settings::get().controls.up) {
				fn(m_Emulator.get(), GB_INPUT_UP);
			}
			if (ev.key.scancode == Settings::get().controls.down) {
				fn(m_Emulator.get(), GB_INPUT_DOWN);
			}
			if (ev.key.scancode == Settings::get().controls.left) {
				fn(m_Emulator.get(), GB_INPUT_LEFT);
			}
			if (ev.key.scancode == Settings::get().controls.right) {
				fn(m_Emulator.get(), GB_INPUT_RIGHT);
			}

			if (ev.key.scancode == Settings::get().controls.b) {
				fn(m_Emulator.get(), GB_INPUT_B);
			}
			if (ev.key.scancode == Settings::get().controls.a) {
				fn(m_Emulator.get(), GB_INPUT_A);
			}
			if (ev.key.scancode == Settings::get().controls.start) {
				fn(m_Emulator.get(), GB_INPUT_START);
			}
			if (ev.key.scancode == Settings::get().controls.select) {
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
	using Clock = std::chrono::high_resolution_clock;

	auto debugger = GuiDebugger(m_Emulator.get());
	auto rom_memory = [&]() -> MemoryEditor {
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

	constexpr double TARGET_FPS = 59.7275;
	constexpr double FRAME_TIME_SEC = 1.0 / TARGET_FPS;

	while (!m_Window.should_close()) {
		uint64_t frame_time_begin = SDL_GetPerformanceCounter();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			m_Window.process_event(event);
			process_event(event);
		}

		if (!m_Paused && m_IsLoaded) {
			gb_emu_advance_frame(m_Emulator.get());
		}

		UpdateTexture(
			m_PpuTexture,
			GB_LCD_WIDTH,
			GB_LCD_HEIGHT,
			(unsigned char*)m_Emulator->ppu.frame
		);

		m_Window.render_begin();

		// GUIs
		draw_control();
		draw_display();
		debugger.draw();
		rom_memory.DrawWindow("GameBoy memory", nullptr, 0x10000);

		m_Window.render_present();

		// Limit FPS
		const uint64_t frame_time_end = SDL_GetPerformanceCounter();
		const double elapsed_sec =
			static_cast<double>(frame_time_end - frame_time_begin) /
			SDL_GetPerformanceFrequency();
		const double delay_time_ms = (FRAME_TIME_SEC - elapsed_sec) * 1000.0;
		if (delay_time_ms > 0 && m_LimitFPS) {
			SDL_Delay(delay_time_ms);
		}
	}
}

void GeimBoi::App::open_rom(const char* fpath)
{
	GeimBoi::Settings::get().general.last_rompath = std::string(fpath);
	reset();
}

void GeimBoi::App::reset()
{
	if (m_Emulator) {
		// Save SAV
		u8* buffer = new u8[GB_MAX_RAMSIZE];

		size_t copied =
			gb_cart_read_battery(&m_Emulator->cart, buffer, GB_MAX_RAMSIZE);

		if (copied) {
			std::string game_name = gb_cart_get_name(&m_Emulator->cart);
			std::ofstream file{
				game_name + ".sav", std::ios::out | std::ios::binary
			};
			file.write((const char*)buffer, copied);
			GB_INFO("Writing save file! Bytes %zu written!", copied);
		}

		delete[] buffer;

		gb_emu_deinit(m_Emulator.get());
	}

	gb_emu_init(m_Emulator.get(), 1);
	const auto& rompath = Settings::get().general.last_rompath;
	m_IsLoaded = false;
	if (!rompath.empty()) {
		m_IsLoaded = gb_emu_load_rom_file(m_Emulator.get(), rompath.c_str());
		const u8 cgb = m_Emulator->cart.rom[0x143] == 0x80 ||
					   m_Emulator->cart.rom[0x143] == 0xC0;
		m_Emulator->cgb_mode = cgb;

		// Load SAV
		std::string game_name = gb_cart_get_name(&m_Emulator->cart);
		std::ifstream file{game_name + ".sav", std::ios::in | std::ios::binary};
		if (file.is_open()) {
			GB_INFO("Trying to load %i", file.is_open());
			u8* buffer = new u8[GB_MAX_RAMSIZE];

			file.read((char*)buffer, GB_MAX_RAMSIZE);

			size_t copied = gb_cart_write_battery(
				&m_Emulator->cart, buffer, GB_MAX_RAMSIZE
			);
			GB_INFO("Loading save file! Bytes %zu copied!", copied);
			delete[] buffer;
		}
	}
}

void GeimBoi::App::draw_display()
{
	ImGui::Begin("Display");
	ImGui::Image(
		(ImTextureID)(intptr_t)m_PpuTexture,
		ImVec2(GB_LCD_WIDTH * 4, GB_LCD_HEIGHT * 4)
	);
	ImGui::End();
}

void GeimBoi::App::draw_control()
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

	ImGui::Begin("Control");
	ImGui::Text("Is Game Loaded: %s", m_IsLoaded ? "true" : "false");
	ImGui::Checkbox("Is paused?", &m_Paused);
	ImGui::Checkbox("Limit FPS?", &m_LimitFPS);
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
}

GeimBoi::App::App() : m_Emulator(std::make_unique<gb_emu_t>())
{
	reset();
	LoadTextureFromMemory(GB_LCD_WIDTH, GB_LCD_HEIGHT, &m_PpuTexture);
}

GeimBoi::App::~App() { reset(); }
