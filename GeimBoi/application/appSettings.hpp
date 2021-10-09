#pragma once
#include <string>
#include "utils/FileIni.hpp"

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
        const std::string iniFilePath = "settings.ini";
        FileIni m_IniFile;

    };
}