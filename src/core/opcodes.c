#include "gbCart.h"
#include "gbEmu.h"
#include "gbReg.h"
#include "gbSM83.h"
#include "log.h"

static inline void _gb_xor(gb_sm83_t* cpu, u8 data)
{
	u8 result = GB_REG_A(cpu->regs) ^= data;
	GB_REG_F(cpu->regs) = result == 0 ? GB_FLAG_ZERO : 0x0;
}

static inline void _gb_dec(gb_sm83_t* cpu, u8* data)
{
	(*data) -= 1;

	GB_REG_F(cpu->regs) &= (u8) ~(GB_FLAG_ZERO | GB_FLAG_HALF);
	GB_REG_F(cpu->regs) |= GB_FLAG_SUBS;
	GB_REG_F(cpu->regs) |= (*data == 0) ? GB_FLAG_ZERO : 0;
	GB_REG_F(cpu->regs) |= GB_IS_BIT(*data, 3) ? GB_FLAG_HALF : 0;
}

static inline void _gb_and(gb_sm83_t* cpu, u8 data)
{
	u8 result = GB_REG_A(cpu->regs) & data;
	GB_REG_F(cpu->regs) = result == 0 ? GB_FLAG_ZERO : 0x0;
	GB_REG_F(cpu->regs) |= GB_FLAG_HALF;
}

static inline void _gb_sub(gb_sm83_t* cpu, u8 data)
{
	u8 result = GB_REG_A(cpu->regs) - data;

	GB_REG_F(cpu->regs) = result == 0 ? GB_FLAG_ZERO : 0x0;
	GB_REG_F(cpu->regs) |= GB_FLAG_SUBS;
	GB_REG_F(cpu->regs) |= (result & GB_BIT(3)) != 0 ? GB_FLAG_HALF : 0;
	GB_REG_F(cpu->regs) |= (result & GB_BIT(7)) != 0 ? GB_FLAG_CARR : 0;
}

static inline void _gb_add_hl_u16(gb_sm83_t* cpu, u16 data)
{
	u16 result = GB_REG_HL(cpu->regs) + data;
	GB_REG_F(cpu->regs) &= !(GB_FLAG_HALF | GB_FLAG_CARR | GB_FLAG_SUBS);
	GB_REG_F(cpu->regs) |= (result & GB_BIT(11)) != 0 ? GB_FLAG_HALF : 0;
	GB_REG_F(cpu->regs) |= (result & GB_BIT(15)) != 0 ? GB_FLAG_CARR : 0;
}

static inline void _gb_srl(gb_sm83_t* cpu, u8* reg)
{
	GB_REG_F(cpu->regs) = (*reg) & GB_BIT(0) ? GB_FLAG_CARR : 0;
	*reg >>= 1;
	GB_REG_F(cpu->regs) |= (*reg) == 0 ? GB_FLAG_ZERO : 0;
}

static inline void _gb_call_addr(gb_emu_t* emu, u16 addr)
{
	gb_emu_push_u16(emu, GB_REG_PC(emu->cpu.regs));
	GB_REG_PC(emu->cpu.regs) = addr;
}

static inline void _gb_call(gb_emu_t* emu)
{
	gb_emu_push_u16(emu, GB_REG_PC(emu->cpu.regs) + 2);
	GB_REG_PC(emu->cpu.regs) = gb_emu_read_u16(emu, GB_REG_PC(emu->cpu.regs));
}

static u8 gb_emu_execute_cb(gb_emu_t* emu);

u8 gb_emu_advance_opcode(gb_emu_t* emu)
{
	u8 opcode = gb_cart_read_u8(&emu->cart, GB_REG_PC(emu->cpu.regs));
	GB_REG_PC(emu->cpu.regs)++;
	switch (opcode) {
		/* LD B, B */ case 0x40:
		/* LD C, C */ case 0x49:
		/* LD D, D */ case 0x52:
		/* LD E, E */ case 0x5B:
		/* LD H, H */ case 0x64:
		/* LD L, L */ case 0x6D:
		/* NOP */ case 0x00: {
			return 1;
		}

		/* CPL */ case 0x2F: {
			GB_REG_F(emu->cpu.regs) |= GB_FLAG_SUBS | GB_FLAG_HALF;
			GB_REG_A(emu->cpu.regs) = ~GB_REG_A(emu->cpu.regs);
			return 1;
		}

		/* CCF */ case 0x3F: {
		    GB_REG_F(emu->cpu.regs) &= (u8)~(GB_FLAG_SUBS | GB_FLAG_HALF);
		    GB_REG_F(emu->cpu.regs) = GB_REG_F(emu->cpu.regs) ^ GB_FLAG_CARR;
			return 1;
		}

		/* DEC B */ case 0x05: {
			_gb_dec(&emu->cpu, &GB_REG_B(emu->cpu.regs));
			return 1;
		}

		/* DEC C */ case 0x0D: {
			_gb_dec(&emu->cpu, &GB_REG_C(emu->cpu.regs));
			return 1;
		}

		/* DEC D */ case 0x15: {
			_gb_dec(&emu->cpu, &GB_REG_D(emu->cpu.regs));
			return 1;
		}

		/* DEC E */ case 0x1D: {
			_gb_dec(&emu->cpu, &GB_REG_E(emu->cpu.regs));
			return 1;
		}

		/* DEC H */ case 0x25: {
			_gb_dec(&emu->cpu, &GB_REG_D(emu->cpu.regs));
			return 1;
		}

		/* DEC L */ case 0x2D: {
            _gb_dec(&emu->cpu, &GB_REG_L(emu->cpu.regs));
            return 1;
        }

		/* DEC (HL) */ case 0x35: {
			u8 data = gb_emu_read_u8(emu, GB_REG_HL(emu->cpu.regs));
			_gb_dec(&emu->cpu, &data);
			gb_emu_write_u8(emu, GB_REG_HL(emu->cpu.regs), data);
			return 3;
		}

		/* DEC A */ case 0x3D: {
            _gb_dec(&emu->cpu, &GB_REG_A(emu->cpu.regs));
            return 1;
        }

		/* LD H, B */ case 0x06: {
			GB_REG_H(emu->cpu.regs) = GB_REG_B(emu->cpu.regs);
			return 1;
		}

		/* LD C, d8 */ case 0x0E: {
			uint16_t addr = GB_REG_PC(emu->cpu.regs)++;
			GB_REG_C(emu->cpu.regs) = gb_emu_read_u8(emu, addr);
			return 2;
		}

		/* LD E, d8 */ case 0x1E: {
			uint16_t addr = GB_REG_PC(emu->cpu.regs)++;
			GB_REG_E(emu->cpu.regs) = gb_emu_read_u8(emu, addr);
			return 2;
		}

		/* LD L, d8 */ case 0x2E: {
			uint16_t addr = GB_REG_PC(emu->cpu.regs)++;
			GB_REG_L(emu->cpu.regs) = gb_emu_read_u8(emu, addr);
			return 2;
		}

		/* LD A, d8 */ case 0x3E: {
			uint16_t addr = GB_REG_PC(emu->cpu.regs)++;
			GB_REG_A(emu->cpu.regs) = gb_emu_read_u8(emu, addr);
			return 2;
		}

		/* LD B, (HL) */ case 0x46: {
			uint16_t addr = GB_REG_HL(emu->cpu.regs);
			GB_REG_B(emu->cpu.regs) = gb_emu_read_u8(emu, addr);
			return 2;
		}

		/* LD E, A */ case 0x5F: {
			GB_REG_E(emu->cpu.regs) = GB_REG_A(emu->cpu.regs);
			return 1;
		}

		/* LD A, H */ case 0x7C: {
			GB_REG_A(emu->cpu.regs) = GB_REG_H(emu->cpu.regs);
			return 1;
		}

		/* LD A, L */ case 0x7D: {
			GB_REG_A(emu->cpu.regs) = GB_REG_L(emu->cpu.regs);
			return 1;
		}

		/* LD A, (HL) */ case 0x7E: {
			uint16_t addr = GB_REG_HL(emu->cpu.regs);
			GB_REG_A(emu->cpu.regs) = gb_emu_read_u8(emu, addr);
			return 2;
		}

		/* jp, a16 */ case 0xC3: {
			GB_REG_PC(emu->cpu.regs) =
				gb_emu_read_u16(emu, GB_REG_PC(emu->cpu.regs));
			return 4;
		}

		/* CALL Z, a16 */ case 0xCC: {
			if (GB_REG_F(emu->cpu.regs) & GB_FLAG_ZERO) {
				_gb_call(emu);
				return 6;
			}
			return 3;
		}
		/* CALL C, a16 */ case 0xDC: {
			if (GB_REG_F(emu->cpu.regs) & GB_FLAG_CARR) {
				_gb_call(emu);
				return 6;
			}
			return 3;
		}

		/* CALL, a16 */ case 0xCD: {
			_gb_call(emu);
			return 6;
		}

		/* RET */ case 0xC9: {
			GB_REG_PC(emu->cpu.regs) = gb_emu_pop_u16(emu);
			return 4;
		}

		/* JR NZ, s8 */ case 0x20: {
			const i8 d = gb_emu_read_i8(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs)++;
			if (!(GB_REG_F(emu->cpu.regs) & GB_FLAG_ZERO)) {
				GB_REG_PC(emu->cpu.regs) += (u16)(i16)d;
				return 3;
			}
			return 2;
		}
		/* JR NC, s8 */ case 0x30: {
			const i8 d = gb_emu_read_i8(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs)++;
			if (!(GB_REG_F(emu->cpu.regs) & GB_FLAG_CARR)) {
				GB_REG_PC(emu->cpu.regs) += (u16)(i16)d;
				return 3;
			}
			return 2;
		}

		/* LD (BC), A */ case 0x02: {
			gb_emu_write_u8(
				emu, GB_REG_BC(emu->cpu.regs), GB_REG_A(emu->cpu.regs)
			);
			return 2;
		}
		/* LD (DE), A */ case 0x12: {
			gb_emu_write_u8(
				emu, GB_REG_DE(emu->cpu.regs), GB_REG_A(emu->cpu.regs)
			);
			return 2;
		}
		/* LD (HL+), A */ case 0x22: {
			gb_emu_write_u8(
				emu, GB_REG_HL(emu->cpu.regs), GB_REG_A(emu->cpu.regs)
			);
			GB_REG_HL(emu->cpu.regs) += 1;
			return 2;
		}
		/* LD (HL-), A */ case 0x32: {
			gb_emu_write_u8(
				emu, GB_REG_HL(emu->cpu.regs), GB_REG_A(emu->cpu.regs)
			);
			GB_REG_HL(emu->cpu.regs) -= 1;
			return 2;
		}

		/* LD BC, d16 */ case 0x01: {
			const u16 d = gb_emu_read_u16(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs) += 2;
			GB_REG_BC(emu->cpu.regs) = d;
			return 3;
		}
		/* LD DE, d16 */ case 0x11: {
			const u16 d = gb_emu_read_u16(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs) += 2;
			GB_REG_DE(emu->cpu.regs) = d;
			return 3;
		}
		/* LD HL, d16 */ case 0x21: {
			const u16 d = gb_emu_read_u16(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs) += 2;
			GB_REG_HL(emu->cpu.regs) = d;
			return 3;
		}
		/* LD HL, d16 */ case 0x31: {
			const u16 d = gb_emu_read_u16(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs) += 2;
			GB_REG_SP(emu->cpu.regs) = d;
			return 3;
		}

		/* XOR B */ case 0xA8: {
			_gb_xor(&emu->cpu, GB_REG_B(emu->cpu.regs));
			return 1;
		}
		/* XOR C */ case 0xA9: {
			_gb_xor(&emu->cpu, GB_REG_C(emu->cpu.regs));
			return 1;
		}
		/* XOR D */ case 0xAA: {
			_gb_xor(&emu->cpu, GB_REG_D(emu->cpu.regs));
			return 1;
		}
		/* XOR E */ case 0xAB: {
			_gb_xor(&emu->cpu, GB_REG_E(emu->cpu.regs));
			return 1;
		}
		/* XOR H */ case 0xAC: {
			_gb_xor(&emu->cpu, GB_REG_H(emu->cpu.regs));
			return 1;
		}
		/* XOR L */ case 0xAD: {
			_gb_xor(&emu->cpu, GB_REG_L(emu->cpu.regs));
			return 1;
		}
		/* XOR [HL] */ case 0xAE: {
			const u8 d = gb_emu_read_u8(emu, GB_REG_HL(emu->cpu.regs));
			_gb_xor(&emu->cpu, d);
			return 2;
		}
		/* XOR A */ case 0xAF: {
			_gb_xor(&emu->cpu, GB_REG_A(emu->cpu.regs));
			return 1;
		}
		/* XOR d8 */ case 0xEE: {
			const u8 d = gb_emu_read_u8(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs)++;
			_gb_xor(&emu->cpu, d);
			return 2;
		}

		/* AND B */ case 0xA0: {
			_gb_and(&emu->cpu, GB_REG_B(emu->cpu.regs));
			return 1;
		}
		/* AND C */ case 0xA1: {
			_gb_and(&emu->cpu, GB_REG_C(emu->cpu.regs));
			return 1;
		}
		/* AND D */ case 0xA2: {
			_gb_and(&emu->cpu, GB_REG_D(emu->cpu.regs));
			return 1;
		}
		/* AND E */ case 0xA3: {
			_gb_and(&emu->cpu, GB_REG_E(emu->cpu.regs));
			return 1;
		}
		/* AND H */ case 0xA4: {
			_gb_and(&emu->cpu, GB_REG_H(emu->cpu.regs));
			return 1;
		}
		/* AND L */ case 0xA5: {
			_gb_and(&emu->cpu, GB_REG_L(emu->cpu.regs));
			return 1;
		}
		/* AND [HL] */ case 0xA6: {
			const u8 d = gb_emu_read_u8(emu, GB_REG_HL(emu->cpu.regs));
			_gb_and(&emu->cpu, d);
			return 2;
		}
		/* AND A */ case 0xA7: {
			_gb_and(&emu->cpu, GB_REG_A(emu->cpu.regs));
			return 1;
		}
		/* AND d8 */ case 0xE6: {
			const u8 d = gb_emu_read_u8(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs)++;
			_gb_and(&emu->cpu, d);
			return 2;
		}

		/* LD (a8), A */ case 0xE0: {
			u16 addr = (u16)0xFF00 |
					   (u16)gb_emu_read_u8(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs)++;
			gb_emu_write_u8(emu, addr, GB_REG_A(emu->cpu.regs));
			return 3;
		}
		/* LD A, (a8) */ case 0xF0: {
			u16 addr = (u16)0xFF00 |
					   (u16)gb_emu_read_u8(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs)++;
			GB_REG_A(emu->cpu.regs) = gb_emu_read_u8(emu, addr);
			return 3;
		}

		/* SUB d8 */ case 0xD6: {
			u8 data = gb_emu_read_u8(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs)++;
			_gb_sub(&emu->cpu, data);
			return 2;
		}

		/* ADD HL, BC */ case 0x09: {
			_gb_add_hl_u16(&emu->cpu, GB_REG_BC(emu->cpu.regs));
			return 2;
		}
		/* ADD HL, DE */ case 0x19: {
			_gb_add_hl_u16(&emu->cpu, GB_REG_DE(emu->cpu.regs));
			return 2;
		}
		/* ADD HL, HL */ case 0x29: {
			_gb_add_hl_u16(&emu->cpu, GB_REG_HL(emu->cpu.regs));
			return 2;
		}
		/* ADD HL, SP */ case 0x39: {
			_gb_add_hl_u16(&emu->cpu, GB_REG_SP(emu->cpu.regs));
			return 2;
		}

		/* RST 0x08 */ case 0xCF: {
			_gb_call_addr(emu, 0x08);
			return 4;
		}
		/* RST 0x18 */ case 0xDF: {
			_gb_call_addr(emu, 0x18);
			return 4;
		}
		/* RST 0x18 */ case 0xEF: {
			_gb_call_addr(emu, 0x28);
			return 4;
		}
		/* RST 0x18 */ case 0xFF: {
			_gb_call_addr(emu, 0x38);
			return 4;
		}

		/* CB PREFIX */ case 0xCB: {
			return gb_emu_execute_cb(emu);
		}

		default: {
			GB_FATAL("UNIMPLEMENTED OPCODE! 0x%X\n", opcode);
			break;
		}
	}
}

static u8 gb_emu_execute_cb(gb_emu_t* emu)
{
	u8 opcode = gb_cart_read_u8(&emu->cart, GB_REG_PC(emu->cpu.regs)++);
	switch (opcode) {
		/* SRL B */ case 0x38: {
			_gb_srl(&emu->cpu, &GB_REG_B(emu->cpu.regs));
			return 2;
		}
		/* SRL C */ case 0x39: {
			_gb_srl(&emu->cpu, &GB_REG_C(emu->cpu.regs));
			return 2;
		}
		/* SRL D */ case 0x3A: {
			_gb_srl(&emu->cpu, &GB_REG_D(emu->cpu.regs));
			return 2;
		}
		/* SRL E */ case 0x3B: {
			_gb_srl(&emu->cpu, &GB_REG_E(emu->cpu.regs));
			return 2;
		}
		/* SRL H */ case 0x3C: {
			_gb_srl(&emu->cpu, &GB_REG_H(emu->cpu.regs));
			return 2;
		}
		/* SRL L */ case 0x3D: {
			_gb_srl(&emu->cpu, &GB_REG_L(emu->cpu.regs));
			return 2;
		}
		/* SRL (HL) */ case 0x3E: {
			u8 data = gb_emu_read_u8(emu, GB_REG_HL(emu->cpu.regs));
			_gb_srl(&emu->cpu, &data);
			gb_emu_write_u8(emu, GB_REG_HL(emu->cpu.regs), data);
			return 4;
		}
		/* SRL A */ case 0x3F: {
			_gb_srl(&emu->cpu, &GB_REG_A(emu->cpu.regs));
			return 2;
		}

		default: {
			GB_FATAL("UNIMPLEMENTED CB OPCODE! 0x%X\n", opcode);
			break;
		}
	}
}
