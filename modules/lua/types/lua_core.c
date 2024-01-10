#include "lua_core.h"

static Hashtable *methods = NULL;

static const luaL_Reg core_meta_methods[] = {
	{ NULL, NULL }
};

void lua_register_core(lua_State *L) {
	LS_ASSERT(methods == NULL);

	methods = hashtable_create(HASHTABLE_KEY_STRING, 1, false);

	luaL_newmetatable(L, "MT_CORE");
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
