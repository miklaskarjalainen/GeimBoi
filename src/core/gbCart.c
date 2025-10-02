#include "gbCart.h"
#include "log.h"

#include <stdlib.h>

static u16 gb_cart_read_u16_be(const gb_cart_t* cart, u16 addr)
{
	const u16 low = gb_cart_read_u8(cart, addr + 1);
	const u16 high = gb_cart_read_u8(cart, addr);
	return (u16)((low) | (high << 8));
}

/**
 * @brief
 * @note Takes ownership
 */
gb_cart_t gb_cart_create(u8* rom, size_t len)
{
	gb_cart_t cart = {.rom = rom, .len = len};
	return cart;
}

void gb_cart_delete(gb_cart_t* cart)
{
	free(cart->rom);
	cart->rom = NULL;
	cart->len = 0;
}

u8 gb_cart_read_u8(const gb_cart_t* cart, u16 addr) { return cart->rom[addr]; }

void gb_cart_write_u8(gb_cart_t* cart, u16 addr, u8 data)
{
	(void)cart;
	(void)addr;
	(void)data;
}

bool gb_cart_cgb_flag(const gb_cart_t* cart)
{
	const u8 flag = cart->rom[0x143];
	return flag == 0x80 || flag == 0xC0;
}

const char* gb_cart_get_name(const gb_cart_t* cart)
{
	static char s_name[17] = {0};

	const u8 count = gb_cart_cgb_flag(cart) ? 11 : 16;
	for (u8 i = 0; i < count; i++) {
		s_name[i] = (char)cart->rom[0x134 + i];
	}

	return s_name;
}

static bool _gb_verify_global(const gb_cart_t* cart)
{
	u16 sum = 0;
	for (size_t i = 0; i < cart->len; i++) {
		if (i == 0x14E || i == 0x14F) {
			continue;
		}
		sum += cart->rom[i];
	}

	u16 expct = gb_cart_read_u16_be(cart, 0x14E);
	if (sum != expct) {
		GB_WARN(
			"Game '%s' global checksum failed! Expected '0x%X' got '0x%X'",
			gb_cart_get_name(cart),
			expct,
			sum
		);
		return false;
	}

	return true;
}

static bool _gb_verify_header(const gb_cart_t* cart)
{
	u8 sum = 0;
	for (u16 address = 0x0134; address <= 0x014C; address++) {
		sum = (u8)(sum - cart->rom[address] - 1);
	}
	u8 excpt = cart->rom[0x14D];
	if (sum != excpt) {
		GB_WARN(
			"Game '%s' header checksum failed! Expected '0x%X' got '0x%X'",
			gb_cart_get_name(cart),
			excpt,
			sum
		);
		return false;
	}
	return true;
}

int gb_verify_checksums(const gb_cart_t* cart)
{
	int r = _gb_verify_header(cart) ? 0 : 1;
	r += _gb_verify_global(cart) ? 0 : 2;
	return r;
}
