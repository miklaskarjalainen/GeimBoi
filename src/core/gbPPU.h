#ifndef _GB_PPU_H
#define _GB_PPU_H

#include "gbReg.h"

#define GB_LCD_HEIGHT 144
#define GB_LCD_WIDTH 144

typedef struct gb_ppu {
    u8 frame[GB_LCD_WIDTH][GB_LCD_HEIGHT][3];
    u16 t_cycles;
    u8 ppu_mode: 2;

    struct gb_sm83* cpu;
} gb_ppu_t;

void gb_ppu_init(gb_ppu_t* ppu, struct gb_sm83* cpu);
void gb_ppu_clock(gb_ppu_t* ppu, u16 t_cycles);

#endif
