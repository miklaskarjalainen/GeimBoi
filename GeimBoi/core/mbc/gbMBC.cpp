#pragma once
#include <filesystem>
#include <fstream>
#include "gbMBC.hpp"
#include "gbMBC1.hpp"
#include "gbMBC2.hpp"
#include "gbMBC3.hpp"
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
        case gbCartType::None:
            return new gbMBC(_cart);
		case gbCartType::MBC1:
			return new gbMBC1(_cart);
		case gbCartType::MBC2:
			return new gbMBC2(_cart);
		case gbCartType::MBC3:
			return new gbMBC3(_cart);
		default:
            printf("Unsopprted mbc type!\n");
            return new gbMBC(_cart);
	}
}

bool gbMBC::SaveRam(const std::string& _path, uint8_t* src, size_t size)
{
    // Create Roms Folder
    if (!std::filesystem::exists("saves"))
    {
        if (!std::filesystem::is_directory("saves"))
        {
            std::filesystem::remove("saves");
        }
        std::filesystem::create_directory("saves");
    }

    std::string new_path = "saves/" + _path;
    std::ofstream wf(new_path, std::ios::binary);
    wf.write((char*)src, size);
    if (wf.bad())
    {
        printf("An error occurred when trying to create a savefile at %s!\n", new_path.c_str());
        wf.close();
        return false;
    }
    wf.close();
    printf("Savefile created to %s\n", new_path.c_str());
    return true;
}

bool gbMBC::LoadRam(const std::string& _path, uint8_t* dst, size_t size)
{
    std::string new_path = "saves/" + _path;

    if (!std::filesystem::exists(new_path))
    {
        printf("No savefile at %s\n", new_path.c_str());
        return false;
    }

    std::ifstream rf(new_path, std::ios::binary);
    rf.read((char*)dst, size);
    if (rf.bad())
    {
        printf("An error occurred when trying to read a savefile at %s!\n", new_path.c_str());
        rf.close();
        return false;
    }
    rf.close();

    printf("Savefile %s loaded\n", new_path.c_str());
    return true;
}
