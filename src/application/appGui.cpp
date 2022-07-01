#include <string>
#include <sstream>
#include <cassert>
#include <SDL2/SDL.h>
#include <FileDialogs/FileDialogs.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"
#include "appGui.hpp"
#include "appSettings.hpp"
#include "gui/rebindButton.hpp"
#include "../version.hpp"

using namespace GeimBoi;
 
static SDL_Window* sWindow = nullptr;
std::shared_ptr<gbGameBoy> gGameBoy = nullptr;

appGui::appGui(SDL_Window* window, void* context, std::shared_ptr<gbGameBoy>& emulator)
{
    sWindow = window;
    mGameBoy = emulator;
    gGameBoy = mGameBoy;
    ImGui::CreateContext();

    // Color scheme
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_MenuBarBg]     = ImColor(23, 115, 5);
    style.Colors[ImGuiCol_TitleBg]       = ImColor(98, 187, 17);
    style.Colors[ImGuiCol_TitleBgActive] = ImColor(98, 187, 17);
    style.Colors[ImGuiCol_Border]        = ImColor(110, 213, 19);
    style.Colors[ImGuiCol_WindowBg]      = ImColor(23, 115, 5);
    style.Colors[ImGuiCol_PopupBg]       = ImColor(23, 115, 5);
    style.Colors[ImGuiCol_CheckMark]     = ImColor(0, 107, 145);
         
    style.Colors[ImGuiCol_Header]        = ImColor(0, 80, 0);
    style.Colors[ImGuiCol_HeaderHovered] = ImColor(98, 187, 17);
    style.Colors[ImGuiCol_HeaderActive]  = ImColor(0, 107, 145);
    style.Colors[ImGuiCol_Separator]     = ImColor(98, 187, 17);
         
    style.Colors[ImGuiCol_FrameBg]         = ImColor(0, 60, 0);
    style.Colors[ImGuiCol_FrameBgHovered]  = ImColor(0, 80, 0);
    style.Colors[ImGuiCol_FrameBgActive]   = ImColor(0, 100, 0);
         
    style.Colors[ImGuiCol_Button]        = ImColor(0, 80, 0); 
    style.Colors[ImGuiCol_ButtonHovered] = ImColor(98, 187, 17);
    style.Colors[ImGuiCol_ButtonActive]  = ImColor(0, 107, 145);
         
    style.Colors[ImGuiCol_SliderGrab]      = ImColor(98, 187, 17);
    style.Colors[ImGuiCol_SliderGrabActive]= ImColor(0, 107, 145);

    style.Colors[ImGuiCol_ScrollbarBg]    = ImColor(98, 187, 17);
    style.Colors[ImGuiCol_ScrollbarGrab]  = ImColor(0, 100, 0);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(98, 187, 17);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImColor(0, 107, 145);

    // Config
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL2_Init();

    // Init lua script
#ifdef LUA_SCRIPTING
    ReloadScripts();
#endif
}

appGui::~appGui()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void appGui::Draw()
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    DrawTopbar();
    DrawOptions();
    DrawDebug();
    DrawAuthors();
    DrawLicences();
    DrawInfo();

#ifdef LUA_SCRIPTING
    DrawScripts();
    for (size_t i = 0; i < mLuaScripts.size(); i++)
    {
        mLuaScripts[i].Update();
    }
#endif
}

void appGui::Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void appGui::DrawTopbar()
{
    ImGuiIO& io = ImGui::GetIO();
    
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::Begin("NavBar", 0,
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoMove  |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoSavedSettings
    );
    
    int x, y;
    SDL_GetWindowPosition(sWindow, &x, &y);
    ImGui::SetWindowPos(ImVec2(x-2, y));
    ImGui::SetWindowSize(ImVec2(io.DisplaySize.x + 4, 1));

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("GeimBoi"))
        {
            if (ImGui::MenuItem("Open Rom..."))
            { 
                OpenRomDialog();
            }
            if (ImGui::MenuItem("Reset")) { mGameBoy->Reset(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {
            if (ImGui::MenuItem("Options"))
                mDrawOptions = !mDrawOptions;
        #ifdef LUA_SCRIPTING
            if (ImGui::MenuItem("Scripts"))
                mDrawScripts = !mDrawScripts;
        #endif
            ImGui::Checkbox("Pause", &mGameBoy->Paused);
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

void appGui::DrawOptions()
{
    if (!mDrawOptions)
        return;

    ImGui::Begin("Options", &mDrawOptions, 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoDocking  |
        ImGuiWindowFlags_NoResize   |
        ImGuiWindowFlags_NoSavedSettings
    );
    ImGui::SetWindowSize({ 618, 160 });

    const ImGuiStyle* style = &ImGui::GetStyle();

    ImGui::Columns(2, nullptr, false);
    enum class Tab
    {
        Binds,
        Audio,
        Palette,
        COUNT
    };
    static Tab current_tab = Tab::Binds;
    // Tabs
    {
        const int ColumnWidth = 140;
        ImGui::SetColumnOffset(1, ColumnWidth);
        const ImVec2 ButtonSize = { ColumnWidth - (style->WindowPadding.x * 2), 32 };

        static const ImColor ActivatedColor = style->Colors[ImGuiCol_ButtonHovered];
        static const char* Buttons[] = {"Binds", "Audio", "Palette"};
        for (int i = 0; i < static_cast<int>(Tab::COUNT); i++)
        {
            const Tab TabIdx = static_cast<Tab>(i);
            
            ImGui::Spacing();
            // If this button is the same our current selection draw it with a different color
            if (current_tab == TabIdx)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ActivatedColor.Value);
                ImGui::Button(Buttons[i], ButtonSize);
                ImGui::PopStyleColor();
                continue;
            }
            if (ImGui::Button(Buttons[i], ButtonSize))
                current_tab = TabIdx;
        }

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 8);
    }

    ImGui::NextColumn();

    // Contents
    {
        switch (current_tab)
        {
        case Tab::Binds:
        {
            static std::vector<rebindButton> buttons = {
            /* 0 */ rebindButton("Up", appSettings::controls.up),
            /* 1 */ rebindButton("Down", appSettings::controls.down),
            /* 2 */ rebindButton("Left", appSettings::controls.left),
            /* 3 */ rebindButton("Right", appSettings::controls.right),
            /* 4 */ rebindButton("Select", appSettings::controls.select),
            /* 5 */ rebindButton("Start", appSettings::controls.start),
            /* 6 */ rebindButton("B", appSettings::controls.b),
            /* 7 */ rebindButton("A", appSettings::controls.a),
            };

            ImGui::NewLine();
            const float CursorStartX = ImGui::GetCursorPosX();

            // Draw Dpad
            const ImVec2 ButtonSize = { 96, 28 };
            ImGui::SetCursorPosX(CursorStartX + (ButtonSize.x / 2));
            buttons[0].Draw(ButtonSize);
            ImGui::SetCursorPosX(CursorStartX);
            const float CursorPosMiddleY = ImGui::GetCursorPosY();
            buttons[2].Draw(ButtonSize);
            ImGui::SameLine();
            buttons[3].Draw(ButtonSize);
            ImGui::SetCursorPosX(CursorStartX + (ButtonSize.x / 2));
            const float CursorPosLowY = ImGui::GetCursorPosY();
            buttons[1].Draw(ButtonSize);

            // Draw Start & Select
            const ImVec2 WideButtonSize = { 148, 28 };
            ImGui::SetCursorPosY(CursorPosLowY);
            ImGui::SetCursorPosX(CursorStartX + (ButtonSize.x * 1.5f) + (style->FramePadding.x * 2));
            buttons[4].Draw(WideButtonSize);
            ImGui::SameLine();
            buttons[5].Draw(WideButtonSize);

            // Draw A & B
            const ImVec2 ActionButtonSize = { 64, 28 };
            ImGui::SetCursorPosY(CursorPosMiddleY);
            ImGui::SetCursorPosX(CursorStartX + (ButtonSize.x * 3.33f));
            buttons[6].Draw(ActionButtonSize);
            ImGui::SameLine();
            buttons[7].Draw(ActionButtonSize);
            break;
        }
        case Tab::Audio:
        {
            ImGui::Text("Volume: ");
            ImGui::SameLine();
            if (ImGui::SliderFloat("##", &appSettings::master_volume, 0.0f, 1.0f, "%.1f", ImGuiSliderFlags_NoRoundToFormat))
            {
                mGameBoy->mApu.masterVolume = appSettings::master_volume;
            }
            break;
        }
        case Tab::Palette:
        {
            static ImColor c1 = ImColor(0xff0fbc9b);
            static ImColor c2 = ImColor(0xff0fac8b);
            static ImColor c3 = ImColor(0xff306230);
            static ImColor c4 = ImColor(0xff0f380f);
            auto update_palette = [&]()
            {
                gbColor color1, color2, color3, color4;
                color1.r = c1.Value.x * 255;
                color1.g = c1.Value.y * 255;
                color1.b = c1.Value.z * 255;

                color2.r = c2.Value.x * 255;
                color2.g = c2.Value.y * 255;
                color2.b = c2.Value.z * 255;
                
                color3.r = c3.Value.x * 255;
                color3.g = c3.Value.y * 255;
                color3.b = c3.Value.z * 255;
                
                color4.r = c4.Value.x * 255;
                color4.g = c4.Value.y * 255;
                color4.b = c4.Value.z * 255;
                mGameBoy->SetPalette(color1, color2, color3, color4);
            };
            
            if (ImGui::ColorEdit3("1", reinterpret_cast<float*>(&c1), ImGuiColorEditFlags_Uint8))
                update_palette();
            if (ImGui::ColorEdit3("2", reinterpret_cast<float*>(&c2), ImGuiColorEditFlags_Uint8))
                update_palette();
            if (ImGui::ColorEdit3("3", reinterpret_cast<float*>(&c3), ImGuiColorEditFlags_Uint8))
                update_palette();
            if (ImGui::ColorEdit3("4", reinterpret_cast<float*>(&c4), ImGuiColorEditFlags_Uint8))
                update_palette();
            

            if (ImGui::Button("Reset", { 48, 24 }))
            {
                c1 = ImColor(0xff0fbc9b);
                c2 = ImColor(0xff0fac8b);
                c3 = ImColor(0xff306230);
                c4 = ImColor(0xff0f380f);
                update_palette();
            }
            break;
        }
        default:
            assert(false);
        }
    }

    
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
        ImGui::Begin("CPU", nullptr, 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoDocking
        );
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
        ImGui::Begin("Cartridge", nullptr, 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoDocking
        );
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
        ImGui::Begin("PPU", nullptr, 
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoDocking
        );
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
        ImGui::Begin("Control", nullptr, 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoDocking
        );
        ImGui::SetWindowSize(ImVec2(140, 136));


        ImGui::Text("LastOp: %s", GetAssembly(cpu->mLastExecutedOpcode));
        ImGui::Text("NextOp: %s", GetAssembly(cpu->ReadByte(cpu->mRegPC.val)));

        if (ImGui::Button("FrameAdvance"))  { mGameBoy->FrameAdvance(); mGameBoy->Paused = true; }
        if (ImGui::Button("ExecuteOpcode")) { mGameBoy->Clock()       ; mGameBoy->Paused = true; }
        ImGui::ProgressBar((float)cpu->mCyclesDone / 70221.0f);
        
        ImGui::End();
    }

    // Debugger
    // TODO: add breakpoints
    {
        // Window
        ImGui::Begin("Debugger", nullptr, 
            ImGuiWindowFlags_None | 
            ImGuiWindowFlags_NoDocking
        );

        for (int16_t offset = 0; offset < 256;) {
            const uint16_t addr = cpu->mRegPC.val + offset;
            std::stringstream assembly;
            uint16_t opcode = cpu->ReadWord(addr);
            assembly << GetAssembly(opcode) << std::hex;

            // prints arguments for the assembly
            for (uint8_t i = 0; i < GetLength(opcode) - 1; i++) {
                assembly << " [0x" << (unsigned int)cpu->ReadByte(addr + i) << "]";
            }

            if (offset == 0)
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 4, 255));
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

    ImGui::Begin("Authors", &mDrawAuthors, 
        ImGuiWindowFlags_NoResize   | 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoDocking  |
        ImGuiWindowFlags_NoSavedSettings
    );
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

    ImGui::Begin("Licences", &mDrawLicences, 
        ImGuiWindowFlags_NoResize   | 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoDocking  |
        ImGuiWindowFlags_NoSavedSettings
    );
    ImGui::SetWindowSize(ImVec2(480, 146));

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
        ImGui::Text("Branch: [docking] 9764adc7bb7a582601dd4dd1c34d4fa17ab5c8e8");
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

    /*
        Get SDL version that's linked with the program.
        Windows builds use SDL2.dll for dynamic linking, so an user might change it to for a newer/older one.
        That's why using a function instead of SDL's preprocessor definitions which would give a compile time version.
    */ 
    static const auto GetSDLVersion = []() -> SDL_version
    {
        SDL_version v;
        SDL_GetVersion(&v);
        return v;
    };
    static const SDL_version SDLVersion = GetSDLVersion();

    ImGui::Begin("Info", &mDrawInfo, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDocking);
    ImGui::SetWindowSize(ImVec2(190, 132));
    ImGui::TextWrapped("GeimBoi is developed and maintained by giffi-dev");
    ImGui::NewLine();
    ImGui::Text("GeimBoi Ver: %s", GEIMBOI_VERSION);
    ImGui::Text("SDL Ver: %u.%u.%u", SDLVersion.major, SDLVersion.minor, SDLVersion.patch);
    ImGui::Text("ImGui Ver: %s", IMGUI_VERSION);
    ImGui::End();   
}

#ifdef LUA_SCRIPTING
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

void appGui::DrawScripts()
{
    if (!mDrawScripts)
        return;
    ImGui::Begin("Lua Scripts", &mDrawScripts,
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoDocking  |
        ImGuiWindowFlags_NoResize   |
        ImGuiWindowFlags_NoSavedSettings
    );
    ImGui::SetWindowSize(ImVec2(320, 420));

    for (const auto& i : mLuaScripts)
    {
        ImGui::Button(i.FilePath.c_str());
    }

    ImGui::End();
}

void appGui::ReloadScripts()
{
    mLuaScripts.clear();

    using namespace boost::filesystem;
    if (!is_directory("./lua"))
    {
        if (!create_directory("./lua/"))
        {
            printf("Couldn't create ./lua directory!");
            exit(-1);
        }
        printf("Directory ./lua created!\n");
    }

    for(auto& entry : boost::make_iterator_range(directory_iterator("./lua"), {}))
    {
        mLuaScripts.emplace_back(entry.path().string());
    }
}
#endif

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
