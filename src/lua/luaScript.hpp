#pragma once
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
        luaScript(const luaScript&);
        luaScript(luaScript&&) = default;
        ~luaScript();

        void Update();
        bool IsStopped() const;
        // if table is nullptr then the function will be in global scope, table is used like a namespace.
        void AddFunction(lua_CFunction func, const char* name, const char* table = nullptr);

        boost::filesystem::path FilePath;
    private:
        void CheckResult(bool r);

        lua_State* mState = nullptr;
        bool mStopped = false;
    };
}
