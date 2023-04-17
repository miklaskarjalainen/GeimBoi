#include <cassert>
#include <lua.hpp>
#include "api/luaApi.hpp"
#include "luaScript.hpp"

using namespace GeimBoi;

struct lua_State;
namespace GeimBoi
{
    luaScript::luaScript(const boost::filesystem::path& filepath)
        : FileName(filepath.filename().string()), FilePath(filepath.string()), mState(nullptr)
    {
        assert(boost::filesystem::exists(filepath));
    }

    luaScript::luaScript(luaScript&& o) noexcept
        : FileName(std::move(o.FileName)), FilePath(std::move(o.FilePath)), Stopped(std::move(o.Stopped)), mState(std::move(o.mState))
    {
        o.mState = nullptr; 
        o.Stopped = true;
    }

    luaScript::~luaScript()
    {
        if (mState)
            lua_close(mState);
    }

    void luaScript::Start()
    {
        assert(Stopped == true);
        mState = luaL_newstate();
        luaL_openlibs(mState);    // open lua's libs
        LuaApi::OpenLibs(this);   // open geimboi's libs

        CheckResult(luaL_dofile(mState, FilePath.c_str()));
        Stopped = false;
    }

    void luaScript::Stop()
    {
        assert(Stopped == false);
        if (mState)
            lua_close(mState);
        Stopped = true;
    }

    void luaScript::Update()
    {
        if (Stopped || mState == nullptr)
            return;
        lua_getglobal(mState, "on_update");
        CheckResult(lua_pcall(mState, 0, 0, 0));
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
            Stopped = true;
            const char* msg = lua_gettop(mState) > 0 ? lua_tostring(mState, -1) : "an error occurred";
            printf("\033[0;31m[Lua] %s\n\033[0m", msg);
        }
    }
}
