#pragma once
#include "reg.hpp"

namespace GeimBoi
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
        uint8_t  ReadByte(uint16_t addr) const;
        uint16_t ReadWord() const;
        uint16_t ReadWord(uint16_t addr) const;

        inline bool GetFlag(gbFlag flag)   const { return (mRegAF.low >> flag) & 1; }
        inline bool GetIE(gbInterrupt interrupt)   const { return (ReadByte(0xFFFF) >> (int)interrupt) & 1; }
        inline bool GetIF(gbInterrupt interrupt)   const { return (ReadByte(0xFF0F) >> (int)interrupt) & 1; }
        inline bool GetIME()               const { return mEnableInterrupts; }                // 'Interrupt Master Enable'
        inline bool IsHalted()             const { return mIsHalted; }

    public:
        Reg16 mRegAF, mRegBC, mRegDE, mRegHL, mRegSP, mRegPC;
        bool mIsHalted = false;
        bool mEnableInterrupts = true;
        unsigned int mCounterFreq = 0; // How many cycles does it take to count the counter once.
        unsigned int mDividerCounter = 0; // Counts the diviertimer cycles, till count up.
        unsigned int mTimerCounter = 0; // Counts the timer cycles, till count up.
        unsigned int mCyclesDone = 0;

        // Debugging
        uint16_t mLastExecutedOpcode = 0x00;

    private:
        gbGameBoy* mGameBoy = nullptr;

        void UpdateTimers(uint16_t cycles);
        void RequestInterrupt(gbInterrupt interrupt);
        void DoInterrupts();
        void ServiceInterrupt(gbInterrupt interrupt);

        void ExecuteNextOpcode();
        void ExecuteExtendedOpcode();
        void     WriteByte(uint16_t addr, uint8_t data);
        void     PushWordOntoStack(uint16_t word);
        uint16_t PopWordOffStack();

        // Instructions
        void CPU8BitInc(uint8_t& reg); 
        void CPU8BitDec(uint8_t& reg); 
        void CPU8BitMemInc(uint16_t addr); 
        void CPU8BitMemDec(uint16_t addr); 
        void CPU8BitAdd(uint8_t& reg, uint8_t amount, bool addCarry = false); 
        void CPU8BitSub(uint8_t& reg, uint8_t amount, bool addCarry = false); 
        void CPU16BitAdd(uint16_t& reg, uint16_t value); 

        void CPU8BitCompare(uint8_t bits);
        void CPU8BitAnd(uint8_t mask);
        void CPU8BitXor    (uint8_t mask);
        void CPU8BitOr(uint8_t mask);

        void CPUJump         (bool doComprision, uint8_t flag, bool state);
        void CPUJumpImmediate(bool doComprision, uint8_t flag, bool state);
        void CPURet          (bool doComprision, uint8_t flag, bool state);
        void CPUCall         (bool doComprision, uint8_t flag, bool state);
        void CPURestart(uint8_t addr);
        void CPU_DAA();

        // CB instructions
        void CPUSwap(uint8_t& reg);
        void CPUGetBit(uint8_t reg, uint8_t bit);
        void CPUSetBit(uint8_t& reg, uint8_t bit);
        void CPUResetBit(uint8_t& reg, uint8_t bit);

        void CPU_RR(uint8_t& reg);
        void CPU_RL(uint8_t& reg);
        void CPU_SLA(uint8_t& reg);
        void CPU_SRA(uint8_t& reg);
        void CPU_RLC(uint8_t& reg);
        void CPU_SRL(uint8_t& reg);
        void CPU_RRC(uint8_t& reg);

        friend gbGameBoy;
        friend gbPPU;
    };

}