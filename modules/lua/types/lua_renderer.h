#ifndef LUA_RENDERER_H
#define LUA_RENDERER_H

#include "core/core.h"

#include "renderer/renderer.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

void lua_register_renderer(lua_State *L);

LS_EXPORT void lua_push_renderer(lua_State *L, Renderer *renderer);
LS_EXPORT bool lua_is_renderer(lua_State *L, int index);
LS_EXPORT Renderer *lua_check_renderer(lua_State *L, int index);
LS_EXPORT Renderer *lua_to_renderer(lua_State *L, int index);

#endif // LUA_RENDERER_H