#pragma once
#include <cstdint>
#include <string>

namespace GeimBoi
{
    class rebindButton {
    public:
        rebindButton(const char* button_text, uint16_t& button_setting);
        void Draw();

    public:
        static uint16_t LastKeyDown;

    private:
        std::string mButtonText;
        uint16_t& mSetting; 

        static rebindButton* sCurrentlyBinding;
    };
}
