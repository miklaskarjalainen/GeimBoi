#pragma once
#include <cstdint>
#include <memory>

namespace Giffi
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
		MBC (Memory Bank Controller) is essentually a mapper on the cartridge,
		but there can also be a Battery, RAM or other hardware.
		Basicly accounts for any extra hardware in the cartridge.
	*/
	class gbMBC
	{
	public:
		virtual ~gbMBC() = default;

		virtual uint16_t GetCurRomBank() const { return 0; }
		virtual uint16_t GetCurRamBank() const { return 0; }

		virtual uint8_t ReadByte(uint16_t _addr) const;
		virtual void WriteByte(uint16_t _addr, uint8_t _data);
		virtual void Reset() {}

		static gbMBC* CreateMBC(gbCart* _cart);
	protected:
		gbCart* mCart = nullptr;
		
		gbMBC(gbCart* _cart)
			:mCart(_cart) {};
	};
}

