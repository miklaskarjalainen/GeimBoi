#pragma once
#include "reg.hpp"

namespace Giffi
{

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

class gbGameBoy;
class gbPPU;

class gbZ80
{
public:
    gbZ80(gbGameBoy* _gameboy)
        : mGameBoy(_gameboy) { Reset(); }
    ~gbZ80() = default;

    void FrameAdvance();
    void Clock();
    void Reset();

    // Only public because of opcodes
    uint8_t  ReadByte(uint16_t _addr);
    void  WriteByte(uint16_t _addr, uint8_t _data);
    uint16_t ReadWord();
    void     PushWordOntoStack(uint16_t _word);
    uint16_t PopWordOffStack  ();

private:
    void ExecuteNextOpcode();
    void ExecuteExtendedOpcode();


    void UpdateTimers( uint16_t cycles );
    void RequestInterupt(uint8_t _id);
    void DoInterupts();
    void ServiceInterupt(uint8_t _interupt);

public:
    Reg16 mRegAF, mRegBC, mRegDE, mRegHL, mRegSP, mRegPC;
    bool mIsHalted = false;
    bool mEnableInterrupts = true;
    unsigned int mCounterFreq    = 0; // How many cycles does it take to count the counter once.
    unsigned int mDividerCounter = 0; // Counts the diviertimer cycles, till count up.
    unsigned int mTimerCounter   = 0; // Counts the timer cycles, till count up.
    unsigned int mCyclesDone = 0;

private:
    gbGameBoy* mGameBoy = nullptr;

    friend gbGameBoy;
    friend gbPPU;
};

} // Namespace