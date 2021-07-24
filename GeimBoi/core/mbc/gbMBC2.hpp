#pragma once
#include "gbMBC.hpp"

namespace Giffi
{
	// Supports ROM sizes up to 2mb (16 banks), and has a internal ram of 512 x 4bits
	class gbMBC2 : public gbMBC
	{
	public:
		~gbMBC2() override;

		uint16_t GetCurRomBank() const override;

		uint8_t ReadByte(uint16_t _addr) const        override;
		void    WriteByte(uint16_t _addr, uint8_t _data) override;
		void    Reset() override;
	private:
		bool mRamEnable = false; // aka 'RAMG'
		uint8_t mCurBank = 0x01; // aka 'ROMB'
		uint8_t mRam[0x200];     // Interal ram, only 4 lower bits used.

	protected:
		gbMBC2(gbCart* _cart);
		friend gbMBC;
	};

} // Namespace

