#include "lua_application.h"
#include "lua_metatables.h"

#include "core/core.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static int lua_error_handler(lua_State *L) {
	const char *msg = lua_tostring(L, -1);
	luaL_traceback(L, L, msg, 2);
	lua_remove(L, -2);
	return 1;
}

void ls_lua_dostring(lua_State *L, const char *str) {
	lua_pushcfunction(L, lua_error_handler);

	int32 err = luaL_loadstring(L, str);
	if (err != LUA_OK) {
		ls_printf("Error(%d) loading string: %s\n", err, lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	err = lua_pcall(L, 0, 0, -2);
	if (err != LUA_OK) {
		ls_printf("Error(%d) running string: %s\n", err, lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	lua_pop(L, 1);
}

void lua_app_init() {
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	lua_register_metatables(L);
	ls_lua_dostring(L, "v1 = vec2(1, 32) v2 = vec2(1.5, 0.5) print(v1, v2, v1*v2)");

	lua_close(L);
}