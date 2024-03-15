#ifndef LUA_CORE_H
#define LUA_CORE_H

#include "core/core.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

void lua_register_core(LSCore *core, lua_State *L);

LS_EXPORT void lua_push_core(lua_State *L, LSCore *core);
LS_EXPORT bool lua_is_core(lua_State *L, int index);
LS_EXPORT LSCore *lua_check_core(lua_State *L, int index);
LS_EXPORT LSCore *lua_to_core(lua_State *L, int index);

#endif // LUA_CORE_H