#include "luaApi.hpp"
#include "apiEmu.hpp"
#include "apiGui.hpp"
#include "apiStd.hpp"
#include "../luaScript.hpp"

using namespace GeimBoi;

void LuaApi::OpenLibs(luaScript* script)
{
    script->AddFunction(LuaApi::print, "print");
    script->AddFunction(LuaApi::push_error, "push_error");
    script->AddFunction(LuaApi::push_warning, "push_warning");
    
    // emu
    script->AddFunction(apiEmu::reset, "reset", "emu");
    script->AddFunction(apiEmu::frameadvance, "frameadvance", "emu");
    script->AddFunction(apiEmu::set_pause, "set_pause", "emu");
    script->AddFunction(apiEmu::is_paused, "is_paused", "emu");
    script->AddFunction(apiEmu::read_byte, "read_byte", "emu");
    script->AddFunction(apiEmu::write_byte, "write_byte", "emu");
    script->AddFunction(apiEmu::load_rom, "load_rom", "emu");
    script->AddFunction(apiEmu::set_button, "set_button", "emu");
    script->AddFunction(apiEmu::get_button, "get_button", "emu");
    script->AddFunction(apiEmu::savestate, "savestate", "emu");
    script->AddFunction(apiEmu::loadstate, "loadstate", "emu");

    // ImGui
    script->AddFunction(apiGui::begin, "begin", "imgui");
    script->AddFunction(apiGui::end, "endd", "imgui"); // FIXME: placeholder, "end" is a keyword in lua so this function cannot be called 
    
    script->AddFunction(apiGui::begin_main_menu_bar, "begin_main_menu_bar", "imgui");
    script->AddFunction(apiGui::end_main_menu_bar, "end_main_menu_bar", "imgui");
    script->AddFunction(apiGui::begin_menu, "begin_menu", "imgui");
    script->AddFunction(apiGui::end_menu, "end_menu", "imgui");

    script->AddFunction(apiGui::button, "button", "imgui");
    script->AddFunction(apiGui::text, "text", "imgui");
    script->AddFunction(apiGui::text_wrapped, "text_wrapped", "imgui");
    script->AddFunction(apiGui::bullet_text, "bullet_text", "imgui");

    script->AddFunction(apiGui::sameline, "sameline", "imgui");
    script->AddFunction(apiGui::separator, "separator", "imgui");
}
