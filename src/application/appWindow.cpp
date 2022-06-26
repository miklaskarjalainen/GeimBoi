#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "appWindow.hpp"
#include "appSettings.hpp"
#include "gui/rebindButton.hpp"

using namespace GeimBoi;

appWindow::appWindow(const char* openRom)
{
    // Init settings / config
    appSettings::Load(SettingsPath);
    const auto width = appSettings::window.width;
    const auto height = appSettings::window.height;

    // Init SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        printf("Unable to init sdl2: %s\n", SDL_GetError()); 
        exit(1);
    }

    // fixes a screen flash when starting and closing the application on KDE
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"); 

    // Setup window & OpenGL context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    mWindow = SDL_CreateWindow("GeimBoi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if (mWindow == NULL)
    {
        printf("Unable to create window: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_SetWindowMinimumSize(mWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
    mGLContext = SDL_GL_CreateContext(mWindow);
    SDL_GL_MakeCurrent(mWindow, mGLContext);
    SDL_GL_SetSwapInterval(0); 

    // GameBoy
    mGameBoy = std::make_shared<gbGameBoy>();
    mGameBoy->SetPalette(0x9bbc0f, 0x8bac0f, 0x306230, 0x0f380f);
    mGameBoy->LoadBios("gb_bios.bin");
    mGameBoy->mApu.masterVolume = appSettings::master_volume;
    if (openRom)
        mGameBoy->LoadRom(openRom);
    
    // Gui
    mGui = std::make_unique<appGui>(mWindow, mGLContext, mGameBoy, width, height);
}

appWindow::~appWindow()
{
    // Save Game on exit
    if (mGameBoy->mCart.IsGameLoaded())
    {
        std::string file_name = mGameBoy->mCart.GetGameName() + ".sav";
        mGameBoy->mCart.SaveBattery(file_name);
    }
    appSettings::Save(SettingsPath);

    SDL_GL_DeleteContext(mGLContext);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void appWindow::Run()
{
    const ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    while (!ShouldWindowClose())
    {
        DoEvents();
        int start_ticks = SDL_GetTicks();

        // Update
        {
            // Emulator gets updated in the gui, because of "pause"
            mGui->Draw();
            if (!mGui->IsPaused())
                mGameBoy->FrameAdvance();
        }


        // Rendering
        {
            // Clear Screen
            auto& io = ImGui::GetIO();
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClearColor(ClearColor.x * ClearColor.w, ClearColor.y * ClearColor.w, ClearColor.z * ClearColor.w, ClearColor.w);
            glClear(GL_COLOR_BUFFER_BIT);
            
            // Render GeimBoi's screen buffer, also don't render under the MenuBar
            constexpr int MenuBarHeight = 39;
            const float MenuBarOffset = static_cast<float>(MenuBarHeight) / io.DisplaySize.y;
            glRasterPos2f(-1, 1.0 - MenuBarOffset);
            glPixelZoom(io.DisplaySize.x / SCREEN_WIDTH, -io.DisplaySize.y / SCREEN_HEIGHT);
            glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, mGameBoy->mPpu.frontBuffer);
            
            // Render Gui
            mGui->Render();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
                SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
            }
        }

        {
            // Using delay to target 60fps
            int end_ticks = SDL_GetTicks();
            if (16 - (end_ticks - start_ticks) > 0)
                SDL_Delay(16 - (end_ticks - start_ticks));
        }
        SDL_GL_SwapWindow(mWindow);
    }
}

void appWindow::DoEvents()
{
    SDL_Event events;
    ImGuiIO& io = ImGui::GetIO();

    while (SDL_PollEvent(&events))
    {
        ImGui_ImplSDL2_ProcessEvent(&events);

        switch (events.type)
        {
            case SDL_QUIT:
            {
                mClosing = true;
                break;
            }

            // Controls currently hardcoded but will be added into a array or something
            case SDL_KEYDOWN:
            {
                uint16_t key = events.key.keysym.scancode;
                rebindButton::LastKeyDown = key;

                io.KeysDown[key] = true;
                io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                io.KeyCtrl  = ((SDL_GetModState() & KMOD_CTRL) != 0);
                io.KeyAlt   = ((SDL_GetModState() & KMOD_ALT) != 0);
                io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);

                // Direction buttons, can't hold opposite directions at the same time
                if (key == appSettings::controls.right)
                { 
                    if (io.KeysDown[appSettings::controls.left])
                    {
                        mGameBoy->ReleaseButton(gbButton::LEFT);
                        mGameBoy->ReleaseButton(gbButton::RIGHT);
                        break;
                    }
                    mGameBoy->PressButton(gbButton::RIGHT);
                    break;
                }
                if (key == appSettings::controls.left)
                {
                    if (io.KeysDown[appSettings::controls.right])
                    {
                        mGameBoy->ReleaseButton(gbButton::LEFT);
                        mGameBoy->ReleaseButton(gbButton::RIGHT);
                        break;
                    }
                    mGameBoy->PressButton(gbButton::LEFT);
                    break;
                }
                if (key == appSettings::controls.up)
                { 
                    if (io.KeysDown[appSettings::controls.down])
                    {
                        mGameBoy->ReleaseButton(gbButton::UP);
                        mGameBoy->ReleaseButton(gbButton::DOWN);
                        break;
                    }
                    mGameBoy->PressButton(gbButton::UP);
                    break;
                }
                if (key == appSettings::controls.down)
                { 
                    if (io.KeysDown[appSettings::controls.up])
                    {
                        mGameBoy->ReleaseButton(gbButton::UP);
                        mGameBoy->ReleaseButton(gbButton::DOWN);
                        break;
                    }
                    mGameBoy->PressButton(gbButton::DOWN);   
                    break; 
                }
                
                if (key == appSettings::controls.a)      { mGameBoy->PressButton(gbButton::A);      break; }
                if (key == appSettings::controls.b)      { mGameBoy->PressButton(gbButton::B);      break; }
                if (key == appSettings::controls.start)  { mGameBoy->PressButton(gbButton::START); break; }
                if (key == appSettings::controls.select) { mGameBoy->PressButton(gbButton::SELECT);  break; }
                
                if (key == SDL_SCANCODE_ESCAPE) { mClosing = true; break; }
                if (io.KeyShift && key == SDL_SCANCODE_R) { mGameBoy->Reset(); break; }
                
                break;
            }
            case SDL_KEYUP:
            {
                uint16_t key = events.key.keysym.scancode;

                io.KeysDown[key] = false;
                io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                io.KeyCtrl  = ((SDL_GetModState() & KMOD_CTRL) != 0);
                io.KeyAlt   = ((SDL_GetModState() & KMOD_ALT) != 0);
                io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);

                if (key == appSettings::controls.right) 
                { 
                    mGameBoy->ReleaseButton(gbButton::RIGHT);
                    if (io.KeysDown[appSettings::controls.left]) {
                        mGameBoy->PressButton(gbButton::LEFT);
                    }
                    break;
                }
                if (key == appSettings::controls.left)
                {
                    mGameBoy->ReleaseButton(gbButton::LEFT);
                    if (io.KeysDown[appSettings::controls.right]) {
                        mGameBoy->PressButton(gbButton::RIGHT);
                    }
                    break;
                }
                if (key == appSettings::controls.up)
                {
                    mGameBoy->ReleaseButton(gbButton::UP);
                    if (io.KeysDown[appSettings::controls.down]) {
                        mGameBoy->PressButton(gbButton::DOWN);
                    }
                    break;
                }
                if (key == appSettings::controls.down)
                {
                    mGameBoy->ReleaseButton(gbButton::DOWN);
                    if (io.KeysDown[appSettings::controls.up])
                    {
                        mGameBoy->PressButton(gbButton::UP);
                    }
                    break;
                }
                
                if (key == appSettings::controls.a)      { mGameBoy->ReleaseButton(gbButton::A);      break; }
                if (key == appSettings::controls.b)      { mGameBoy->ReleaseButton(gbButton::B);      break; }
                if (key == appSettings::controls.start)  { mGameBoy->ReleaseButton(gbButton::START);  break; }
                if (key == appSettings::controls.select) { mGameBoy->ReleaseButton(gbButton::SELECT);  break; }
                break;
            }
            case SDL_DROPFILE: // File gets dropped into the program
            {
                std::string file_path = std::string(events.drop.file);
                if (file_path.find(".gb") > 0) // works with .gb .gbc etc
                {
                    mGameBoy->Reset();
                    mGameBoy->LoadRom(file_path);
                }
                break;
            }
            case SDL_WINDOWEVENT:
            {
                if (events.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    appSettings::window.width  = static_cast<uint16_t>(events.window.data1);
                    appSettings::window.height = static_cast<uint16_t>(events.window.data2);
                }
                break;
            }
            default:
                break;

        }
    }

    // Imgui stuff
    io.DeltaTime = 1.0f / 60.0f;
}