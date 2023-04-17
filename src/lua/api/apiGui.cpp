#include <cstdio>
#include <lua.hpp>
#include "apiGui.hpp"
#include "../../application/imgui/imgui.h"

using namespace GeimBoi;

int apiGui::begin(lua_State* state)
{
    const char* WindowTitle = luaL_checkstring(state, -1);
    bool r = ImGui::Begin(WindowTitle);
    lua_pushboolean(state, r);
    return 1;
}

int apiGui::end(lua_State*)
{
    ImGui::End();
    return 0;
}
