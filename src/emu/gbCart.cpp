#include "gbCart.hpp"
#include <cstdio>   // fopen
#include <string.h>

namespace Giffi
{   
    std::string gbCart::GetGameName()
    {
        char name[11] = "";
        memcpy( &name, mCart + 0x134, sizeof(char) * 11 );
        return std::string(name);
    }

    void gbCart::Reset()
    {

    }

    void gbCart::LoadRom(std::string _path)
    {
        // Init Cart
        memset(mCart,0,sizeof(mCart));
        FILE *in;
        in = fopen( _path.c_str(), "rb" );
        fread(mCart, 1, 0x200000, in);
        fclose(in); 
        mCurRomBank = 1;
        
        // Init Ram
        // memset( &mRam,0,sizeof(mRam) );
        mCurRamBank = 0;

        switch (mCart[0x147])
        {
            case 1: // MBC1
            case 2: // MBC1+RAM
            case 3: // MBC1+RAM+BATTERY
                mMBC = CartType::MBC1;
                break;
            
            case 4: // MBC2
            case 5: // MBC2+BATTERY
                mMBC = CartType::MBC2;
                break;
            
            default: // None or unsupported
                mMBC = CartType::None;
                break;
        } 
    }

    uint8_t& gbCart::ReadByte(uint16_t _addr)
    {
        // are we reading from the rom memory bank?
        if ((_addr>=0x4000) && (_addr < 0x8000))
        {
            uint16_t new_addr = (_addr - 0x4000) + (0x4000 * mCurRomBank);
            return mCart[new_addr];
        }
        return mCart[_addr];
    }

    void gbCart::WriteByte(uint16_t _addr, uint8_t _data)
    {
        if ( _addr >= 0x2000 && _addr < 0x4000) // ROM Change
        {
            if (_data == 0x00) {mCurRomBank = 0x01; return;}
            if (_data == 0x20) {mCurRomBank = 0x21; return;}
            if (_data == 0x40) {mCurRomBank = 0x41; return;}
            if (_data == 0x60) {mCurRomBank = 0x61; return;}
            mCurRomBank = (_data & 0x1F);
        }
    }


}
