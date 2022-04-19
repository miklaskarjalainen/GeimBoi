#include <utils/Benchmark.hpp>
#include <thread>
#include "imgui/imgui.h"
#include "appWindow.hpp"
#include "appGui.hpp"

using namespace Giffi;

appWindow::appWindow(const char* openRom, int width, int height)
{
    PROFILE_FUNCTION();

    // Init SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        printf("Unable to init sdl2: %s\n", SDL_GetError()); 
        exit(1);
    }
    mWindow  = SDL_CreateWindow("GeimBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
    if (mWindow == NULL)
    {
        printf("Unable to create window: %s\n", SDL_GetError());
        exit(1);
    }
    
    mRenderer= SDL_CreateRenderer(mWindow, 0, SDL_RENDERER_ACCELERATED);
    if (mRenderer == NULL)
    {
        printf("Unable to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    // Init GameBoy
    mGameBoy = std::make_shared<gbGameBoy>();
    mGameBoy->Reset();
    mGameBoy->SetPalette(0x9bbc0f, 0x8bac0f, 0x306230, 0x0f380f);
    mGameBoy->mBootRom.LoadBios("gb_bios.bin");
    if (openRom)
        mGameBoy->LoadRom(openRom);
    
    // Gui
    mGui = std::make_unique<appGui>(mRenderer, mGameBoy, width, height);
}

void appWindow::Run()
{
    PROFILE_FUNCTION();

    // Create Surface out of raw pixel data.
    constexpr int pitch = 3 * 160;
    constexpr int depth = 24;
    constexpr Uint32 rmask = 0x000000FF;
    constexpr Uint32 gmask = 0x0000FF00;
    constexpr Uint32 bmask = 0x00FF0000;
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(&mGameBoy->mPpu.frontBuffer, 160, 144, depth, pitch, rmask, gmask, bmask, NULL);
    
    while (!ShouldWindowClose())
    {
        DoEvents();
        int start_ticks = SDL_GetTicks();

        // Update
        {
            PROFILE_SCOPE("Update");
            // Emulator gets updated in the gui, because of "pause"
            mGui->Update();
            if (!mGui->IsPaused())
                mGameBoy->FrameAdvance();
        }


        // Rendering
        {
            PROFILE_SCOPE("Rendering");
            SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);

            // Clear Background
            SDL_SetRenderDrawColor(mRenderer, 114, 144, 154, 255);
            SDL_RenderClear(mRenderer);

            // Render Emulator
            SDL_Rect dst = { 0, 19, 0, 0 }; // Take in count window size, and top bar which takes 19px.
            SDL_GetWindowSize(mWindow, &dst.w, &dst.h);
            dst.h -= dst.y;
        
            SDL_RenderCopy(mRenderer, texture, NULL, &dst);
            mGui->Draw();

            // Render
            SDL_RenderPresent(mRenderer);
            SDL_DestroyTexture(texture);
        }

        {
            PROFILE_SCOPE("SDL_Delay");
            // Using delay to target 60fps
            int end_ticks = SDL_GetTicks();
            if (16 - (end_ticks - start_ticks) > 0)
                SDL_Delay(16 - (end_ticks - start_ticks));
        }
        
    }

    SDL_FreeSurface(surface);
}

appWindow::~appWindow()
{
    PROFILE_FUNCTION();

    // Save Game on exit
    if (mGameBoy->mCart.IsGameLoaded())
    {
        std::string file_name = mGameBoy->mCart.GetGameName() + ".sav";
        mGameBoy->mCart.SaveBattery(file_name);
    }
    SDL_Quit();
}

void appWindow::DoEvents()
{
    SDL_Event events;
    ImGuiIO& io = ImGui::GetIO();

    while (SDL_PollEvent(&events))
    {
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
                io.KeysDown[key] = true;
                io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                io.KeyCtrl  = ((SDL_GetModState() & KMOD_CTRL) != 0);
                io.KeyAlt   = ((SDL_GetModState() & KMOD_ALT) != 0);
                io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);

                if (key == SDL_SCANCODE_ESCAPE)    { mClosing = true; break; }
                if (key == SDL_SCANCODE_D)         { mGameBoy->PressButton(gbButton::RIGHT);  break; }
                if (key == SDL_SCANCODE_A)         { mGameBoy->PressButton(gbButton::LEFT);   break; }
                if (key == SDL_SCANCODE_W)         { mGameBoy->PressButton(gbButton::UP);     break; }
                if (key == SDL_SCANCODE_S)         { mGameBoy->PressButton(gbButton::DOWN);   break; }
                if (key == SDL_SCANCODE_K)         { mGameBoy->PressButton(gbButton::A);      break; }
                if (key == SDL_SCANCODE_J)         { mGameBoy->PressButton(gbButton::B);      break; }
                if (key == SDL_SCANCODE_RETURN)    { mGameBoy->PressButton(gbButton::START); break; }
                if (key == SDL_SCANCODE_BACKSLASH) { mGameBoy->PressButton(gbButton::SELECT);  break; }
                
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

                if (key == SDL_SCANCODE_D)         { mGameBoy->ReleaseButton(gbButton::RIGHT);  break; }
                if (key == SDL_SCANCODE_A)         { mGameBoy->ReleaseButton(gbButton::LEFT);   break; }
                if (key == SDL_SCANCODE_W)         { mGameBoy->ReleaseButton(gbButton::UP);     break; }
                if (key == SDL_SCANCODE_S)         { mGameBoy->ReleaseButton(gbButton::DOWN);   break; }
                if (key == SDL_SCANCODE_K)         { mGameBoy->ReleaseButton(gbButton::A);      break; }
                if (key == SDL_SCANCODE_J)         { mGameBoy->ReleaseButton(gbButton::B);      break; }
                if (key == SDL_SCANCODE_RETURN)    { mGameBoy->ReleaseButton(gbButton::START);  break; }
                if (key == SDL_SCANCODE_BACKSLASH) { mGameBoy->ReleaseButton(gbButton::SELECT);  break; }
                break;
            }
            case SDL_TEXTINPUT:
            {
                io.AddInputCharactersUTF8(events.text.text);
                break;
            }
            case SDL_DROPFILE: // File gets dropped into the program
            {
                std::string file_path = std::string(events.drop.file);
                if (file_path.find(".gb") > 0) // works with .gb .gbc .gbc etc
                {
                    mGameBoy->Reset();
                    mGameBoy->LoadRom(file_path);
                }
                break;
            }
            case SDL_MOUSEWHEEL:
            {
                io.MouseWheel = (float)(events.wheel.y);
                break;
            }
            case SDL_WINDOWEVENT:
            {
                if (events.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    io.DisplaySize.x = (float)(events.window.data1);
                    io.DisplaySize.y = (float)(events.window.data2);
                }
                break;
            }
            default:
                break;

        }
    }

    // Imgui stuff
    int mouseX, mouseY;
    const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
    io.DeltaTime = 1.0f / 60.0f;
    io.MousePos = ImVec2((float)mouseX, (float)mouseY);
    io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
}