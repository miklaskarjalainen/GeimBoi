#include <filesystem>
#include "gbBootRom.hpp"

using namespace Giffi;

bool gbBootRom::LoadBios(const std::string& _path)
{
	if (!std::filesystem::exists(_path))
	{
		printf("Couldn't find %s!\n", _path.c_str());
		mHasBios = false;
		return false;
	}

	// Load Bios, maybe add bios hashes for more safety?
	memset(mBootRom, 0, sizeof(mBootRom));
	FILE* in;
	in = fopen(_path.c_str(), "rb");
	fread(mBootRom, 1, 0x100, in);
	fclose(in);
	printf("%s Loaded successfully!\n", _path.c_str());
	mHasBios = true;

	return true;
}

uint8_t gbBootRom::ReadByte(uint16_t _addr) const
{
	if (_addr == 0xFF)
	{
		mLoaded = false;
	}

	return mBootRom[_addr];
} 

void gbBootRom::Reset()
{
	mLoaded = true;
}

bool gbBootRom::IsLoaded() const
{
	return mLoaded && mHasBios;
}

