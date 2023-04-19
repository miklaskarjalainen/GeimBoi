#pragma once
#include <cstdint>
#include <string>
#include <memory>

namespace GeimBoi
{
	class gbCart;

	enum class gbCartType
	{
		None = 0,
		MBC1 = 1,
		MBC2 = 2,
		MBC3 = 3,
	};

	/*
		MBC (Memory Bank Controller) is essentially a mapper on the cartridge,
		but there can also be a Battery, RAM or other hardware.
		Basically inherited classes accounts for any extra hardware in the cartridge.
	*/
	class gbMBC
	{
	public:
		virtual ~gbMBC() = default;

		virtual uint16_t GetCurRomBank() const { return 0; }
		virtual uint16_t GetCurRamBank() const { return 0; }

		bool SaveBattery(const std::string&);
		bool LoadBattery(const std::string&);

		virtual bool WriteState(std::ofstream&) { return false; } ;
		virtual bool ReadState(std::ifstream&) { return false; };

		virtual uint8_t ReadByte(uint16_t addr) const;
		virtual void WriteByte(uint16_t addr, uint8_t data);
		virtual void Reset() {}

		/*
			Gets the correct "mapper" for the cartridge.
		*/
		static std::unique_ptr<gbMBC> CreateMBC(gbCart* cart);


	protected:
		gbCart* mCart = nullptr;

		virtual bool SaveBatteryImpl(std::ofstream&) { return false; };
		virtual bool LoadBatteryImpl(std::ifstream&) { return false; };

		gbMBC(gbCart* cart)
			:mCart(cart) {};
	};
}

