#include "gbEmu.h"
#include "gbCart.h"
#include "gbReg.h"
#include "gbSM83.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>

gb_emu_t gb_emu_create(void)
{
    return (gb_emu_t) {
        .cpu = gb_cpu_create()
    };
}

void gb_emu_delete(gb_emu_t* emu) { gb_cart_delete(&emu->cart); }

bool gb_emu_load_rom_bytes(gb_emu_t* emu, u8* rom, size_t length)
{
    if (length < 0x8000) {
        free(rom);
        return false;
    }
    emu->cart = gb_cart_create(rom, length);
    return true;
}

bool gb_emu_load_rom_file(gb_emu_t* emu, const char* fpath)
{
    FILE* file = fopen(fpath, "rb");
    if (!file) {
        GB_ERROR("Could not open file '%s'!", fpath);
        return false;
    }

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

u16 gb_emu_read_u16(gb_emu_t* emu, u16 addr)
{
    return gb_cart_read_u16(&emu->cart, addr);
}

u8 gb_emu_read_u8(gb_emu_t* emu, u16 addr)
{
    return gb_cart_read_u8(&emu->cart, addr);
}
