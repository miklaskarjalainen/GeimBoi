#pragma once

struct lua_State;

// imgui wrapper for lua
namespace GeimBoi::apiGui
{
    int begin(lua_State* state);
    int end(lua_State* state);
}
