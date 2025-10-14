#include "gbCart.h"
#include "log.h"

static inline u8 _gb_mbc1_read_rom(const gb_cart_t* cart, u16 addr)
{
	GB_ASSERT(addr < 0x8000, "?");
	if (addr < 0x4000) {
		if (!cart->mapper_data.mbc1.banking_mode) {
			return cart->rom[addr];
		}
		u16 bank = (u8)(cart->mapper_data.mbc1.rom_bank_high << 5);
		bank &= cart->rom_banks - 1;
		const u32 banked_addr = (u32)((0x4000 * bank) + addr);
		return cart->rom[banked_addr];
	}
	if (addr < 0x8000) {
		u16 bank;
		if (!cart->mapper_data.mbc1.banking_mode) {
			bank = (u16)(cart->mapper_data.mbc1.rom_bank_low |
						 (cart->mapper_data.mbc1.rom_bank_high << 5));
		}
		else {
			bank = (u16)(cart->mapper_data.mbc1.rom_bank_low);
		}
		bank &= cart->rom_banks - 1;
		u32 banked_addr = (u32)((addr - 0x4000) + (bank * 0x4000));
		return cart->rom[banked_addr];
	}
	GB_FATAL("?");
	return 0xFF;
}

static inline void _gb_mbc1_write_rom(gb_cart_t* cart, u16 addr, u8 data)
{
	GB_ASSERT(addr < 0x8000, "?");
	if (addr < 0x2000) { // RAM Enable
		if (cart->ram_banks == 0) {
			return;
		}
		cart->mapper_data.mbc1.ram_enable = (data & 0xF) == 0xA ? 1 : 0;
	}
	else if (addr < 0x4000) { // Change BANK1
		cart->mapper_data.mbc1.rom_bank_low = data & 0x1FU;
	}
	else if (addr < 0x6000) { // Change BANK2
		cart->mapper_data.mbc1.rom_bank_high = data & 0x3U;
	}
	else if (addr < 0x8000) { // Banking Mode Select
		cart->mapper_data.mbc1.banking_mode = data & 0x1U;
	}
}

static inline u8 _gb_mbc1_read_ram(const gb_cart_t* cart, u16 addr)
{
	GB_ASSERT(addr >= 0xA000 && addr <= 0xBFFF, "?");
	if (cart->ram_banks == 0) {
		return 0xFF;
	}

	const u16 ram_addr = addr - 0xA000;
	u8 bank = cart->mapper_data.mbc1.banking_mode
				  ? cart->mapper_data.mbc1.rom_bank_high
				  : 0;
	bank &= cart->ram_banks - 1;
	return cart->ram[(ram_addr + (bank * 0x2000))];
}

static inline void _gb_mbc1_write_ram(gb_cart_t* cart, u16 addr, u8 data)
{
	GB_ASSERT(addr >= 0xA000 && addr <= 0xBFFF, "?");
	if (!cart->mapper_data.mbc1.ram_enable) {
		return;
	}

	const u16 ram_addr = addr - 0xA000;
	u8 bank = cart->mapper_data.mbc1.banking_mode
				  ? cart->mapper_data.mbc1.rom_bank_high
				  : 0;
	bank &= cart->ram_banks - 1;
	cart->ram[(ram_addr + (bank * 0x2000))] = data;
}

void _gb_mbc1_reset(gb_cart_t* cart)
{
	cart->mapper_data.mbc1.banking_mode = 0;
	cart->mapper_data.mbc1.ram_enable = 0;
	cart->mapper_data.mbc1.rom_bank_low = 0x01;
	cart->mapper_data.mbc1.rom_bank_high = 0x00;
	cart->write_rom = _gb_mbc1_write_rom;
	cart->read_rom = _gb_mbc1_read_rom;
	cart->write_ram = _gb_mbc1_write_ram;
	cart->read_ram = _gb_mbc1_read_ram;
}
