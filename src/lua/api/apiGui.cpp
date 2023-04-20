#include <cstdio>
#include <lua.hpp>
#include "apiGui.hpp"
#include "../../application/imgui/imgui.h"

using namespace GeimBoi;

static ImVec2 luaL_checkimvec2(lua_State* state, int n) {
    // todo: error handling
    ImVec2 out;
    if (lua_istable(state, n)) {
        // get X
        lua_rawgeti(state, -1, 1);
        out.x = luaL_checkinteger(state, -1);
        lua_pop(state, 1);

        // get Y
        lua_rawgeti(state, -1, 2);
        out.y = luaL_checkinteger(state, -1);
        lua_pop(state, 1);
    }
    return out;
}

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

int apiGui::begin_tooltip(lua_State* state)
{
    ImGui::BeginTooltip();
    return 0;
}

int apiGui::end_tooltip(lua_State* state)
{
    ImGui::EndTooltip();
    return 0;
}

int apiGui::begin_main_menu_bar(lua_State* state)
{
    lua_pushboolean(state, ImGui::BeginMainMenuBar());
    return 1;
}

int apiGui::end_main_menu_bar(lua_State*)
{
    ImGui::EndMainMenuBar();
    return 0;
}

int apiGui::begin_menu(lua_State* state)
{
    const char* shortcut = nullptr;
    if (lua_gettop(state) == 2) {
        shortcut = luaL_checkstring(state, -1);
        lua_pop(state, -1);
    }
    const char* Title = luaL_checkstring(state, -1);
    lua_pushboolean(state, ImGui::BeginMenu(Title, shortcut));
    return 1;
}

int apiGui::end_menu(lua_State*)
{
    ImGui::EndMainMenuBar();
    return 0;
}

int apiGui::collapsing_header(lua_State* state)
{
    const char* Text = luaL_checkstring(state, -1);
    lua_pushboolean(state, ImGui::CollapsingHeader(Text));
    return 1;
}

int apiGui::button(lua_State* state)
{
    const int ArgCount = lua_gettop(state);
    ImVec2 buttonSize = { 0, 0 };
    if (ArgCount == 2) {
        buttonSize = luaL_checkimvec2(state, -1);
        lua_pop(state, 1);
    }

    const char* ButtonText = luaL_checkstring(state, -1);
    lua_pushboolean(state, ImGui::Button(ButtonText, buttonSize));
    return 1;
}

int apiGui::checkbox(lua_State* state)
{
    const char* Text = luaL_checkstring(state, -2);
    if (!lua_isboolean(state, -1)) {
        luaL_error(state, "expected boolean");
        return 0;
    }
    bool checkbox_state = lua_toboolean(state, -1);
    ImGui::Checkbox(Text, &checkbox_state);

    lua_pop(state, -1);
    lua_pushboolean(state, checkbox_state);
    return 1;
}

int apiGui::slider_float(lua_State* state)
{
    const char* Text = luaL_checkstring(state, -4);
    float value = luaL_checknumber(state, -3);
    const float MinValue = luaL_checknumber(state, -2);
    const float MaxValue = luaL_checknumber(state, -1);
    ImGui::SliderFloat(Text, &value, MinValue, MaxValue);
    lua_pushnumber(state, value);
    return 1;
}

int apiGui::slider_int(lua_State* state)
{
    const char* Text = luaL_checkstring(state, -4);
    int value = luaL_checkinteger(state, -3);
    const int MinValue = luaL_checkinteger(state, -2);
    const int MaxValue = luaL_checkinteger(state, -1);
    ImGui::SliderInt(Text, &value, MinValue, MaxValue);
    lua_pushinteger(state, value);
    return 1;
}

int apiGui::text(lua_State* state)
{
    const char* Text = luaL_checkstring(state, -1);
    ImGui::Text(Text);
    return 0;
}

int apiGui::text_wrapped(lua_State* state) 
{
    const char* Text = luaL_checkstring(state, -1);
    ImGui::TextWrapped(Text);
    return 0;
}

int apiGui::bullet_text(lua_State* state)
{
    const char* Text = luaL_checkstring(state, -1);
    ImGui::BulletText(Text);
    return 0;
}

int apiGui::sameline(lua_State*)
{
    ImGui::SameLine();
    return 0;
}

int apiGui::separator(lua_State*)
{
    ImGui::Separator();
    return 0;
}

int apiGui::is_item_hovered(lua_State* state)
{
    lua_pushboolean(state, ImGui::IsItemHovered());
    return 1;
}

int apiGui::set_tooltip(lua_State* state)
{
    const char* Text = luaL_checkstring(state, -1);
    ImGui::SetTooltip(Text);
    return 0;
}
