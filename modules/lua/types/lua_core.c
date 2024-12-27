#include "lua_core.h"

#include "core/input/keycodes.h"
#include "lua_event_manager.h"
#include "lua_input_manager.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static const luaL_Reg core_meta_methods[] = {
	{ NULL, NULL }
};

static int32 lua_keycode_to_string(lua_State *L) {
	LSKeycode keycode = luaL_checkinteger(L, 1);
	lua_pushstring(L, keycode_to_string(keycode));
	return 1;
}

static int32 lua_mouse_button_to_string(lua_State *L) {
	LSMouseButton button = luaL_checkinteger(L, 1);
	lua_pushstring(L, mouse_button_to_string(button));
	return 1;
}

extern void lua_core_init_constants(lua_State *L, int32 table_index);

static void lua_core_set_fields(LSCore *core, lua_State *L, int32 table_index) {
	lua_push_input_manager(L, core_get_input_manager(core));
	lua_setfield(L, table_index, "input_manager");
	lua_push_event_manager(L, core_get_event_manager(core));
	lua_setfield(L, table_index, "event_manager");
}

static void lua_core_set_methods(lua_State *L, int32 table_index) {
	lua_pushcfunction(L, lua_keycode_to_string);
	lua_setfield(L, table_index, "keycode_to_string");
	lua_pushcfunction(L, lua_mouse_button_to_string);
	lua_setfield(L, table_index, "mouse_button_to_string");
}

void lua_register_core(LSCore *core, lua_State *L) {
	luaL_newmetatable(L, "MT_CORE");

	lua_newtable(L);
	lua_core_init_constants(L, -2);
	lua_core_set_fields(core, L, -2);
	lua_core_set_methods(L, -2);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, core_meta_methods, 0);
	lua_pop(L, 1);
}

void lua_push_core(lua_State *L, LSCore *core) {
	LSCore **udata = (LSCore **)lua_newuserdata(L, sizeof(LSCore *));
	*udata = core;

	luaL_getmetatable(L, "MT_CORE");
	lua_setmetatable(L, -2);
}

bool lua_is_core(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_CORE");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

LSCore *lua_check_core(lua_State *L, int index) {
	return *(LSCore **)luaL_checkudata(L, index, "MT_CORE");
}

LSCore *lua_to_core(lua_State *L, int index) {
	return *(LSCore **)lua_touserdata(L, index);
}