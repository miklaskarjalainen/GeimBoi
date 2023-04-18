#pragma once

struct lua_State;

namespace GeimBoi
{
    namespace apiEmu
    {
        int reset(lua_State* state);     // () -> void
        int set_pause(lua_State* state); // (state: bool) -> void
        int is_paused(lua_State* state); // () -> bool
        int read_byte(lua_State* state); // (addr: u16) -> u8
        int write_byte(lua_State* state); // (addr: u16, data: u8) -> void
        int load_rom(lua_State* state);  // (filepath: string) -> void

        int set_button(lua_State* state); // (btn: int, state: bool)
        int get_button(lua_State* state); // (btn: int) -> bool
    }
}
