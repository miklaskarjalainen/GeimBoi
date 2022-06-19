#ifdef _WIN32
    #include <direct.h>
    #define get_current_dir_name() _getcwd(NULL,0)
#elif __unix__
    #include <unistd.h>
#endif
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "appSettings.hpp"

using namespace GeimBoi;
using namespace boost::property_tree;

std::string appSettings::lastrom_path = "";
appSettings::Controls appSettings::controls = {};

void appSettings::Load(const std::string& path)
{
    if (!boost::filesystem::is_regular_file(path))
        return;

    ptree file;
    ini_parser::read_ini(path, file);

    // get fields
    lastrom_path = file.get<std::string>("general.lastrom_path", lastrom_path);

    controls.up    = file.get<uint16_t>("controls.up"   , controls.up); 
    controls.down  = file.get<uint16_t>("controls.down" , controls.down); 
    controls.right = file.get<uint16_t>("controls.right", controls.right); 
    controls.left  = file.get<uint16_t>("controls.left" , controls.left); 
    controls.start = file.get<uint16_t>("controls.start" , controls.start); 
    controls.select= file.get<uint16_t>("controls.select", controls.select); 
    controls.a  = file.get<uint16_t>("controls.a" , controls.a); 
    controls.b  = file.get<uint16_t>("controls.b" , controls.b); 
    
    printf("%s loaded succesfully!\n", path.c_str());
}

void appSettings::Save(const std::string& path)
{
    ptree iniFile;
    
    // set fields
    iniFile.add("general.lastrom_path", lastrom_path);

    iniFile.add("controls.up"   , controls.up);
    iniFile.add("controls.down" , controls.down);
    iniFile.add("controls.right", controls.right);
    iniFile.add("controls.left" , controls.left);
    iniFile.add("controls.start" , controls.start);
    iniFile.add("controls.select", controls.select);
    iniFile.add("controls.a" , controls.a);
    iniFile.add("controls.b" , controls.b);
    
    // save file
    write_ini(path, iniFile);    

    printf("%s was saved succesfully!\n", path.c_str());
}
