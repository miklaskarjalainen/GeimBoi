#include "imgui/imgui.h"
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

    mWindow  = SDL_CreateWindow("GeimBoi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
    if (mWindow == NULL)
    {
        printf("Unable to create window: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_SetWindowMinimumSize(mWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    mRenderer= SDL_CreateRenderer(mWindow, 0, SDL_RENDERER_ACCELERATED);
    if (mRenderer == NULL)
    {
        printf("Unable to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    // GameBoy
    mGameBoy = std::make_shared<gbGameBoy>();
    mGameBoy->SetPalette(0x9bbc0f, 0x8bac0f, 0x306230, 0x0f380f);
    mGameBoy->LoadBios("gb_bios.bin");
    mGameBoy->mApu.masterVolume = appSettings::master_volume;
    if (openRom)
        mGameBoy->LoadRom(openRom);
    
    // Gui
    mGui = std::make_unique<appGui>(mWindow, mRenderer, mGameBoy, width, height);
}

void appWindow::Run()
{
    // Create Surface out of raw pixel data.
    constexpr int pitch = 3 * 160;
    constexpr int depth = 24;
    constexpr Uint32 rmask = 0x000000FF;
    constexpr Uint32 gmask = 0x0000FF00;
    constexpr Uint32 bmask = 0x00FF0000;
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(mGameBoy->mPpu.frontBuffer, 160, 144, depth, pitch, rmask, gmask, bmask, (uint32_t)NULL);
    if (!surface) {
        printf("Error creating a surface for screenbuffer! %s\n", SDL_GetError());
        exit(-1);
    }

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
            SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);

            // Clear Background
            SDL_SetRenderDrawColor(mRenderer, 114, 144, 154, 255);
            SDL_RenderClear(mRenderer);

            // Render Emulator
            SDL_Rect dst = { 0, 19, 0, 0 }; // Take in count window size, and top bar which takes 19px.
            SDL_GetWindowSize(mWindow, &dst.w, &dst.h);
            dst.h -= dst.y;
        
            SDL_RenderCopy(mRenderer, texture, NULL, &dst);
            mGui->Render();

            // Render
            SDL_RenderPresent(mRenderer);
            SDL_DestroyTexture(texture);
        }

        {
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
    // Save Game on exit
    if (mGameBoy->mCart.IsGameLoaded())
    {
        std::string file_name = mGameBoy->mCart.GetGameName() + ".sav";
        mGameBoy->mCart.SaveBattery(file_name);
    }
    SDL_Quit();
    appSettings::Save(SettingsPath);    
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
            case SDL_TEXTINPUT:
            {
                io.AddInputCharactersUTF8(events.text.text);
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
            case SDL_MOUSEWHEEL:
            {
                io.MouseWheel = (float)(events.wheel.y);
                break;
            }
            case SDL_WINDOWEVENT:
            {
                if (events.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    const Sint16 new_width  = events.window.data1;
                    const Sint16 new_height = events.window.data2;
                    
                    appSettings::window.width  = static_cast<uint16_t>(new_width);
                    appSettings::window.height = static_cast<uint16_t>(new_height);
                    io.DisplaySize.x = static_cast<float>(new_width);
                    io.DisplaySize.y = static_cast<float>(new_height);
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