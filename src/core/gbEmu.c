#include "gbEmu.h"
#include "gbReg.h"
#include "gbZ80.h"

#include <stdio.h>
#include <stdlib.h>

gb_emu_t gb_emu_create(void)
{
    return (gb_emu_t) {
        .cpu = gb_cpu_create()
    };
}

void gb_emu_delete(gb_emu_t* emu) { free(emu->rom); }

bool gb_emu_load_rom_bytes(gb_emu_t* emu, u8* rom, size_t length)
{
    if (length < 0x8000) {
        free(rom);
        return false;
    }
    emu->rom = rom;
    return true;
}

bool gb_emu_load_rom_file(gb_emu_t* emu, const char* fpath)
{
    FILE* file = fopen(fpath, "rb");

    // Get size
    fseek(file, 0, SEEK_END);
    size_t file_len = (size_t)ftell(file);
    rewind(file);

    // Allocate & Copy
    u8* buffer = (u8*)malloc(file_len * sizeof(u8));
    fread(buffer, 1, file_len, file);

    // Load & Close
    bool r = gb_emu_load_rom_bytes(emu, buffer, file_len);
    fclose(file);

    return r;
}

void gb_emu_advance_frame(gb_emu_t* emu) { (void)emu; }

void gb_emu_advance_opcode(gb_emu_t* emu) {
    u8 opcode = emu->rom[GB_REG_PC(emu->cpu.regs)++];
    switch (opcode) {
        // NOP
        case 0x00: {
            break;
        }

        // jp, a16
        case 0xC3: {
            gb_reg16_t reg;
            reg.nibble.low = emu->rom[GB_REG_PC(emu->cpu.regs)++];
            reg.nibble.high = emu->rom[GB_REG_PC(emu->cpu.regs)++];
            GB_REG_PC(emu->cpu.regs) = reg.value;
            break;
        }

        default: {
            printf("UNIMPLEMENTED OPCODE! 0x%X\n", opcode);
            break;
        }
    }
}
