#include <cassert>
#include <lua/lua.hpp>
#include "api/luaApi.hpp"
#include "luaScript.hpp"

using namespace GeimBoi;

struct lua_State;
namespace GeimBoi
{
    luaScript::luaScript(const std::string& filePath)
        : FilePath(filePath), mState(luaL_newstate())
    {
        assert(boost::filesystem::exists(filePath));

        luaL_openlibs(mState);    // open lua's libs
        LuaApi::OpenLibs(this);   // open geimboi's libs
        CheckResult(luaL_dofile(mState, filePath.c_str()));
    }

    luaScript::luaScript(luaScript&& o)
        : FilePath(std::move(o.FilePath)), mState(std::move(o.mState))
    {
        o.mState = nullptr; 
    }

    luaScript::~luaScript()
    {
        if (mState)
            lua_close(mState);
    }

    void luaScript::Update()
    {
        if (mStopped || mState == nullptr)
            return;
        lua_getglobal(mState, "on_update");
        CheckResult(lua_pcall(mState, 0, 0, 0));
    }

    bool luaScript::IsStopped() const
    {
        return mStopped;
    }

    void luaScript::AddFunction(lua_CFunction func, const char* name, const char* table)
    {
        if (table == nullptr)
        {
            lua_pushcfunction(mState, func);
            lua_setglobal(mState, name);
        }
        else
        {
            if (!lua_getglobal(mState, table))
                lua_newtable(mState);
            lua_pushstring(mState, name);
            lua_pushcfunction(mState, func);
            lua_settable(mState, -3);
            lua_setglobal(mState, table);
        }
    }

    void luaScript::CheckResult(bool r)
    {
        if (r)
        {
            mStopped = true;
            const char* msg = lua_gettop(mState) > 0 ? lua_tostring(mState, -1) : "an error occurred";
            printf("\033[0;31m[Lua] %s\n\033[0m", msg);
        }
    }
}
