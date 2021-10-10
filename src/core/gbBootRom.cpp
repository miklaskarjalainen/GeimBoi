#include <utils/Benchmark.hpp>
#include <utils/Dir.hpp>
#include <fstream>
#include "gbBootRom.hpp"

using namespace Giffi;

bool gbBootRom::LoadBios(const std::string& _path)
{
	PROFILE_FUNCTION();
	if (!Dir::FileExists(_path))
	{
		printf("Couldn't find a bios at %s!\n", _path.c_str());
		mHasBios = false;
		return false;
	}

	// Load Bios, maybe add bios hashes for more safety?
	std::ifstream rf(_path, std::ios::binary);
	rf.read((char*)mBootRom, sizeof(mBootRom));
	if (rf.bad())
	{
		printf("An error occurred when trying to read a bios at %s!\n", _path.c_str());
		rf.close();
		mHasBios = false;
		return false;
	}
	rf.close();

	printf("Bios %s loaded\n", _path.c_str());
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

bool gbBootRom::IsBiosLoaded() const
{
	return mLoaded && mHasBios;
}
