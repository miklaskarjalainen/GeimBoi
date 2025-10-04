#ifndef _GB_CONSOLE_H
#define _GB_CONSOLE_H

#include "gbSM83.h"
#include "gbCart.h"
#include "gbMMU.h"
#include "gbPPU.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct gb_emu {
    gb_sm83_t cpu;
    gb_cart_t cart;
    gb_ppu_t ppu;
    gb_mmu_t mmu;
} gb_emu_t;

/**
 * @brief Crients the emulator on the heap. And initializes it.
 * @see gb_emu_init.
 */
gb_emu_t* gb_emu_create(void);
/**
 * @brief Deinitialises and frees the emulator
 * @see gb_emu_deinit.
 */
void gb_emu_delete(gb_emu_t* emu);

/**
 * @brief initializes on an already allocated memory.
 * @see gb_emu_create.
 */
void gb_emu_init(gb_emu_t* emu);
/**
 * @brief deinitialises the emulator.
 * @warning DOES NOT FREE THE SOURCE PTR. Use `gb_emu_delete` instead!
 * @see gb_emu_delete
 */
void gb_emu_deinit(gb_emu_t* emu);

/**
 * @brief Copies the bytes to be used for the emulator.
 * @note Takes ownership of the `rom` variable.
 */
bool gb_emu_load_rom_bytes(gb_emu_t* emu, u8* rom, size_t length);
bool gb_emu_load_rom_file(gb_emu_t* emu, const char* fpath);

void gb_emu_advance_frame(gb_emu_t* emu);
void gb_emu_advance_opcode(gb_emu_t* emu);

#endif
