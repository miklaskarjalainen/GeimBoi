#include "gbSM83.h"
#include "gbEmu.h"
#include "log.h"

#define GB_ADDR_IE (0xFFFF) // Interrupt Enable
#define GB_ADDR_IF (0xFF0F) // Interrupt Flag

#define CGB_MODE 0

gb_sm83_t gb_cpu_create(struct gb_emu* emu)
{
	gb_sm83_t cpu = {.interrupt_enable = 0, .emu = emu};

	// CFB initial values
	if (CGB_MODE) {
    	GB_REG_AF(cpu.regs) = 0x1180;
    	GB_REG_BC(cpu.regs) = 0x0000;
    	GB_REG_DE(cpu.regs) = 0xFF56;
    	GB_REG_HL(cpu.regs) = 0x000D;
	}
	// DMG intial values
	else {
	    GB_REG_AF(cpu.regs) = 0x01B0;
    	GB_REG_BC(cpu.regs) = 0x0013;
    	GB_REG_DE(cpu.regs) = 0x00D8;
    	GB_REG_HL(cpu.regs) = 0x014D;
	}

	GB_REG_SP(cpu.regs) = 0xFFFE;
	GB_REG_PC(cpu.regs) = 0x0100;
	return cpu;
}

u8 gb_cpu_read_u8(const gb_sm83_t* cpu, u16 addr)
{
	return cpu->memory[addr - 0x8000];
}

void gb_cpu_write_u8(gb_sm83_t* cpu, u16 addr, u8 data)
{
	cpu->memory[addr - 0x8000] = data;
}

static void _gb_cpu_serve_interrupt(gb_sm83_t* cpu, u8 interrupt)
{
	cpu->interrupt_enable = 0;

	gb_emu_push_u16(cpu->emu, GB_REG_PC(cpu->regs));

	switch (interrupt)
	{
		case GB_INTERRUPT_VBLANK: {
			GB_REG_PC(cpu->regs) = 0x40;
			break;
		}
		case GB_INTERRUPT_LCD: {
			GB_REG_PC(cpu->regs) = 0x48;
			break;
		}
		case GB_INTERRUPT_TIMER: {
			GB_REG_PC(cpu->regs) = 0x50;
			break;
		}
		case GB_INTERRUPT_SERIAL: {
			GB_REG_PC(cpu->regs) = 0x58;
			break;
		}
		case GB_INTERRUPT_JOYPAD: {
			GB_REG_PC(cpu->regs) = 0x60;
			break;
		}

		default: {
			// GB_FATAL("Invalid interrupt.");
		}
	}
}

void gb_cpu_request_interrupt(gb_sm83_t* cpu, u8 interrupt)
{
	(void)interrupt;
	// gb_cpu_write_u8(cpu, u16 addr, u8 data)

	/*
	if (gb_cpu_read_u8(cpu, GB_ADDR_IE)) {

	}
	 */

	_gb_cpu_serve_interrupt(cpu, interrupt);
}
