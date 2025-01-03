#ifndef LUA_WINDOW_H
#define LUA_WINDOW_H

#include "core/core.h"

#include "renderer/window.h"

#include "lua_state.h"

void lua_register_window(lua_State *L);

LS_EXPORT void lua_push_window(lua_State *L, LSWindow *window);
LS_EXPORT void lua_push_const_window(lua_State *L, const LSWindow *window);
LS_EXPORT bool lua_is_window(lua_State *L, int index);
LS_EXPORT LSWindow *lua_check_window(lua_State *L, int index);
LS_EXPORT const LSWindow *lua_check_const_window(lua_State *L, int index);
LS_EXPORT LSWindow *lua_to_window(lua_State *L, int index);
LS_EXPORT const LSWindow *lua_to_const_window(lua_State *L, int index);

#endif // LUA_WINDOW_H