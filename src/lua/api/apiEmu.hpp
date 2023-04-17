#pragma once

struct lua_State;

namespace GeimBoi
{
    namespace apiEmu
    {
        int reset(lua_State* state);
        int set_pause(lua_State* state); // (bool)
        int is_paused(lua_State* state); 
        int read_byte(lua_State* state); // (int)
        int write_byte(lua_State* state); // (int, int)
        int load_rom(lua_State* state);  // (string)

        int set_button(lua_State* state); // (int, bool)
        int get_button(lua_State* state); // (int)
    }
}
