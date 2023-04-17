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
        : FileName(std::move(o.FileName)), FilePath(std::move(o.FilePath)), mState(std::move(o.mState))
    {
        o.mState = nullptr; 
    }

    luaScript::~luaScript()
    {
        if (mState) {
            lua_close(mState);
        }
    }

    void luaScript::Start()
    {
        assert(!mState);
        mState = luaL_newstate();
        luaL_openlibs(mState);    // open lua's libs
        LuaApi::OpenLibs(this);   // open geimboi's libs
        CheckResult(luaL_dofile(mState, FilePath.c_str()));
    }

    void luaScript::Stop()
    {
        assert(mState);
        lua_close(mState);
        mState = nullptr;
    }

    void luaScript::Update()
    {
        if (mState == nullptr)
            return;
        lua_getglobal(mState, "on_update");
        if (!lua_isfunction(mState, -1)) {
            lua_pop(mState, 1);
            lua_getglobal(mState, "push_warning");
            lua_pushstring(mState, "There's is no 'on_update' function! Stopping...");
            lua_pcall(mState, 1, 0, 0);
            Stop();
            return;
        }
        CheckResult(lua_pcall(mState, 0, 0, 0));
    }

    void luaScript::AddFunction(lua_CFunction func, const char* name, const char* table)
    {
        assert(mState);
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
            const char* msg = lua_gettop(mState) > 0 ? lua_tostring(mState, -1) : "an error occurred";
            lua_pop(mState, 1);
            lua_getglobal(mState, "push_error");
            lua_pushstring(mState, msg);
            lua_pcall(mState, 1, 0, 0);
            Stop();
        }
    }
}
