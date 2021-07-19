#pragma once
#include <memory>
#include "gbMBC.hpp"

namespace Giffi
{
	class gbMBC1 : public gbMBC
	{
	public:
		~gbMBC1() override { printf("MBC1 Destroyed\n"); }

		uint16_t GetCurRomBank() const override;
		uint16_t GetCurRamBank() const override;

		uint8_t ReadByte(uint16_t _addr) const        override;
		void    WriteByte(uint16_t _addr, uint8_t _data) override;
		void    Reset() override;
	private:
		bool mMode      = false; // false = Rom Mode, true = "Ram Mode".
		bool mRamEnable = false; // If false, no ram can be read or written.
		uint8_t mRam[0x8000];    // Max rambanks is 4 (in this chip), 1 ram bank is 0x2000
		uint8_t mCurBank = 0x01; // lower 5 bits (Bank1 for rombanking) after 2bits for (Bank2 for ram / rom -banking)

	protected:
		gbMBC1(gbCart* _cart);
		friend gbMBC;
	};

} // Namespace

