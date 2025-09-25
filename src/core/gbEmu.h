#ifndef _GB_CONSOLE_H
#define _GB_CONSOLE_H

#include "gbSM83.h"
#include "gbCart.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct gb_emu {
    gb_sm83_t cpu;
    gb_cart_t cart;
} gb_emu_t;

gb_emu_t gb_emu_create(void);
void gb_emu_delete(gb_emu_t* emu);

/**
 * @brief Copies the bytes to be used for the emulator.
 * @note Takes ownership of the `rom` variable.
 */
bool gb_emu_load_rom_bytes(gb_emu_t* emu, u8* rom, size_t length);
bool gb_emu_load_rom_file(gb_emu_t* emu, const char* fpath);

u16 gb_emu_read_u16(const gb_emu_t* emu, u16 addr);
u8 gb_emu_read_u8(const gb_emu_t* emu, u16 addr);
i8 gb_emu_read_i8(const gb_emu_t* emu, u16 addr);

void gb_emu_write_u8(gb_emu_t* emu, u16 addr, u8 data);

void gb_emu_push_u8(gb_emu_t* emu, u8 data);
void gb_emu_push_u16(gb_emu_t* emu, u16 data);
u8 gb_emu_pop_u8(gb_emu_t* emu);
u16 gb_emu_pop_u16(gb_emu_t* emu);

void gb_emu_advance_frame(gb_emu_t* emu);
/**
 * @note returns the amount of cycles used.
 */
u8 gb_emu_advance_opcode(gb_emu_t* emu);

#endif
