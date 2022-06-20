#include <sstream>
#include <SDL2/SDL.h>
#include <FileDialogs/FileDialogs.hpp>
#include "appGui.hpp"
#include "appSettings.hpp"
#include "gui/rebindButton.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_sdl.h"

using namespace GeimBoi;

appGui::appGui(SDL_Renderer* renderer, std::shared_ptr<gbGameBoy>& emulator, int widht, int height)
{
    mGameBoy = emulator;
    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, widht, height);

    // MAP KEYS
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
    io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
    io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
    io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
    io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
    io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
    io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
    io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
}

appGui::~appGui()
{
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
}

void appGui::Draw()
{
    ImGui::NewFrame();
    DrawTopbar();
    DrawControls();
    DrawDebug();
    DrawAuthors();
    DrawLicences();
    DrawInfo();
}

void appGui::Render()
{
    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());
}

bool appGui::IsPaused()
{
    return mEmuPaused;
}

void appGui::DrawTopbar()
{
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
            if (ImGui::MenuItem("Controls"))
                mDrawControls = !mDrawControls;
            ImGui::Checkbox("Pause", &mEmuPaused);
            ImGui::Checkbox("Show Debug", &mDrawDebug);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
            if (ImGui::MenuItem("Authors"))
                mDrawAuthors = !mDrawAuthors;
            if (ImGui::MenuItem("Licences"))
                mDrawLicences = !mDrawLicences;
            if (ImGui::MenuItem("Info"))
                mDrawInfo = !mDrawInfo;
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

void appGui::DrawControls()
{
    if (!mDrawControls)
        return;

    // Window
    ImGui::Begin("Controls", &mDrawControls, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(ImVec2(156, 208));

    static std::vector<rebindButton> buttons = {
        rebindButton("Up", appSettings::controls.up),
        rebindButton("Down", appSettings::controls.down),
        rebindButton("Right", appSettings::controls.right),
        rebindButton("Left", appSettings::controls.left),
        rebindButton("Start", appSettings::controls.start),
        rebindButton("Select", appSettings::controls.select),
        rebindButton("A", appSettings::controls.a),
        rebindButton("B", appSettings::controls.b),
    };

    for (auto& b : buttons)
        b.Draw();
    
    ImGui::End();
}


void appGui::DrawDebug()
{
    if (!mDrawDebug) 
        return;

    const gbZ80* cpu   = &mGameBoy->mCpu;
    const gbCart* cart = &mGameBoy->mCart;

    // Cpu
    {
        // Window
        ImGui::Begin("CPU", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(180, 388));

        // Registers
        ImGui::Text("Registers:");
        ImGui::Text("A:0x%02X F:0x%02X", cpu->mRegAF.high, cpu->mRegAF.low);
        ImGui::Text("B:0x%02X C:0x%02X", cpu->mRegBC.high, cpu->mRegBC.low);
        ImGui::Text("D:0x%02X E:0x%02X", cpu->mRegDE.high, cpu->mRegDE.low);
        ImGui::Text("H:0x%02X L:0x%02X", cpu->mRegHL.high, cpu->mRegHL.low);
        ImGui::Text("PC: 0x%04X", cpu->mRegPC.val);
        ImGui::Text("SP: 0x%04X", cpu->mRegSP.val);
        ImGui::NewLine();
        // Flags
        ImGui::Text("Flags:");
        ImGui::Text("Carry: %u HalfCarry: %u", cpu->GetFlag(gbFlag::Carry), cpu->GetFlag(gbFlag::HalfCarry));
        ImGui::Text("Zero:  %u Substract: %u", cpu->GetFlag(gbFlag::Zero), cpu->GetFlag(gbFlag::Substract));
        ImGui::NewLine();
        // Interrupt flag
        ImGui::Text("Interrupt Flag:");
        ImGui::Text("Vblank: %u LCD:    %u", cpu->GetIF(gbInterrupt::VBlank), cpu->GetIF(gbInterrupt::LCD));
        ImGui::Text("Timer:  %u Joypad: %u", cpu->GetIF(gbInterrupt::Timer), cpu->GetIF(gbInterrupt::Joypad));
        ImGui::NewLine();
        // Interrupt enable
        ImGui::Text("Interrupt Enable:");
        ImGui::Text("Vblank: %u LCD:    %u", cpu->GetIE(gbInterrupt::VBlank), cpu->GetIE(gbInterrupt::LCD));
        ImGui::Text("Timer:  %u Joypad: %u", cpu->GetIE(gbInterrupt::Timer), cpu->GetIE(gbInterrupt::Joypad));
        ImGui::NewLine();
        // Other
        ImGui::Text("IME: %u HALT: %u", cpu->GetIME(), cpu->IsHalted());

        ImGui::End();
    }

    // Cartridge
    {
        // Window
        ImGui::Begin("Cartridge", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(160, 205));
        ImGui::Text("Game: %s", cart->GetGameName().c_str());
        ImGui::Text("Version: %u", cart->GetGameVersion());
        ImGui::Text("Type: %s", cart->GetCartTypeText().c_str());
        ImGui::Text("RomBanks: %u", cart->GetRomBankCount());
        ImGui::Text("Ram: %uKb", cart->GetRamSize());
        ImGui::NewLine();
        
        ImGui::Text("RomBankCount: %u", cart->GetRomBankCount());
        ImGui::Text("CurRomBank: %u", cart->GetCurRomBank());
        ImGui::Text("RamBankCount: %u", cart->GetRamBankCount());
        ImGui::Text("CurRamBank: %u", cart->GetCurRamBank());
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


        ImGui::Text("LastOp: %s", GetAssembly(cpu->mLastExecutedOpcode));
        ImGui::Text("NextOp: %s", GetAssembly(cpu->ReadByte(cpu->mRegPC.val)));

        if (ImGui::Button("FrameAdvance")) { mGameBoy->FrameAdvance(); mEmuPaused = true; }
        if (ImGui::Button("ExecuteOpcode")) { mGameBoy->Clock(); mEmuPaused = true; }
        ImGui::ProgressBar((float)cpu->mCyclesDone / 70221.0f);
        
        ImGui::End();
    }

    // Debugger
    // TODO: add breakpoints
    {
        // Window
        ImGui::Begin("Debugger", nullptr, ImGuiWindowFlags_None);

        for (int16_t offset = 0; offset < 256;) {
            const uint16_t addr = cpu->mRegPC.val + offset;
            if (addr < 0)
                continue;

            std::stringstream assembly;
            uint16_t opcode = cpu->ReadWord(addr);
            assembly << GetAssembly(opcode) << std::hex;

            // prints arguments for the assembly
            for (uint8_t i = 0; i < GetLength(opcode) - 1; i++) {
                assembly << " [0x" << (unsigned int)cpu->ReadByte(addr + i) << "]";
            }

            if (offset == 0)
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 255, 255));
            ImGui::Text("0x%04X: %s", addr, assembly.str().c_str());
            if (offset == 0)
                ImGui::PopStyleColor();

            offset += GetLength(opcode);
        }
        ImGui::End();
    }
}

void appGui::DrawAuthors()
{
    if (!mDrawAuthors)
        return;

    ImGui::Begin("Authors", &mDrawAuthors, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(ImVec2(260, 120));

    ImGui::Text("Developers");
    ImGui::Separator();
    ImGui::Text("Miklas Karjalainen (giffi-dev)");
    
    ImGui::End();
}

void appGui::DrawLicences()
{
    if (!mDrawLicences)
        return;

    ImGui::Begin("Licences", &mDrawLicences, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(ImVec2(480, 169));

    if (ImGui::CollapsingHeader("GeimBoi"))
    {
        ImGui::TextWrapped("MIT Licence\n\n"

        "Copyright (c) 2021-2022 Miklas Karjalainen\n\n"

        "Permission is hereby granted, free of charge, to any person obtaining a copy"
        "of this software and associated documentation files (the \"Software\"), to deal"
        "in the Software without restriction, including without limitation the rights"
        "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell"
        "copies of the Software, and to permit persons to whom the Software is"
        "furnished to do so, subject to the following conditions:\n\n"

        "The above copyright notice and this permission notice shall be included in all"
        "copies or substantial portions of the Software.\n\n"

        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE"
        "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
        "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
        "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
    }
    if (ImGui::CollapsingHeader("SDL2"))
    {
        ImGui::TextWrapped("This software is provided 'as-is', without any express or implied"
        "warranty.  In no event will the authors be held liable for any damages"
        "arising from the use of this software.\n\n"

        "Permission is granted to anyone to use this software for any purpose,"
        "including commercial applications, and to alter it and redistribute it"
        "freely, subject to the following restrictions:\n\n"

        "1. The origin of this software must not be misrepresented; you must not"
        "claim that you wrote the original software. If you use this software"
        "in a product, an acknowledgment in the product documentation would be"
        "appreciated but is not required.\n"
        "2. Altered source versions must be plainly marked as such, and must not be"
        "misrepresented as being the original software.\n"
        "3. This notice may not be removed or altered from any source distribution.");
    }
    if (ImGui::CollapsingHeader("Dear ImGui"))
    {
        ImGui::TextWrapped("The MIT License (MIT)\n\n"

        "Copyright (c) 2014-2022 Omar Cornut\n\n"

        "Permission is hereby granted, free of charge, to any person obtaining a copy"
        "of this software and associated documentation files (the \"Software\"), to deal"
        "in the Software without restriction, including without limitation the rights"
        "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell"
        "copies of the Software, and to permit persons to whom the Software is"
        "furnished to do so, subject to the following conditions:\n\n"

        "The above copyright notice and this permission notice shall be included in all"
        "copies or substantial portions of the Software.\n\n"

        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE"
        "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
        "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
        "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
    }
    if (ImGui::CollapsingHeader("imgui_sdl"))
    {
        ImGui::TextWrapped("MIT License\n"
        "Copyright (c) 2018\n\n"

        "Permission is hereby granted, free of charge, to any person obtaining a copy"
        "of this software and associated documentation files (the \"Software\"), to deal"
        "in the Software without restriction, including without limitation the rights"
        "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell"
        "copies of the Software, and to permit persons to whom the Software is"
        "furnished to do so, subject to the following conditions:\n\n"

        "The above copyright notice and this permission notice shall be included in all"
        "copies or substantial portions of the Software.\n\n"

        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
        "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE"
        "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
        "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
        "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
    }
    if (ImGui::CollapsingHeader("Portable File Dialogs"))
    {
        ImGui::TextWrapped("Copyright © 2018 - 2020 Sam Hocevar <sam@hocevar.net>\n\n"

        "This library is free software. It comes without any warranty, to"
        "the extent permitted by applicable law. You can redistribute it"
        "and/or modify it under the terms of the Do What the Fuck You Want"
        "to Public License, Version 2, as published by the WTFPL Task Force."
        "See http://www.wtfpl.net/ for more details.");
    }
    if (ImGui::CollapsingHeader("Boost C++ Libraries"))
    {
        ImGui::TextWrapped("Boost Software License - Version 1.0 - August 17th, 2003\n\n"

        "Permission is hereby granted, free of charge, to any person or organization"
        "obtaining a copy of the software and accompanying documentation covered by"
        "this license (the \"Software\") to use, reproduce, display, distribute,"
        "execute, and transmit the Software, and to prepare derivative works of the"
        "Software, and to permit third-parties to whom the Software is furnished to"
        "do so, all subject to the following:\n\n"

        "The copyright notices in the Software and this entire statement, including"
        "the above license grant, this restriction and the following disclaimer,"
        "must be included in all copies of the Software, in whole or in part, and"
        "all derivative works of the Software, unless such copies or derivative"
        "works are solely in the form of machine-executable object code generated by"
        "a source language processor.\n\n"

        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
        "FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT"
        "SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE"
        "FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,"
        "ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER"
        "DEALINGS IN THE SOFTWARE.");
    }

    ImGui::End();   
}

void appGui::DrawInfo()
{
    if (!mDrawInfo)
        return;
    
    ImGui::Begin("Info", &mDrawInfo, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(ImVec2(190, 85));
    ImGui::TextWrapped("GeimBoi is developed and maintained by giffi-dev");
    ImGui::Text("Licence: %s", "MIT");
    ImGui::End();   
}

void appGui::OpenRomDialog()
{
    // File open
    std::string path = appSettings::lastrom_path;
    auto f = pfd::open_file("Open rom", path,
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
            appSettings::lastrom_path = f.result()[0];
        }
    }
}
