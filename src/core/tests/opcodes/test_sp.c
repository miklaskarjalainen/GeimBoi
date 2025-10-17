// SPDX-License-Identifier: GPL-2.0-only

#include <stdlib.h>
#include <unity.h>

#include "gbEmu.h"
#include "gbReg.h"

#define GB_ARRAY_LEN(arr) (sizeof(arr) / (sizeof(*(arr))))

static gb_emu_t* _test_create_emu_with_payload(const u8* payload, size_t size)
{
	gb_emu_t* emu = gb_emu_create();
	// For consistency
	GB_REG_F(emu->cpu.regs) = 0x00;
	GB_REG_PC(emu->cpu.regs) = 0x100;
	gb_emu_load_rom_bytes(emu, payload, size);
	return emu;
}

void test_opcode_sp(void)
{
	const u8 payload[] = {
		[0x100] = 0x00,

		// LD SP, $000A
		0x31,
		0x0A,
		0x00,
		// INC SP
		0x33,
		// INC SP
		0x33,

		// DEC SP
		0x3B,
		// DEC SP
		0x3B,

		// ADD HL, SP
		0x39,
		// ADD HL, SP
		0x39,

		// LD HL, $FF85
		0x21,
		0x85,
		0xFF,
		// ADD SP, HL
		0xF9,

		// ADD SP, 1
		0xE8,
		0x01,
		// ADD SP, -1
		0xE8,
		(u8)(-1),
		// ADD SP, 127
		0xE8,
		0x7F,
		// LD HL, SP+1
		0xF8,
		0x1,
		// LD HL, SP-1
		0xF8,
		(u8)(-1),
	};

	gb_emu_t* emu =
		_test_create_emu_with_payload(payload, GB_ARRAY_LEN(payload));
	gb_emu_advance_opcode(emu);

	// Increments
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0xA, GB_REG_SP(emu->cpu.regs));
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0xB, GB_REG_SP(emu->cpu.regs));
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0xC, GB_REG_SP(emu->cpu.regs));

	// Decrements
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0xB, GB_REG_SP(emu->cpu.regs));
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0xA, GB_REG_SP(emu->cpu.regs));

	// ADD HL, SP (half-carry)
	GB_REG_HL(emu->cpu.regs) = 0x0FEE;
	GB_REG_SP(emu->cpu.regs) = 0x33;
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT16(0x1021, GB_REG_HL(emu->cpu.regs));
	TEST_ASSERT(GB_REG_F(emu->cpu.regs) & GB_FLAG_HALF);
	TEST_ASSERT((GB_REG_F(emu->cpu.regs) & GB_FLAG_CARR) == 0);
	TEST_ASSERT((GB_REG_F(emu->cpu.regs) & GB_FLAG_SUBS) == 0);

	// ADD HL, SP (carry)
	GB_REG_HL(emu->cpu.regs) = 0xFF02;
	GB_REG_SP(emu->cpu.regs) = 0xFF;
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT16(0x0001, GB_REG_HL(emu->cpu.regs));
	TEST_ASSERT(GB_REG_F(emu->cpu.regs) & GB_FLAG_HALF);
	TEST_ASSERT(GB_REG_F(emu->cpu.regs) & GB_FLAG_CARR);
	TEST_ASSERT((GB_REG_F(emu->cpu.regs) & GB_FLAG_SUBS) == 0);

	// LD HL, $1234
	gb_emu_advance_opcode(emu);
	// LD SP, HL
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT16(0xFF85, GB_REG_SP(emu->cpu.regs));
	TEST_ASSERT_EQUAL_UINT16(0xFF85, GB_REG_HL(emu->cpu.regs));

	// ADD SP, 1
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT16(0xFF86, GB_REG_SP(emu->cpu.regs));
	TEST_ASSERT_EQUAL_UINT8(0x0, GB_REG_F(emu->cpu.regs));

	// ADD SP, -1
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT16(0xFF85, GB_REG_SP(emu->cpu.regs));
	TEST_ASSERT_EQUAL_UINT8(
		GB_FLAG_CARR | GB_FLAG_HALF, GB_REG_F(emu->cpu.regs)
	);

	// ADD SP, 127
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT16(0x0004, GB_REG_SP(emu->cpu.regs));
	TEST_ASSERT_EQUAL_UINT8(
		GB_FLAG_CARR | GB_FLAG_HALF, GB_REG_F(emu->cpu.regs)
	);

	// LD HL, SP + 1
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT16(0x0004, GB_REG_SP(emu->cpu.regs));
	TEST_ASSERT_EQUAL_UINT16(0x0005, GB_REG_HL(emu->cpu.regs));
	TEST_ASSERT_EQUAL_UINT8(0, GB_REG_F(emu->cpu.regs));

	// LD HL, SP - 1
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT16(0x0004, GB_REG_SP(emu->cpu.regs));
	TEST_ASSERT_EQUAL_UINT16(0x0003, GB_REG_HL(emu->cpu.regs));
	TEST_ASSERT_EQUAL_UINT8(
		GB_FLAG_CARR | GB_FLAG_HALF, GB_REG_F(emu->cpu.regs)
	);

	gb_emu_delete(emu);
}
