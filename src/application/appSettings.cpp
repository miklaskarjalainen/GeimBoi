#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "appSettings.hpp"

using namespace GeimBoi;
using namespace boost::property_tree;

appSettings::General appSettings::general = {};
appSettings::Window appSettings::window = {};
appSettings::Controls appSettings::controls = {};
appSettings::Hotkeys appSettings::hotkeys = {};

void appSettings::Load(const std::string& path)
{
    if (!boost::filesystem::is_regular_file(path))
        return;

    ptree file;
    ini_parser::read_ini(path, file);

#define LOAD_FROM_INI(type, setting) \
    setting = file.get<type>(#setting, setting)

    // get fields
    LOAD_FROM_INI(float      , general.master_volume);
    LOAD_FROM_INI(std::string, general.lastrom_path);

    LOAD_FROM_INI(uint16_t, window.width);
    LOAD_FROM_INI(uint16_t, window.height);

    // controls
    LOAD_FROM_INI(uint16_t, controls.up);
    LOAD_FROM_INI(uint16_t, controls.down);
    LOAD_FROM_INI(uint16_t, controls.left);
    LOAD_FROM_INI(uint16_t, controls.right);
    LOAD_FROM_INI(uint16_t, controls.start);
    LOAD_FROM_INI(uint16_t, controls.select);
    LOAD_FROM_INI(uint16_t, controls.a);
    LOAD_FROM_INI(uint16_t, controls.b);
    
    // hotkeys
    LOAD_FROM_INI(uint16_t, hotkeys.load_state1);
    LOAD_FROM_INI(uint16_t, hotkeys.load_state2);
    LOAD_FROM_INI(uint16_t, hotkeys.load_state3);
    LOAD_FROM_INI(uint16_t, hotkeys.load_state4);
    LOAD_FROM_INI(uint16_t, hotkeys.load_state5);
    LOAD_FROM_INI(uint16_t, hotkeys.load_state6);
    LOAD_FROM_INI(uint16_t, hotkeys.load_state7);
    LOAD_FROM_INI(uint16_t, hotkeys.load_state8);
    LOAD_FROM_INI(uint16_t, hotkeys.load_state9);
    LOAD_FROM_INI(uint16_t, hotkeys.save_state1);
    LOAD_FROM_INI(uint16_t, hotkeys.save_state2);
    LOAD_FROM_INI(uint16_t, hotkeys.save_state3);
    LOAD_FROM_INI(uint16_t, hotkeys.save_state4);
    LOAD_FROM_INI(uint16_t, hotkeys.save_state5);
    LOAD_FROM_INI(uint16_t, hotkeys.save_state6);
    LOAD_FROM_INI(uint16_t, hotkeys.save_state7);
    LOAD_FROM_INI(uint16_t, hotkeys.save_state8);
    LOAD_FROM_INI(uint16_t, hotkeys.save_state9);

    LOAD_FROM_INI(uint16_t, hotkeys.hard_reset);

    printf("%s loaded succesfully!\n", path.c_str());
}

void appSettings::Save(const std::string& path)
{
    ptree file;
    
#define SAVE_TO_INI(setting) \
    file.add(#setting, setting)

    // set fields
    SAVE_TO_INI(general.master_volume);
    SAVE_TO_INI(general.lastrom_path);

    SAVE_TO_INI(window.width);
    SAVE_TO_INI(window.height);

    // controls
    SAVE_TO_INI(controls.up);
    SAVE_TO_INI(controls.down);
    SAVE_TO_INI(controls.right);
    SAVE_TO_INI(controls.left);
    SAVE_TO_INI(controls.start);
    SAVE_TO_INI(controls.select);
    SAVE_TO_INI(controls.a);
    SAVE_TO_INI(controls.b);
    
    // hotkeys
    SAVE_TO_INI(hotkeys.load_state1);
    SAVE_TO_INI(hotkeys.load_state2);
    SAVE_TO_INI(hotkeys.load_state3);
    SAVE_TO_INI(hotkeys.load_state4);
    SAVE_TO_INI(hotkeys.load_state5);
    SAVE_TO_INI(hotkeys.load_state6);
    SAVE_TO_INI(hotkeys.load_state7);
    SAVE_TO_INI(hotkeys.load_state8);
    SAVE_TO_INI(hotkeys.load_state9);
    SAVE_TO_INI(hotkeys.save_state1);
    SAVE_TO_INI(hotkeys.save_state2);
    SAVE_TO_INI(hotkeys.save_state3);
    SAVE_TO_INI(hotkeys.save_state4);
    SAVE_TO_INI(hotkeys.save_state5);
    SAVE_TO_INI(hotkeys.save_state6);
    SAVE_TO_INI(hotkeys.save_state7);
    SAVE_TO_INI(hotkeys.save_state8);
    SAVE_TO_INI(hotkeys.save_state9);
    
    SAVE_TO_INI(hotkeys.hard_reset);

    // save file
    write_ini(path, file);    

    printf("%s was saved succesfully!\n", path.c_str());
}
