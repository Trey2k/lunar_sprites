#ifndef LUA_WINDOW_H
#define LUA_WINDOW_H

#include "core/core.h"

#include "renderer/window.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

void lua_register_window(lua_State *L);
int lua_new_window(lua_State *L);

LS_EXPORT void lua_push_window(lua_State *L, LSWindow *window);
LS_EXPORT bool lua_is_window(lua_State *L, int index);
LS_EXPORT LSWindow *lua_check_window(lua_State *L, int index);
LS_EXPORT LSWindow *lua_to_window(lua_State *L, int index);

#endif // LUA_WINDOW_H