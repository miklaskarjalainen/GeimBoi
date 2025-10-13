#pragma once

struct SDL_Window;
struct SDL_GLContextState;
union SDL_Event;

namespace GeimBoi {

class App;

/**
 * @brief Manages the window, opengl & imgui contexes.
 */
class Window {
public:
	void process_event(const SDL_Event& ev) noexcept;

	[[nodiscard]] inline bool should_close() const noexcept
	{
		return m_ShouldClose;
	};

	void render_begin() noexcept;
	void render_present() noexcept;

private:
	Window();
	~Window();

	SDL_Window* m_Window;
	SDL_GLContextState* m_GL;
	bool m_ShouldClose = false;
	friend App;
};

} // namespace GeimBoi
