#include <SDL2/SDL.h>
#include "rebindButton.hpp"
#include "../imgui/imgui.h"

using namespace GeimBoi;

rebindButton* rebindButton::sCurrentlyBinding = nullptr;
uint16_t rebindButton::LastKeyDown = 0;

rebindButton::rebindButton(const char* button_text, uint16_t& button_setting)
    : mButtonText(button_text), mSetting(button_setting)
{

}

void rebindButton::Draw()
{
    const bool binding_this = sCurrentlyBinding == this;
    const char* btn_text = binding_this ? "..." : SDL_GetScancodeName((SDL_Scancode)mSetting);
    std::string t = mButtonText + ": [" + btn_text + "]";

    if (ImGui::Button( t.c_str(), {140, 18}))
    {
        sCurrentlyBinding = this;
        LastKeyDown = 0;
    }

    if (binding_this && LastKeyDown)
    {
        mSetting = LastKeyDown;
        sCurrentlyBinding = nullptr;
    }
}
