#pragma once

struct lua_State;

// imgui wrapper for lua
namespace GeimBoi
{
    namespace apiGui
    {
        int begin(lua_State* state); // (string)
        int end(lua_State* state);
        int button(lua_State* state); // (string)
    }
}
