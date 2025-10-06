#ifndef _GB_SM83_H
#define _GB_SM83_H

#include "gbReg.h"

#define GB_REG_AF(regs) (regs)[0].value
#define GB_REG_BC(regs) (regs)[1].value
#define GB_REG_DE(regs) (regs)[2].value
#define GB_REG_HL(regs) (regs)[3].value
#define GB_REG_SP(regs) (regs)[4].value
#define GB_REG_PC(regs) (regs)[5].value
#define GB_REG_COUNT 6

#define GB_REG_A(regs) (regs[0].nibble.high)
#define GB_REG_F(regs) (regs[0].nibble.low)
#define GB_REG_B(regs) (regs[1].nibble.high)
#define GB_REG_C(regs) (regs[1].nibble.low)
#define GB_REG_D(regs) (regs[2].nibble.high)
#define GB_REG_E(regs) (regs[2].nibble.low)
#define GB_REG_H(regs) (regs[3].nibble.high)
#define GB_REG_L(regs) (regs[3].nibble.low)

#define GB_INTERRUPT_JOYPAD GB_BIT(4)
#define GB_INTERRUPT_SERIAL GB_BIT(3)
#define GB_INTERRUPT_TIMER GB_BIT(2)
#define GB_INTERRUPT_LCD GB_BIT(1)
#define GB_INTERRUPT_VBLANK GB_BIT(0)
#define GB_INTERRUPT_MASK (0x1F)

// USE SPARINGLY! Meant for addresses over 0x8000, which can be directly accessed. Like IE,IF.
// Any address which has a "side effect" is no go, so no echo ram or anything.
#define GB_CPU_MEM(cpu, addr) (cpu->memory[addr - 0x8000])

typedef struct gb_sm83 {
	gb_reg16_t regs[GB_REG_COUNT];
	u8 memory[0x8000]; // 0x8000 - 0xFFFF

	u8 interrupt_enable:1;
	u8 is_halted:1;

	u16 timer_div_increment;
	u16 timer_tima_increment;

	u32 m_cycles; // Machine cycles

	struct gb_mmu* mmu;

    /*
     * !Don't modify directly.
     * Use gb_emu_press_input, gb_emu_release_input so interrupts are triggered correctly.
     */
    u8 keys_down;
} gb_sm83_t;

void gb_cpu_init(gb_sm83_t* cpu, struct gb_mmu* mmu);

/**
 * @brief jumps to an interrupt handler, if an interrupt can be served.
 */
void gb_cpu_poll_interrupts(gb_sm83_t* cpu);
void gb_cpu_request_interrupt(gb_sm83_t* cpu, u8 interrupt);
/**
 * @note returns the amount of m-cycles used.
 */
u8 gb_cpu_execute_opcode(gb_sm83_t* emu);
void gb_cpu_clock_timers(gb_sm83_t* emu, u8 m_cycles);

#endif
