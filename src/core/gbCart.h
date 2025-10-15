#ifndef _GB_CART_H
#define _GB_CART_H

#include "gbReg.h"

#include <stdbool.h>
#include <stddef.h>

/*
 * The maximum possible size of a GB/(C) rom is 8 MiB.
 */
#define GB_MAX_CARTSIZE 0x800000
/*
 * The maximum amount of ram on a GB/(C) rom is 128 KiB.
 */
#define GB_MAX_RAMSIZE (0x8000 * 4)

struct gb_cart;

typedef void (*_gb_cart_write_fn)(struct gb_cart* cart, u16 addr, u8 byte);
typedef u8 (*_gb_cart_read_fn)(const struct gb_cart* cart, u16 addr);

/**
 * @brief Represents the game cartridge.
 */
typedef struct gb_cart {
	u8 rom[GB_MAX_CARTSIZE];
	u8 ram[GB_MAX_RAMSIZE];
	u8 ram_banks;
	u16 rom_banks;
	_gb_cart_write_fn write_rom, write_ram;
	_gb_cart_read_fn read_rom, read_ram;

	union {
		struct {
			u8 rom_bank_low : 5;
			u8 rom_bank_high : 2;
			u8 ram_enable : 1;
			u8 banking_mode : 1;
			u8 ram_bank : 2;
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

void gb_cart_init(gb_cart_t* cart);
void gb_cart_load(gb_cart_t* cart, const u8* rom, size_t len);
gb_cart_mapper_e gb_cart_mapper_type(const gb_cart_t* cart);

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
/*
 * @brief copies the contents of on board ram into a buffer.
 * (Usually hosting the save file)
 * @warning To guarantee that all of ram can be copied,
 * use a buffer size of ram_banks * 0x2000.
 * @returns the number of bytes copied.
 */
size_t
gb_cart_read_battery(const gb_cart_t* cart, u8* buffer, size_t buffer_size);
/*
 * @brief copies the contents of buffer into ram .
 * (Usually hosting the save file)
 * @returns the number of bytes copied.
 */
size_t gb_cart_write_battery(gb_cart_t* cart, u8* buffer, size_t buffer_size);
#endif
