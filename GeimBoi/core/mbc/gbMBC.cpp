#pragma once
#include "gbMBC.hpp"
#include "gbMBC1.hpp"
#include "gbMBC2.hpp"
#include "../gbCart.hpp"

using namespace Giffi;

uint8_t gbMBC::ReadByte(uint16_t _addr) const
{
	return mCart->mCart[_addr];
}

void gbMBC::WriteByte(uint16_t _addr, uint8_t _data)
{
	// Don't write to ROM lmfao
}

gbMBC* gbMBC::CreateMBC(gbCart* _cart)
{
	switch (_cart->GetCartType())
	{
		case gbCartType::MBC1:
			return new gbMBC1(_cart);
		case gbCartType::MBC2:
			return new gbMBC2(_cart);
		default:
			return new gbMBC(_cart);
	}
}