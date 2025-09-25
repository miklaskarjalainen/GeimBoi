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
        std::printf("%X\n", GB_REG_PC(emu.cpu.regs));
        std::printf("%s\n", gb_cart_get_name(&emu.cart));
        std::printf("%i\n", gb_verify_checksums(&emu.cart));
        return 0;
    }
}
