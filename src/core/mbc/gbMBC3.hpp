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

		bool SaveBattery(const std::string& path) override;
		bool LoadBattery(const std::string& path) override;

		uint8_t ReadByte(uint16_t addr) const           override;
		void    WriteByte(uint16_t addr, uint8_t data) override;
		void    Reset() override;
	private:
		bool mRamEnable = false; // If false, no ram can be read or written.
		uint8_t mRam[0x8000];    // Max rambanks is 4 (in this chip), 1 ram bank is 0x2000

		uint8_t mRomBank = 0x01; // 7 bits used.
		uint8_t mRamBank = 0x00; // 0-3 (4 banks) 4-12 (Rtc Registers)
	
		// RTC
		void LatchRTC();
		struct Rtc {
			uint8_t LatchData = 0;
			time_t LastLatch = 0; // seconds
			uint8_t Sec   = 0;
			uint8_t Min   = 0;
			uint8_t Hour  = 0;
			uint16_t Day  = 0; // 9-bit number
			bool Halt     = false;
			bool DayCarry = false;
		} mRtc;
	protected:
		gbMBC3(gbCart* cart);
		friend gbMBC;
	};

}