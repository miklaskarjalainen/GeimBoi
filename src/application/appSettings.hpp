#pragma once
#include <string>
#include <SDL2/SDL.h>
#include "../core/gbPPU.hpp" // screen size macros

namespace GeimBoi
{
    class appSettings {
    public:
        static std::string lastrom_path;

        static struct Window {
            uint16_t width = SCREEN_WIDTH*2;
            uint16_t height = SCREEN_HEIGHT*2;
        } window;

        static struct Controls {
            uint16_t up    = SDL_SCANCODE_W;
            uint16_t down  = SDL_SCANCODE_S;
            uint16_t right = SDL_SCANCODE_D;
            uint16_t left  = SDL_SCANCODE_A;
            uint16_t start = SDL_SCANCODE_RETURN;
            uint16_t select = SDL_SCANCODE_BACKSPACE;
            uint16_t a = SDL_SCANCODE_K;
            uint16_t b = SDL_SCANCODE_J;
        } controls;

        static void Load(const std::string& path);
        static void Save(const std::string& path);
    
    private:
        appSettings() = delete;
        appSettings(const appSettings&) = delete;
        ~appSettings() = delete;
    };
}
