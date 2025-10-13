
typedef struct gb_emu gb_emu_t;

namespace GeimBoi {
class GuiDebugger {
public:
	GuiDebugger(gb_emu_t* emu);
	~GuiDebugger();

	void draw();

private:
	void draw_cart();
	void draw_cpu();
	void draw_ppu();

	gb_emu_t* m_Emulator;
};
} // namespace GeimBoi
