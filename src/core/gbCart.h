#ifndef _GB_CART_H
#define _GB_CART_H

#include "gbReg.h"

#include <stddef.h>
#include <stdbool.h>

/*
 * The maximum possible size of a GB/(C) rom is 8 MiB.
 */
#define GB_MAX_CARTSIZE 0x800000

struct gb_cart;

typedef void(*_gb_cart_write_fn)(struct gb_cart* cart, u16 addr, u8 byte);
typedef u8(*_gb_cart_read_fn)(const struct gb_cart* cart, u16 addr);


/**
 * @brief Represents the game cartridge.
 */
typedef struct gb_cart {
    u8 rom[GB_MAX_CARTSIZE];
    u8 ram_banks;
    u16 rom_banks;
    _gb_cart_write_fn write;
    _gb_cart_read_fn read;

    union {
        struct {
            u8 rom_bank_low: 5;
            u8 rom_bank_high: 2;
            u8 ram_enable: 1;
            u8 banking_mode: 1;
            u8 ram_bank: 2;
        } mbc1;
    } mapper_data;
} gb_cart_t;

typedef enum gb_cart_mapper {
    GB_MAPPER_NONE = 0,
    GB_MAPPER_MBC1,
    GB_MAPPER_MBC2,
    GB_MAPPER_MBC3,
    GB_MAPPER_MBC5,
    GB_MAPPER_MBC7,
    GB_MAPPER_MMM01,
} gb_cart_mapper_e;

void gb_cart_load(gb_cart_t* cart, const u8* rom, size_t len);
gb_cart_mapper_e gb_cart_mapper_type(const gb_cart_t* cart);

u8 gb_cart_read_u8(const gb_cart_t* cart, u16 addr);

/**
 * @brief null-terminated game name
 * @note pointer to static array, copy before next call!
 */
const char* gb_cart_get_name(const gb_cart_t* cart);
bool gb_cart_cgb_flag(const gb_cart_t* cart);

/**
 * @brief 0 - success, 1 - header mismatch, 2 - rom mismatch, 3 - both.
 */
int gb_verify_checksums(const gb_cart_t* cart);



#endif
