#pragma once
#include "gbMBC.hpp"

namespace GeimBoi
{

	class gbMBC3 : public gbMBC
	{
	public:
		~gbMBC3() override;

		uint16_t GetCurRomBank() const override;
		uint16_t GetCurRamBank() const override;

		bool SaveBattery(const std::string& _path) override;
		bool LoadBattery(const std::string& _path) override;

		uint8_t ReadByte(uint16_t _addr) const           override;
		void    WriteByte(uint16_t _addr, uint8_t _data) override;
		void    Reset() override;
	private:
		bool mMode = false; // false = Rom Mode, true = "Ram Mode".
		bool mRamEnable = false; // If false, no ram can be read or written.
		uint8_t mRam[0x8000];    // Max rambanks is 4 (in this chip), 1 ram bank is 0x2000

		uint8_t mRomBank = 0x01; // 7 bits used.
		uint8_t mRamBank = 0x00; // 0-3 (4 banks)
	protected:
		gbMBC3(gbCart* _cart);
		friend gbMBC;
	};

}