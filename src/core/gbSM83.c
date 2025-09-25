#include "gbSM83.h"

gb_sm83_t gb_cpu_create(void)
{
	gb_sm83_t cpu = {0};
	GB_REG_PC(cpu.regs) = 0x100;
	GB_REG_SP(cpu.regs) = 0xFFFE;
	return cpu;
}

u8 gb_cpu_read_u8(const gb_sm83_t* cpu, u16 addr) { return cpu->memory[addr - 0x8000]; }

void gb_cpu_write_u8(gb_sm83_t* cpu, u16 addr, u8 data) { cpu->memory[addr - 0x8000] = data; }

void gb_cpu_push_u8(gb_sm83_t* cpu, u8 data)
{
	GB_REG_SP(cpu->regs) -= 1;
	gb_cpu_write_u8(cpu, GB_REG_SP(cpu->regs), data);
}

void gb_cpu_push_u16(gb_sm83_t* cpu, u16 data)
{
	gb_cpu_push_u8(cpu, (u8)data);
	gb_cpu_push_u8(cpu, (u8)(data >> 8));
}
