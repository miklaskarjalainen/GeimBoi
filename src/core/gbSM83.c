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

    if (addr >= 0xFEA0 && addr <= 0xFEFF) {
        return 0xFF;
    }
   	if (addr == 0xFF40) {
		return cpu->emu->ppu.lcdc;
	}
	if (addr == 0xFF41) {
	    return cpu->emu->ppu.stat;
	}
	if (addr == 0xFF44) {
	    return cpu->emu->ppu.ly;
	}
	if (addr == 0xFF45) {
	    return cpu->emu->ppu.lyc;
	}
	return cpu->memory[addr - 0x8000];
}

void gb_cpu_write_u8(gb_sm83_t* cpu, u16 addr, u8 data)
{
    if (addr == 0xFF02 && data == 0x81) {
		// printf("%c", gb_cpu_read_u8(cpu, 0xFF01));
		return;
	}

    // DMA transfer
    if (addr == 0xFF46) {
        const u16 src = (u16)(data << 8);
        const u16 dst = 0xFE00 - 0x8000;
        for (u8 i = 0; i < 160; i++) {
            cpu->memory[dst + i] = gb_emu_read_u8(cpu->emu, src + i);
        }
        cpu->m_cycles += 160;
        return;
    }

   	if (addr == 0xFF44) {
	    return;
	}

    if (addr == 0xFF00) {
	    cpu->memory[addr - 0x8000] &= 0xF;
        cpu->memory[addr - 0x8000] |= (u8)(data & ~(0xF));
		return;
	}
	if (addr == 0xFF40) {
	    cpu->emu->ppu.lcdc = data;
		return;
	}
	if (addr == 0xFF41) {
	    cpu->emu->ppu.stat = data & 0xF8;
		return;
	}
	if (addr == 0xFF44) {
	    cpu->emu->ppu.ly = 0;
		return;
	}
	if (addr == 0xFF45) {
	    cpu->emu->ppu.lyc = data;
		return;
	}

	cpu->memory[addr - 0x8000] = data;
}

/**
 * @note interrupt_bit is 0-4, the bit number instead of a mask.
 */
static void _gb_cpu_serve_interrupt(gb_sm83_t* cpu, u8 interrupt_bit)
{
	cpu->interrupt_enable = 0;

	gb_emu_push_u16(cpu->emu, GB_REG_PC(cpu->regs));

	static u8 s_JumpTable[] = {0x40, 0x48, 0x50, 0x58, 0x60};
	GB_REG_PC(cpu->regs) = s_JumpTable[interrupt_bit];
}

static inline u8 _gb_first_bit_pos(u8 num)
{
	for (u8 i = 0; i < 8; i++) {
		if (GB_IS_BIT(num, i)) {
			return i;
		}
	}
	return 255;
}

void gb_cpu_poll_interrupts(gb_sm83_t* cpu)
{
	const u8 IE = gb_cpu_read_u8(cpu, GB_ADDR_IE);
	const u8 IF = gb_cpu_read_u8(cpu, GB_ADDR_IF);
	u8 ints = (IE & IF) & GB_INTERRUPT_MASK;

	if (!ints) {
		return;
	}

	cpu->m_cycles += cpu->is_halted;
	cpu->is_halted = false;

	if (!cpu->interrupt_enable) {
		return;
	}

	const u8 INT = _gb_first_bit_pos(ints);
	_gb_cpu_serve_interrupt(cpu, INT);
	gb_cpu_write_u8(cpu, GB_ADDR_IF, IF ^ (u8)GB_BIT(INT));
	cpu->m_cycles += 5;
}

void gb_cpu_request_interrupt(gb_sm83_t* cpu, u8 interrupt)
{

    u8 unhandled = gb_cpu_read_u8(cpu, GB_ADDR_IF);
	gb_cpu_write_u8(cpu, GB_ADDR_IF, unhandled | interrupt);
}
