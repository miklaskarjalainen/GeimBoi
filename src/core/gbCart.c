#include "gbCart.h"

#include <stdlib.h>

/**
 * @brief
 * @note Takes ownership
 */
gb_cart_t gb_cart_create(u8* rom, size_t len)
{
    gb_cart_t cart = {
        .rom = rom,
        .len = len
    };
    return cart;
}

void gb_cart_delete(gb_cart_t* cart)
{
    free(cart->rom);
    cart->rom = NULL;
    cart->len = 0;
}


u8 gb_cart_read_u8(gb_cart_t* cart, u16 addr)
{
    return cart->rom[addr];
}

u16 gb_cart_read_u16(gb_cart_t* cart, u16 addr)
{
    const u16 low = gb_cart_read_u8(cart, addr);
    const u16 high = gb_cart_read_u8(cart, addr+1);
    return (u16)((low) | (high << 8));
}

void gb_cart_write_u8(gb_cart_t* cart, u16 addr, u8 data)
{
    (void)cart;
    (void)addr;
    (void)data;
}
