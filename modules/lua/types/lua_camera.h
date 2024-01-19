#ifndef LUA_CAMERA_H
#define LUA_CAMERA_H

#include "core/core.h"

#include "renderer/camera.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

void lua_register_camera(lua_State *L);

LS_EXPORT void lua_push_camera(lua_State *L, Camera *camera);
LS_EXPORT bool lua_is_camera(lua_State *L, int index);
LS_EXPORT Camera *lua_check_camera(lua_State *L, int index);
LS_EXPORT Camera *lua_to_camera(lua_State *L, int index);

#endif // LUA_CAMERA_H