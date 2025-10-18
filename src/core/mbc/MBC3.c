// SPDX-License-Identifier: GPL-2.0-only

#include "gbCart.h"
#include "log.h"

static inline void _gb_mbc3_latch(gb_cart_t* cart)
{
	time_t now = time(NULL);
	time_t diff = now - cart->mapper_data.mbc3.rtc.last_latch;
	cart->mapper_data.mbc3.rtc.last_latch = now;

	if (diff > 0) {
		cart->mapper_data.mbc3.rtc.sec += (u8)(diff % 60);
		if (cart->mapper_data.mbc3.rtc.sec >= 59) {
			cart->mapper_data.mbc3.rtc.sec -= 60;
			cart->mapper_data.mbc3.rtc.min += 1;
		}
		diff /= 60;

		cart->mapper_data.mbc3.rtc.min += (u8)(diff % 60);
		if (cart->mapper_data.mbc3.rtc.min >= 59) {
			cart->mapper_data.mbc3.rtc.min -= 60;
			cart->mapper_data.mbc3.rtc.hour += 1;
		}
		diff /= 60;

		cart->mapper_data.mbc3.rtc.hour += (u8)(diff % 24);
		if (cart->mapper_data.mbc3.rtc.hour >= 23) {
			cart->mapper_data.mbc3.rtc.hour -= 24;
			cart->mapper_data.mbc3.rtc.day += 1;
		}
		diff /= 24;

		cart->mapper_data.mbc3.rtc.day += (u8)diff;
	}
}

static inline u8 _gb_mbc3_read_rom(const gb_cart_t* cart, u16 addr)
{
	GB_ASSERT(addr < 0x8000, "?");
	if (addr < 0x4000) {
		return cart->rom[addr];
	}
	if (addr < 0x8000) {
		u8 bank = cart->mapper_data.mbc3.rom_bank;
		bank &= (u8)(cart->rom_banks - 1U);
		if (!bank) {
			bank = 1;
		}
		u32 banked_addr = (u32)((addr - 0x4000) + (bank * 0x4000));
		return cart->rom[banked_addr];
	}
	GB_FATAL("?");
	return 0xFF;
}

static inline void _gb_mbc3_write_rom(gb_cart_t* cart, u16 addr, u8 data)
{
	GB_ASSERT(addr < 0x8000, "?");
	if (addr < 0x2000) { // RAM Enable
		cart->mapper_data.mbc3.ram_rtc_enable = (data & 0xFU) == 0xA ? 1 : 0;
	}
	else if (addr < 0x4000) { // Change ROM bank
		cart->mapper_data.mbc3.rom_bank = data & 0x7FU;
	}
	else if (addr < 0x6000) { // Change RAM / RTC bank
		cart->mapper_data.mbc3.ram_rtc_bank = data & 0xFU;
	}
	else if (addr < 0x8000) { // Clock LATCH
		if (cart->mapper_data.mbc3.rtc.latch_data == 0x00 && data == 0x01) {
			_gb_mbc3_latch(cart);
		}
		cart->mapper_data.mbc3.rtc.latch_data = data;
	}
}

static inline u8 _gb_mbc3_read_ram(const gb_cart_t* cart, u16 addr)
{
	GB_ASSERT(addr >= 0xA000 && addr <= 0xBFFF, "?");

	if (!cart->mapper_data.mbc3.ram_rtc_enable) {
		return 0xFF;
	}

	// RAM
	if (cart->mapper_data.mbc3.ram_rtc_bank < 0x8) {
		const u16 ram_addr =
			(u16)((addr - 0xA000) +
				  (cart->mapper_data.mbc3.ram_rtc_bank * 0x2000));
		return cart->ram[ram_addr];
	}

	// RTC
	if (cart->mapper_data.mbc3.ram_rtc_bank < 0xD) {
		switch (cart->mapper_data.mbc3.ram_rtc_bank) {
			case 0x08: {
				return cart->mapper_data.mbc3.rtc.sec;
			}
			case 0x09: {
				return cart->mapper_data.mbc3.rtc.min;
			}
			case 0x0A: {
				return cart->mapper_data.mbc3.rtc.hour;
			}
			case 0x0B: {
				return cart->mapper_data.mbc3.rtc.day;
			}
			case 0x0C: {
				GB_WARN("todo: RTC flags");
				return 0;
			}
			default: {
				GB_FATAL("?");
			}
		}
	}
	GB_FATAL("?");
	return 0xFF;
}

static inline void _gb_mbc3_write_ram(gb_cart_t* cart, u16 addr, u8 data)
{
	GB_ASSERT(addr >= 0xA000 && addr <= 0xBFFF, "?");

	if (!cart->mapper_data.mbc3.ram_rtc_enable) {
		return;
	}

	// RAM
	if (cart->mapper_data.mbc3.ram_rtc_bank < 0x8) {
		const u16 ram_addr =
			(u16)((addr - 0xA000) +
				  (cart->mapper_data.mbc3.ram_rtc_bank * 0x2000));
		cart->ram[ram_addr] = data;
		return;
	}

	// RTC
	if (cart->mapper_data.mbc3.ram_rtc_bank < 0xD) {
		GB_FATAL("RTC not implemented");
	}
	GB_FATAL("?");
}

void _gb_mbc3_reset(gb_cart_t* cart)
{
	cart->mapper_data.mbc3.ram_rtc_enable = 0;
	cart->mapper_data.mbc3.rom_bank = 1;
	cart->mapper_data.mbc3.ram_rtc_bank = 0;

	cart->write_rom = _gb_mbc3_write_rom;
	cart->read_rom = _gb_mbc3_read_rom;
	cart->write_ram = _gb_mbc3_write_ram;
	cart->read_ram = _gb_mbc3_read_ram;
}
