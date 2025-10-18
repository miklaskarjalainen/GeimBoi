/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _GB_MMU_H
#define _GB_MMU_H

#include "gbReg.h"

#define GB_ADDR_OEM_BEGIN (0xFE00)
#define GB_ADDR_OBP0 (0xFF48) // Obj palette0
#define GB_ADDR_OBP1 (0xFF49) // Obj palette1
#define GB_ADDR_SCY (0xFF42)  // Scroll Y
#define GB_ADDR_SCX (0xFF43)  // Scroll X
#define GB_ADDR_WY (0xFF4A)	  // Window Y
#define GB_ADDR_WX (0xFF4B)	  // Window X

// APU registers
#define GB_ADDR_NR50 (0xFF24) // Master Volume & VIN panning
#define GB_ADDR_NR51 (0xFF25) // Sound Panning
#define GB_ADDR_NR52 (0xFF26) // Audio master control

#define GB_ADDR_NR10 (0xFF10) // Channel 1 sweep
#define GB_ADDR_NR11 (0xFF11) // Channel 1 length timer & duty cycle
#define GB_ADDR_NR12 (0xFF12) // Channel 1 volume & envelope
#define GB_ADDR_NR13 (0xFF13) // Channel 1 period low
#define GB_ADDR_NR14 (0xFF14) // Channel 1 period high & control

// CGB registers
#define GB_ADDR_SPEED_SW (0xFF4D) // Speed Switch (KEY1)
#define GB_ADDR_BGPI (0xFF68)	  // Background color palette index
#define GB_ADDR_BGPD (0xFF69)	  // Background color palette data
#define GB_ADDR_OBPI (0xFF6A)	  // Object color palette index
#define GB_ADDR_OBPD (0xFF6B)	  // Object color palette data

#define GB_ADDR_P1 (0xFF00)	  // Joypad input
#define GB_ADDR_DIV (0xFF04)  // Divider register
#define GB_ADDR_TIMA (0xFF05) // Timer counter
#define GB_ADDR_TMA (0xFF06)  // Timer modulo
#define GB_ADDR_TAC (0xFF07)  // Timer control
#define GB_ADDR_IF (0xFF0F)	  // Interrupt Flag
#define GB_ADDR_IE (0xFFFF)	  // Interrupt Enable

/*
 * @brief Maps gameboy's 16-bit address space to the correct components.
 */
typedef struct gb_mmu {
	struct gb_apu* apu;
	struct gb_cart* cart;
	struct gb_sm83* cpu;
	struct gb_ppu* ppu;
	struct gb_emu* emu;
} gb_mmu_t;

void gb_mmu_init(
	gb_mmu_t* mmu,
	struct gb_apu* apu,
	struct gb_cart* cart,
	struct gb_sm83* cpu,
	struct gb_ppu* ppu,
	struct gb_emu* emu
);

u16 gb_mmu_read_u16(const gb_mmu_t* mmu, u16 addr);
u8 gb_mmu_read_u8(const gb_mmu_t* mmu, u16 addr);
i8 gb_mmu_read_i8(const gb_mmu_t* mmu, u16 addr);

void gb_mmu_write_u8(gb_mmu_t* mmu, u16 addr, u8 data);

void gb_mmu_push_u8(gb_mmu_t* mmu, u8 data);
void gb_mmu_push_u16(gb_mmu_t* mmu, u16 data);
u8 gb_mmu_pop_u8(gb_mmu_t* mmu);
u16 gb_mmu_pop_u16(gb_mmu_t* mmu);

#endif
