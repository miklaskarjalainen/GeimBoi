#include <iostream>
#include <filesystem>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>
#include "appSettings.hpp"


using namespace GeimBoi;

appSettings::General appSettings::general = {};
appSettings::Window appSettings::window = {};
appSettings::Controls appSettings::controls = {};
appSettings::Hotkeys appSettings::hotkeys = {};

void appSettings::Load(const std::string& path)
{
    if (!std::filesystem::is_regular_file(path))
        return;
    
    toml::parse_result result = toml::parse_file(path);
    if (!result) {
        std::cout << "TOML: parsing error\n" << result.error() << "\n";
        return;
    }
    toml::table file = std::move(result).table();

#define LOAD_FROM_FILE(setting) setting = file.at_path(#setting).value_or(setting)

    // get fields
    LOAD_FROM_FILE(general.master_volume);
    LOAD_FROM_FILE(general.lastrom_path);

    LOAD_FROM_FILE(window.width);
    LOAD_FROM_FILE(window.height);

    // controls
    LOAD_FROM_FILE(controls.up);
    LOAD_FROM_FILE(controls.down);
    LOAD_FROM_FILE(controls.left);
    LOAD_FROM_FILE(controls.right);
    LOAD_FROM_FILE(controls.start);
    LOAD_FROM_FILE(controls.select);
    LOAD_FROM_FILE(controls.a);
    LOAD_FROM_FILE(controls.b);
    
    // hotkeys
    LOAD_FROM_FILE(hotkeys.load_state1);
    LOAD_FROM_FILE(hotkeys.load_state2);
    LOAD_FROM_FILE(hotkeys.load_state3);
    LOAD_FROM_FILE(hotkeys.load_state4);
    LOAD_FROM_FILE(hotkeys.load_state5);
    LOAD_FROM_FILE(hotkeys.load_state6);
    LOAD_FROM_FILE(hotkeys.load_state7);
    LOAD_FROM_FILE(hotkeys.load_state8);
    LOAD_FROM_FILE(hotkeys.load_state9);
    LOAD_FROM_FILE(hotkeys.save_state1);
    LOAD_FROM_FILE(hotkeys.save_state2);
    LOAD_FROM_FILE(hotkeys.save_state3);
    LOAD_FROM_FILE(hotkeys.save_state4);
    LOAD_FROM_FILE(hotkeys.save_state5);
    LOAD_FROM_FILE(hotkeys.save_state6);
    LOAD_FROM_FILE(hotkeys.save_state7);
    LOAD_FROM_FILE(hotkeys.save_state8);
    LOAD_FROM_FILE(hotkeys.save_state9);

    LOAD_FROM_FILE(hotkeys.hard_reset);
#undef LOAD_FROM_FILE

    std::cout << "'" << path << "' loaded succesfully!\n";
}

void appSettings::Save(const std::string& path)
{
    toml::table toml_file{
        {
            "general",
            toml::table {
                { "master_volume", general.master_volume },
                { "lastrom_path" , general.lastrom_path },
            },
        },

        {
            "window",
            toml::table {
                { "width" , window.width },
                { "height", window.height },
            },
        },

        {
            "controls",
            toml::table {
                { "up"    , controls.up },
                { "down"  , controls.down },
                { "right" , controls.right },
                { "left"  , controls.left },
                { "start" , controls.start },
                { "select", controls.select },
                { "a"     , controls.a },
                { "b"     , controls.b },
            },
        },

        {
            "hotkeys",
            toml::table {
                { "load_state1", hotkeys.load_state1 },
                { "load_state2", hotkeys.load_state2 },
                { "load_state3", hotkeys.load_state3 },
                { "load_state4", hotkeys.load_state4 },
                { "load_state5", hotkeys.load_state5 },
                { "load_state6", hotkeys.load_state6 },
                { "load_state7", hotkeys.load_state7 },
                { "load_state8", hotkeys.load_state8 },
                { "load_state9", hotkeys.load_state9 },
                { "save_state1", hotkeys.save_state1 },
                { "save_state2", hotkeys.save_state2 },
                { "save_state3", hotkeys.save_state3 },
                { "save_state4", hotkeys.save_state4 },
                { "save_state5", hotkeys.save_state5 },
                { "save_state6", hotkeys.save_state6 },
                { "save_state7", hotkeys.save_state7 },
                { "save_state8", hotkeys.save_state8 },
                { "save_state9", hotkeys.save_state9 },
            },
        },
    };

    std::ofstream file(path);
    if (!file.is_open()) {
        std::cout << "Could not save settings to '" << path << "'\n";
        return;
    }
    file << toml_file;
    file.close();

    std::cout << "Settings saves succesfully to'" << path << "'!\n";
}
