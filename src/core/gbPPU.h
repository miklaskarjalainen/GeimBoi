#ifndef _GB_PPU_H
#define _GB_PPU_H

#include "gbReg.h"

#define GB_LCD_HEIGHT 144
#define GB_LCD_WIDTH 160

typedef struct gb_ppu {
	u8 frame[GB_LCD_HEIGHT][GB_LCD_WIDTH][3];
	// Which color-id was used by bg or window. Used for object pixel priority.
	u8 priority[GB_LCD_WIDTH];

	u16 t_cycles;
	u8 ly, lyc, stat, lcdc;

	// Window things
	u8 window_ly_eq : 1; // Has window's ly ever been ly?
	u8 window_scanline;

	struct gb_mmu* mmu;
} gb_ppu_t;

void gb_ppu_init(gb_ppu_t* ppu, struct gb_mmu* mmu);
void gb_ppu_clock(gb_ppu_t* ppu, u16 t_cycles);

#endif
