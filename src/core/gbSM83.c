#include "gbSM83.h"
#include "gbEmu.h"
#include "log.h"

#define CGB_MODE 0

#define CPU_READ_U8(cpu, addr) (cpu->memory[addr - 0x8000])

void gb_cpu_init(gb_sm83_t* cpu, struct gb_mmu* mmu)
{
    *cpu = (gb_sm83_t){ 0 };
    cpu->mmu = mmu;

    // CFB initial values
	if (CGB_MODE) {
		GB_REG_AF(cpu->regs) = 0x1180;
		GB_REG_BC(cpu->regs) = 0x0000;
		GB_REG_DE(cpu->regs) = 0xFF56;
		GB_REG_HL(cpu->regs) = 0x000D;
	}
	// DMG intial values
	else {
		GB_REG_AF(cpu->regs) = 0x01B0;
		GB_REG_BC(cpu->regs) = 0x0013;
		GB_REG_DE(cpu->regs) = 0x00D8;
		GB_REG_HL(cpu->regs) = 0x014D;
	}

	GB_REG_SP(cpu->regs) = 0xFFFE;
	GB_REG_PC(cpu->regs) = 0x0100;

	cpu->memory[0xFF00 - 0x8000] = 0x0F;
	cpu->memory[0xFFFF - 0x8000] = 0xFF;
}

/**
 * @note interrupt_bit is 0-4, the bit number instead of a mask.
 */
static void _gb_cpu_serve_interrupt(gb_sm83_t* cpu, u8 interrupt_bit)
{
	cpu->interrupt_enable = 0;

	gb_mmu_push_u16(cpu->mmu, GB_REG_PC(cpu->regs));

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
	const u8 IE = gb_mmu_read_u8(cpu->mmu, GB_ADDR_IE);
	const u8 IF = gb_mmu_read_u8(cpu->mmu, GB_ADDR_IF);
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
	gb_mmu_write_u8(cpu->mmu, GB_ADDR_IF, IF ^ (u8)GB_BIT(INT));
	cpu->m_cycles += 5;
}

void gb_cpu_request_interrupt(gb_sm83_t* cpu, u8 interrupt)
{
	u8 unhandled = cpu->memory[GB_ADDR_IF - 0x8000];
	cpu->memory[GB_ADDR_IF - 0x8000] = interrupt | unhandled;
}

// Hz
#define MACHINE_CLOCK 1048576U
#define TIMER_CLOCK 16384U

void gb_cpu_clock_timers(gb_sm83_t* cpu, u8 m_cycles)
{
    // Increment internal timers
    {
        cpu->timer_div_increment += m_cycles;
        cpu->timer_tima_increment += m_cycles;
    }

    // Handle DIV register
    {
        const u8 increment_rate = MACHINE_CLOCK / TIMER_CLOCK;
        if (cpu->timer_div_increment >= increment_rate) {
            cpu->timer_div_increment -= 1;
            cpu->memory[GB_ADDR_DIV - 0x8000] += 1;
        }
    }

    // Handle TIMA register
    {
        const u8 TAC = cpu->memory[GB_ADDR_TAC - 0x8000];
        if (!GB_IS_BIT(TAC, 2)) {
            return;
        }

        // How often the TIMA should be incremented (m-cycles)
        static const u16 TIMA_increment[4] = {256, 4, 16, 64};
        const u16 increment = TIMA_increment[TAC & 0x3];

        if (cpu->timer_tima_increment >= increment) {
            u8* tima = &cpu->memory[GB_ADDR_TIMA - 0x8000];
            cpu->timer_tima_increment -= 1;

            if (*tima != 0xFF) {
                *tima += 1;
                return;
            }

            *tima = cpu->memory[GB_ADDR_TMA - 0x8000];
            gb_cpu_request_interrupt(cpu, GB_INTERRUPT_TIMER);
        }
    }
}
