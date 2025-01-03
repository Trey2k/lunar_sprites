#ifndef LUA_VECTOR_H
#define LUA_VECTOR_H

#include "core/core.h"

#include "lua_state.h"

void lua_register_vector2(lua_State *L);

LS_EXPORT void lua_push_vector2(lua_State *L, Vector2 v);
LS_EXPORT bool lua_is_vector2(lua_State *L, int index);
LS_EXPORT Vector2 lua_check_vector2(lua_State *L, int index);
LS_EXPORT Vector2 lua_to_vector2(lua_State *L, int index);

void lua_register_vector2i(lua_State *L);

LS_EXPORT void lua_push_vector2i(lua_State *L, Vector2i v);
LS_EXPORT bool lua_is_vector2i(lua_State *L, int index);
LS_EXPORT Vector2i lua_check_vector2i(lua_State *L, int index);
LS_EXPORT Vector2i lua_to_vector2i(lua_State *L, int index);

void lua_register_vector2u(lua_State *L);

LS_EXPORT void lua_push_vector2u(lua_State *L, Vector2u v);
LS_EXPORT bool lua_is_vector2u(lua_State *L, int index);
LS_EXPORT Vector2u lua_check_vector2u(lua_State *L, int index);
LS_EXPORT Vector2u lua_to_vector2u(lua_State *L, int index);

void lua_register_vector3(lua_State *L);

LS_EXPORT void lua_push_vector3(lua_State *L, Vector3 v);
LS_EXPORT bool lua_is_vector3(lua_State *L, int index);
LS_EXPORT Vector3 lua_check_vector3(lua_State *L, int index);
LS_EXPORT Vector3 lua_to_vector3(lua_State *L, int index);

void lua_register_vector3i(lua_State *L);

LS_EXPORT void lua_push_vector3i(lua_State *L, Vector3i v);
LS_EXPORT bool lua_is_vector3i(lua_State *L, int index);
LS_EXPORT Vector3i lua_check_vector3i(lua_State *L, int index);
LS_EXPORT Vector3i lua_to_vector3i(lua_State *L, int index);

void lua_register_vector3u(lua_State *L);

LS_EXPORT void lua_push_vector3u(lua_State *L, Vector3u v);
LS_EXPORT bool lua_is_vector3u(lua_State *L, int index);
LS_EXPORT Vector3u lua_check_vector3u(lua_State *L, int index);
LS_EXPORT Vector3u lua_to_vector3u(lua_State *L, int index);

#endif // LUA_VECTOR_H