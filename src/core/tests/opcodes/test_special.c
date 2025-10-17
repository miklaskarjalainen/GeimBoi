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

void test_opcode_special(void)
{
	const u8 payload[] = {
		[0x100] = 0x00,

		// LD A, $20
		0x3E,
		0x20,
		// LD E, $40
		0x1E,
		0x40,
		// ADD A, E
		0x83,
		// DAA
		0x27,

		// LD A, $49
		0x3E,
		0x49,
		// LD E, $42
		0x1E,
		0x42,
		// ADD A, E
		0x83,
		// DAA,
		0x27
	};

	gb_emu_t* emu =
		_test_create_emu_with_payload(payload, GB_ARRAY_LEN(payload));

	// 0x20 + 0x40 => 0x60 => 0x60
	gb_emu_advance_opcode(emu);
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0x20, GB_REG_A(emu->cpu.regs));
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0x40, GB_REG_E(emu->cpu.regs));
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0x60, GB_REG_A(emu->cpu.regs));
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0x60, GB_REG_A(emu->cpu.regs));

	// 0x49 + 0x42 => 0x8b => 0x91
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0x49, GB_REG_A(emu->cpu.regs));
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0x42, GB_REG_E(emu->cpu.regs));
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0x8B, GB_REG_A(emu->cpu.regs));
	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0x91, GB_REG_A(emu->cpu.regs));

	gb_emu_delete(emu);
}
