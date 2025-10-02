#include "gbPPU.h"
#include "gbEmu.h"
#include "gbReg.h"
#include "gbSM83.h"

#include <stddef.h>
#include <stdio.h>

#define PPU_SET_MODE(ppu, mode) (ppu)->stat = ((ppu)->stat & 0xFC) | (mode)

#define PPU_MODE_HBLANK 0
#define PPU_MODE_VBLANK 1
#define PPU_MODE_OAM 2
#define PPU_MODE_RENDER 3


void gb_ppu_init(gb_ppu_t* ppu, struct gb_sm83* cpu)
{

    for (int y = 0; y < GB_LCD_HEIGHT; y++) {
        for (int x = 0; x < GB_LCD_WIDTH; x++) {
            ppu->frame[y][x][0] = (u8)(255);
            ppu->frame[y][x][1] = (u8)(0);
            ppu->frame[y][x][2] = (u8)(0);
        }
    }
    ppu->ly = 0;
    ppu->lyc = 0;
    ppu->stat = 0;
    ppu->lcdc = 0;
    ppu->t_cycles = 0;
    ppu->cpu = cpu;
}

struct gb_tile_data {
    u8 data[8][8][3];
};

struct gb_tile_data get_as_tile(u8* begin)
{
    struct gb_tile_data d = { 0 };

    for (int y = 0; y < 8; y++)
    {
        u8 row1 = *(begin + y*2);
        u8 row2 = *(begin + y*2 + 1);

        for (int x = 0; x < 8; x++)
        {
            u8 pixel1 = GB_IS_BIT(row1, (7 - x));
            u8 pixel2 = (u8)(GB_IS_BIT(row2, (7 - x)) << 1);

            switch (pixel1 | pixel2) {
                case 0x0: {
                    d.data[y][x][0] = 12;
                    d.data[y][x][1] = 12;
                    d.data[y][x][2] = 12;
                    break;
                }
                case 0x1: {
                    d.data[y][x][0] = 102;
                    d.data[y][x][1] = 102;
                    d.data[y][x][2] = 102;
                    break;
                }
                case 0x2: {
                    d.data[y][x][0] = 198;
                    d.data[y][x][1] = 198;
                    d.data[y][x][2] = 198;
                    break;
                }
                case 0x3: {
                    d.data[y][x][0] = 0xFF;
                    d.data[y][x][1] = 0xFF;
                    d.data[y][x][2] = 0xFF;

                    break;
                }
                default: {
                    printf("?");
                }
            }

        }
    }

    return d;
static inline u8 _gb_get_pixel_color(u8 color)
{
	switch (color) {
		case 0x0: {
			return 12;
		}
		case 0x1: {
			return 102;
		}
		case 0x2: {
			return 198;
		}
		case 0x3: {
			return 255;
		}
		default: {
			printf("?");
		}
	}
}

static inline _gb_render_background(gb_ppu_t* ppu)
{
	// Fetch the base address to read from.
	const u16 tile_addr = GB_IS_BIT(ppu->lcdc, 4) ? 0x8000 : 0x8800;
	const u16 bg_addr = GB_IS_BIT(ppu->lcdc, 3) ? 0x9C00 : 0x9800;
	const u16 tile_row = (u16)(ppu->ly / 8 * 32);

	for (u8 lx = 0; lx < 160; lx++) {
		// Some magic to determine where to read the tile from.
		const u16 tile_column = lx / 8;
		const i16 tile_num =
			gb_cpu_read_u8(ppu->cpu, bg_addr + tile_row + tile_column);
		const u16 tile_location = tile_addr + (tile_num * 16);

		// Fetch the row of pixels for the tile
		const u8 tile_line = ppu->ly % 8;
		const u8 data1 =
			gb_cpu_read_u8(ppu->cpu, tile_location + (tile_line * 2));
		const u8 data2 =
			gb_cpu_read_u8(ppu->cpu, tile_location + (tile_line * 2) + 1);

		// Get the color of the pixel
		const u8 colour_bit = 7 - (lx % 8);
		const u8 color_id = (((data2 >> colour_bit)) & 0b1) |
							(((data1 >> colour_bit) & 0b1) << 1);
		const u8 color = _gb_get_pixel_color(color_id);

		// Draw the pixel
		ppu->frame[ppu->ly][lx][0] = color;
		ppu->frame[ppu->ly][lx][1] = color;
		ppu->frame[ppu->ly][lx][2] = color;
	}
}

void gb_render_scanline(gb_ppu_t* ppu)
{
	const u8 ly = ppu->ly;
	if (ly >= 144) {
		return;
	}

	_gb_render_background(ppu);
}

static void clock_oam_scan(gb_ppu_t* ppu) {
    if (ppu->t_cycles < 80) {
        return;
    }
    ppu->t_cycles -= 80;
    PPU_SET_MODE(ppu, PPU_MODE_RENDER);
}

static void clock_drawing(gb_ppu_t* ppu) {
    if (ppu->t_cycles < 172) {
        return;
    }
    ppu->t_cycles -= 172;
    PPU_SET_MODE(ppu, PPU_MODE_HBLANK);
    if (GB_IS_BIT(ppu->stat, 3)) {
        gb_cpu_request_interrupt(ppu->cpu, GB_INTERRUPT_LCD);
    }
}

static inline void _gb_check_coinsidence_flag(gb_ppu_t* ppu) {
    if (ppu->ly == ppu->lyc) {
        ppu->stat |= GB_BIT(2);
        if (GB_IS_BIT(ppu->stat, 6)) {
            gb_cpu_request_interrupt(ppu->cpu, GB_INTERRUPT_LCD);
        }
    }
    else {
        ppu->stat &= (u8)~GB_BIT(2);
    }
}

static void clock_hblank(gb_ppu_t* ppu) {
    if (ppu->t_cycles < 204) {
        return;
    }
    ppu->t_cycles -= 204;

    if (ppu->ly != 143) {
        PPU_SET_MODE(ppu, PPU_MODE_OAM);
        if (GB_IS_BIT(ppu->stat, 5)) {
            gb_cpu_request_interrupt(ppu->cpu, GB_INTERRUPT_LCD);
        }
    }
    else {
        PPU_SET_MODE(ppu, PPU_MODE_VBLANK);
        if (GB_IS_BIT(ppu->stat, 4)) {
            gb_cpu_request_interrupt(ppu->cpu, GB_INTERRUPT_LCD);
        }
        gb_cpu_request_interrupt(ppu->cpu, GB_INTERRUPT_VBLANK);
    }

    ppu->ly += 1;
    _gb_check_coinsidence_flag(ppu);
}

static void clock_vblank(gb_ppu_t* ppu) {
    if (ppu->t_cycles < 456) {
        return;
    }
    ppu->t_cycles -= 456;

    if (ppu->ly == 153) {
        ppu->ly = 0;
        PPU_SET_MODE(ppu, PPU_MODE_OAM);
        if (GB_IS_BIT(ppu->stat, 3)) {
            gb_cpu_request_interrupt(ppu->cpu, GB_INTERRUPT_LCD);
        }
    }
    else {
        ppu->ly += 1;
    }

    _gb_check_coinsidence_flag(ppu);
}

void gb_ppu_clock(gb_ppu_t* ppu, u16 t_cycles)
{
    if (!GB_IS_BIT(ppu->lcdc, 7)) {
        ppu->ly = 0;
        return;
    }

    for (u16 i = 0; i < t_cycles; i++ ) {
        ppu->t_cycles++;
        switch (ppu->stat & 0x3) {
            case PPU_MODE_HBLANK: {
                clock_hblank(ppu);
                gb_render_scanline(ppu);
                break;
            }
            case PPU_MODE_VBLANK: {
                clock_vblank(ppu);
                break;
            }
            case PPU_MODE_OAM: {
                clock_oam_scan(ppu);
                break;
            }
            case PPU_MODE_RENDER: {
                clock_drawing(ppu);
                break;
            }
        }
    }
}
