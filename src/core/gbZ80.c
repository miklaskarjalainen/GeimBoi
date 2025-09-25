#include "gbZ80.h"

gbZ80_t gb_cpu_create(void) {
    gbZ80_t cpu = { 0 };
    GB_REG_PC(cpu.regs) = 0x100;
    GB_REG_SP(cpu.regs) = 0xFFFE;
    return cpu;
}
