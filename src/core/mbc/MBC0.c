#include "gbCart.h"
#include "log.h"

static inline u8 _gb_mbc0_read_rom(const gb_cart_t* cart, u16 addr)
{
	GB_ASSERT(addr < 0x8000, "?");
	return cart->rom[addr];
}

static inline void _gb_mbc0_write_rom(gb_cart_t* cart, u16 addr, u8 data)
{
	(void)cart;
	(void)addr;
	(void)data;
}

static inline u8 _gb_mbc0_read_ram(const gb_cart_t* cart, u16 addr)
{
	GB_ASSERT(addr >= 0xA000 && addr <= 0xBFFF, "?");
	if (cart->ram_banks == 0) {
		return 0xFF;
	}
	return cart->ram[addr - 0xA000];
}

static inline void _gb_mbc0_write_ram(gb_cart_t* cart, u16 addr, u8 data)
{
	GB_ASSERT(addr >= 0xA000 && addr <= 0xBFFF, "?");
	if (cart->ram_banks != 0) {
		cart->ram[addr - 0xA000] = data;
	}
}

void _gb_mbc0_reset(gb_cart_t* cart)
{
	cart->write_rom = _gb_mbc0_write_rom;
	cart->read_rom = _gb_mbc0_read_rom;
	cart->write_ram = _gb_mbc0_write_ram;
	cart->read_ram = _gb_mbc0_read_ram;
}
