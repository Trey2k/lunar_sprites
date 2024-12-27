#ifndef LUA_MATRIX_H
#define LUA_MATRIX_H

#include "core/core.h"

#include "lua_state.h"

void lua_register_matrix4(lua_State *L);

LS_EXPORT void lua_push_matrix4(lua_State *L, Matrix4 m);
LS_EXPORT bool lua_is_matrix4(lua_State *L, int index);
LS_EXPORT Matrix4 lua_check_matrix4(lua_State *L, int index);
LS_EXPORT Matrix4 *lua_check_matrix4_ptr(lua_State *L, int index);
LS_EXPORT Matrix4 lua_to_matrix4(lua_State *L, int index);
LS_EXPORT Matrix4 *lua_to_matrix4_ptr(lua_State *L, int index);

#endif // LUA_MATRIX_H