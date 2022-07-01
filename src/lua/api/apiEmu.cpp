#include <cstdio>
#include <memory>
#include <lua/lua.hpp>
#include "apiEmu.hpp"
#include "../../core/gbGameBoy.hpp"

using namespace GeimBoi;

extern std::shared_ptr<gbGameBoy> gGameBoy;

int apiEmu::reset(lua_State*)
{
    gGameBoy->Reset();
    return 0;
}

int apiEmu::set_pause(lua_State* state)
{
    bool s = lua_toboolean(state, -1);
    gGameBoy->Paused = s;
    return 0;
}

int apiEmu::is_paused(lua_State* state)
{
    lua_pushboolean(state, gGameBoy->Paused);
    return 1;
}

int apiEmu::read_byte(lua_State* state)
{
    int addr = luaL_checkinteger(state, -1);
    lua_pushinteger(state, gGameBoy->ReadByte(addr));
    return 1;
}

int apiEmu::write_byte(lua_State* state)
{
    int addr = luaL_checkinteger(state, -2);
    int number = luaL_checkinteger(state, -1);
    gGameBoy->WriteByte(addr, number);
    return 0;
}

int apiEmu::load_rom(lua_State* state)
{
    const char* fp = luaL_checkstring(state, -1);
    gGameBoy->LoadRom(fp);
    return 0;
}

int apiEmu::set_button(lua_State* state)
{
    int btn = luaL_checkinteger(state, -2);
    bool btn_state = lua_toboolean(state, -1);
    if (btn_state)
        gGameBoy->PressButton((gbButton)btn);
    else
        gGameBoy->ReleaseButton((gbButton)btn);

    return 0;
}

int apiEmu::get_button(lua_State* state)
{
    int btn = luaL_checkinteger(state, -1);
    bool btn_state = gGameBoy->IsButtonPressed((gbButton)btn);
    lua_pushboolean(state, btn_state);
    return 1;
}
