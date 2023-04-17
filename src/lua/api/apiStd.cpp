#include <cstdio>
#include <lua.hpp>
#include "apiStd.hpp"

using namespace GeimBoi;

int LuaApi::print(lua_State* state)
{
    const char* msg = luaL_checkstring(state, -1);

    printf("\033[0;34m" "[Lua] %s\n" "\033[0m", msg);
    return 0;
}
