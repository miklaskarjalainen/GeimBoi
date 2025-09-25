#include "gbCart.h"
#include "gbEmu.h"
#include "gbReg.h"
#include "gbSM83.h"
#include "log.h"

static void _gb_xor(gb_sm83_t* cpu, u8 data)
{
	u8 result = GB_REG_A(cpu->regs) ^= data;
	GB_REG_F(cpu->regs) = result == 0 ? GB_FLAG_ZERO : 0x0;
}

static void _gb_call(gb_emu_t* emu)
{
	gb_cpu_push_u16(&emu->cpu, GB_REG_PC(emu->cpu.regs) + 2);
	GB_REG_PC(emu->cpu.regs) = gb_emu_read_u16(emu, GB_REG_PC(emu->cpu.regs));
}

u8 gb_emu_advance_opcode(gb_emu_t* emu)
{
	u8 opcode = gb_cart_read_u8(&emu->cart, GB_REG_PC(emu->cpu.regs)++);
	switch (opcode) {
		// NOP
		case 0x00: {
			return 1;
		}

		// jp, a16
		case 0xC3: {
			GB_REG_PC(emu->cpu.regs) = gb_emu_read_u16(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs) += 2;
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
			gb_emu_write_u8(emu, GB_REG_BC(emu->cpu.regs), GB_REG_A(emu->cpu.regs));
			return 2;
		}
		/* LD (DE), A */ case 0x12: {
			gb_emu_write_u8(emu, GB_REG_DE(emu->cpu.regs), GB_REG_A(emu->cpu.regs));
			return 2;
		}
		/* LD (HL+), A */ case 0x22: {
			gb_emu_write_u8(emu, GB_REG_HL(emu->cpu.regs), GB_REG_A(emu->cpu.regs));
			GB_REG_HL(emu->cpu.regs) += 1;
			return 2;
		}
		/* LD (HL-), A */ case 0x32: {
			gb_emu_write_u8(emu, GB_REG_HL(emu->cpu.regs), GB_REG_A(emu->cpu.regs));
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
		/* XOR A */ case 0xEE: {
			const u8 d = gb_emu_read_u8(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs)++;
			_gb_xor(&emu->cpu, d);
			return 2;
		}

		/* LD (a8), A */ case 0xE0: {
		    u16 addr = (u16)0xFF00 | (u16)gb_emu_read_u8(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs)++;
			gb_emu_write_u8(emu, addr, GB_REG_A(emu->cpu.regs));
		    return 3;
		}
		/* LD A, (a8) */ case 0xF0: {
		    u16 addr = (u16)0xFF00 | (u16)gb_emu_read_u8(emu, GB_REG_PC(emu->cpu.regs));
			GB_REG_PC(emu->cpu.regs)++;
			GB_REG_A(emu->cpu.regs) = gb_emu_read_u8(emu, addr);
		    return 3;
		}

		default: {
			GB_FATAL("UNIMPLEMENTED OPCODE! 0x%X\n", opcode);
			break;
		}
	}
}
