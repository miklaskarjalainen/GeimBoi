#pragma once

struct lua_State;

// imgui wrapper for lua
namespace GeimBoi
{
    namespace apiGui
    {
                                            // (function args) -> return type
        int begin(lua_State* state);        // (text: string) -> bool
        int end(lua_State* state);          // () -> void

        int begin_main_menu_bar(lua_State* state);  // () -> bool
        int end_main_menu_bar(lua_State* state);    // () -> void
        int begin_menu(lua_State* state);           // (text: string, shortcut := "") -> bool
        int end_menu(lua_State* state);             // () -> void

        int button(lua_State* state);       // (text: string, size := {-1, -1}) -> bool
        int text(lua_State* state);         // (text: string) -> void
        int text_wrapped(lua_State* state); // (text: string) -> void
        int bullet_text(lua_State* state); // (text: string) -> void

        int sameline(lua_State* state);     // () -> void
        int separator(lua_State* state);    // () -> void
    
    }
}
