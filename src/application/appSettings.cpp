#ifdef _WIN32
    #include <direct.h>
    #define get_current_dir_name() _getcwd(NULL,0)
#elif __unix__
    #include <unistd.h>
#endif
#include <boost/property_tree/ini_parser.hpp>
#include <utils/Benchmark.hpp>
#include "appSettings.hpp"

using namespace Giffi;

void appSettings::SetLastRomPath(const std::string& path)
{
    Get().iniFile.put("General.LastRomPath", path);
}

std::string appSettings::GetLastRomPath()
{
    return Get().iniFile.get<std::string>("General.LastRomPath", std::string(get_current_dir_name()));
}

appSettings& appSettings::Get()
{
    static appSettings p;
    return p;
}

appSettings::appSettings() {
    boost::property_tree::ini_parser::read_ini(iniFilePath, iniFile);
}

appSettings::~appSettings()
{
    std::fstream file(iniFilePath);
    boost::property_tree::write_ini(file, iniFile);
}