#include "gbMMU.h"
#include "gbPPU.h"
#include "gbSM83.h"
#include "gbCart.h"

void gb_mmu_init(
	gb_mmu_t* mmu, struct gb_cart* cart, struct gb_sm83* cpu, struct gb_ppu* ppu
) {
    *mmu = (gb_mmu_t){ 0 };

    mmu->cart = cart;
    mmu->cpu = cpu;
    mmu->ppu = ppu;
}

u16 gb_mmu_read_u16(const gb_mmu_t* mmu, u16 addr)
{
	const u16 low = gb_mmu_read_u8(mmu, addr);
	const u16 high = gb_mmu_read_u8(mmu, addr + 1);
	return (u16)((low) | (high << 8));
}

u8 gb_mmu_read_u8(const gb_mmu_t* mmu, u16 addr)
{
	if (addr < 0x8000) {
		return gb_cart_read_u8(mmu->cart, addr);
	}

	if (addr >= 0xFEA0 && addr <= 0xFEFF) {
		return 0xFF;
	}

	if (addr == 0xFF04) {
		return 0xFF;
	}
	if (addr == 0xFF05) {
		return 0xFF;
	}
	if (addr == 0xFF06) {
		return 0xFF;
	}
	if (addr == 0xFF07) {
		return 0x07;
	}

	if (addr == 0xFF40) {
		return mmu->ppu->lcdc;
	}
	if (addr == 0xFF41) {
		return mmu->ppu->stat;
	}
	if (addr == 0xFF44) {
		return mmu->ppu->ly;
	}
	if (addr == 0xFF45) {
		return mmu->ppu->lyc;
	}

	return mmu->cpu->memory[addr - 0x8000];
}

// for serial for now.
#include <stdio.h>

void gb_mmu_write_u8(gb_mmu_t* mmu, u16 addr, u8 data)
{
	if (addr < 0x8000) {
		return;
	}

	// Serial
	if (addr == 0xFF02 && data == 0x81) {
		printf("%c", gb_mmu_read_u8(mmu, 0xFF01));
		return;
	}

	// DMA transfer
	if (addr == 0xFF46) {
		const u16 src = (u16)(data << 8);
		const u16 dst = 0xFE00 - 0x8000;
		for (u8 i = 0; i < 160; i++) {
			mmu->cpu->memory[dst + i] = gb_mmu_read_u8(mmu, src + i);
		}
		return;
	}

	if (addr == 0xFF44) {
		return;
	}

	if (addr == 0xFF00) {
		mmu->cpu->memory[addr - 0x8000] &= 0xF;
		mmu->cpu->memory[addr - 0x8000] |= (u8)(data & ~(0xF));
		return;
	}
	if (addr == 0xFF40) {
		mmu->cpu->mmu->ppu->lcdc = data;
		return;
	}
	if (addr == 0xFF41) {
		mmu->cpu->mmu->ppu->stat = data & 0xF8;
		return;
	}
	if (addr == 0xFF44) {
		mmu->cpu->mmu->ppu->ly = 0; // this might be cap?
		return;
	}
	if (addr == 0xFF45) {
		mmu->cpu->mmu->ppu->lyc = data;
		return;
	}

	mmu->cpu->memory[addr - 0x8000] = data;
}

i8 gb_mmu_read_i8(const gb_mmu_t* mmu, u16 addr)
{
	return (i8)gb_mmu_read_u8(mmu, addr);
}

void gb_mmu_push_u8(gb_mmu_t* mmu, u8 data)
{
	GB_REG_SP(mmu->cpu->regs) -= 1;
	gb_mmu_write_u8(mmu, GB_REG_SP(mmu->cpu->regs), data);
}

void gb_mmu_push_u16(gb_mmu_t* mmu, u16 data)
{
    gb_mmu_push_u8(mmu, (u8)(data >> 8));
    gb_mmu_push_u8(mmu, (u8)(data & 0xFF));
}

u8 gb_mmu_pop_u8(gb_mmu_t* mmu)
{
	u8 d = gb_mmu_read_u8(mmu, GB_REG_SP(mmu->cpu->regs));
	GB_REG_SP(mmu->cpu->regs) += 1;
	return d;
}

u16 gb_mmu_pop_u16(gb_mmu_t* mmu)
{
	return (u16)((gb_mmu_pop_u8(mmu)) | gb_mmu_pop_u8(mmu) << 8);
}
