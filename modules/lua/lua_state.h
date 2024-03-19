#ifndef LUA_STATE_H
#define LUA_STATE_H

#include "core/core.h"

#include "renderer/renderer.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

typedef struct lua_State lua_State;

LS_EXPORT lua_State *ls_lua_new_settings_state();
LS_EXPORT lua_State *ls_lua_new_application_state(LSCore *core, Renderer *renderer);

LS_EXPORT bool ls_lua_dostring(lua_State *L, String string);
LS_EXPORT bool ls_lua_dofile(lua_State *L, String filename);

LS_EXPORT bool ls_lua_call(lua_State *L, int nargs, int nresults, int error_handler_index);

LS_EXPORT int32 ls_lua_error_handler(lua_State *L);

LS_EXPORT void lua_push_error(lua_State *L, String error);

#endif // LUA_STATE_H