#pragma once
#include <cstdint>
#include <string>
#include "../imgui/imgui.h"

namespace GeimBoi
{
    // TODO: modifiers
    class rebindButton {
    public:
        rebindButton(const char* button_text, uint16_t& button_setting);
        void Draw(const ImVec2 size = {0,0});

    public:
        static uint16_t LastKeyDown;

    private:
        std::string mButtonText;
        uint16_t& mSetting; 

        static rebindButton* sCurrentlyBinding;
    };
}
