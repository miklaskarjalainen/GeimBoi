// By github.com/giffi-dev/ (me)
// yes.. i'm using c++17 which has std::filesystem but would like to get this project to c++14 and these were easy to write...
// using c++17 for std::variant in FileIni.hpp.

#pragma once
#ifdef __unix__
    #include <dirent.h>
    #define _makedir(path) mkdir(path, 0700)
#elif _WIN64
    #include <direct.h>
    #define _makedir(path) _mkdir(path)
#endif

#include <string>
#include <sys/types.h>
#include <sys/stat.h>

namespace Dir {


/* File or Dir */
inline bool FileExists(const std::string& path)
{
    struct stat st = {0};
    return stat(path.c_str(), &st) == 0;
}

inline bool CreateDirectory(const std::string& folder)
{
    struct stat st = {0};
    if (!FileExists(folder)) {
        mkdir(folder.c_str(), 0700);
        return true;
    }
    return false;
}

} // Benchmark Dir
