#include "gbPPU.h"
#include "gbEmu.h"
#include "gbMMU.h"
#include "gbReg.h"
#include "gbSM83.h"
#include "log.h"

#include <stddef.h>
#include <stdio.h>

#define PPU_SET_MODE(ppu, mode) (ppu)->stat = ((ppu)->stat & 0xFC) | (mode)

#define PPU_MODE_HBLANK 0
#define PPU_MODE_VBLANK 1
#define PPU_MODE_OAM 2
#define PPU_MODE_RENDER 3

#define GB_ADDR_BG_PALETTE (0xFF47)

void gb_ppu_init(gb_ppu_t* ppu, struct gb_mmu* mmu)
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
	ppu->lcdc = 0x80;
	ppu->t_cycles = 0;
	ppu->mmu = mmu;
}

struct gb_tile_data {
	u8 data[8][8][3];
};

struct gb_tile_data get_as_tile(u8* begin)
{
	struct gb_tile_data d = {0};

	for (int y = 0; y < 8; y++) {
		u8 row1 = *(begin + y * 2);
		u8 row2 = *(begin + y * 2 + 1);

		for (int x = 0; x < 8; x++) {
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
}

typedef struct gb_color {
	u8 r, g, b;
} gb_color_t;

static inline gb_color_t
_gb_bg_pixel_color(u8 palette_index, u8 bg_palette)
{
	static gb_color_t s_Colors[4] = {
		{.r = 0x84, .g = 0x96, .b = 0x00}, // White
		{.r = 0x4A, .g = 0x69, .b = 0x00},
		{.r = 0x29, .g = 0x55, .b = 0x00},
		{.r = 0x10, .g = 0x41, .b = 0x00}, // Black
	};
	GB_ASSERT(palette_index < 4, "invalid palette index");

	const u8 shift = palette_index * 2;
	const u8 color = (u8)((bg_palette >> shift) & 0x3);
	return s_Colors[color];
}

static inline void _gb_render_background(gb_ppu_t* ppu)
{
	// Fetch the base address to read from.
	const u16 tile_addr = GB_IS_BIT(ppu->lcdc, 4) ? 0x8000 : 0x8800;
	const u16 bg_addr = GB_IS_BIT(ppu->lcdc, 3) ? 0x9C00 : 0x9800;
	const u8 bg_palette = gb_mmu_read_u8(ppu->mmu, GB_ADDR_BG_PALETTE);
	const u8 scroll_y = GB_CPU_MEM(ppu->mmu->cpu, GB_ADDR_SCY) + ppu->ly;
	const u16 tile_row = (u16)(scroll_y / 8 * 32);

	for (u8 lx = 0; lx < 160; lx++) {
		// Some magic to determine where to read the tile from.
		const u8 scroll_x = GB_CPU_MEM(ppu->mmu->cpu, GB_ADDR_SCX) + lx;
		const u16 tile_column = scroll_x / 8;
		const i16 tile_num =
			gb_mmu_read_u8(ppu->mmu, bg_addr + tile_row + tile_column);
		const u16 tile_location = GB_IS_BIT(ppu->lcdc, 4)
									  ? (u16)(tile_addr + (tile_num * 16))
									  : (u16)(tile_addr + ((tile_num + 128) * 16));

		// Fetch the row of pixels for the tile
		const u8 tile_line = scroll_y % 8;
		const u8 data1 =
			gb_mmu_read_u8(ppu->mmu, tile_location + (tile_line * 2));
		const u8 data2 =
			gb_mmu_read_u8(ppu->mmu, tile_location + (tile_line * 2) + 1);

		// Get the color of the pixel
		const u8 colour_bit = 7 - (scroll_x & 7);
        const u8 color_id = (u8)((((data2 >> colour_bit)) & 0x1) |
            (((data1 >> colour_bit) & 0x1) << 1));
		const gb_color_t color = _gb_bg_pixel_color(color_id, bg_palette);

		// Draw the pixel
		ppu->frame[ppu->ly][lx][0] = color.r;
		ppu->frame[ppu->ly][lx][1] = color.g;
		ppu->frame[ppu->ly][lx][2] = color.b;
	}
}

struct gb_oam_entry {
    u8 pos_y, pos_x, tile_idx, flags;
};

struct gb_oam_entry _gb_get_oam(gb_ppu_t* ppu, u8 entry)
{
    GB_ASSERT(entry < 40, "invalid entry");

    const u8* oem_entry = &GB_CPU_MEM(ppu->mmu->cpu, GB_ADDR_OEM_BEGIN + (entry * 4));

    return (struct gb_oam_entry) {
        .pos_y = oem_entry[0],
        .pos_x = oem_entry[1],
        .tile_idx = oem_entry[2],
        .flags = oem_entry[3]
    };
}

static inline void _gb_render_objects(gb_ppu_t* ppu)
{
    const u8 oem_entry_count = 40;
    const u8 enable_obj = GB_IS_BIT(ppu->lcdc, 1);

    if (!enable_obj) {
        return;
    }

    const u8* obj_tiles = &GB_CPU_MEM(ppu->mmu->cpu, 0x8000);
    const u8 palette0 = GB_CPU_MEM(ppu->mmu->cpu, GB_ADDR_OBP0);
    const u8 palette1 = GB_CPU_MEM(ppu->mmu->cpu, GB_ADDR_OBP1);
    const u16 sprite_height = GB_IS_BIT(ppu->lcdc, 2) ? 16 : 8;

    for (u8 i = 0; i < oem_entry_count; i++)
    {
        struct gb_oam_entry oam = _gb_get_oam(ppu, i);
        const u8 visible_ly = ppu->ly + 16;

        // visible?
        if ((visible_ly < oam.pos_y) || visible_ly >= (oam.pos_y + sprite_height) ) {
            continue;
        }
        if (!oam.pos_x || oam.pos_x >= 168) {
            continue;
        }

        if (sprite_height == 16) {
            oam.tile_idx &= ~(0x1);
        }

        const u8 flip_x = GB_IS_BIT(oam.flags, 5);
        const u8 flip_y = GB_IS_BIT(oam.flags, 6);
        i32 tile_y = visible_ly - oam.pos_y;
        if (flip_y) {
            tile_y = sprite_height - tile_y;
        }
        tile_y *= 2;

        const u32 tile_index = oam.tile_idx * 16;
        const u8 data1 = obj_tiles[tile_index + tile_y];
        const u8 data2 = obj_tiles[tile_index + tile_y + 1];
        const u8 palette = GB_IS_BIT(oam.flags, 4) ? palette1 : palette0;

        for (u8 sprite_x = 0; sprite_x < 8; sprite_x++) {
            const i16 lx = oam.pos_x + sprite_x - 8;
            if (lx < 0 || lx >= 160) {
                continue;
            }

            // Get the color of the pixel
            const u8 color_index = flip_x ? sprite_x : (7 - sprite_x);
            const u8 color_id = (u8)((((data2 >> color_index)) & 0x1) |
                (((data1 >> color_index) & 0x1) << 1));

            if (!color_id) {
                continue;
            }

            const gb_color_t color = _gb_bg_pixel_color(color_id, palette);


            // Draw the pixel
           	ppu->frame[ppu->ly][lx][0] = color.r;
           	ppu->frame[ppu->ly][lx][1] = color.g;
           	ppu->frame[ppu->ly][lx][2] = color.b;
        }
    }
}

static inline void _gb_render_scanline(gb_ppu_t* ppu)
{
	const u8 ly = ppu->ly;
	if (ly >= 144) {
		return;
	}

	_gb_render_background(ppu);
	_gb_render_objects(ppu);
}

static inline void clock_oam_scan(gb_ppu_t* ppu)
{
	if (ppu->t_cycles < 80) {
		return;
	}
	ppu->t_cycles -= 80;
	PPU_SET_MODE(ppu, PPU_MODE_RENDER);
}

static void clock_drawing(gb_ppu_t* ppu)
{
	if (ppu->t_cycles < 172) {
		return;
	}
	ppu->t_cycles -= 172;
	PPU_SET_MODE(ppu, PPU_MODE_HBLANK);
	if (GB_IS_BIT(ppu->stat, 3)) {
		gb_cpu_request_interrupt(ppu->mmu->cpu, GB_INTERRUPT_LCD);
	}
	_gb_render_scanline(ppu);
}

static inline void _gb_check_coinsidence_flag(gb_ppu_t* ppu)
{
	if (ppu->ly == ppu->lyc) {
		ppu->stat |= GB_BIT(2);
		if (GB_IS_BIT(ppu->stat, 6)) {
			gb_cpu_request_interrupt(ppu->mmu->cpu, GB_INTERRUPT_LCD);
		}
	}
	else {
		ppu->stat &= (u8)~GB_BIT(2);
	}
}

static void clock_hblank(gb_ppu_t* ppu)
{
	if (ppu->t_cycles < 204) {
		return;
	}
	ppu->t_cycles -= 204;

	if (ppu->ly != 143) {
		PPU_SET_MODE(ppu, PPU_MODE_OAM);
		if (GB_IS_BIT(ppu->stat, 5)) {
			gb_cpu_request_interrupt(ppu->mmu->cpu, GB_INTERRUPT_LCD);
		}
	}
	else {
		PPU_SET_MODE(ppu, PPU_MODE_VBLANK);
		if (GB_IS_BIT(ppu->stat, 4)) {
			gb_cpu_request_interrupt(ppu->mmu->cpu, GB_INTERRUPT_LCD);
		}
		gb_cpu_request_interrupt(ppu->mmu->cpu, GB_INTERRUPT_VBLANK);
	}

	ppu->ly += 1;
	_gb_check_coinsidence_flag(ppu);
}

static void clock_vblank(gb_ppu_t* ppu)
{
	if (ppu->t_cycles < 456) {
		return;
	}
	ppu->t_cycles -= 456;

	if (ppu->ly == 153) {
		ppu->ly = 0;
		PPU_SET_MODE(ppu, PPU_MODE_OAM);
		if (GB_IS_BIT(ppu->stat, 3)) {
			gb_cpu_request_interrupt(ppu->mmu->cpu, GB_INTERRUPT_LCD);
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
		ppu->t_cycles = 0;
		PPU_SET_MODE(ppu, PPU_MODE_OAM);
		return;
	}

	for (u16 i = 0; i < t_cycles; i++) {
		ppu->t_cycles++;
		switch (ppu->stat & 0x3) {
			case PPU_MODE_HBLANK: {
				clock_hblank(ppu);
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
