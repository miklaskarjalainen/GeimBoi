#include "App.hpp"
#include "gbPPU.h"

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

void GeimBoi::App::run()
{
	// Main loop
	bool done = false;

	ImGuiIO& io = ImGui::GetIO();
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	auto text_opcode = [&](uint16_t addr) -> int {
		uint8_t opcode = gb_emu_read_u8(m_Emulator, addr);
		uint8_t opcode_size = gb_opcode_size(opcode);

		switch (opcode_size) {
			case 3: {
				ImGui::Text(
					"[0x%04X] %s (0x%04X)",
					addr,
					gb_opcode_asm(opcode),
					gb_emu_read_u16(m_Emulator, addr + 1)
				);
				break;
			}
			case 2: {
				ImGui::Text(
					"[0x%04X] %s (0x%02X)",
					addr,
					gb_opcode_asm(opcode),
					gb_emu_read_u8(m_Emulator, addr + 1)
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

	while (!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				done = true;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
				event.window.windowID == SDL_GetWindowID(m_Window))
				done = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		UpdateTexture(
			my_image_texture,
			GB_LCD_WIDTH,
			GB_LCD_HEIGHT,
			(unsigned char*)m_Emulator->ppu.frame
		);

		// Our rendering stuff :p
		ImGui::Begin("PPU");
		ImGui::Text("T-Cycles 0x%04X", m_Emulator->ppu.t_cycles);
		ImGui::Text("Mode %u", m_Emulator->ppu.ppu_mode);
		ImGui::Image(
			(ImTextureID)(intptr_t)my_image_texture,
			ImVec2(GB_LCD_WIDTH * 4, GB_LCD_HEIGHT * 4)
		);
		ImGui::End();

		ImGui::Begin("CPU State");

		ImGui::SeparatorText("Registers");
		ImGui::Text("AF 0x%04X", GB_REG_AF(m_Emulator->cpu.regs));
		ImGui::SameLine();
		ImGui::Text("BC 0x%04X", GB_REG_BC(m_Emulator->cpu.regs));
		ImGui::Text("DE 0x%04X", GB_REG_DE(m_Emulator->cpu.regs));
		ImGui::SameLine();
		ImGui::Text("HL 0x%04X", GB_REG_HL(m_Emulator->cpu.regs));
		ImGui::Text(
			"SP 0x%04X [0x%04X]",
			GB_REG_SP(m_Emulator->cpu.regs),
			gb_emu_read_u16(m_Emulator, GB_REG_SP(m_Emulator->cpu.regs))
		);
		ImGui::Text(
			"PC 0x%04X [0x%04X]",
			GB_REG_PC(m_Emulator->cpu.regs),
			gb_emu_read_u16(m_Emulator, GB_REG_PC(m_Emulator->cpu.regs))
		);

		ImGui::Text(
			"FLAGS %c %c %c %c",
			GB_IS_BIT(GB_REG_F(m_Emulator->cpu.regs), GB_FLAG_ZERO_BIT) ? 'Z'
																		: '-',
			GB_IS_BIT(GB_REG_F(m_Emulator->cpu.regs), GB_FLAG_SUBS_BIT) ? 'N'
																		: '-',
			GB_IS_BIT(GB_REG_F(m_Emulator->cpu.regs), GB_FLAG_HALF_BIT) ? 'H'
																		: '-',
			GB_IS_BIT(GB_REG_F(m_Emulator->cpu.regs), GB_FLAG_CARR_BIT) ? 'C'
																		: '-'
		);

		ImGui::Text(
			"InterruptEnable: %s",
			m_Emulator->cpu.interrupt_enable ? "true" : "false"
		);

		ImGui::Text("Last Executed Opcode: ");
		ImGui::SameLine();
		text_opcode(m_LastExecutedOpcode);

		ImGui::SeparatorText("Upcoming instructions");
		int offset = 0;
		for (int i = 0; i < 8; i++) {
			uint16_t addr = GB_REG_PC(m_Emulator->cpu.regs) + offset;
			offset += text_opcode(addr);
		}

		ImGui::SeparatorText("Control");
		if (ImGui::Button("Execute opcode")) {
			uint16_t addr = GB_REG_PC(m_Emulator->cpu.regs);
			m_LastExecutedOpcode = addr;
			gb_emu_advance_opcode(m_Emulator);
		}
		ImGui::SameLine();
		if (ImGui::Button("Execute 100x ops")) {
			for (int i = 0; i < 99; i++) {
				gb_emu_advance_opcode(m_Emulator);
			}
			uint16_t addr = GB_REG_PC(m_Emulator->cpu.regs);
			m_LastExecutedOpcode = addr;
			gb_emu_advance_opcode(m_Emulator);
		}
		if (ImGui::Button("Execute 1000x ops")) {
			for (int i = 0; i < 999; i++) {
				gb_emu_advance_opcode(m_Emulator);
			}
			uint16_t addr = GB_REG_PC(m_Emulator->cpu.regs);
			m_LastExecutedOpcode = addr;
			gb_emu_advance_opcode(m_Emulator);
		}
		if (ImGui::Button("Execute 10000x ops")) {
			for (int i = 0; i < 9999; i++) {
				uint16_t addr = GB_REG_PC(m_Emulator->cpu.regs);
				if (gb_emu_read_u8(m_Emulator, addr) == 0xFB) {
					break;
				}
				gb_emu_advance_opcode(m_Emulator);
			}
			uint16_t addr = GB_REG_PC(m_Emulator->cpu.regs);
			m_LastExecutedOpcode = addr;
			gb_emu_advance_opcode(m_Emulator);
		}
		if (ImGui::Button("INT VBLANK")) {
			gb_cpu_request_interrupt(&m_Emulator->cpu, GB_INTERRUPT_VBLANK);
		}

		ImGui::End();

		static MemoryEditor rom_memory = [&]() {
			MemoryEditor mem;
			mem.UserData = (void*)m_Emulator;
			mem.ReadFn = [](auto, size_t addr, void* void_emu) -> ImU8 {
				const gb_emu_t* emu = (const gb_emu_t*)void_emu;
				return gb_emu_read_u8(emu, (uint16_t)addr);
			};
			mem.WriteFn = [](auto, size_t addr, ImU8 byte, void* void_emu) {
				gb_emu_t* emu = (gb_emu_t*)void_emu;
				gb_emu_write_u8(emu, (uint16_t)addr, (u8)byte);
			};
			return mem;
		}();
		rom_memory.DrawWindow("GameBoy memory", nullptr, 0x10000);

		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(
			clear_color.x * clear_color.w,
			clear_color.y * clear_color.w,
			clear_color.z * clear_color.w,
			clear_color.w
		);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
			SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
		}

		SDL_GL_SwapWindow(m_Window);
	}
}

GeimBoi::App::App()
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
		GB_FATAL("Could not initialize SDL3! Message: '%s'", SDL_GetError());
		return;
	}

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100 (WebGL 1.0)
	const char* glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
	// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
	const char* glsl_version = "#version 300 es";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
	); // Always required on Mac
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE
	);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE
	);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
								   SDL_WINDOW_HIDDEN |
								   SDL_WINDOW_HIGH_PIXEL_DENSITY;
	m_Window = SDL_CreateWindow(
		"Dear ImGui SDL3+OpenGL3 example",
		(int)(1280 * main_scale),
		(int)(800 * main_scale),
		window_flags
	);
	if (m_Window == nullptr) {
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return;
	}
	m_GL = SDL_GL_CreateContext(m_Window);
	if (m_GL == nullptr) {
		printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
		return;
	}

	SDL_GL_MakeCurrent(m_Window, m_GL);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	SDL_SetWindowPosition(
		m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
	);
	SDL_ShowWindow(m_Window);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;
	style.FontSizeBase = 12.0f;
	io.ConfigDpiScaleFonts = true;
	io.ConfigDpiScaleViewports = true;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplSDL3_InitForOpenGL(m_Window, m_GL);
	ImGui_ImplOpenGL3_Init(glsl_version);

	m_Emulator = gb_emu_create();
	gb_emu_load_rom_file(
		m_Emulator, "/home/giffi/Downloads/Tetris (World) (Rev 1).gb"
	);
}

GeimBoi::App::~App()
{
	gb_emu_delete(m_Emulator);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DestroyContext(m_GL);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}
