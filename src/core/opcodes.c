#include "gbCart.h"
#include "gbEmu.h"
#include "gbPPU.h"
#include "gbReg.h"
#include "gbSM83.h"
#include "log.h"

static inline void _gb_or(gb_sm83_t* cpu, u8 data)
{
	GB_REG_A(cpu->regs) |= data;
	GB_REG_F(cpu->regs) = GB_REG_A(cpu->regs) == 0 ? GB_FLAG_ZERO : 0x0;
}

static inline void _gb_xor(gb_sm83_t* cpu, u8 data)
{
	GB_REG_A(cpu->regs) ^= data;
	GB_REG_F(cpu->regs) = GB_REG_A(cpu->regs) == 0 ? GB_FLAG_ZERO : 0x0;
}

static inline void _gb_cp(gb_sm83_t* cpu, u8 data)
{
	u8 before = GB_REG_A(cpu->regs);

	GB_REG_F(cpu->regs) = GB_FLAG_SUBS;
	GB_REG_F(cpu->regs) |= GB_REG_A(cpu->regs) == data ? GB_FLAG_ZERO : 0;
	GB_REG_F(cpu->regs) |= GB_REG_A(cpu->regs) < data ? GB_FLAG_CARR : 0;

	int16_t htest = before & 0xF;
	htest -= (data & 0xF);
	if (htest < 0) {
		GB_REG_F(cpu->regs) |= GB_FLAG_HALF;
	}
}

static inline void _gb_add(gb_sm83_t* cpu, u8 data)
{
	u16 result = GB_REG_A(cpu->regs) + data;

	const u8 half = ((data & 0xF) + (GB_REG_A(cpu->regs) & 0xF)) > 0xF;
	const u8 carry = result > 0xFF;

	GB_REG_F(cpu->regs) = (result & 0xFF) ? 0 : GB_FLAG_ZERO;
	GB_REG_F(cpu->regs) |= half ? GB_FLAG_HALF : 0;
	GB_REG_F(cpu->regs) |= carry ? GB_FLAG_CARR : 0;

	GB_REG_A(cpu->regs) = (u8)result;
}

static inline void _gb_adc(gb_sm83_t* cpu, u8 data)
{
	u8 add_carry = GB_IS_BIT(GB_REG_F(cpu->regs), GB_FLAG_CARR_BIT);
	u16 result = (u16)(GB_REG_A(cpu->regs) + data + add_carry);

	const u8 half =
		((data & 0xF) + (GB_REG_A(cpu->regs) & 0xF) + add_carry) > 0xF;
	const u8 carry = result > 0xFF;

	GB_REG_F(cpu->regs) = (result & 0xFF) ? 0 : GB_FLAG_ZERO;
	GB_REG_F(cpu->regs) |= half ? GB_FLAG_HALF : 0;
	GB_REG_F(cpu->regs) |= carry ? GB_FLAG_CARR : 0;

	GB_REG_A(cpu->regs) = (u8)result;
}

static inline void _gb_sbc(gb_sm83_t* cpu, u8 data)
{
    const u8 sub_carry = GB_IS_BIT(GB_REG_F(cpu->regs), GB_FLAG_CARR_BIT);
	const u16 sub_amount = (u16)data + sub_carry;
	const u16 result = (u16)GB_REG_A(cpu->regs) - sub_amount;

	const u8 half  = (GB_REG_A(cpu->regs) & 0xF) < ((data & 0xF) + sub_carry);
	const u8 carry = GB_REG_A(cpu->regs) < sub_amount;

	GB_REG_F(cpu->regs) = GB_FLAG_SUBS;
	GB_REG_F(cpu->regs) |= (result & 0xFF) == 0 ? GB_FLAG_ZERO : 0;
	GB_REG_F(cpu->regs) |= half ? GB_FLAG_HALF : 0;
	GB_REG_F(cpu->regs) |= carry ? GB_FLAG_CARR : 0;

	GB_REG_A(cpu->regs) = (u8)result;
}

static inline void _gb_dec(gb_sm83_t* cpu, u8* data)
{
	u8 before = *data;
	(*data) -= 1;

	GB_REG_F(cpu->regs) &= (u8) ~(GB_FLAG_ZERO | GB_FLAG_HALF);
	GB_REG_F(cpu->regs) |= GB_FLAG_SUBS;
	GB_REG_F(cpu->regs) |= (*data == 0) ? GB_FLAG_ZERO : 0;
	GB_REG_F(cpu->regs) |= (before & 0xF) == 0 ? GB_FLAG_HALF : 0;
}

static inline void _gb_inc(gb_sm83_t* cpu, u8* data)
{
	u8 before = *data;
	(*data) += 1;

	GB_REG_F(cpu->regs) &= (u8) ~(GB_FLAG_ZERO | GB_FLAG_SUBS | GB_FLAG_HALF);
	GB_REG_F(cpu->regs) |= (*data == 0) ? GB_FLAG_ZERO : 0;
	GB_REG_F(cpu->regs) |= (before & 0xF) == 0xF ? GB_FLAG_HALF : 0;
}

static inline void _gb_and(gb_sm83_t* cpu, u8 data)
{
	u8 result = GB_REG_A(cpu->regs) & data;
	GB_REG_F(cpu->regs) = result == 0 ? GB_FLAG_ZERO : 0x0;
	GB_REG_F(cpu->regs) |= GB_FLAG_HALF;
	GB_REG_A(cpu->regs) = result;
}

static inline void _gb_sub(gb_sm83_t* cpu, u8 data)
{
	const u16 result = (u16)GB_REG_A(cpu->regs) - data;
	const u8 half  = (GB_REG_A(cpu->regs) & 0xF) < ((data & 0xF));
	const u8 carry = GB_REG_A(cpu->regs) < data;

	GB_REG_F(cpu->regs) = GB_FLAG_SUBS;
	GB_REG_F(cpu->regs) |= (result & 0xFF) == 0 ? GB_FLAG_ZERO : 0;
	GB_REG_F(cpu->regs) |= half ? GB_FLAG_HALF : 0;
	GB_REG_F(cpu->regs) |= carry ? GB_FLAG_CARR : 0;

	GB_REG_A(cpu->regs) = (u8)result;
}

static inline void _gb_add_u16(gb_sm83_t* cpu, u16 data)
{
	u32 result = GB_REG_HL(cpu->regs) + data;
	u32 half = (GB_REG_HL(cpu->regs) & 0xFFF) + (data & 0xFFF);

	GB_REG_F(cpu->regs) &= (u8) ~(GB_FLAG_HALF | GB_FLAG_CARR | GB_FLAG_SUBS);
	GB_REG_F(cpu->regs) |= result > 0xFFFF ? GB_FLAG_CARR : 0;
	GB_REG_F(cpu->regs) |= half > 0xFFF ? GB_FLAG_HALF : 0;
	GB_REG_HL(cpu->regs) = (u16)result;
}

static inline void _gb_rr(gb_sm83_t* cpu, u8* reg) {
    const u8 carry = (GB_REG_F(cpu->regs) & GB_FLAG_CARR) != 0;
    GB_REG_F(cpu->regs) = GB_IS_BIT(*reg, 0) ? GB_FLAG_CARR : 0;

    *reg >>= 1;
    *reg |= (carry << 7);

    GB_REG_F(cpu->regs) |= *reg == 0 ? GB_FLAG_ZERO : 0;
}

static inline void _gb_rlc(gb_sm83_t* cpu, u8* reg)
{
    const u8 carry = GB_IS_BIT(*reg, 7);
	GB_REG_F(cpu->regs) = carry ? GB_FLAG_CARR : 0;
	*reg <<= 1;
	*reg |= carry;
	GB_REG_F(cpu->regs) |= (*reg) == 0 ? GB_FLAG_ZERO : 0;
}

static inline void _gb_rrc(gb_sm83_t* cpu, u8* reg)
{
    const u8 carry = GB_IS_BIT(*reg, 0);
	GB_REG_F(cpu->regs) = carry ? GB_FLAG_CARR : 0;
	*reg >>= 1;
	*reg |= (carry << 7);
	GB_REG_F(cpu->regs) |= (*reg) == 0 ? GB_FLAG_ZERO : 0;
}

static inline void _gb_rl(gb_sm83_t* cpu, u8* reg)
{
    const u8 do_carry = (GB_REG_F(cpu->regs) & GB_FLAG_CARR) != 0;
    const u8 has_carry = GB_IS_BIT(*reg, 7);

	*reg <<= 1;
	*reg |= do_carry;

	GB_REG_F(cpu->regs) = has_carry ? GB_FLAG_CARR : 0;
	GB_REG_F(cpu->regs) |= (*reg) == 0 ? GB_FLAG_ZERO : 0;
}

static inline void _gb_sra(gb_sm83_t* cpu, u8* reg)
{
    const u8 do_carry = GB_GET_BIT(*reg, 7);
    const u8 has_carry = GB_GET_BIT(*reg, 0);

	*reg >>= 1;
	*reg |= do_carry;

	GB_REG_F(cpu->regs) = has_carry ? GB_FLAG_CARR : 0;
	GB_REG_F(cpu->regs) |= (*reg) == 0 ? GB_FLAG_ZERO : 0;
}


static inline void _gb_srl(gb_sm83_t* cpu, u8* reg)
{
	GB_REG_F(cpu->regs) = GB_GET_BIT(*reg, 0) ? GB_FLAG_CARR : 0;
	*reg >>= 1;
	GB_REG_F(cpu->regs) |= (*reg) == 0 ? GB_FLAG_ZERO : 0;
}

static inline void _gb_sla(gb_sm83_t* cpu, u8* reg)
{
	GB_REG_F(cpu->regs) = GB_GET_BIT(*reg, 7) ? GB_FLAG_CARR : 0;
	*reg <<= 1;
	GB_REG_F(cpu->regs) |= (*reg) == 0 ? GB_FLAG_ZERO : 0;
}

static inline void _gb_get_bit(gb_sm83_t* cpu, u8 data, u8 bit)
{
	GB_REG_F(cpu->regs) &= (u8) ~(GB_FLAG_ZERO | GB_FLAG_SUBS);
	GB_REG_F(cpu->regs) |= GB_FLAG_HALF;
	GB_REG_F(cpu->regs) |= GB_GET_BIT(data, bit) ? 0x0 : GB_FLAG_ZERO;
}

static inline void _gb_swap(gb_sm83_t* cpu, u8* reg)
{
	u8 lower = *reg & 0xF;
	u8 upper = (*reg) >> 4;
	*reg = (u8)(lower << 4) | upper;

	GB_REG_F(cpu->regs) = *reg == 0 ? GB_FLAG_ZERO : 0;
}

static inline void _gb_rst(gb_sm83_t* cpu, u16 addr)
{
	gb_mmu_push_u16(cpu->mmu, GB_REG_PC(cpu->regs));
	GB_REG_PC(cpu->regs) = addr;
}

static inline void _gb_call(gb_sm83_t* cpu)
{
	gb_mmu_push_u16(cpu->mmu, GB_REG_PC(cpu->regs) + 2);
	GB_REG_PC(cpu->regs) = gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
}

static inline void _gb_daa(gb_sm83_t* cpu)
{
    const u8 flags = GB_REG_F(cpu->regs);
    u16 reg_a = GB_REG_A(cpu->regs);

    // Handle addition
    if ( !(GB_IS_BIT(flags, GB_FLAG_SUBS_BIT)) )
    {
        if ( GB_IS_BIT(flags, GB_FLAG_HALF_BIT) || ((reg_a & 0xF) > 9))
        {
            reg_a += 0x6;
        }

        if ( GB_IS_BIT(flags, GB_FLAG_CARR_BIT) || (reg_a > 0x9F))
        {
            reg_a += 0x60;
            GB_REG_F(cpu->regs) |= GB_FLAG_CARR;
        }
    }
    // Substract
    else
    {
        if ( GB_IS_BIT(flags, GB_FLAG_HALF_BIT) )
        {
            reg_a -= 0x6;
        }

        if ( GB_IS_BIT(flags, GB_FLAG_CARR_BIT) )
        {
            reg_a -= 0x60;
        }
    }

    // Reset Flags
    GB_REG_F(cpu->regs) &= (u8)~(GB_FLAG_ZERO);
    GB_REG_F(cpu->regs) &= (u8)~(GB_FLAG_HALF);

    if ( (reg_a & 0xFF) == 0U)
    {
        GB_REG_F(cpu->regs) |= GB_FLAG_ZERO;
    }

    GB_REG_A(cpu->regs) = (u8)reg_a;
}

static u8 _gb_emu_execute_cb(gb_sm83_t* cpu);

u8 gb_cpu_execute_opcode(gb_sm83_t* cpu)
{
	u8 opcode = gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
	GB_REG_PC(cpu->regs) = (u16)(GB_REG_PC(cpu->regs) + !cpu->halt_bugged);
	cpu->halt_bugged = 0;

	switch (opcode) {
		/* LD B, B */ case 0x40:
		/* LD C, C */ case 0x49:
		/* LD D, D */ case 0x52:
		/* LD E, E */ case 0x5B:
		/* LD H, H */ case 0x64:
		/* LD L, L */ case 0x6D:
		/* LD A, A */ case 0x7F:
		/* NOP */ case 0x00: {
			return 1;
		}

		/* RLCA */ case 0x07: {
			u8 carry = GB_IS_BIT(GB_REG_A(cpu->regs), 7) != 0;
			GB_REG_F(cpu->regs) = carry ? GB_FLAG_CARR : 0;
			GB_REG_A(cpu->regs) =
				(u8)((GB_REG_A(cpu->regs) << 1) | carry);
			return 1;
		}

		/* RLA */ case 0x17: {
			u8 has_carry = GB_IS_BIT(GB_REG_A(cpu->regs), 7) != 0;
			u8 do_carry = (GB_REG_F(cpu->regs) & GB_FLAG_CARR) != 0;
			GB_REG_F(cpu->regs) = has_carry ? GB_FLAG_CARR : 0;
			GB_REG_A(cpu->regs) =
				(u8)((GB_REG_A(cpu->regs) << 1) | do_carry);
			return 1;
		}

		/* RRCA */ case 0x0F: {
			u8 carry = GB_IS_BIT(GB_REG_A(cpu->regs), 0) != 0;
			GB_REG_F(cpu->regs) = carry ? GB_FLAG_CARR : 0;
			GB_REG_A(cpu->regs) =
				(u8)((GB_REG_A(cpu->regs) >> 1) | (carry << 7));
			return 1;
		}

		/* RRA */ case 0x1F: {
			u8 has_carry = GB_IS_BIT(GB_REG_A(cpu->regs), 0) != 0;
			u8 do_carry = (GB_REG_F(cpu->regs) & GB_FLAG_CARR) != 0;
			GB_REG_F(cpu->regs) = has_carry ? GB_FLAG_CARR : 0;
			GB_REG_A(cpu->regs) =
				(u8)((GB_REG_A(cpu->regs) >> 1) | (do_carry << 7));
			return 1;
		}

		/* CPL */ case 0x2F: {
			GB_REG_F(cpu->regs) |= GB_FLAG_SUBS | GB_FLAG_HALF;
			GB_REG_A(cpu->regs) = ~GB_REG_A(cpu->regs);
			return 1;
		}

		/* SCF */ case 0x37: {
			GB_REG_F(cpu->regs) &= (u8) ~(GB_FLAG_SUBS | GB_FLAG_HALF);
			GB_REG_F(cpu->regs) |= GB_FLAG_CARR;
			return 1;
		}

		/* CCF */ case 0x3F: {
			GB_REG_F(cpu->regs) &= (u8) ~(GB_FLAG_SUBS | GB_FLAG_HALF);
			GB_REG_F(cpu->regs) = GB_REG_F(cpu->regs) ^ GB_FLAG_CARR;
			return 1;
		}

		/* INC B */ case 0x04: {
			_gb_inc(cpu, &GB_REG_B(cpu->regs));
			return 1;
		}

		/* INC C */ case 0x0C: {
			_gb_inc(cpu, &GB_REG_C(cpu->regs));
			return 1;
		}

		/* INC D */ case 0x14: {
			_gb_inc(cpu, &GB_REG_D(cpu->regs));
			return 1;
		}

		/* INC E */ case 0x1C: {
			_gb_inc(cpu, &GB_REG_E(cpu->regs));
			return 1;
		}

		/* INC H */ case 0x24: {
			_gb_inc(cpu, &GB_REG_H(cpu->regs));
			return 1;
		}

		/* INC L */ case 0x2C: {
			_gb_inc(cpu, &GB_REG_L(cpu->regs));
			return 1;
		}

		/* INC (HL) */ case 0x34: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_inc(cpu, &data);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 3;
		}

		/* INC A */ case 0x3C: {
			_gb_inc(cpu, &GB_REG_A(cpu->regs));
			return 1;
		}

		/* DEC B */ case 0x05: {
			_gb_dec(cpu, &GB_REG_B(cpu->regs));
			return 1;
		}

		/* DEC C */ case 0x0D: {
			_gb_dec(cpu, &GB_REG_C(cpu->regs));
			return 1;
		}

		/* DEC D */ case 0x15: {
			_gb_dec(cpu, &GB_REG_D(cpu->regs));
			return 1;
		}

		/* DEC E */ case 0x1D: {
			_gb_dec(cpu, &GB_REG_E(cpu->regs));
			return 1;
		}

		/* DEC H */ case 0x25: {
			_gb_dec(cpu, &GB_REG_H(cpu->regs));
			return 1;
		}

		/* DEC L */ case 0x2D: {
			_gb_dec(cpu, &GB_REG_L(cpu->regs));
			return 1;
		}

		/* DEC (HL) */ case 0x35: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_dec(cpu, &data);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 3;
		}

		/* DEC A */ case 0x3D: {
			_gb_dec(cpu, &GB_REG_A(cpu->regs));
			return 1;
		}

		/* INC BC */ case 0x03: {
			GB_REG_BC(cpu->regs) += 1;
			return 2;
		}

		/* INC DE */ case 0x13: {
			GB_REG_DE(cpu->regs) += 1;
			return 2;
		}

		/* INC HL */ case 0x23: {
			GB_REG_HL(cpu->regs) += 1;
			return 2;
		}

		/* INC SP */ case 0x33: {
			GB_REG_SP(cpu->regs) += 1;
			return 2;
		}

		/* DEC BC */ case 0x0B: {
			GB_REG_BC(cpu->regs) -= 1;
			return 2;
		}

		/* DEC DE */ case 0x1B: {
			GB_REG_DE(cpu->regs) -= 1;
			return 2;
		}

		/* DEC HL */ case 0x2B: {
			GB_REG_HL(cpu->regs) -= 1;
			return 2;
		}

		/* DEC SP */ case 0x3B: {
			GB_REG_SP(cpu->regs) -= 1;
			return 2;
		}

		/* LD (a16), SP */ case 0x08: {
		    u16 addr = gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs) += 2;
			gb_mmu_write_u8(cpu->mmu, addr, (u8)GB_REG_SP(cpu->regs));
			gb_mmu_write_u8(cpu->mmu, addr + 1, (u8)(GB_REG_SP(cpu->regs) >> 8));
			return 5;
		}

		/* LD SP, HL */ case 0xF9: {
			GB_REG_SP(cpu->regs) = GB_REG_HL(cpu->regs);
			return 2;
		}

		/* LD B, d8 */ case 0x06: {
			u16 addr = GB_REG_PC(cpu->regs)++;
			GB_REG_B(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD D, d8 */ case 0x16: {
			u16 addr = GB_REG_PC(cpu->regs)++;
			GB_REG_D(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD H, d8 */ case 0x26: {
			u16 addr = GB_REG_PC(cpu->regs)++;
			GB_REG_H(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD (HL), d8 */ case 0x36: {
			u16 read_addr = GB_REG_PC(cpu->regs)++;
			u16 write_addr = GB_REG_HL(cpu->regs);
			gb_mmu_write_u8(cpu->mmu, write_addr, gb_mmu_read_u8(cpu->mmu, read_addr));
			return 3;
		}

		/* LD C, d8 */ case 0x0E: {
			uint16_t addr = GB_REG_PC(cpu->regs)++;
			GB_REG_C(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD E, d8 */ case 0x1E: {
			uint16_t addr = GB_REG_PC(cpu->regs)++;
			GB_REG_E(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD L, d8 */ case 0x2E: {
			uint16_t addr = GB_REG_PC(cpu->regs)++;
			GB_REG_L(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD A, d8 */ case 0x3E: {
			uint16_t addr = GB_REG_PC(cpu->regs)++;
			GB_REG_A(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

			/* LOAD B, X */

		/* LD B, C */ case 0x41: {
			GB_REG_B(cpu->regs) = GB_REG_C(cpu->regs);
			return 1;
		}

		/* LD B, D */ case 0x42: {
			GB_REG_B(cpu->regs) = GB_REG_D(cpu->regs);
			return 1;
		}

		/* LD B, E */ case 0x43: {
			GB_REG_B(cpu->regs) = GB_REG_E(cpu->regs);
			return 1;
		}

		/* LD B, H */ case 0x44: {
			GB_REG_B(cpu->regs) = GB_REG_H(cpu->regs);
			return 1;
		}

		/* LD B, L */ case 0x45: {
			GB_REG_B(cpu->regs) = GB_REG_L(cpu->regs);
			return 1;
		}

		/* LD B, (HL) */ case 0x46: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			GB_REG_B(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD B, A */ case 0x47: {
			GB_REG_B(cpu->regs) = GB_REG_A(cpu->regs);
			return 1;
		}

			/* LOAD C, X */

		/* LD C, B */ case 0x48: {
			GB_REG_C(cpu->regs) = GB_REG_B(cpu->regs);
			return 1;
		}

		/* LD C, D */ case 0x4A: {
			GB_REG_C(cpu->regs) = GB_REG_D(cpu->regs);
			return 1;
		}

		/* LD C, E */ case 0x4B: {
			GB_REG_C(cpu->regs) = GB_REG_E(cpu->regs);
			return 1;
		}

		/* LD C, H */ case 0x4C: {
			GB_REG_C(cpu->regs) = GB_REG_H(cpu->regs);
			return 1;
		}

		/* LD C, L */ case 0x4D: {
			GB_REG_C(cpu->regs) = GB_REG_L(cpu->regs);
			return 1;
		}

		/* LD C, (HL) */ case 0x4E: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			GB_REG_C(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD C, A */ case 0x4F: {
			GB_REG_C(cpu->regs) = GB_REG_A(cpu->regs);
			return 1;
		}

			/* LOAD D, X */

		/* LD D, B */ case 0x50: {
			GB_REG_D(cpu->regs) = GB_REG_B(cpu->regs);
			return 1;
		}

		/* LD D, C */ case 0x51: {
			GB_REG_D(cpu->regs) = GB_REG_C(cpu->regs);
			return 1;
		}

		/* LD D, E */ case 0x53: {
			GB_REG_D(cpu->regs) = GB_REG_E(cpu->regs);
			return 1;
		}

		/* LD D, H */ case 0x54: {
			GB_REG_D(cpu->regs) = GB_REG_H(cpu->regs);
			return 1;
		}

		/* LD D, L */ case 0x55: {
			GB_REG_D(cpu->regs) = GB_REG_L(cpu->regs);
			return 1;
		}

		/* LD D, (HL) */ case 0x56: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			GB_REG_D(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD D, A */ case 0x57: {
			GB_REG_D(cpu->regs) = GB_REG_A(cpu->regs);
			return 1;
		}

			/* LOAD E, X */

		/* LD E, B */ case 0x58: {
			GB_REG_E(cpu->regs) = GB_REG_B(cpu->regs);
			return 1;
		}

		/* LD E, C */ case 0x59: {
			GB_REG_E(cpu->regs) = GB_REG_C(cpu->regs);
			return 1;
		}

		/* LD E, D */ case 0x5A: {
			GB_REG_E(cpu->regs) = GB_REG_D(cpu->regs);
			return 1;
		}

		/* LD E, H */ case 0x5C: {
			GB_REG_E(cpu->regs) = GB_REG_H(cpu->regs);
			return 1;
		}

		/* LD E, L */ case 0x5D: {
			GB_REG_E(cpu->regs) = GB_REG_L(cpu->regs);
			return 1;
		}

		/* LD E, (HL) */ case 0x5E: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			GB_REG_E(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD E, A */ case 0x5F: {
			GB_REG_E(cpu->regs) = GB_REG_A(cpu->regs);
			return 1;
		}

			/* LOAD H, X */

		/* LD H, B */ case 0x60: {
			GB_REG_H(cpu->regs) = GB_REG_B(cpu->regs);
			return 1;
		}

		/* LD H, C */ case 0x61: {
			GB_REG_H(cpu->regs) = GB_REG_C(cpu->regs);
			return 1;
		}

		/* LD H, D */ case 0x62: {
			GB_REG_H(cpu->regs) = GB_REG_D(cpu->regs);
			return 1;
		}

		/* LD H, E */ case 0x63: {
			GB_REG_H(cpu->regs) = GB_REG_E(cpu->regs);
			return 1;
		}

		/* LD H, L */ case 0x65: {
			GB_REG_H(cpu->regs) = GB_REG_L(cpu->regs);
			return 1;
		}

		/* LD H, (HL) */ case 0x66: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			GB_REG_H(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD H, A */ case 0x67: {
			GB_REG_H(cpu->regs) = GB_REG_A(cpu->regs);
			return 1;
		}

			/* LOAD L, X */

		/* LD L, B */ case 0x68: {
			GB_REG_L(cpu->regs) = GB_REG_B(cpu->regs);
			return 1;
		}

		/* LD L, C */ case 0x69: {
			GB_REG_L(cpu->regs) = GB_REG_C(cpu->regs);
			return 1;
		}

		/* LD L, D */ case 0x6A: {
			GB_REG_L(cpu->regs) = GB_REG_D(cpu->regs);
			return 1;
		}

		/* LD L, E */ case 0x6B: {
			GB_REG_L(cpu->regs) = GB_REG_E(cpu->regs);
			return 1;
		}

		/* LD L, H */ case 0x6C: {
			GB_REG_L(cpu->regs) = GB_REG_H(cpu->regs);
			return 1;
		}

		/* LD L, (HL) */ case 0x6E: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			GB_REG_L(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* LD L, A */ case 0x6F: {
			GB_REG_L(cpu->regs) = GB_REG_A(cpu->regs);
			return 1;
		}

			/* LOAD (HL), X */

		/* LD (HL), B */ case 0x70: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			gb_mmu_write_u8(cpu->mmu, addr, GB_REG_B(cpu->regs));
			return 2;
		}

		/* LD (HL), C */ case 0x71: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			gb_mmu_write_u8(cpu->mmu, addr, GB_REG_C(cpu->regs));
			return 2;
		}

		/* LD (HL), D */ case 0x72: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			gb_mmu_write_u8(cpu->mmu, addr, GB_REG_D(cpu->regs));
			return 2;
		}

		/* LD (HL), E */ case 0x73: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			gb_mmu_write_u8(cpu->mmu, addr, GB_REG_E(cpu->regs));
			return 2;
		}

		/* LD (HL), H */ case 0x74: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			gb_mmu_write_u8(cpu->mmu, addr, GB_REG_H(cpu->regs));
			return 2;
		}

		/* LD (HL), L */ case 0x75: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			gb_mmu_write_u8(cpu->mmu, addr, GB_REG_L(cpu->regs));
			return 2;
		}

		/* LD (HL), A */ case 0x77: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			gb_mmu_write_u8(cpu->mmu, addr, GB_REG_A(cpu->regs));
			return 2;
		}

			/* LOAD A, X */

		/* LD A, B */ case 0x78: {
			GB_REG_A(cpu->regs) = GB_REG_B(cpu->regs);
			return 1;
		}

		/* LD A, C */ case 0x79: {
			GB_REG_A(cpu->regs) = GB_REG_C(cpu->regs);
			return 1;
		}

		/* LD A, D */ case 0x7A: {
			GB_REG_A(cpu->regs) = GB_REG_D(cpu->regs);
			return 1;
		}

		/* LD A, E */ case 0x7B: {
			GB_REG_A(cpu->regs) = GB_REG_E(cpu->regs);
			return 1;
		}

		/* LD A, H */ case 0x7C: {
			GB_REG_A(cpu->regs) = GB_REG_H(cpu->regs);
			return 1;
		}

		/* LD A, L */ case 0x7D: {
			GB_REG_A(cpu->regs) = GB_REG_L(cpu->regs);
			return 1;
		}

		/* LD A, (HL) */ case 0x7E: {
			uint16_t addr = GB_REG_HL(cpu->regs);
			GB_REG_A(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* jp, a16 */ case 0xC3: {
			GB_REG_PC(cpu->regs) =
				gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
			return 4;
		}

		/* jp, HL */ case 0xE9: {
			GB_REG_PC(cpu->regs) = GB_REG_HL(cpu->regs);
			return 1;
		}

		/* JP Z, a16 */ case 0xCA: {
			if (GB_REG_F(cpu->regs) & GB_FLAG_ZERO) {
				GB_REG_PC(cpu->regs) =
					gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
				return 4;
			}
			GB_REG_PC(cpu->regs) += 2;
			return 3;
		}

		/* JP C, a16 */ case 0xDA: {
			if (GB_REG_F(cpu->regs) & GB_FLAG_CARR) {
				GB_REG_PC(cpu->regs) =
					gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
				return 4;
			}
			GB_REG_PC(cpu->regs) += 2;
			return 3;
		}

		/* JP NZ, a16 */ case 0xC2: {
			if (!(GB_REG_F(cpu->regs) & GB_FLAG_ZERO)) {
				GB_REG_PC(cpu->regs) =
					gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
				return 4;
			}
			GB_REG_PC(cpu->regs) += 2;
			return 3;
		}

		/* JP NC, a16 */ case 0xD2: {
			if (!(GB_REG_F(cpu->regs) & GB_FLAG_CARR)) {
				GB_REG_PC(cpu->regs) =
					gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
				return 4;
			}
			GB_REG_PC(cpu->regs) += 2;
			return 3;
		}

		/* CALL NZ, a16 */ case 0xC4: {
			if (!(GB_REG_F(cpu->regs) & GB_FLAG_ZERO)) {
				_gb_call(cpu);
				return 6;
			}
			GB_REG_PC(cpu->regs) += 2;
			return 3;
		}
		/* CALL NC, a16 */ case 0xD4: {
			if (!(GB_REG_F(cpu->regs) & GB_FLAG_CARR)) {
				_gb_call(cpu);
				return 6;
			}
			GB_REG_PC(cpu->regs) += 2;
			return 3;
		}

		/* CALL Z, a16 */ case 0xCC: {
			if (GB_REG_F(cpu->regs) & GB_FLAG_ZERO) {
				_gb_call(cpu);
				return 6;
			}
			GB_REG_PC(cpu->regs) += 2;
			return 3;
		}
		/* CALL C, a16 */ case 0xDC: {
			if (GB_REG_F(cpu->regs) & GB_FLAG_CARR) {
				_gb_call(cpu);
				return 6;
			}
			GB_REG_PC(cpu->regs) += 2;
			return 3;
		}

		/* CALL, a16 */ case 0xCD: {
			_gb_call(cpu);
			return 6;
		}

		/* RET */ case 0xC9: {
			GB_REG_PC(cpu->regs) = gb_mmu_pop_u16(cpu->mmu);
			return 4;
		}
		/* RET Z */ case 0xC8: {
			if ((GB_REG_F(cpu->regs) & GB_FLAG_ZERO)) {
				GB_REG_PC(cpu->regs) = gb_mmu_pop_u16(cpu->mmu);
				return 5;
			}
			return 2;
		}
		/* RET C */ case 0xD8: {
			if ((GB_REG_F(cpu->regs) & GB_FLAG_CARR)) {
				GB_REG_PC(cpu->regs) = gb_mmu_pop_u16(cpu->mmu);
				return 5;
			}
			return 2;
		}
		/* RET NZ */ case 0xC0: {
			if (!(GB_REG_F(cpu->regs) & GB_FLAG_ZERO)) {
				GB_REG_PC(cpu->regs) = gb_mmu_pop_u16(cpu->mmu);
				return 5;
			}
			return 2;
		}
		/* RET NC */ case 0xD0: {
			if (!(GB_REG_F(cpu->regs) & GB_FLAG_CARR)) {
				GB_REG_PC(cpu->regs) = gb_mmu_pop_u16(cpu->mmu);
				return 5;
			}
			return 2;
		}

		/* JR s8 */ case 0x18: {
			const i8 d = gb_mmu_read_i8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			GB_REG_PC(cpu->regs) += (u16)(i16)d;
			return 3;
		}
		/* JR Z, s8 */ case 0x28: {
			const i8 d = gb_mmu_read_i8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			if ((GB_REG_F(cpu->regs) & GB_FLAG_ZERO)) {
				GB_REG_PC(cpu->regs) += (u16)(i16)d;
				return 3;
			}
			return 2;
		}
		/* JR C, s8 */ case 0x38: {
			const i8 d = gb_mmu_read_i8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			if ((GB_REG_F(cpu->regs) & GB_FLAG_CARR)) {
				GB_REG_PC(cpu->regs) += (u16)(i16)d;
				return 3;
			}
			return 2;
		}

		/* JR NZ, s8 */ case 0x20: {
			const i8 d = gb_mmu_read_i8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			if (!(GB_REG_F(cpu->regs) & GB_FLAG_ZERO)) {
				GB_REG_PC(cpu->regs) += (u16)(i16)d;
				return 3;
			}
			return 2;
		}
		/* JR NC, s8 */ case 0x30: {
			const i8 d = gb_mmu_read_i8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			if (!(GB_REG_F(cpu->regs) & GB_FLAG_CARR)) {
				GB_REG_PC(cpu->regs) += (u16)(i16)d;
				return 3;
			}
			return 2;
		}

		/* LD (BC), A */ case 0x02: {
			gb_mmu_write_u8(
				cpu->mmu, GB_REG_BC(cpu->regs), GB_REG_A(cpu->regs)
			);
			return 2;
		}
		/* LD (DE), A */ case 0x12: {
			gb_mmu_write_u8(
				cpu->mmu, GB_REG_DE(cpu->regs), GB_REG_A(cpu->regs)
			);
			return 2;
		}

		/* LD (HL+), A */ case 0x22: {
			gb_mmu_write_u8(
				cpu->mmu, GB_REG_HL(cpu->regs), GB_REG_A(cpu->regs)
			);
			GB_REG_HL(cpu->regs) += 1;
			return 2;
		}

		/* LD (HL-), A */ case 0x32: {
			gb_mmu_write_u8(
				cpu->mmu, GB_REG_HL(cpu->regs), GB_REG_A(cpu->regs)
			);
			GB_REG_HL(cpu->regs) -= 1;
			return 2;
		}

		/* LD A, (BC) */ case 0x0A: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_BC(cpu->regs));
			GB_REG_A(cpu->regs) = data;
			return 2;
		}

		/* LD A, (DE) */ case 0x1A: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_DE(cpu->regs));
			GB_REG_A(cpu->regs) = data;
			return 2;
		}

		/* LD A, (HL+) */ case 0x2A: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			GB_REG_HL(cpu->regs) += 1;
			GB_REG_A(cpu->regs) = data;
			return 2;
		}

		/* LD A, (HL-) */ case 0x3A: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			GB_REG_HL(cpu->regs) -= 1;
			GB_REG_A(cpu->regs) = data;
			return 2;
		}

		/* LD BC, d16 */ case 0x01: {
			const u16 d = gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs) += 2;
			GB_REG_BC(cpu->regs) = d;
			return 3;
		}
		/* LD DE, d16 */ case 0x11: {
			const u16 d = gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs) += 2;
			GB_REG_DE(cpu->regs) = d;
			return 3;
		}
		/* LD HL, d16 */ case 0x21: {
			const u16 d = gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs) += 2;
			GB_REG_HL(cpu->regs) = d;
			return 3;
		}
		/* LD SP, d16 */ case 0x31: {
			const u16 d = gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs) += 2;
			GB_REG_SP(cpu->regs) = d;
			return 3;
		}

		/* OR B */ case 0xB0: {
			_gb_or(cpu, GB_REG_B(cpu->regs));
			return 1;
		}
		/* OR C */ case 0xB1: {
			_gb_or(cpu, GB_REG_C(cpu->regs));
			return 1;
		}
		/* OR D */ case 0xB2: {
			_gb_or(cpu, GB_REG_D(cpu->regs));
			return 1;
		}
		/* OR E */ case 0xB3: {
			_gb_or(cpu, GB_REG_E(cpu->regs));
			return 1;
		}
		/* OR H */ case 0xB4: {
			_gb_or(cpu, GB_REG_H(cpu->regs));
			return 1;
		}
		/* OR L */ case 0xB5: {
			_gb_or(cpu, GB_REG_L(cpu->regs));
			return 1;
		}
		/* OR [HL] */ case 0xB6: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_or(cpu, d);
			return 2;
		}
		/* OR A */ case 0xB7: {
			_gb_or(cpu, GB_REG_A(cpu->regs));
			return 1;
		}
		/* OR d8 */ case 0xF6: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			_gb_or(cpu, d);
			return 2;
		}

		/* XOR B */ case 0xA8: {
			_gb_xor(cpu, GB_REG_B(cpu->regs));
			return 1;
		}
		/* XOR C */ case 0xA9: {
			_gb_xor(cpu, GB_REG_C(cpu->regs));
			return 1;
		}
		/* XOR D */ case 0xAA: {
			_gb_xor(cpu, GB_REG_D(cpu->regs));
			return 1;
		}
		/* XOR E */ case 0xAB: {
			_gb_xor(cpu, GB_REG_E(cpu->regs));
			return 1;
		}
		/* XOR H */ case 0xAC: {
			_gb_xor(cpu, GB_REG_H(cpu->regs));
			return 1;
		}
		/* XOR L */ case 0xAD: {
			_gb_xor(cpu, GB_REG_L(cpu->regs));
			return 1;
		}
		/* XOR [HL] */ case 0xAE: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_xor(cpu, d);
			return 2;
		}
		/* XOR A */ case 0xAF: {
			_gb_xor(cpu, GB_REG_A(cpu->regs));
			return 1;
		}
		/* XOR d8 */ case 0xEE: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			_gb_xor(cpu, d);
			return 2;
		}

		/* CP B */ case 0xB8: {
			_gb_cp(cpu, GB_REG_B(cpu->regs));
			return 1;
		}
		/* CP C */ case 0xB9: {
			_gb_cp(cpu, GB_REG_C(cpu->regs));
			return 1;
		}
		/* CP D */ case 0xBA: {
			_gb_cp(cpu, GB_REG_D(cpu->regs));
			return 1;
		}
		/* CP E */ case 0xBB: {
			_gb_cp(cpu, GB_REG_E(cpu->regs));
			return 1;
		}
		/* CP H */ case 0xBC: {
			_gb_cp(cpu, GB_REG_H(cpu->regs));
			return 1;
		}
		/* CP L */ case 0xBD: {
			_gb_cp(cpu, GB_REG_L(cpu->regs));
			return 1;
		}
		/* CP [HL] */ case 0xBE: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_cp(cpu, d);
			return 2;
		}
		/* CP A */ case 0xBF: {
			_gb_cp(cpu, GB_REG_A(cpu->regs));
			return 1;
		}
		/* CP d8 */ case 0xFE: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			_gb_cp(cpu, d);
			return 2;
		}

		/* AND B */ case 0xA0: {
			_gb_and(cpu, GB_REG_B(cpu->regs));
			return 1;
		}
		/* AND C */ case 0xA1: {
			_gb_and(cpu, GB_REG_C(cpu->regs));
			return 1;
		}
		/* AND D */ case 0xA2: {
			_gb_and(cpu, GB_REG_D(cpu->regs));
			return 1;
		}
		/* AND E */ case 0xA3: {
			_gb_and(cpu, GB_REG_E(cpu->regs));
			return 1;
		}
		/* AND H */ case 0xA4: {
			_gb_and(cpu, GB_REG_H(cpu->regs));
			return 1;
		}
		/* AND L */ case 0xA5: {
			_gb_and(cpu, GB_REG_L(cpu->regs));
			return 1;
		}
		/* AND [HL] */ case 0xA6: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_and(cpu, d);
			return 2;
		}
		/* AND A */ case 0xA7: {
			_gb_and(cpu, GB_REG_A(cpu->regs));
			return 1;
		}
		/* AND d8 */ case 0xE6: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			_gb_and(cpu, d);
			return 2;
		}

		/* ADD SP, s8 */ case 0xE8: {
		    const i8 imm = gb_mmu_read_i8(cpu->mmu, GB_REG_PC(cpu->regs));
		    const u16 reg = GB_REG_SP(cpu->regs);
			const u16 result = (u16)(reg + imm);

			GB_REG_PC(cpu->regs) += 1;

			GB_REG_F(cpu->regs) = 0;
			if ((reg & 0xF) + (imm & 0xF) > 0xF) {
			    GB_REG_F(cpu->regs) |= GB_FLAG_HALF;
			}
			if ((reg & 0xFF) + (imm & 0xFF) > 0xFF) {
			    GB_REG_F(cpu->regs) |= GB_FLAG_CARR;
			}
			GB_REG_SP(cpu->regs) = result;
			return 4;
		}

		/* LD HL, (SP+s8) */ case 0xF8: {
		    const i8 imm = gb_mmu_read_i8(cpu->mmu, GB_REG_PC(cpu->regs));
		    const u16 reg = GB_REG_SP(cpu->regs);
			const u16 result = (u16)(reg + imm);

			GB_REG_PC(cpu->regs) += 1;

			GB_REG_F(cpu->regs) = 0;
			if ((reg & 0xF) + (imm & 0xF) > 0xF) {
			    GB_REG_F(cpu->regs) |= GB_FLAG_HALF;
			}
			if ((reg & 0xFF) + (imm & 0xFF) > 0xFF) {
			    GB_REG_F(cpu->regs) |= GB_FLAG_CARR;
			}
			GB_REG_HL(cpu->regs) = result;
			return 3;
		}

		/* ADD B */ case 0x80: {
			_gb_add(cpu, GB_REG_B(cpu->regs));
			return 1;
		}
		/* ADD C */ case 0x81: {
			_gb_add(cpu, GB_REG_C(cpu->regs));
			return 1;
		}
		/* ADD D */ case 0x82: {
			_gb_add(cpu, GB_REG_D(cpu->regs));
			return 1;
		}
		/* ADD E */ case 0x83: {
			_gb_add(cpu, GB_REG_E(cpu->regs));
			return 1;
		}
		/* ADD H */ case 0x84: {
			_gb_add(cpu, GB_REG_H(cpu->regs));
			return 1;
		}
		/* ADD L */ case 0x85: {
			_gb_add(cpu, GB_REG_L(cpu->regs));
			return 1;
		}
		/* ADD [HL] */ case 0x86: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_add(cpu, d);
			return 2;
		}
		/* ADD A */ case 0x87: {
			_gb_add(cpu, GB_REG_A(cpu->regs));
			return 1;
		}
		/* ADD d8 */ case 0xC6: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			_gb_add(cpu, d);
			return 2;
		}

		/* ADC B */ case 0x88: {
			_gb_adc(cpu, GB_REG_B(cpu->regs));
			return 1;
		}
		/* ADC C */ case 0x89: {
			_gb_adc(cpu, GB_REG_C(cpu->regs));
			return 1;
		}
		/* ADC D */ case 0x8A: {
			_gb_adc(cpu, GB_REG_D(cpu->regs));
			return 1;
		}
		/* ADC E */ case 0x8B: {
			_gb_adc(cpu, GB_REG_E(cpu->regs));
			return 1;
		}
		/* ADC H */ case 0x8C: {
			_gb_adc(cpu, GB_REG_H(cpu->regs));
			return 1;
		}
		/* ADC L */ case 0x8D: {
			_gb_adc(cpu, GB_REG_L(cpu->regs));
			return 1;
		}
		/* ADC [HL] */ case 0x8E: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_adc(cpu, d);
			return 2;
		}
		/* ADC A */ case 0x8F: {
			_gb_adc(cpu, GB_REG_A(cpu->regs));
			return 1;
		}
		/* ADC d8 */ case 0xCE: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			_gb_adc(cpu, d);
			return 2;
		}

		/* SBC B */ case 0x98: {
			_gb_sbc(cpu, GB_REG_B(cpu->regs));
			return 1;
		}
		/* SBC C */ case 0x99: {
			_gb_sbc(cpu, GB_REG_C(cpu->regs));
			return 1;
		}
		/* SBC D */ case 0x9A: {
			_gb_sbc(cpu, GB_REG_D(cpu->regs));
			return 1;
		}
		/* SBC E */ case 0x9B: {
			_gb_sbc(cpu, GB_REG_E(cpu->regs));
			return 1;
		}
		/* SBC H */ case 0x9C: {
			_gb_sbc(cpu, GB_REG_H(cpu->regs));
			return 1;
		}
		/* SBC L */ case 0x9D: {
			_gb_sbc(cpu, GB_REG_L(cpu->regs));
			return 1;
		}
		/* SBC [HL] */ case 0x9E: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_sbc(cpu, d);
			return 2;
		}
		/* SBC A */ case 0x9F: {
			_gb_sbc(cpu, GB_REG_A(cpu->regs));
			return 1;
		}
		/* SBC d8 */ case 0xDE: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			_gb_sbc(cpu, d);
			return 2;
		}

		/* LD (a8), A */ case 0xE0: {
			u16 addr = (u16)0xFF00 |
					   (u16)gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			gb_mmu_write_u8(cpu->mmu, addr, GB_REG_A(cpu->regs));
			return 3;
		}

		/* LD A, (a8) */ case 0xF0: {
			u16 addr = (u16)0xFF00 |
					   (u16)gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			GB_REG_A(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 3;
		}

		/* LD (a16), A */ case 0xEA: {
			u16 addr = gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs) += 2;
			gb_mmu_write_u8(cpu->mmu, addr, GB_REG_A(cpu->regs));
			return 4;
		}

		/* LD A, (a16) */ case 0xFA: {
			u16 addr = gb_mmu_read_u16(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs) += 2;
			GB_REG_A(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 4;
		}

		/* SUB B */ case 0x90: {
			_gb_sub(cpu, GB_REG_B(cpu->regs));
			return 1;
		}
		/* SUB C */ case 0x91: {
			_gb_sub(cpu, GB_REG_C(cpu->regs));
			return 1;
		}
		/* SUB D */ case 0x92: {
			_gb_sub(cpu, GB_REG_D(cpu->regs));
			return 1;
		}
		/* SUB E */ case 0x93: {
			_gb_sub(cpu, GB_REG_E(cpu->regs));
			return 1;
		}
		/* SUB H */ case 0x94: {
			_gb_sub(cpu, GB_REG_H(cpu->regs));
			return 1;
		}
		/* SUB L */ case 0x95: {
			_gb_sub(cpu, GB_REG_L(cpu->regs));
			return 1;
		}
		/* SUB [HL] */ case 0x96: {
			const u8 d = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_sub(cpu, d);
			return 2;
		}
		/* SUB A */ case 0x97: {
			_gb_sub(cpu, GB_REG_A(cpu->regs));
			return 1;
		}
		/* SUB d8 */ case 0xD6: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs));
			GB_REG_PC(cpu->regs)++;
			_gb_sub(cpu, data);
			return 2;
		}

		/* ADD HL, BC */ case 0x09: {
			_gb_add_u16(cpu, GB_REG_BC(cpu->regs));
			return 2;
		}
		/* ADD HL, DE */ case 0x19: {
			_gb_add_u16(cpu, GB_REG_DE(cpu->regs));
			return 2;
		}
		/* ADD HL, HL */ case 0x29: {
			_gb_add_u16(cpu, GB_REG_HL(cpu->regs));
			return 2;
		}
		/* ADD HL, SP */ case 0x39: {
			_gb_add_u16(cpu, GB_REG_SP(cpu->regs));
			return 2;
		}

		/* POP BC */ case 0xC1: {
			GB_REG_BC(cpu->regs) = gb_mmu_pop_u16(cpu->mmu);
			return 3;
		}
		/* POP DE */ case 0xD1: {
			GB_REG_DE(cpu->regs) = gb_mmu_pop_u16(cpu->mmu);
			return 3;
		}
		/* POP HL */ case 0xE1: {
			GB_REG_HL(cpu->regs) = gb_mmu_pop_u16(cpu->mmu);
			return 3;
		}
		/* POP AF */ case 0xF1: {
			GB_REG_AF(cpu->regs) = gb_mmu_pop_u16(cpu->mmu) & 0xFFF0;
			return 3;
		}

		/* PUSH BC */ case 0xC5: {
			gb_mmu_push_u16(cpu->mmu, GB_REG_BC(cpu->regs));
			return 4;
		}
		/* PUSH DE */ case 0xD5: {
			gb_mmu_push_u16(cpu->mmu, GB_REG_DE(cpu->regs));
			return 4;
		}
		/* PUSH HL */ case 0xE5: {
			gb_mmu_push_u16(cpu->mmu, GB_REG_HL(cpu->regs));
			return 4;
		}
		/* PUSH AF */ case 0xF5: {
			gb_mmu_push_u16(cpu->mmu, GB_REG_AF(cpu->regs));
			return 4;
		}

		/* RST 0x00 */ case 0xC7: {
			_gb_rst(cpu, 0x00);
			return 4;
		}
		/* RST 0x10 */ case 0xD7: {
			_gb_rst(cpu, 0x10);
			return 4;
		}
		/* RST 0x20 */ case 0xE7: {
			_gb_rst(cpu, 0x20);
			return 4;
		}
		/* RST 0x30 */ case 0xF7: {
			_gb_rst(cpu, 0x30);
			return 4;
		}

		/* RST 0x08 */ case 0xCF: {
			_gb_rst(cpu, 0x08);
			return 4;
		}
		/* RST 0x18 */ case 0xDF: {
			_gb_rst(cpu, 0x18);
			return 4;
		}
		/* RST 0x18 */ case 0xEF: {
			_gb_rst(cpu, 0x28);
			return 4;
		}
		/* RST 0x18 */ case 0xFF: {
			_gb_rst(cpu, 0x38);
			return 4;
		}

		/* RETI */ case 0xD9: {
			cpu->interrupt_enable = 1;
			GB_REG_PC(cpu->regs) = gb_mmu_pop_u16(cpu->mmu);
			return 4;
		}

		/* DI */ case 0xF3: {
		    cpu->interrupt_enable = 0;
			return 1;
		}

		/* EI */ case 0xFB: {
		    cpu->interrupt_enable = 1;
			return 1;
		}

		/* HALT */ case 0x76: {
		    // Halt bug?
		    if (!cpu->interrupt_enable) {
				const u8 IE = GB_CPU_MEM(cpu, GB_ADDR_IE);
				const u8 IF = GB_CPU_MEM(cpu, GB_ADDR_IF);
				if ((IE & IF & GB_INTERRUPT_MASK) != 0) {
				    cpu->halt_bugged = 1;
				    return 1;
				}
			}

		    cpu->is_halted = 1;
		    return 1;
		}

		/* STOP */ case 0x10: {
		    // @TODO: CGB

			const u8 IE = GB_CPU_MEM(cpu, GB_ADDR_IE);
			const u8 IF = GB_CPU_MEM(cpu, GB_ADDR_IF);
			// reset timers
			gb_mmu_write_u8(cpu->mmu, GB_ADDR_DIV, 0x00);
			cpu->is_halted = 1; // @TODO: proper "stop mode"
			if ((IE & IF) != 0) {
			    GB_REG_PC(cpu->regs) += 1;
			}
		    return 1;
		}

		/* LD (C), A */ case 0xE2: {
			u16 addr = 0xFF00 | GB_REG_C(cpu->regs);
			gb_mmu_write_u8(cpu->mmu, addr, GB_REG_A(cpu->regs));
			return 2;
		}

		/* LD A, (C) */ case 0xF2: {
			u16 addr = 0xFF00 | GB_REG_C(cpu->regs);
			GB_REG_A(cpu->regs) = gb_mmu_read_u8(cpu->mmu, addr);
			return 2;
		}

		/* DAA */ case 0x27: {
		    _gb_daa(cpu);
		    return 1;
		}

		/* CB PREFIX */ case 0xCB: {
			return _gb_emu_execute_cb(cpu);
		}

		default: {
			GB_FATAL("UNIMPLEMENTED OPCODE! 0x%X\n", opcode);
			return 1;
			break;
		}
	}
	return 1;
}

static u8 _gb_emu_execute_cb(gb_sm83_t* cpu)
{
	u8 opcode = gb_mmu_read_u8(cpu->mmu, GB_REG_PC(cpu->regs)++);
	switch (opcode) {
		/* SWAP B */ case 0x30: {
			_gb_swap(cpu, &GB_REG_B(cpu->regs));
			return 2;
		}
		/* SWAP C */ case 0x31: {
			_gb_swap(cpu, &GB_REG_C(cpu->regs));
			return 2;
		}
		/* SWAP D */ case 0x32: {
			_gb_swap(cpu, &GB_REG_D(cpu->regs));
			return 2;
		}
		/* SWAP E */ case 0x33: {
			_gb_swap(cpu, &GB_REG_E(cpu->regs));
			return 2;
		}
		/* SWAP H */ case 0x34: {
			_gb_swap(cpu, &GB_REG_H(cpu->regs));
			return 2;
		}
		/* SWAP L */ case 0x35: {
			_gb_swap(cpu, &GB_REG_L(cpu->regs));
			return 2;
		}
		/* SWAP (HL) */ case 0x36: {
			u8 value = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_swap(cpu, &value);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), value);
			return 4;
		}
		/* SWAP A */ case 0x37: {
			_gb_swap(cpu, &GB_REG_A(cpu->regs));
			return 2;
		}

		/* RLC B */ case 0x00: {
			_gb_rlc(cpu, &GB_REG_B(cpu->regs));
			return 2;
		}
		/* RLC C */ case 0x01: {
			_gb_rlc(cpu, &GB_REG_C(cpu->regs));
			return 2;
		}
		/* RLC D */ case 0x02: {
			_gb_rlc(cpu, &GB_REG_D(cpu->regs));
			return 2;
		}
		/* RLC E */ case 0x03: {
			_gb_rlc(cpu, &GB_REG_E(cpu->regs));
			return 2;
		}
		/* RLC H */ case 0x04: {
			_gb_rlc(cpu, &GB_REG_H(cpu->regs));
			return 2;
		}
		/* RLC L */ case 0x05: {
			_gb_rlc(cpu, &GB_REG_L(cpu->regs));
			return 2;
		}
		/* RLC (HL) */ case 0x06: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_rlc(cpu, &data);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RLC A */ case 0x07: {
			_gb_rlc(cpu, &GB_REG_A(cpu->regs));
			return 2;
		}

		/* RRC B */ case 0x08: {
			_gb_rrc(cpu, &GB_REG_B(cpu->regs));
			return 2;
		}
		/* RRC C */ case 0x09: {
			_gb_rrc(cpu, &GB_REG_C(cpu->regs));
			return 2;
		}
		/* RRC D */ case 0x0A: {
			_gb_rrc(cpu, &GB_REG_D(cpu->regs));
			return 2;
		}
		/* RRC E */ case 0x0B: {
			_gb_rrc(cpu, &GB_REG_E(cpu->regs));
			return 2;
		}
		/* RRC H */ case 0x0C: {
			_gb_rrc(cpu, &GB_REG_H(cpu->regs));
			return 2;
		}
		/* RRC L */ case 0x0D: {
			_gb_rrc(cpu, &GB_REG_L(cpu->regs));
			return 2;
		}
		/* RRC (HL) */ case 0x0E: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_rrc(cpu, &data);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RRC A */ case 0x0F: {
			_gb_rrc(cpu, &GB_REG_A(cpu->regs));
			return 2;
		}

		/* RL B */ case 0x10: {
			_gb_rl(cpu, &GB_REG_B(cpu->regs));
			return 2;
		}
		/* RL C */ case 0x11: {
			_gb_rl(cpu, &GB_REG_C(cpu->regs));
			return 2;
		}
		/* RL D */ case 0x12: {
			_gb_rl(cpu, &GB_REG_D(cpu->regs));
			return 2;
		}
		/* RL E */ case 0x13: {
			_gb_rl(cpu, &GB_REG_E(cpu->regs));
			return 2;
		}
		/* RL H */ case 0x14: {
			_gb_rl(cpu, &GB_REG_H(cpu->regs));
			return 2;
		}
		/* RL L */ case 0x15: {
			_gb_rl(cpu, &GB_REG_L(cpu->regs));
			return 2;
		}
		/* RL (HL) */ case 0x16: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_rl(cpu, &data);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RL A */ case 0x17: {
			_gb_rl(cpu, &GB_REG_A(cpu->regs));
			return 2;
		}

		/* RR B */ case 0x18: {
			_gb_rr(cpu, &GB_REG_B(cpu->regs));
			return 2;
		}
		/* RR C */ case 0x19: {
			_gb_rr(cpu, &GB_REG_C(cpu->regs));
			return 2;
		}
		/* RR D */ case 0x1A: {
			_gb_rr(cpu, &GB_REG_D(cpu->regs));
			return 2;
		}
		/* RR E */ case 0x1B: {
			_gb_rr(cpu, &GB_REG_E(cpu->regs));
			return 2;
		}
		/* RR H */ case 0x1C: {
			_gb_rr(cpu, &GB_REG_H(cpu->regs));
			return 2;
		}
		/* RR L */ case 0x1D: {
			_gb_rr(cpu, &GB_REG_L(cpu->regs));
			return 2;
		}
		/* RR (HL) */ case 0x1E: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_rr(cpu, &data);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RR A */ case 0x1F: {
			_gb_rr(cpu, &GB_REG_A(cpu->regs));
			return 2;
		}

		/* SRA B */ case 0x28: {
			_gb_sra(cpu, &GB_REG_B(cpu->regs));
			return 2;
		}
		/* SRA C */ case 0x29: {
			_gb_sra(cpu, &GB_REG_C(cpu->regs));
			return 2;
		}
		/* SRA D */ case 0x2A: {
			_gb_sra(cpu, &GB_REG_D(cpu->regs));
			return 2;
		}
		/* SRA E */ case 0x2B: {
			_gb_sra(cpu, &GB_REG_E(cpu->regs));
			return 2;
		}
		/* SRA H */ case 0x2C: {
			_gb_sra(cpu, &GB_REG_H(cpu->regs));
			return 2;
		}
		/* SRA L */ case 0x2D: {
			_gb_sra(cpu, &GB_REG_L(cpu->regs));
			return 2;
		}
		/* SRA (HL) */ case 0x2E: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_sra(cpu, &data);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SRA A */ case 0x2F: {
			_gb_sra(cpu, &GB_REG_A(cpu->regs));
			return 2;
		}

		/* SRL B */ case 0x38: {
			_gb_srl(cpu, &GB_REG_B(cpu->regs));
			return 2;
		}
		/* SRL C */ case 0x39: {
			_gb_srl(cpu, &GB_REG_C(cpu->regs));
			return 2;
		}
		/* SRL D */ case 0x3A: {
			_gb_srl(cpu, &GB_REG_D(cpu->regs));
			return 2;
		}
		/* SRL E */ case 0x3B: {
			_gb_srl(cpu, &GB_REG_E(cpu->regs));
			return 2;
		}
		/* SRL H */ case 0x3C: {
			_gb_srl(cpu, &GB_REG_H(cpu->regs));
			return 2;
		}
		/* SRL L */ case 0x3D: {
			_gb_srl(cpu, &GB_REG_L(cpu->regs));
			return 2;
		}
		/* SRL (HL) */ case 0x3E: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_srl(cpu, &data);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SRL A */ case 0x3F: {
			_gb_srl(cpu, &GB_REG_A(cpu->regs));
			return 2;
		}

		/* SLA B */ case 0x20: {
			_gb_sla(cpu, &GB_REG_B(cpu->regs));
			return 2;
		}
		/* SLA C */ case 0x21: {
			_gb_sla(cpu, &GB_REG_C(cpu->regs));
			return 2;
		}
		/* SLA D */ case 0x22: {
			_gb_sla(cpu, &GB_REG_D(cpu->regs));
			return 2;
		}
		/* SLA E */ case 0x23: {
			_gb_sla(cpu, &GB_REG_E(cpu->regs));
			return 2;
		}
		/* SLA H */ case 0x24: {
			_gb_sla(cpu, &GB_REG_H(cpu->regs));
			return 2;
		}
		/* SLA L */ case 0x25: {
			_gb_sla(cpu, &GB_REG_L(cpu->regs));
			return 2;
		}
		/* SLA (HL) */ case 0x26: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_sla(cpu, &data);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SLA A */ case 0x27: {
			_gb_sla(cpu, &GB_REG_A(cpu->regs));
			return 2;
		}

		/* BIT 0, B */ case 0x40: {
			_gb_get_bit(cpu, GB_REG_B(cpu->regs), 0);
			return 2;
		}
		/* BIT 0, C */ case 0x41: {
			_gb_get_bit(cpu, GB_REG_C(cpu->regs), 0);
			return 2;
		}
		/* BIT 0, D */ case 0x42: {
			_gb_get_bit(cpu, GB_REG_D(cpu->regs), 0);
			return 2;
		}
		/* BIT 0, E */ case 0x43: {
			_gb_get_bit(cpu, GB_REG_E(cpu->regs), 0);
			return 2;
		}
		/* BIT 0, H */ case 0x44: {
			_gb_get_bit(cpu, GB_REG_H(cpu->regs), 0);
			return 2;
		}
		/* BIT 0, L */ case 0x45: {
			_gb_get_bit(cpu, GB_REG_L(cpu->regs), 0);
			return 2;
		}
		/* BIT 0, (HL) */ case 0x46: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_get_bit(cpu, data, 0);
			return 3;
		}
		/* BIT 0, A */ case 0x47: {
			_gb_get_bit(cpu, GB_REG_A(cpu->regs), 0);
			return 2;
		}

		/* BIT 1, B */ case 0x48: {
			_gb_get_bit(cpu, GB_REG_B(cpu->regs), 1);
			return 2;
		}
		/* BIT 1, C */ case 0x49: {
			_gb_get_bit(cpu, GB_REG_C(cpu->regs), 1);
			return 2;
		}
		/* BIT 1, D */ case 0x4A: {
			_gb_get_bit(cpu, GB_REG_D(cpu->regs), 1);
			return 2;
		}
		/* BIT 1, E */ case 0x4B: {
			_gb_get_bit(cpu, GB_REG_E(cpu->regs), 1);
			return 2;
		}
		/* BIT 1, H */ case 0x4C: {
			_gb_get_bit(cpu, GB_REG_H(cpu->regs), 1);
			return 2;
		}
		/* BIT 1, L */ case 0x4D: {
			_gb_get_bit(cpu, GB_REG_L(cpu->regs), 1);
			return 2;
		}
		/* BIT 1, (HL) */ case 0x4E: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_get_bit(cpu, data, 1);
			return 3;
		}
		/* BIT 1, A */ case 0x4F: {
			_gb_get_bit(cpu, GB_REG_A(cpu->regs), 1);
			return 2;
		}

		/* BIT 2, B */ case 0x50: {
			_gb_get_bit(cpu, GB_REG_B(cpu->regs), 2);
			return 2;
		}
		/* BIT 2, C */ case 0x51: {
			_gb_get_bit(cpu, GB_REG_C(cpu->regs), 2);
			return 2;
		}
		/* BIT 2, D */ case 0x52: {
			_gb_get_bit(cpu, GB_REG_D(cpu->regs), 2);
			return 2;
		}
		/* BIT 2, E */ case 0x53: {
			_gb_get_bit(cpu, GB_REG_E(cpu->regs), 2);
			return 2;
		}
		/* BIT 2, H */ case 0x54: {
			_gb_get_bit(cpu, GB_REG_H(cpu->regs), 2);
			return 2;
		}
		/* BIT 2, L */ case 0x55: {
			_gb_get_bit(cpu, GB_REG_L(cpu->regs), 2);
			return 2;
		}
		/* BIT 2, (HL) */ case 0x56: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_get_bit(cpu, data, 2);
			return 3;
		}
		/* BIT 2, A */ case 0x57: {
			_gb_get_bit(cpu, GB_REG_A(cpu->regs), 2);
			return 2;
		}

		/* BIT 3, B */ case 0x58: {
			_gb_get_bit(cpu, GB_REG_B(cpu->regs), 3);
			return 2;
		}
		/* BIT 3, C */ case 0x59: {
			_gb_get_bit(cpu, GB_REG_C(cpu->regs), 3);
			return 2;
		}
		/* BIT 3, D */ case 0x5A: {
			_gb_get_bit(cpu, GB_REG_D(cpu->regs), 3);
			return 2;
		}
		/* BIT 3, E */ case 0x5B: {
			_gb_get_bit(cpu, GB_REG_E(cpu->regs), 3);
			return 2;
		}
		/* BIT 3, H */ case 0x5C: {
			_gb_get_bit(cpu, GB_REG_H(cpu->regs), 3);
			return 2;
		}
		/* BIT 3, L */ case 0x5D: {
			_gb_get_bit(cpu, GB_REG_L(cpu->regs), 3);
			return 2;
		}
		/* BIT 3, (HL) */ case 0x5E: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_get_bit(cpu, data, 3);
			return 3;
		}
		/* BIT 3, A */ case 0x5F: {
			_gb_get_bit(cpu, GB_REG_A(cpu->regs), 3);
			return 2;
		}

		/* BIT 4, B */ case 0x60: {
			_gb_get_bit(cpu, GB_REG_B(cpu->regs), 4);
			return 2;
		}
		/* BIT 4, C */ case 0x61: {
			_gb_get_bit(cpu, GB_REG_C(cpu->regs), 4);
			return 2;
		}
		/* BIT 4, D */ case 0x62: {
			_gb_get_bit(cpu, GB_REG_D(cpu->regs), 4);
			return 2;
		}
		/* BIT 4, E */ case 0x63: {
			_gb_get_bit(cpu, GB_REG_E(cpu->regs), 4);
			return 2;
		}
		/* BIT 4, H */ case 0x64: {
			_gb_get_bit(cpu, GB_REG_H(cpu->regs), 4);
			return 2;
		}
		/* BIT 4, L */ case 0x65: {
			_gb_get_bit(cpu, GB_REG_L(cpu->regs), 4);
			return 2;
		}
		/* BIT 4, (HL) */ case 0x66: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_get_bit(cpu, data, 4);
			return 3;
		}
		/* BIT 4, A */ case 0x67: {
			_gb_get_bit(cpu, GB_REG_A(cpu->regs), 4);
			return 2;
		}

		/* BIT 5, B */ case 0x68: {
			_gb_get_bit(cpu, GB_REG_B(cpu->regs), 5);
			return 2;
		}
		/* BIT 5, C */ case 0x69: {
			_gb_get_bit(cpu, GB_REG_C(cpu->regs), 5);
			return 2;
		}
		/* BIT 5, D */ case 0x6A: {
			_gb_get_bit(cpu, GB_REG_D(cpu->regs), 5);
			return 2;
		}
		/* BIT 5, E */ case 0x6B: {
			_gb_get_bit(cpu, GB_REG_E(cpu->regs), 5);
			return 2;
		}
		/* BIT 5, H */ case 0x6C: {
			_gb_get_bit(cpu, GB_REG_H(cpu->regs), 5);
			return 2;
		}
		/* BIT 5, L */ case 0x6D: {
			_gb_get_bit(cpu, GB_REG_L(cpu->regs), 5);
			return 2;
		}
		/* BIT 5, (HL) */ case 0x6E: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_get_bit(cpu, data, 5);
			return 3;
		}
		/* BIT 5, A */ case 0x6F: {
			_gb_get_bit(cpu, GB_REG_A(cpu->regs), 5);
			return 2;
		}

		/* BIT 6, B */ case 0x70: {
			_gb_get_bit(cpu, GB_REG_B(cpu->regs), 6);
			return 2;
		}
		/* BIT 6, C */ case 0x71: {
			_gb_get_bit(cpu, GB_REG_C(cpu->regs), 6);
			return 2;
		}
		/* BIT 6, D */ case 0x72: {
			_gb_get_bit(cpu, GB_REG_D(cpu->regs), 6);
			return 2;
		}
		/* BIT 6, E */ case 0x73: {
			_gb_get_bit(cpu, GB_REG_E(cpu->regs), 6);
			return 2;
		}
		/* BIT 6, H */ case 0x74: {
			_gb_get_bit(cpu, GB_REG_H(cpu->regs), 6);
			return 2;
		}
		/* BIT 6, L */ case 0x75: {
			_gb_get_bit(cpu, GB_REG_L(cpu->regs), 6);
			return 2;
		}
		/* BIT 6, (HL) */ case 0x76: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_get_bit(cpu, data, 6);
			return 3;
		}
		/* BIT 6, A */ case 0x77: {
			_gb_get_bit(cpu, GB_REG_A(cpu->regs), 6);
			return 2;
		}

		/* BIT 7, B */ case 0x78: {
			_gb_get_bit(cpu, GB_REG_B(cpu->regs), 7);
			return 2;
		}
		/* BIT 7, C */ case 0x79: {
			_gb_get_bit(cpu, GB_REG_C(cpu->regs), 7);
			return 2;
		}
		/* BIT 7, D */ case 0x7A: {
			_gb_get_bit(cpu, GB_REG_D(cpu->regs), 7);
			return 2;
		}
		/* BIT 7, E */ case 0x7B: {
			_gb_get_bit(cpu, GB_REG_E(cpu->regs), 7);
			return 2;
		}
		/* BIT 7, H */ case 0x7C: {
			_gb_get_bit(cpu, GB_REG_H(cpu->regs), 7);
			return 2;
		}
		/* BIT 7, L */ case 0x7D: {
			_gb_get_bit(cpu, GB_REG_L(cpu->regs), 7);
			return 2;
		}
		/* BIT 7, (HL) */ case 0x7E: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			_gb_get_bit(cpu, data, 7);
			return 3;
		}
		/* BIT 7, A */ case 0x7F: {
			_gb_get_bit(cpu, GB_REG_A(cpu->regs), 7);
			return 2;
		}

		/* SET 0, B */ case 0xC0: {
			GB_REG_B(cpu->regs) |= GB_BIT(0);
			return 2;
		}
		/* SET 0, C */ case 0xC1: {
			GB_REG_C(cpu->regs) |= GB_BIT(0);
			return 2;
		}
		/* SET 0, D */ case 0xC2: {
			GB_REG_D(cpu->regs) |= GB_BIT(0);
			return 2;
		}
		/* SET 0, E */ case 0xC3: {
			GB_REG_E(cpu->regs) |= GB_BIT(0);
			return 2;
		}
		/* SET 0, H */ case 0xC4: {
			GB_REG_H(cpu->regs) |= GB_BIT(0);
			return 2;
		}
		/* SET 0, L */ case 0xC5: {
			GB_REG_L(cpu->regs) |= GB_BIT(0);
			return 2;
		}
		/* SET 0, (HL) */ case 0xC6: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data |= GB_BIT(0);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SET 0, A */ case 0xC7: {
			GB_REG_A(cpu->regs) |= GB_BIT(0);
			return 2;
		}

		/* SET 1, B */ case 0xC8: {
			GB_REG_B(cpu->regs) |= GB_BIT(1);
			return 2;
		}
		/* SET 1, C */ case 0xC9: {
			GB_REG_C(cpu->regs) |= GB_BIT(1);
			return 2;
		}
		/* SET 1, D */ case 0xCA: {
			GB_REG_D(cpu->regs) |= GB_BIT(1);
			return 2;
		}
		/* SET 1, E */ case 0xCB: {
			GB_REG_E(cpu->regs) |= GB_BIT(1);
			return 2;
		}
		/* SET 1, H */ case 0xCC: {
			GB_REG_H(cpu->regs) |= GB_BIT(1);
			return 2;
		}
		/* SET 1, L */ case 0xCD: {
			GB_REG_L(cpu->regs) |= GB_BIT(1);
			return 2;
		}
		/* SET 1, (HL) */ case 0xCE: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data |= GB_BIT(1);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SET 1, A */ case 0xCF: {
			GB_REG_A(cpu->regs) |= GB_BIT(1);
			return 2;
		}

		/* SET 2, B */ case 0xD0: {
			GB_REG_B(cpu->regs) |= GB_BIT(2);
			return 2;
		}
		/* SET 2, C */ case 0xD1: {
			GB_REG_C(cpu->regs) |= GB_BIT(2);
			return 2;
		}
		/* SET 2, D */ case 0xD2: {
			GB_REG_D(cpu->regs) |= GB_BIT(2);
			return 2;
		}
		/* SET 2, E */ case 0xD3: {
			GB_REG_E(cpu->regs) |= GB_BIT(2);
			return 2;
		}
		/* SET 2, H */ case 0xD4: {
			GB_REG_H(cpu->regs) |= GB_BIT(2);
			return 2;
		}
		/* SET 2, L */ case 0xD5: {
			GB_REG_L(cpu->regs) |= GB_BIT(2);
			return 2;
		}
		/* SET 2, (HL) */ case 0xD6: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data |= GB_BIT(2);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SET 2, A */ case 0xD7: {
			GB_REG_A(cpu->regs) |= GB_BIT(2);
			return 2;
		}

		/* SET 3, B */ case 0xD8: {
			GB_REG_B(cpu->regs) |= GB_BIT(3);
			return 2;
		}
		/* SET 3, C */ case 0xD9: {
			GB_REG_C(cpu->regs) |= GB_BIT(3);
			return 2;
		}
		/* SET 3, D */ case 0xDA: {
			GB_REG_D(cpu->regs) |= GB_BIT(3);
			return 2;
		}
		/* SET 3, E */ case 0xDB: {
			GB_REG_E(cpu->regs) |= GB_BIT(3);
			return 2;
		}
		/* SET 3, H */ case 0xDC: {
			GB_REG_H(cpu->regs) |= GB_BIT(3);
			return 2;
		}
		/* SET 3, L */ case 0xDD: {
			GB_REG_L(cpu->regs) |= GB_BIT(3);
			return 2;
		}
		/* SET 3, (HL) */ case 0xDE: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data |= GB_BIT(3);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SET 3, A */ case 0xDF: {
			GB_REG_A(cpu->regs) |= GB_BIT(3);
			return 2;
		}

		/* SET 4, B */ case 0xE0: {
			GB_REG_B(cpu->regs) |= GB_BIT(4);
			return 2;
		}
		/* SET 4, C */ case 0xE1: {
			GB_REG_C(cpu->regs) |= GB_BIT(4);
			return 2;
		}
		/* SET 4, D */ case 0xE2: {
			GB_REG_D(cpu->regs) |= GB_BIT(4);
			return 2;
		}
		/* SET 4, E */ case 0xE3: {
			GB_REG_E(cpu->regs) |= GB_BIT(4);
			return 2;
		}
		/* SET 4, H */ case 0xE4: {
			GB_REG_H(cpu->regs) |= GB_BIT(4);
			return 2;
		}
		/* SET 4, L */ case 0xE5: {
			GB_REG_L(cpu->regs) |= GB_BIT(4);
			return 2;
		}
		/* SET 4, (HL) */ case 0xE6: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data |= GB_BIT(4);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SET 4, A */ case 0xE7: {
			GB_REG_A(cpu->regs) |= GB_BIT(4);
			return 2;
		}

		/* SET 5, B */ case 0xE8: {
			GB_REG_B(cpu->regs) |= GB_BIT(5);
			return 2;
		}
		/* SET 5, C */ case 0xE9: {
			GB_REG_C(cpu->regs) |= GB_BIT(5);
			return 2;
		}
		/* SET 5, D */ case 0xEA: {
			GB_REG_D(cpu->regs) |= GB_BIT(5);
			return 2;
		}
		/* SET 5, E */ case 0xEB: {
			GB_REG_E(cpu->regs) |= GB_BIT(5);
			return 2;
		}
		/* SET 5, H */ case 0xEC: {
			GB_REG_H(cpu->regs) |= GB_BIT(5);
			return 2;
		}
		/* SET 5, L */ case 0xED: {
			GB_REG_L(cpu->regs) |= GB_BIT(5);
			return 2;
		}
		/* SET 5, (HL) */ case 0xEE: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data |= GB_BIT(5);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SET 5, A */ case 0xEF: {
			GB_REG_A(cpu->regs) |= GB_BIT(5);
			return 2;
		}

		/* SET 6, B */ case 0xF0: {
			GB_REG_B(cpu->regs) |= GB_BIT(6);
			return 2;
		}
		/* SET 6, C */ case 0xF1: {
			GB_REG_C(cpu->regs) |= GB_BIT(6);
			return 2;
		}
		/* SET 6, D */ case 0xF2: {
			GB_REG_D(cpu->regs) |= GB_BIT(6);
			return 2;
		}
		/* SET 6, E */ case 0xF3: {
			GB_REG_E(cpu->regs) |= GB_BIT(6);
			return 2;
		}
		/* SET 6, H */ case 0xF4: {
			GB_REG_H(cpu->regs) |= GB_BIT(6);
			return 2;
		}
		/* SET 6, L */ case 0xF5: {
			GB_REG_L(cpu->regs) |= GB_BIT(6);
			return 2;
		}
		/* SET 6, (HL) */ case 0xF6: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data |= GB_BIT(6);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SET 6, A */ case 0xF7: {
			GB_REG_A(cpu->regs) |= GB_BIT(6);
			return 2;
		}

		/* SET 7, B */ case 0xF8: {
			GB_REG_B(cpu->regs) |= GB_BIT(7);
			return 2;
		}
		/* SET 7, C */ case 0xF9: {
			GB_REG_C(cpu->regs) |= GB_BIT(7);
			return 2;
		}
		/* SET 7, D */ case 0xFA: {
			GB_REG_D(cpu->regs) |= GB_BIT(7);
			return 2;
		}
		/* SET 7, E */ case 0xFB: {
			GB_REG_E(cpu->regs) |= GB_BIT(7);
			return 2;
		}
		/* SET 7, H */ case 0xFC: {
			GB_REG_H(cpu->regs) |= GB_BIT(7);
			return 2;
		}
		/* SET 7, L */ case 0xFD: {
			GB_REG_L(cpu->regs) |= GB_BIT(7);
			return 2;
		}
		/* SET 7, (HL) */ case 0xFE: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data |= GB_BIT(7);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* SET 7, A */ case 0xFF: {
			GB_REG_A(cpu->regs) |= GB_BIT(7);
			return 2;
		}

		/* RES 0, B */ case 0x80: {
			GB_REG_B(cpu->regs) &= (u8)~GB_BIT(0);
			return 2;
		}
		/* RES 0, C */ case 0x81: {
			GB_REG_C(cpu->regs) &= (u8)~GB_BIT(0);
			return 2;
		}
		/* RES 0, D */ case 0x82: {
			GB_REG_D(cpu->regs) &= (u8)~GB_BIT(0);
			return 2;
		}
		/* RES 0, E */ case 0x83: {
			GB_REG_E(cpu->regs) &= (u8)~GB_BIT(0);
			return 2;
		}
		/* RES 0, H */ case 0x84: {
			GB_REG_H(cpu->regs) &= (u8)~GB_BIT(0);
			return 2;
		}
		/* RES 0, L */ case 0x85: {
			GB_REG_L(cpu->regs) &= (u8)~GB_BIT(0);
			return 2;
		}
		/* RES 0, (HL) */ case 0x86: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data &= (u8)~GB_BIT(0);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RES 0, A */ case 0x87: {
			GB_REG_A(cpu->regs) &= (u8)~GB_BIT(0);
			return 2;
		}

		/* RES 1, B */ case 0x88: {
			GB_REG_B(cpu->regs) &= (u8)~GB_BIT(1);
			return 2;
		}
		/* RES 1, C */ case 0x89: {
			GB_REG_C(cpu->regs) &= (u8)~GB_BIT(1);
			return 2;
		}
		/* RES 1, D */ case 0x8A: {
			GB_REG_D(cpu->regs) &= (u8)~GB_BIT(1);
			return 2;
		}
		/* RES 1, E */ case 0x8B: {
			GB_REG_E(cpu->regs) &= (u8)~GB_BIT(1);
			return 2;
		}
		/* RES 1, H */ case 0x8C: {
			GB_REG_H(cpu->regs) &= (u8)~GB_BIT(1);
			return 2;
		}
		/* RES 1, L */ case 0x8D: {
			GB_REG_L(cpu->regs) &= (u8)~GB_BIT(1);
			return 2;
		}
		/* RES 1, (HL) */ case 0x8E: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data &= (u8)~GB_BIT(1);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RES 1, A */ case 0x8F: {
			GB_REG_A(cpu->regs) &= (u8)~GB_BIT(1);
			return 2;
		}

		/* RES 2, B */ case 0x90: {
			GB_REG_B(cpu->regs) &= (u8)~GB_BIT(2);
			return 2;
		}
		/* RES 2, C */ case 0x91: {
			GB_REG_C(cpu->regs) &= (u8)~GB_BIT(2);
			return 2;
		}
		/* RES 2, D */ case 0x92: {
			GB_REG_D(cpu->regs) &= (u8)~GB_BIT(2);
			return 2;
		}
		/* RES 2, E */ case 0x93: {
			GB_REG_E(cpu->regs) &= (u8)~GB_BIT(2);
			return 2;
		}
		/* RES 2, H */ case 0x94: {
			GB_REG_H(cpu->regs) &= (u8)~GB_BIT(2);
			return 2;
		}
		/* RES 2, L */ case 0x95: {
			GB_REG_L(cpu->regs) &= (u8)~GB_BIT(2);
			return 2;
		}
		/* RES 2, (HL) */ case 0x96: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data &= (u8)~GB_BIT(2);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RES 2, A */ case 0x97: {
			GB_REG_A(cpu->regs) &= (u8)~GB_BIT(2);
			return 2;
		}

		/* RES 3, B */ case 0x98: {
			GB_REG_B(cpu->regs) &= (u8)~GB_BIT(3);
			return 2;
		}
		/* RES 3, C */ case 0x99: {
			GB_REG_C(cpu->regs) &= (u8)~GB_BIT(3);
			return 2;
		}
		/* RES 3, D */ case 0x9A: {
			GB_REG_D(cpu->regs) &= (u8)~GB_BIT(3);
			return 2;
		}
		/* RES 3, E */ case 0x9B: {
			GB_REG_E(cpu->regs) &= (u8)~GB_BIT(3);
			return 2;
		}
		/* RES 3, H */ case 0x9C: {
			GB_REG_H(cpu->regs) &= (u8)~GB_BIT(3);
			return 2;
		}
		/* RES 3, L */ case 0x9D: {
			GB_REG_L(cpu->regs) &= (u8)~GB_BIT(3);
			return 2;
		}
		/* RES 3, (HL) */ case 0x9E: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data &= (u8)~GB_BIT(3);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RES 3, A */ case 0x9F: {
			GB_REG_A(cpu->regs) &= (u8)~GB_BIT(3);
			return 2;
		}

		/* RES 4, B */ case 0xA0: {
			GB_REG_B(cpu->regs) &= (u8)~GB_BIT(4);
			return 2;
		}
		/* RES 4, C */ case 0xA1: {
			GB_REG_C(cpu->regs) &= (u8)~GB_BIT(4);
			return 2;
		}
		/* RES 4, D */ case 0xA2: {
			GB_REG_D(cpu->regs) &= (u8)~GB_BIT(4);
			return 2;
		}
		/* RES 4, E */ case 0xA3: {
			GB_REG_E(cpu->regs) &= (u8)~GB_BIT(4);
			return 2;
		}
		/* RES 4, H */ case 0xA4: {
			GB_REG_H(cpu->regs) &= (u8)~GB_BIT(4);
			return 2;
		}
		/* RES 4, L */ case 0xA5: {
			GB_REG_L(cpu->regs) &= (u8)~GB_BIT(4);
			return 2;
		}
		/* RES 4, (HL) */ case 0xA6: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data &= (u8)~GB_BIT(4);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RES 4, A */ case 0xA7: {
			GB_REG_A(cpu->regs) &= (u8)~GB_BIT(4);
			return 2;
		}

		/* RES 5, B */ case 0xA8: {
			GB_REG_B(cpu->regs) &= (u8)~GB_BIT(5);
			return 2;
		}
		/* RES 5, C */ case 0xA9: {
			GB_REG_C(cpu->regs) &= (u8)~GB_BIT(5);
			return 2;
		}
		/* RES 5, D */ case 0xAA: {
			GB_REG_D(cpu->regs) &= (u8)~GB_BIT(5);
			return 2;
		}
		/* RES 5, E */ case 0xAB: {
			GB_REG_E(cpu->regs) &= (u8)~GB_BIT(5);
			return 2;
		}
		/* RES 5, H */ case 0xAC: {
			GB_REG_H(cpu->regs) &= (u8)~GB_BIT(5);
			return 2;
		}
		/* RES 5, L */ case 0xAD: {
			GB_REG_L(cpu->regs) &= (u8)~GB_BIT(5);
			return 2;
		}
		/* RES 5, (HL) */ case 0xAE: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data &= (u8)~GB_BIT(5);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RES 5, A */ case 0xAF: {
			GB_REG_A(cpu->regs) &= (u8)~GB_BIT(5);
			return 2;
		}

		/* RES 6, B */ case 0xB0: {
			GB_REG_B(cpu->regs) &= (u8)~GB_BIT(6);
			return 2;
		}
		/* RES 6, C */ case 0xB1: {
			GB_REG_C(cpu->regs) &= (u8)~GB_BIT(6);
			return 2;
		}
		/* RES 6, D */ case 0xB2: {
			GB_REG_D(cpu->regs) &= (u8)~GB_BIT(6);
			return 2;
		}
		/* RES 6, E */ case 0xB3: {
			GB_REG_E(cpu->regs) &= (u8)~GB_BIT(6);
			return 2;
		}
		/* RES 6, H */ case 0xB4: {
			GB_REG_H(cpu->regs) &= (u8)~GB_BIT(6);
			return 2;
		}
		/* RES 6, L */ case 0xB5: {
			GB_REG_L(cpu->regs) &= (u8)~GB_BIT(6);
			return 2;
		}
		/* RES 6, (HL) */ case 0xB6: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data &= (u8)~GB_BIT(6);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RES 6, A */ case 0xB7: {
			GB_REG_A(cpu->regs) &= (u8)~GB_BIT(6);
			return 2;
		}

		/* RES 7, B */ case 0xB8: {
			GB_REG_B(cpu->regs) &= (u8)~GB_BIT(7);
			return 2;
		}
		/* RES 7, C */ case 0xB9: {
			GB_REG_C(cpu->regs) &= (u8)~GB_BIT(7);
			return 2;
		}
		/* RES 7, D */ case 0xBA: {
			GB_REG_D(cpu->regs) &= (u8)~GB_BIT(7);
			return 2;
		}
		/* RES 7, E */ case 0xBB: {
			GB_REG_E(cpu->regs) &= (u8)~GB_BIT(7);
			return 2;
		}
		/* RES 7, H */ case 0xBC: {
			GB_REG_H(cpu->regs) &= (u8)~GB_BIT(7);
			return 2;
		}
		/* RES 7, L */ case 0xBD: {
			GB_REG_L(cpu->regs) &= (u8)~GB_BIT(7);
			return 2;
		}
		/* RES 7, (HL) */ case 0xBE: {
			u8 data = gb_mmu_read_u8(cpu->mmu, GB_REG_HL(cpu->regs));
			data &= (u8)~GB_BIT(7);
			gb_mmu_write_u8(cpu->mmu, GB_REG_HL(cpu->regs), data);
			return 4;
		}
		/* RES 7, A */ case 0xBF: {
			GB_REG_A(cpu->regs) &= (u8)~GB_BIT(7);
			return 2;
		}

		default: {
			GB_FATAL("UNIMPLEMENTED CB OPCODE! 0x%X\n", opcode);
			return 2;
			break;
		}
	}
	return 2;
}
