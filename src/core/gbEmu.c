#include "gbEmu.h"
#include "gbCart.h"
#include "gbReg.h"
#include "gbSM83.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

gb_emu_t* gb_emu_create(void)
{
	gb_emu_t* emu = malloc(sizeof(gb_emu_t));
	gb_emu_init(emu);
	return emu;
}

void gb_emu_delete(gb_emu_t* emu)
{
	gb_emu_deinit(emu);
	free(emu);
}

void gb_emu_init(gb_emu_t* emu) {
    memset((void*)emu, 0, sizeof(gb_emu_t));
    emu->cpu = gb_cpu_create(emu);
    gb_ppu_init(&emu->ppu);
}

void gb_emu_deinit(gb_emu_t* emu) { gb_cart_delete(&emu->cart); }

bool gb_emu_load_rom_bytes(gb_emu_t* emu, u8* rom, size_t length)
{
	if (length < 0x8000) {
		free(rom);
		return false;
	}
	emu->cart = gb_cart_create(rom, length);
	return true;
}

bool gb_emu_load_rom_file(gb_emu_t* emu, const char* fpath)
{
	FILE* file = fopen(fpath, "rb");
	if (!file) {
		GB_ERROR("Could not open file '%s'!", fpath);
		return false;
	}

	// Get size
	fseek(file, 0, SEEK_END);
	size_t file_len = (size_t)ftell(file);
	rewind(file);

	// Allocate & Copy
	u8* buffer = (u8*)malloc(file_len * sizeof(u8));
	fread(buffer, 1, file_len, file);

	// Load & Close
	bool r = gb_emu_load_rom_bytes(emu, buffer, file_len);
	fclose(file);

	return r;
}

void gb_emu_advance_frame(gb_emu_t* emu) { (void)emu; }

u16 gb_emu_read_u16(const gb_emu_t* emu, u16 addr)
{
	const u16 low = gb_emu_read_u8(emu, addr);
	const u16 high = gb_emu_read_u8(emu, addr + 1);
	return (u16)((low) | (high << 8));
}

u8 gb_emu_read_u8(const gb_emu_t* emu, u16 addr)
{
	if (addr < 0x8000) {
		return gb_cart_read_u8(&emu->cart, addr);
	}
	return gb_cpu_read_u8(&emu->cpu, addr);
}

i8 gb_emu_read_i8(const gb_emu_t* emu, u16 addr)
{
	return (i8)gb_emu_read_u8(emu, addr);
}

void gb_emu_write_u8(gb_emu_t* emu, u16 addr, u8 data)
{
	if (addr < 0x8000) {
		return;
	}

	gb_cpu_write_u8(&emu->cpu, addr, data);
	return;
}

void gb_emu_push_u8(gb_emu_t* emu, u8 data)
{
	GB_REG_SP(emu->cpu.regs) -= 1;
	gb_emu_write_u8(emu, GB_REG_SP(emu->cpu.regs), data);
}

void gb_emu_push_u16(gb_emu_t* emu, u16 data)
{
	gb_emu_push_u8(emu, (u8)data);
	gb_emu_push_u8(emu, (u8)(data >> 8));
}

u8 gb_emu_pop_u8(gb_emu_t* emu)
{
	u8 d = gb_emu_read_u8(emu, GB_REG_SP(emu->cpu.regs));
	GB_REG_SP(emu->cpu.regs) += 1;
	return d;
}

u16 gb_emu_pop_u16(gb_emu_t* emu)
{
	return (u16)((gb_emu_pop_u8(emu) << 8) | gb_emu_pop_u8(emu));
}
