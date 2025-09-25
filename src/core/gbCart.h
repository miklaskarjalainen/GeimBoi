#ifndef _GB_CART_H
#define _GB_CART_H

#include "gbReg.h"

#include <stddef.h>

/**
 * @brief Represents the game cartridge.
 */
typedef struct gb_cart {
    u8* rom;
    size_t len;
} gb_cart_t;

/**
 * @brief
 * @note Takes ownership
 */
gb_cart_t gb_cart_create(u8* rom, size_t len);
void gb_cart_delete(gb_cart_t* cart);

u8 gb_cart_read_u8(gb_cart_t* cart, u16 addr);
u16 gb_cart_read_u16(gb_cart_t* cart, u16 addr);
void gb_cart_write_u8(gb_cart_t* cart, u16 addr, u8 data);

#endif
