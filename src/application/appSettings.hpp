#pragma once
#include <string>

namespace GeimBoi
{
    class appSettings {
    public:
        static std::string lastrom_path;

        static void Load(const std::string& path);
        static void Save(const std::string& path);
    
    private:
        appSettings() = delete;
        appSettings(const appSettings&) = delete;
        ~appSettings() = delete;
    };
}
