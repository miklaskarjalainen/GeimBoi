#pragma once
#include "reg.hpp"

namespace Giffi
{

class gbGameBoy;
class gbPPU;

// Not enum class because used in bitshifts, a lot.
enum gbFlag : uint8_t
{
    Zero      = 7, // Set if previous operation resulted in a zero
    Substract = 6, // Set if previous operation substracted otherwise 0
    HalfCarry = 5, // If there was a carry from lower nibble to higher nibble
    Carry     = 4, // If there was carry
};

enum class gbInterrupt : uint8_t
{
    VBlank = 0,
    LCD    = 1,
    Timer  = 2,
    Serial = 3,
    Joypad = 4,
};

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
    uint8_t  ReadByte(uint16_t _addr) const;
    void     WriteByte(uint16_t _addr, uint8_t _data);
    uint16_t ReadWord() const;
    void     PushWordOntoStack(uint16_t _word);
    uint16_t PopWordOffStack  ();

    bool GetFlag(gbFlag flag)   const { return (mRegAF.low >> flag) & 1; }
    bool GetIE(gbInterrupt i)   const { return (ReadByte(0xFFFF) >> (int)i) & 1; } // Get 'Interrupt Enable'
    bool GetIF(gbInterrupt i)   const { return (ReadByte(0xFF0F) >> (int)i) & 1; } // Get 'Interrupt Flag'
    bool GetIME()               const { return mEnableInterrupts; }                // 'Interrupt Master Enable'
    bool IsHalted()             const { return mIsHalted; }
private:
    void ExecuteNextOpcode();
    void ExecuteExtendedOpcode();

    void UpdateTimers(uint16_t cycles);
    void RequestInterrupt(gbInterrupt _interrupt);
    void DoInterrupts();
    void ServiceInterrupt(gbInterrupt _interupt);

public:
    Reg16 mRegAF, mRegBC, mRegDE, mRegHL, mRegSP, mRegPC;
    bool mIsHalted = false;
    bool mEnableInterrupts = true;
    unsigned int mCounterFreq    = 0; // How many cycles does it take to count the counter once.
    unsigned int mDividerCounter = 0; // Counts the diviertimer cycles, till count up.
    unsigned int mTimerCounter   = 0; // Counts the timer cycles, till count up.
    unsigned int mCyclesDone = 0;

    // Debugging
    uint16_t mLastExecutedOpcode = 0x00;
private:
    gbGameBoy* mGameBoy = nullptr;

    friend gbGameBoy;
    friend gbPPU;
};

} // Namespace