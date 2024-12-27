#include "lua_input_manager.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static const luaL_Reg input_manager_meta_methods[] = {
	{ NULL, NULL }
};

static int32 lua_input_is_key_pressed(lua_State *L) {
	InputManager *input_manager = lua_check_input_manager(L, 1);
	LSKeycode key = luaL_checkinteger(L, 2);
	lua_pushboolean(L, input_is_key_pressed(input_manager, key));
	return 1;
}

static int32 lua_input_is_key_just_pressed(lua_State *L) {
	InputManager *input_manager = lua_check_input_manager(L, 1);
	LSKeycode key = luaL_checkinteger(L, 2);
	lua_pushboolean(L, input_is_key_just_pressed(input_manager, key));
	return 1;
}

static int32 lua_input_is_key_released(lua_State *L) {
	InputManager *input_manager = lua_check_input_manager(L, 1);
	LSKeycode key = luaL_checkinteger(L, 2);
	lua_pushboolean(L, input_is_key_released(input_manager, key));
	return 1;
}

static int32 lua_input_is_key_just_released(lua_State *L) {
	InputManager *input_manager = lua_check_input_manager(L, 1);
	LSKeycode key = luaL_checkinteger(L, 2);
	lua_pushboolean(L, input_is_key_just_released(input_manager, key));
	return 1;
}

static int32 lua_input_is_mouse_pressed(lua_State *L) {
	InputManager *input_manager = lua_check_input_manager(L, 1);
	LSMouseButton button = luaL_checkinteger(L, 2);
	lua_pushboolean(L, input_is_mouse_pressed(input_manager, button));
	return 1;
}

static int32 lua_input_is_mouse_just_pressed(lua_State *L) {
	InputManager *input_manager = lua_check_input_manager(L, 1);
	LSMouseButton button = luaL_checkinteger(L, 2);
	lua_pushboolean(L, input_is_mouse_just_pressed(input_manager, button));
	return 1;
}

static int32 lua_input_is_mouse_released(lua_State *L) {
	InputManager *input_manager = lua_check_input_manager(L, 1);
	LSMouseButton button = luaL_checkinteger(L, 2);
	lua_pushboolean(L, input_is_mouse_released(input_manager, button));
	return 1;
}

static int32 lua_input_is_mouse_just_released(lua_State *L) {
	InputManager *input_manager = lua_check_input_manager(L, 1);
	LSMouseButton button = luaL_checkinteger(L, 2);
	lua_pushboolean(L, input_is_mouse_just_released(input_manager, button));
	return 1;
}

static void lua_input_manager_set_methods(lua_State *L, int32 table_index) {
	lua_pushcfunction(L, lua_input_is_key_pressed);
	lua_setfield(L, table_index, "is_key_pressed");

	lua_pushcfunction(L, lua_input_is_key_just_pressed);
	lua_setfield(L, table_index, "is_key_just_pressed");

	lua_pushcfunction(L, lua_input_is_key_released);
	lua_setfield(L, table_index, "is_key_released");

	lua_pushcfunction(L, lua_input_is_key_just_released);
	lua_setfield(L, table_index, "is_key_just_released");

	lua_pushcfunction(L, lua_input_is_mouse_pressed);
	lua_setfield(L, table_index, "is_mouse_pressed");

	lua_pushcfunction(L, lua_input_is_mouse_just_pressed);
	lua_setfield(L, table_index, "is_mouse_just_pressed");

	lua_pushcfunction(L, lua_input_is_mouse_released);
	lua_setfield(L, table_index, "is_mouse_released");

	lua_pushcfunction(L, lua_input_is_mouse_just_released);
	lua_setfield(L, table_index, "is_mouse_just_released");
}

void lua_register_input_manager(lua_State *L) {
	luaL_newmetatable(L, "MT_INPUT_MANAGER");

	lua_newtable(L);
	lua_input_manager_set_methods(L, -2);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, input_manager_meta_methods, 0);
	lua_pop(L, 1);
}

void lua_push_input_manager(lua_State *L, InputManager *input_manager) {
	InputManager **udata = (InputManager **)lua_newuserdata(L, sizeof(InputManager *));
	*udata = input_manager;

	luaL_getmetatable(L, "MT_INPUT_MANAGER");
	lua_setmetatable(L, -2);
}

bool lua_is_input_manager(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_INPUT_MANAGER");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

InputManager *lua_check_input_manager(lua_State *L, int index) {
	return *(InputManager **)luaL_checkudata(L, index, "MT_INPUT_MANAGER");
}

InputManager *lua_to_input_manager(lua_State *L, int index) {
	return *(InputManager **)lua_touserdata(L, index);
}