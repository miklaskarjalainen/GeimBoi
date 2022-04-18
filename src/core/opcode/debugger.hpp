#pragma once
#include <string>
#include <string_view>

namespace Giffi
{
	std::string_view GetAssembly(uint16_t opcode);
	uint8_t GetLength(uint16_t opcode);
}
