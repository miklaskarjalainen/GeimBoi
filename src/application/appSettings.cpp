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

std::string appSettings::lastrom_path = "";

void appSettings::Load(const std::string& path)
{
    if (!boost::filesystem::is_regular_file(path))
        return;

    boost::property_tree::ptree file;
    boost::property_tree::ini_parser::read_ini(path, file);

    // get fields
    lastrom_path = file.get<std::string>("general.lastrom_path", lastrom_path); 
    
    printf("%s loaded succesfully!\n", path.c_str());
}

void appSettings::Save(const std::string& path)
{
    boost::property_tree::ptree iniFile;
    
    // set fields
    iniFile.add("general.lastrom_path", lastrom_path);

    // save file
    boost::property_tree::write_ini(path, iniFile);    

    printf("%s was saved succesfully!\n", path.c_str());
}
