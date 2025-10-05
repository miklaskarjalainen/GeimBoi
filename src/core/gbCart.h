#ifndef _GB_CART_H
#define _GB_CART_H

#include "gbReg.h"

#include <stddef.h>
#include <stdbool.h>

#define GB_MAX_CARTSIZE 0x8000

/**
 * @brief Represents the game cartridge.
 */
typedef struct gb_cart {
    u8 rom[GB_MAX_CARTSIZE]; // TODO: MBCs
} gb_cart_t;

void gb_cart_load(gb_cart_t* cart, const u8* rom, size_t len);

u8 gb_cart_read_u8(const gb_cart_t* cart, u16 addr);
void gb_cart_write_u8(gb_cart_t* cart, u16 addr, u8 data);

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
