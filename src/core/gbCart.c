#include "gbCart.h"
#include "log.h"

#include <stdlib.h>

extern void _gb_mbc0_reset(gb_cart_t* cart);
extern void _gb_mbc1_reset(gb_cart_t* cart);

static u16 gb_cart_read_u16_be(const gb_cart_t* cart, u16 addr)
{
	const u16 low = cart->read_rom(cart, addr + 1);
	const u16 high = cart->read_rom(cart, addr);
	return (u16)((low) | (high << 8));
}

void gb_cart_init(gb_cart_t* cart)
{
	// Zero initialize
	for (size_t i = 0; i < GB_MAX_CARTSIZE; i++) {
		cart->rom[i] = 0x00;
	}
	for (size_t i = 0; i < GB_MAX_RAMSIZE; i++) {
		cart->ram[i] = 0xFF;
	}

	_gb_mbc0_reset(cart);
}

void gb_cart_load(gb_cart_t* cart, const u8* rom, size_t len)
{
	gb_cart_init(cart);

	if (len >= GB_MAX_CARTSIZE) {
		GB_WARN(
			"Tried to open file which is larger than the maximum"
			"of a size a rom! Stopped."
		);
		return;
	}

	// Loads the contents of the rom
	for (size_t i = 0; i < len; i++) {
		cart->rom[i] = rom[i];
	}

	// Init variables
	cart->rom_banks = (u16)(2 << cart->rom[0x148]);
	switch (cart->rom[0x149]) {
		case 0: {
			cart->ram_banks = 0;
			break;
		}
		/* 1 is unused */
		case 2: {
			cart->ram_banks = 1;
			break;
		}
		case 3: {
			cart->ram_banks = 4;
			break;
		}
		case 4: {
			cart->ram_banks = 16;
			break;
		}
		case 5: {
			cart->ram_banks = 8;
			break;
		}

		default: {
			GB_FATAL("invalid ram size! 0x%02X", cart->rom[0x149]);
		}
	}

	switch (gb_cart_mapper_type(cart)) {
		case GB_MAPPER_NONE: {
			_gb_mbc0_reset(cart);
			break;
		}
		case GB_MAPPER_MBC1: {
			_gb_mbc1_reset(cart);
			break;
		}
		default: {
			GB_FATAL("Unsupported mapper! %i", cart->rom[0x147]);
		}
	}

	return;
}

gb_cart_mapper_e gb_cart_mapper_type(const gb_cart_t* cart)
{
	const u8 cart_type = cart->rom[0x147];

	switch (cart_type) {
		case 0x00:
		case 0x08:
		case 0x09:
			return GB_MAPPER_NONE;

		case 0x01:
		case 0x02:
		case 0x03:
			return GB_MAPPER_MBC1;
		case 0x04:
		case 0x05:
			return GB_MAPPER_MBC2;
		case 0x0F:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
			return GB_MAPPER_MBC3;

		case 0x0B:
		case 0x0C:
		case 0x0D:
			return GB_MAPPER_MMM01;

		default: {
			break;
		}
	}

	return GB_MAPPER_NONE;
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
	for (size_t i = 0; i < GB_MAX_CARTSIZE; i++) {
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

size_t
gb_cart_read_battery(const gb_cart_t* cart, u8* buffer, size_t buffer_size)
{
	if (!buffer) {
		return 0;
	}

	const size_t used_ram = cart->ram_banks * 0x2000;
	size_t i = 0;
	for (; i < buffer_size && i < used_ram; i++) {
		buffer[i] = cart->ram[i];
	}
	return i;
}

size_t gb_cart_write_battery(gb_cart_t* cart, u8* buffer, size_t buffer_size)
{
	if (!buffer) {
		return 0;
	}

	const size_t used_ram = cart->ram_banks * 0x2000;
	size_t i = 0;
	for (; i < buffer_size && i < used_ram; i++) {
		cart->ram[i] = buffer[i];
	}
	return i;
}
