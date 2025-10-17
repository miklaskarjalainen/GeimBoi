// SPDX-License-Identifier: GPL-2.0-only

#include <stdlib.h>
#include <unity.h>

#include "gbEmu.h"
#include "gbMMU.h"
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

void test_opcode_bits(void)
{
	const u8 payload[] = {
		[0x100] = 0x00,

		// LD, $FF
		0x3E,
		0xFF,
		// RES 5, A
		0xCB,
		0xAF,
		// RES 7, A
		0xCB,
		0xBF,
		// RES 2, A
		0xCB,
		0x97,

		// LD HL, $8001
		0x21,
		0x01,
		0x80,
		// LD (HL), $FF
		0x36,
		0xFF,
		// RES 1, (HL)
		0xCB,
		0x8E,
		// SET 1, (HL)
		0xCB,
		0xCE
	};

	gb_emu_t* emu =
		_test_create_emu_with_payload(payload, GB_ARRAY_LEN(payload));
	gb_emu_advance_opcode(emu);

	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0xFF, GB_REG_A(emu->cpu.regs));

	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0xFF & ~GB_BIT(5), GB_REG_A(emu->cpu.regs));

	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(
		0xFF & ~(GB_BIT(7) | GB_BIT(5)), GB_REG_A(emu->cpu.regs)
	);

	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(
		0xFF & ~(GB_BIT(7) | GB_BIT(5) | GB_BIT(2)), GB_REG_A(emu->cpu.regs)
	);

	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT16(0x8001, GB_REG_HL(emu->cpu.regs));

	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0xFF, gb_mmu_read_u8(&emu->mmu, 0x8001));

	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(
		0xFF & ~(GB_BIT(1)), gb_mmu_read_u8(&emu->mmu, 0x8001)
	);

	gb_emu_advance_opcode(emu);
	TEST_ASSERT_EQUAL_UINT8(0xFF, gb_mmu_read_u8(&emu->mmu, 0x8001));

	gb_emu_delete(emu);
}
