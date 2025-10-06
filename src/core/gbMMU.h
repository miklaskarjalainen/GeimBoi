#ifndef _GB_MMU_H
#define _GB_MMU_H

#include "gbReg.h"

#define GB_ADDR_DIV (0xFF04) // Divider register
#define GB_ADDR_TIMA (0xFF05) // Timer counter
#define GB_ADDR_TMA (0xFF06) // Timer modulo
#define GB_ADDR_TAC (0xFF07) // Timer control
#define GB_ADDR_IF (0xFF0F) // Interrupt Flag
#define GB_ADDR_IE (0xFFFF) // Interrupt Enable

/*
 * @brief Maps gameboy's 16-bit address space to the correct components.
 */
typedef struct gb_mmu {
	struct gb_cart* cart;
	struct gb_sm83* cpu;
	struct gb_ppu* ppu;
} gb_mmu_t;

void gb_mmu_init(
	gb_mmu_t* mmu, struct gb_cart* cart, struct gb_sm83* cpu, struct gb_ppu* ppu
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
