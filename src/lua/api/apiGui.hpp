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

        int begin_tooltip(lua_State* state);        // () -> void
        int end_tooltip(lua_State* state);          // () -> void
        int begin_main_menu_bar(lua_State* state);  // () -> bool
        int end_main_menu_bar(lua_State* state);    // () -> void
        int begin_menu(lua_State* state);           // (text: string, shortcut := "") -> bool
        int end_menu(lua_State* state);             // () -> void

        int collapsing_header(lua_State* state);// (text: string) -> bool
        int button(lua_State* state);           // (text: string, size := {-1, -1}) -> bool
        int checkbox(lua_State* state);         // (text: string, state: bool) -> bool
        int slider_float(lua_State* state);     // (text: string, value: float, min_value: float, max_value: float) -> float
        int slider_int(lua_State* state);       // (text: string, value: int  , min_value: int  , max_value: int) -> int

        int text(lua_State* state);         // (text: string) -> void
        int text_wrapped(lua_State* state); // (text: string) -> void
        int bullet_text(lua_State* state);  // (text: string) -> void

        int sameline(lua_State* state);     // () -> void
        int separator(lua_State* state);    // () -> void
        int is_item_hovered(lua_State* state);    // () -> bool
        int set_tooltip(lua_State* state);    // (text: string) -> void
    
    }
}
