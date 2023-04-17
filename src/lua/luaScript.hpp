#pragma once
#include <string>
#include <boost/filesystem.hpp>

struct lua_State;
typedef int (*lua_CFunction) (lua_State *L);

namespace GeimBoi
{
    class luaScript
    {
    public:
        luaScript() = default;
        luaScript(const boost::filesystem::path& filepath);
        luaScript(luaScript&&) noexcept;
        luaScript(const luaScript&) = delete;
        ~luaScript();

        void Start();
        void Stop();
        void Update();
        // if table is nullptr then the function will be in global scope, table is used like a namespace.
        void AddFunction(lua_CFunction func, const char* name, const char* table = nullptr);
        inline bool IsStopped() const { return mState == nullptr; }

        std::string FileName;
        std::string FilePath;
    private:
        void CheckResult(bool r);

        lua_State* mState = nullptr;
    };
}
