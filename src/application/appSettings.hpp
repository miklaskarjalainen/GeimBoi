#pragma once
#include <string>
#include <boost/property_tree/ptree.hpp>

namespace Giffi
{
    class appSettings
    {
    public:
        static void SetLastRomPath(const std::string& path);
        static std::string GetLastRomPath();

    private:
        appSettings();
        ~appSettings(); 
        
        static appSettings& Get();
    
    private:
        const std::string iniFilePath = "./settings.ini";
        boost::property_tree::ptree iniFile;
    };
}