#include "gbEmu.h"
#include "log.h"

void gb_emu_advance_opcode(gb_emu_t* emu) {
    u8 opcode = gb_cart_read_u8(&emu->cart, GB_REG_PC(emu->cpu.regs)++);
    switch (opcode) {
        // NOP
        case 0x00: {
            break;
        }

        // jp, a16
        case 0xC3: {
            GB_REG_PC(emu->cpu.regs) = gb_cart_read_u16(&emu->cart, GB_REG_PC(emu->cpu.regs));
            break;
        }

        default: {
            GB_FATAL("UNIMPLEMENTED OPCODE! 0x%X\n", opcode);
            break;
        }
    }
}
