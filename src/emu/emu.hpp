#pragma once 
#include <array>
#include "base.hpp"

#define CPU_CLOCKSPEED 1048576
#define FLAG_Z 7 // Zero Flag, set if previous operation resulted in a zero
#define FLAG_N 6 // Substract Flag, set if previous operation substracted otherwise 0
#define FLAG_H 5 // Half Carry Flag, 
#define FLAG_C 4 // Carry Flag

#define INTERUPT_VBLANK 0U
#define INTERUPT_LCD    1U
#define INTERUPT_TIMER  2U
#define INTERUPT_SERIAL 3U
#define INTERUPT_JOYPAD 4U

// Include the emulator //

namespace Giffi
{

class GBEmu
{
public:
    GBEmu();
    ~GBEmu();

    void    Run    ();
    void    AdvanceClock();
    void    AdvanceFrame();

    uint8_t& ReadByte         (uint16_t _addr);
    uint16_t ReadWord         ();
    void     WriteByte        (uint16_t _addr, uint8_t _byte);
    void     PushWordOntoStack(uint16_t _word);
    uint16_t PopWordOffStack  ();

    void    UpdateTimers(uint16_t _cycles);

    void    UpdateGraphics(uint16_t _cycles);
    void    SetLCDStatus(int& _retrace);
    void    RenderScanline();
    void	RenderBackground();
	void	RenderSprites	();

    void RequestInterupt(uint8_t _id);
    void DoInterupts();
    void ServiceInterupt(uint8_t _interupt);

    void    LoadRom       (const char* _file_path);
    void    Reset         ();

    void    ExecuteNextOpcode();
    void    ExecuteExtendedOpcode();
public:
    uint16_t last_unkown_opcode = 0x0000;
    uint8_t mLastInterupt = 0x00;
    uint8_t mLastOpcode = 0x00;
    bool mHaltBug = false;

    // Cpu //
    Reg16 mRegAF, mRegBC, mRegDE, mRegHL;
    Reg16 mRegPC; // ProgramCounter
    Reg16 mRegSP; // StackPointer
    unsigned int mCyclesDone = 0;
    unsigned int mDividerCounter = 0;
    unsigned int mCounterFreq = 0; // How many cycles does it take to count the counter once.
    unsigned int mTimerCounter = 0; // Counts the timer cycles, till count up.
    bool mIsHalted = false;
    bool mEnableInterrupts = true;

    // Gpu //
    Image mCanvasBuffer;

    // Rom //
    uint8_t mCart[0x200000]; 
    uint8_t mRom[0x10000];
    uint16_t mCurRomBank = 1;
    bool mMBC1 = false;
    bool mMBC2 = false;

    // Ram //
    uint8_t mRam[0x8000];
    uint8_t mCurRamBank = 0;
};



}

