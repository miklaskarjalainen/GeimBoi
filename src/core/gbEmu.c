#include "gbEmu.h"
#include "gbCart.h"
#include "gbMMU.h"
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

void gb_emu_init(gb_emu_t* emu)
{
	memset((void*)emu, 0, sizeof(gb_emu_t));

	gb_mmu_init(&emu->mmu, &emu->cart, &emu->cpu, &emu->ppu);
	gb_cpu_init(&emu->cpu, &emu->mmu);
	gb_ppu_init(&emu->ppu, &emu->mmu);
}

void gb_emu_deinit(gb_emu_t* emu) { (void)emu; }

bool gb_emu_load_rom_bytes(gb_emu_t* emu, const u8* rom, size_t length)
{
    gb_cart_load(&emu->cart, rom, length);
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
	free(buffer);

	return r;
}

void gb_emu_advance_frame(gb_emu_t* emu) {
    const u32 FrameCycles = 70221 / 4;
    while (emu->cpu.m_cycles < FrameCycles) {
        gb_emu_advance_opcode(emu);
    }
    emu->cpu.m_cycles -= FrameCycles;
}

void gb_emu_advance_opcode(gb_emu_t* emu)
{
	gb_cpu_poll_interrupts(&emu->cpu);

	const u8 cycles = emu->cpu.is_halted ? 1 : gb_cpu_execute_opcode(&emu->cpu);
	gb_cpu_clock_timers(&emu->cpu, cycles);
	gb_ppu_clock(&emu->ppu, cycles * 4);
	emu->cpu.m_cycles += cycles;
}

void gb_emu_press_key(gb_emu_t* emu, gb_input_e input)
{
    // 0 -> pressed (the same way it works on the gameboy)
    // 1 -> unpressed

    const u8 changed = (emu->cpu.keys_down & input) != 0;
    if (!changed) {
        return;
    }

    const u8 p1 = gb_mmu_read_u8(&emu->mmu, GB_ADDR_P1);
    const u8 is_dpad = (input & 0x0F) != 0;
    const u8 is_action = (input & 0xF0) != 0;
    emu->cpu.keys_down &= (u8)~input;

    if (GB_IS_BIT(p1, 5) && is_dpad) {
        gb_cpu_request_interrupt(&emu->cpu, GB_INTERRUPT_JOYPAD);
    }
    else if ((GB_IS_BIT(p1, 4) && is_action)) {
        gb_cpu_request_interrupt(&emu->cpu, GB_INTERRUPT_JOYPAD);
    }
}

void gb_emu_release_key(gb_emu_t* emu, gb_input_e input)
{
    // 0 -> pressed (the same way it works on the gameboy)
    // 1 -> unpressed
    emu->cpu.keys_down |= (u8)input;
}
