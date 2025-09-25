#include "gbCore.h"

#include <iostream>

namespace GeimBoi {
    int main()
    {
        gb_emu_t emu = gb_emu_create();
        gb_emu_load_rom_file(&emu, "");

        gb_emu_advance_opcode(&emu);
        gb_emu_advance_opcode(&emu);
        gb_emu_advance_opcode(&emu);

        std::printf("%X\n", gb_emu_read_u16(&emu, GB_REG_PC(emu.cpu.regs)));

        for (int j = 0; j < 255;){
            u8 opcode = gb_emu_read_u8(&emu, GB_REG_PC(emu.cpu.regs) + j);
            std::printf("[0x%04X] %s ", GB_REG_PC(emu.cpu.regs) + j, gb_opcode_asm(opcode));

            u8 size = gb_opcode_size(opcode);
            switch(size)
            {
                case 3: {
                    std::printf("[0x%04X]", gb_emu_read_u16(&emu, GB_REG_PC(emu.cpu.regs) + j + 1));
                    break;
                }
                case 2: {
                    std::printf("[0x%02X]", gb_emu_read_u8(&emu, GB_REG_PC(emu.cpu.regs) + j + 1));
                    break;
                }
                default: {
                    break;
                }
            }
            j += size;
            std::printf("\n");
        }

        std::printf("%s\n", gb_cart_get_name(&emu.cart));
        std::printf("%i\n", gb_verify_checksums(&emu.cart));
        return 0;
    }
}
