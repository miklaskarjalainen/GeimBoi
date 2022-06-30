#pragma once

struct lua_State;

namespace GeimBoi
{
    class luaScript;

    namespace LuaApi
    {
        void OpenLibs(GeimBoi::luaScript* state);
    }
}

