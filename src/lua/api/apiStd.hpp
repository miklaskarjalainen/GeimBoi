#pragma once

struct lua_State;

namespace GeimBoi
{
    namespace LuaApi
    {
        int print(lua_State* state);
        int push_error(lua_State* state);
        int push_warning(lua_State* state);
    }
}
