#ifndef LUA_COLOR_H
#define LUA_COLOR_H

#include "core/core.h"

#include "lua_state.h"

void lua_register_color(lua_State *L);

LS_EXPORT void lua_push_color(lua_State *L, Color c);
LS_EXPORT bool lua_is_color(lua_State *L, int index);
LS_EXPORT Color lua_check_color(lua_State *L, int index);
LS_EXPORT Color *lua_check_color_ptr(lua_State *L, int index);
LS_EXPORT Color lua_to_color(lua_State *L, int index);
LS_EXPORT Color *lua_to_color_ptr(lua_State *L, int index);

#endif // LUA_COLOR_H