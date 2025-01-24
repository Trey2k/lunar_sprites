#include "lua_state.h"
#include "types/lua_types.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

int32 ls_lua_error_handler(lua_State *L) {
	const char *msg = lua_tostring(L, -1);
	luaL_traceback(L, L, msg, 2);
	lua_remove(L, -2);
	return 1;
}

lua_State *ls_lua_new_settings_state() {
	lua_State *settings_state = luaL_newstate();
	luaL_openlibs(settings_state);

	return settings_state;
}

lua_State *ls_lua_new_application_state(LSCore *core) {
	lua_State *application_state = luaL_newstate();
	luaL_openlibs(application_state);

	lua_register_types(core, application_state);

	lua_push_core(application_state, core);
	lua_setglobal(application_state, "CORE");

	return application_state;
}

bool ls_lua_dostring(lua_State *L, String string) {
	lua_pushcfunction(L, ls_lua_error_handler);

	int32 err = luaL_loadstring(L, string);
	if (err != LUA_OK) {
		ls_printf("Error(%d) loading string: %s\n", err, lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}

	err = lua_pcall(L, 0, 0, -2);
	if (err != LUA_OK) {
		ls_printf("Error(%d) running string: %s\n", err, lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}

	lua_pop(L, 1);

	return true;
}

bool ls_lua_dofile(lua_State *L, String filename) {
	lua_pushcfunction(L, ls_lua_error_handler);

	int32 err = luaL_loadfile(L, filename);
	if (err != LUA_OK) {
		ls_printf("Error(%d) loading file: %s\n", err, lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}

	err = lua_pcall(L, 0, 0, -2);
	if (err != LUA_OK) {
		ls_printf("Error(%d) running file: %s\n", err, lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}

	lua_pop(L, 1);

	return true;
}

bool ls_lua_call(lua_State *L, int nargs, int nresults, int error_handler_index) {
	int32 err = lua_pcall(L, nargs, nresults, error_handler_index);
	if (err != LUA_OK) {
		ls_printf("Error(%d) calling function: %s\n", err, lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}

	return true;
}

void lua_push_error(lua_State *L, String error) {
	lua_pushstring(L, error);
	lua_error(L);
}