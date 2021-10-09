#include <SDL2/SDL.h>
#include <utils/Benchmark.hpp>
#include "appGui.hpp"
#include "appSettings.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_sdl.h"
#include "utils/FileDialogs.hpp"

using namespace Giffi;

std::shared_ptr<gbGameBoy> appGui::mGameBoy = nullptr;
bool appGui::mDrawDebug = false;
bool appGui::mEmuPaused = false;

void appGui::Init(SDL_Renderer* _renderer, std::shared_ptr<gbGameBoy>& _emu, int _widht, int _height) {
    PROFILE_FUNCTION();

    appGui::mGameBoy = _emu; // Pointer to emulator core
    ImGui::CreateContext();
    ImGuiSDL::Initialize(_renderer, _widht, _height);
}

void appGui::Update()
{
    PROFILE_FUNCTION();
    ImGui::NewFrame();
    UpdateTopbar();
    UpdateDebug();
}

void appGui::Draw()
{
    PROFILE_FUNCTION();

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());
}

bool appGui::IsPaused()
{
    return mEmuPaused;
}

void appGui::UpdateTopbar()
{
    PROFILE_FUNCTION();
    ImGuiIO& io = ImGui::GetIO();
    
    ImGui::Begin("NavBar", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    ImGui::SetWindowPos(ImVec2(-2, 0));
    ImGui::SetWindowSize(ImVec2(io.DisplaySize.x + 4, 0));
    
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("GeimBoy"))
        {
            if (ImGui::MenuItem("Open Rom"))
            { 
                OpenRomDialog();
            }
            if (ImGui::MenuItem("Reset")) { mGameBoy->Reset(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {
            ImGui::Checkbox("Pause", &mEmuPaused);
            ImGui::Checkbox("Show Debug", &mDrawDebug);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
            ImGui::Text("GeimBoy DEV");
            ImGui::Text("Developed by Giffi");
            ImGui::Text("Uses: SDL2 & ImGui");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

void appGui::OpenRomDialog()
{
    // File open
    std::string path = appSettings::GetLastRomPath();
    auto f = pfd::open_file("Opem rom", path,
                            { "GB(C) Roms", "*.gb *.gbc",
                            "All Files", "*" },
                            pfd::opt::none);
    
    if (f.result().size() == 1)
    {
        // Load
        mGameBoy->LoadRom(f.result()[0]);
        if (mGameBoy->mCart.IsGameLoaded())
        {
            const std::string file_name = mGameBoy->mCart.GetGameName() + ".sav";
            mGameBoy->mCart.LoadBattery(file_name);
            appSettings::SetLastRomPath(f.result()[0]);
        }
    }
}

void appGui::UpdateDebug()
{
    PROFILE_FUNCTION();
    if (!mDrawDebug) { return; }

    static const gbZ80* sCpu   = &mGameBoy->mCpu;
    static const gbCart* sCart = &mGameBoy->mCart;
    // Cpu
    {
        // Window
        ImGui::Begin("CPU", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(180, 388));

        // Registers
        ImGui::Text("Registers:");
        ImGui::Text("A:0x%02X F:0x%02X", sCpu->mRegAF.high, sCpu->mRegAF.low);
        ImGui::Text("B:0x%02X C:0x%02X", sCpu->mRegBC.high, sCpu->mRegBC.low);
        ImGui::Text("D:0x%02X E:0x%02X", sCpu->mRegDE.high, sCpu->mRegDE.low);
        ImGui::Text("H:0x%02X L:0x%02X", sCpu->mRegHL.high, sCpu->mRegHL.low);
        ImGui::Text("PC: 0x%04X", sCpu->mRegPC.val);
        ImGui::Text("SP: 0x%04X", sCpu->mRegSP.val);
        ImGui::NewLine();
        // Flags
        ImGui::Text("Flags:");
        ImGui::Text("Carry: %u HalfCarry: %u", sCpu->GetFlag(gbFlag::Carry), sCpu->GetFlag(gbFlag::HalfCarry));
        ImGui::Text("Zero:  %u Substract: %u", sCpu->GetFlag(gbFlag::Zero),  sCpu->GetFlag(gbFlag::Substract));
        ImGui::NewLine();
        // Interrupt flag
        ImGui::Text("Interrupt Flag:");
        ImGui::Text("Vblank: %u LCD:    %u", sCpu->GetIF(gbInterrupt::VBlank), sCpu->GetIF(gbInterrupt::LCD));
        ImGui::Text("Timer:  %u Joypad: %u", sCpu->GetIF(gbInterrupt::Timer),  sCpu->GetIF(gbInterrupt::Joypad));
        ImGui::NewLine();
        // Interrupt enable
        ImGui::Text("Interrupt Enable:");
        ImGui::Text("Vblank: %u LCD:    %u", sCpu->GetIE(gbInterrupt::VBlank), sCpu->GetIE(gbInterrupt::LCD));
        ImGui::Text("Timer:  %u Joypad: %u", sCpu->GetIE(gbInterrupt::Timer),  sCpu->GetIE(gbInterrupt::Joypad));
        ImGui::NewLine();
        // Other
        ImGui::Text("IME: %u HALT: %u", sCpu->GetIME(), sCpu->IsHalted());

        ImGui::End();
    }

    // Cartridge
    {
        // Window
        ImGui::Begin("Cartridge", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(160, 205));
        ImGui::Text("Game: %s", sCart->GetGameName().c_str());
        ImGui::Text("Version: %u", sCart->GetGameVersion());
        ImGui::Text("Type: %s", sCart->GetCartTypeText().c_str());
        ImGui::Text("RomBanks: %u", sCart->GetRomBankCount());
        ImGui::Text("Ram: %uKb", sCart->GetRamSize());
        ImGui::NewLine();
        
        ImGui::Text("RomBankCount: %u", sCart->GetRomBankCount());
        ImGui::Text("CurRomBank: %u", sCart->GetCurRomBank());
        ImGui::Text("RamBankCount: %u", sCart->GetRamBankCount());
        ImGui::Text("CurRamBank: %u", sCart->GetCurRamBank()); 
        ImGui::End();
    }

    // PPU
    {
        // Window
        ImGui::Begin("PPU", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(120, 120));
        ImGui::Text("Enabled: %u", (mGameBoy->ReadByte(0xFF40) >> 7) & 1);
        ImGui::Text("LY:  %u", mGameBoy->ReadByte(0xFF44));
        ImGui::Text("LYC: %u", mGameBoy->ReadByte(0xFF45));
        ImGui::Text("SCY: %u", mGameBoy->ReadByte(0xFF42));
        ImGui::Text("SCX: %u", mGameBoy->ReadByte(0xFF43));
        ImGui::End();
    }

    // Controlling
    {
        // Window
        ImGui::Begin("Control", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(140, 136));

        ImGui::Text("LastOp: 0x%02X", sCpu->mLastExecutedOpcode);
        ImGui::Text("NextOp: 0x%02X", sCpu->ReadByte(sCpu->mRegPC.val));

        if (ImGui::Button("FrameAdvance")) { mGameBoy->FrameAdvance(); mEmuPaused = true; }
        if (ImGui::Button("ExecuteOpcode")) { mGameBoy->Clock(); mEmuPaused = true; }
        ImGui::ProgressBar((float)sCpu->mCyclesDone / 70221.0f);
        
        ImGui::End();
    }
}
