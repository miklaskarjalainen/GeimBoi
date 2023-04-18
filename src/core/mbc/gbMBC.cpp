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

void gbMBC::WriteByte(uint16_t, uint8_t)
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

bool gbMBC::SaveBattery(const std::string& path) {
    std::ofstream wf(path);
    return SaveBatteryImpl(wf);
}

bool gbMBC::LoadBattery(const std::string& path) {
    if (!boost::filesystem::is_regular_file(path))
    {
        printf("No savefile at %s\n", path.c_str());
        return false;
    }
    
    std::ifstream rf(path);
    return LoadBatteryImpl(rf);
}
