#ifndef _GB_CONSOLE_H
#define _GB_CONSOLE_H

#include "gbZ80.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct gb_emu {
    gbZ80_t cpu;
    u8* rom; // here, for now.
} gb_emu_t;

gb_emu_t gb_emu_create(void);
void gb_emu_delete(gb_emu_t* emu);

/**
 * @brief Copies the bytes to be used for the emulator.
 * @note Takes ownership of the `rom` variable.
 */
bool gb_emu_load_rom_bytes(gb_emu_t* emu, u8* rom, size_t length);
bool gb_emu_load_rom_file(gb_emu_t* emu, const char* fpath);

void gb_emu_advance_frame(gb_emu_t* emu);
void gb_emu_advance_opcode(gb_emu_t* emu);

#endif
