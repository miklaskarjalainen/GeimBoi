#include <unity.h>
#include <stdlib.h>

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

void test_opcode_jp(void)
{
    const u8 payload[] = {
        [0x100] = 0x00,

        // JP, $0108
        0xC3,
        0x08,
        0x01,

        [0x0108] = 0x00,

        // XOR A
        0xAF,

        // JP Z, $0120
        0xCA,
        0x20,
        0x01,

        [0x0120] = 0x00,
        // JR, -1
        0x18,
        (u8)-3,
    };

    gb_emu_t* emu = _test_create_emu_with_payload(payload, GB_ARRAY_LEN(payload));
    gb_emu_advance_opcode(emu);

    gb_emu_advance_opcode(emu);
    TEST_ASSERT_EQUAL_UINT16(0x0108, GB_REG_PC(emu->cpu.regs));

    gb_emu_advance_opcode(emu);
    gb_emu_advance_opcode(emu);
    TEST_ASSERT_EQUAL_UINT8(0x00, GB_REG_A(emu->cpu.regs));
    TEST_ASSERT_EQUAL_UINT8(GB_FLAG_ZERO, GB_REG_F(emu->cpu.regs));

    gb_emu_advance_opcode(emu);
    TEST_ASSERT_EQUAL_UINT16(0x0120, GB_REG_PC(emu->cpu.regs));

    gb_emu_advance_opcode(emu);
    gb_emu_advance_opcode(emu);
    TEST_ASSERT_EQUAL_UINT16(0x0120, GB_REG_PC(emu->cpu.regs));

    gb_emu_delete(emu);
}
