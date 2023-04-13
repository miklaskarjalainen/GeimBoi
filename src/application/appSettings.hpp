#pragma once
#include <string>
#include <SDL2/SDL.h>
#include "../core/gbPPU.hpp" // screen size macros

namespace GeimBoi
{
    class appSettings {
    public:
        static std::string lastrom_path;
        static float master_volume;

        static struct Window {
            uint16_t width = SCREEN_WIDTH*2;
            uint16_t height = SCREEN_HEIGHT*2;
        } window;

        static struct Controls {
            uint16_t up    = SDL_SCANCODE_W;
            uint16_t down  = SDL_SCANCODE_S;
            uint16_t right = SDL_SCANCODE_D;
            uint16_t left  = SDL_SCANCODE_A;
            uint16_t select = SDL_SCANCODE_BACKSPACE;
            uint16_t start = SDL_SCANCODE_RETURN;
            uint16_t b = SDL_SCANCODE_J;
            uint16_t a = SDL_SCANCODE_K;
        } controls;

        static struct Hotkeys {
            // Load States
            uint16_t load_state1 = SDL_SCANCODE_1;
            uint16_t load_state2 = SDL_SCANCODE_2;
            uint16_t load_state3 = SDL_SCANCODE_3;
            uint16_t load_state4 = SDL_SCANCODE_4;
            uint16_t load_state5 = SDL_SCANCODE_5;
            uint16_t load_state6 = SDL_SCANCODE_6;
            uint16_t load_state7 = SDL_SCANCODE_7;
            uint16_t load_state8 = SDL_SCANCODE_8;
            uint16_t load_state9 = SDL_SCANCODE_9;

            // Save States
            uint16_t save_state1 = SDL_SCANCODE_F1;
            uint16_t save_state2 = SDL_SCANCODE_F2;
            uint16_t save_state3 = SDL_SCANCODE_F3;
            uint16_t save_state4 = SDL_SCANCODE_F4;
            uint16_t save_state5 = SDL_SCANCODE_F5;
            uint16_t save_state6 = SDL_SCANCODE_F6;
            uint16_t save_state7 = SDL_SCANCODE_F7;
            uint16_t save_state8 = SDL_SCANCODE_F8;
            uint16_t save_state9 = SDL_SCANCODE_F9;
        } hotkeys;

        static void Load(const std::string& path);
        static void Save(const std::string& path);
    
    private:
        appSettings() = delete;
        appSettings(const appSettings&) = delete;
        ~appSettings() = delete;
    };
}
