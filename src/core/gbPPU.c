#include "gbPPU.h"
#include "gbEmu.h"
#include "gbSM83.h"

#include <stddef.h>
#include <stdio.h>

void gb_ppu_init(gb_ppu_t* ppu, struct gb_sm83* cpu)
{
    for (int y = 0; y < GB_LCD_HEIGHT; y++) {
        for (int x = 0; x < GB_LCD_WIDTH; x++) {
            ppu->frame[y][x][0] = (u8)(255);
            ppu->frame[y][x][1] = (u8)(0);
            ppu->frame[y][x][2] = (u8)(0);
        }
    }
    ppu->t_cycles = 0;
    ppu->ppu_mode = 2;
    ppu->cpu = cpu;
}

struct gb_tile_data {
    u8 data[8][8][3];
};

struct gb_tile_data get_as_tile(u8* begin)
{
    (void)begin;
    struct gb_tile_data d;

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
}

void gb_render_scanline(gb_ppu_t* ppu, u8 ly)
{
    for (u8 lx = 0; lx < GB_LCD_WIDTH; lx++)
    {
        ppu->frame[ly][lx][0] = 0;
        ppu->frame[ly][lx][1] = ly;
        ppu->frame[ly][lx][2] = lx;
    }
}

void gb_ppu_clock(gb_ppu_t* ppu, u16 t_cycles)
{
    for (u16 i = 0; i < t_cycles; i++ ) {
        ppu->t_cycles++;
        switch (ppu->ppu_mode) {
            // Horizontal blank
            case 0: {
                if (ppu->t_cycles < 204) {
                    return;
                }
                ppu->t_cycles -= 204;
                u8 ly = gb_cpu_read_u8(ppu->cpu, 0xFF44);
                gb_render_scanline(ppu, ly);
                if (ly == 143) {
                    ppu->ppu_mode = 1;
                    gb_cpu_request_interrupt(ppu->cpu, GB_INTERRUPT_VBLANK);
                }
                else {
                    ppu->ppu_mode = 2;
                }

                gb_cpu_write_u8(ppu->cpu, 0xFF44, ly + 1);
                break;
            }
            case 1: {
                if (ppu->t_cycles < 4560) {
                    return;
                }
                ppu->t_cycles -= 4560;

                u8 ly = gb_cpu_read_u8(ppu->cpu, 0xFF44);
                if (ly == 153) {
                    gb_cpu_write_u8(ppu->cpu, 0xFF44, 0);
                    ppu->ppu_mode = 2;
                }
                else {
                    gb_cpu_write_u8(ppu->cpu, 0xFF44, ly + 1);
                }
                break;
            }
            case 2: {
                if (ppu->t_cycles < 80) {
                    return;
                }
                ppu->t_cycles -= 80;
                ppu->ppu_mode = 3;
                break;
            }
            case 3: {
                if (ppu->t_cycles < 172) {
                    return;
                }
                ppu->t_cycles -= 172;
                ppu->ppu_mode = 0;
                break;
            }
        }
    }
}
