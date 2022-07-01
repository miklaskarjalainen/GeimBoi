#include "luaApi.hpp"
#include "apiEmu.hpp"
#include "apiGui.hpp"
#include "apiStd.hpp"
#include "../luaScript.hpp"

using namespace GeimBoi;

void LuaApi::OpenLibs(luaScript* script)
{
    script->AddFunction(LuaApi::print, "print");
    
    // emu
    script->AddFunction(apiEmu::reset, "reset", "emu");
    script->AddFunction(apiEmu::set_pause, "set_pause", "emu");
    script->AddFunction(apiEmu::is_paused, "is_paused", "emu");
    script->AddFunction(apiEmu::read_byte, "read_byte", "emu");
    script->AddFunction(apiEmu::write_byte, "write_byte", "emu");
    script->AddFunction(apiEmu::load_rom, "load_rom", "emu");
    script->AddFunction(apiEmu::set_button, "set_button", "emu");
    script->AddFunction(apiEmu::get_button, "get_button", "emu");

    // ImGui
    script->AddFunction(apiGui::begin, "begin", "imgui");
    // FIXME: placeholder, "end" is a keyword in lua so this function cannot be called 
    script->AddFunction(apiGui::end, "endd", "imgui"); 
}
