#include <boost/filesystem.hpp>
#include <fstream>
#include "gbMBC.hpp"
#include "gbMBC1.hpp"
#include "gbMBC2.hpp"
#include "gbMBC3.hpp"
#include "../gbCart.hpp"

using namespace GeimBoi;

uint8_t gbMBC::ReadByte(uint16_t addr) const
{
	return mCart->mCart[addr];
}

void gbMBC::WriteByte(uint16_t addr, uint8_t data)
{
	// Don't write to ROM lmfao
}

std::unique_ptr<gbMBC> gbMBC::CreateMBC(gbCart* cart)
{
    gbMBC* ptr = nullptr;
	switch (cart->GetCartType())
	{
        case gbCartType::None:
            ptr = new gbMBC(cart);
            break;
		case gbCartType::MBC1:
			ptr = new gbMBC1(cart);
            break;
		case gbCartType::MBC2:
			ptr = new gbMBC2(cart);
            break;
		case gbCartType::MBC3:
			ptr = new gbMBC3(cart);
            break;
		default:
            printf("Unsopprted mbc type!\n");
            ptr = new gbMBC(cart);
            break;
	}
    return std::move( std::unique_ptr<gbMBC>(ptr) );
}


bool gbMBC::SaveBatteryImpl(const std::string& path, uint8_t* src, size_t size)
{
    std::ofstream wf(path, std::ios::binary);
    wf.write((char*)src, size);
    if (wf.bad())
    {
        printf("An error occurred when trying to create a savefile at %s!\n", path.c_str());
        wf.close();
        return false;
    }
    wf.close();
    printf("Savefile created to %s\n", path.c_str());
    return true;
}

bool gbMBC::LoadBatteryImpl(const std::string& path, uint8_t* dst, size_t size)
{
    if (!boost::filesystem::is_regular_file(path))
    {
        printf("No savefile at %s\n", path.c_str());
        return false;
    }

    std::ifstream rf(path, std::ios::binary);
    rf.read((char*)dst, size);
    if (rf.bad())
    {
        printf("An error occurred when trying to read a savefile at %s!\n", path.c_str());
        rf.close();
        return false;
    }
    rf.close();

    printf("Savefile %s loaded\n", path.c_str());
    return true;
}
