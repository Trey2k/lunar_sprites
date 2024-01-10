#ifndef LUA_TYPES_H
#define LUA_TYPES_H

#include "lua_core.h"
#include "lua_matrix.h"
#include "lua_renderer.h"
#include "lua_vector.h"
#include "lua_window.h"

void lua_register_types(lua_State *L);

#endif // LUA_TYPES_H