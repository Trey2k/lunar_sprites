#include "lua_color.h"
#include "core/memory.h"
#include "core/types/string.h"
#include "lua.h"
#include "modules/lua/lua_state.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static int lua_new_color(lua_State *L) {
	int32 argc = lua_gettop(L);
	switch (argc) {
		case 3: {
			float32 r = luaL_checknumber(L, 1);
			float32 g = luaL_checknumber(L, 2);
			float32 b = luaL_checknumber(L, 3);
			Color c = { r, g, b, 1.0f };
			lua_push_color(L, c);
			return 1;
		} break;

		case 4: {
			float32 r = luaL_checknumber(L, 1);
			float32 g = luaL_checknumber(L, 2);
			float32 b = luaL_checknumber(L, 3);
			float32 a = luaL_checknumber(L, 4);
			Color c = { r, g, b, a };
			lua_push_color(L, c);
			return 1;
		} break;

		default: {
			{
				char error[80];
				ls_sprintf(error, 80,
						"invalid arguments to color constructor, expected 3 or 4 arguments, got %d",
						argc);
				return luaL_error(L, error);
			}
			break;
		}
	}
}

static int lua_color_tostring(lua_State *L) {
	Color c = lua_check_color(L, 1);
	char *str = ls_str_format("Color[%f, %f, %f, %f]", c.r, c.g, c.b, c.a);
	lua_pushstring(L, str);
	ls_free(str);
	return 1;
}

static int lua_color_index(lua_State *L) {
	Color c = lua_check_color(L, 1);
	const char *key = luaL_checkstring(L, 2);
	if (ls_str_equals(key, "r")) {
		lua_pushnumber(L, c.r);
		return 1;
	} else if (ls_str_equals(key, "g")) {
		lua_pushnumber(L, c.g);
		return 1;
	} else if (ls_str_equals(key, "b")) {
		lua_pushnumber(L, c.b);
		return 1;
	} else if (ls_str_equals(key, "a")) {
		lua_pushnumber(L, c.a);
		return 1;
	}

	return luaL_error(L, "invalid index '%s' for color", key);
}

static int lua_color_newindex(lua_State *L) {
	Color *c = lua_check_color_ptr(L, 1);
	const char *key = luaL_checkstring(L, 2);
	if (ls_str_equals(key, "r")) {
		c->r = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "g")) {
		c->g = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "b")) {
		c->b = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "a")) {
		c->a = luaL_checknumber(L, 3);
		return 0;
	}

	return luaL_error(L, "invalid index '%s' for color", key);
}

static int lua_color_add(lua_State *L) {
	Color c1 = lua_check_color(L, 1);
	Color c2 = lua_check_color(L, 2);
	lua_push_color(L, color_add(c1, c2));
	return 1;
}

static int lua_color_subtract(lua_State *L) {
	Color c1 = lua_check_color(L, 1);
	Color c2 = lua_check_color(L, 2);
	lua_push_color(L, color_subtract(c1, c2));
	return 1;
}

static int lua_color_multiply(lua_State *L) {
	Color c1 = lua_check_color(L, 1);
	Color c2 = lua_check_color(L, 2);
	lua_push_color(L, color_multiply(c1, c2));
	return 1;
}

static int lua_color_divide(lua_State *L) {
	Color c1 = lua_check_color(L, 1);
	Color c2 = lua_check_color(L, 2);
	lua_push_color(L, color_divide(c1, c2));
	return 1;
}

static int lua_color_equals(lua_State *L) {
	Color c1 = lua_check_color(L, 1);
	Color c2 = lua_check_color(L, 2);
	lua_pushboolean(L, color_equals(c1, c2));
	return 1;
}

static const luaL_Reg color_meta_methods[] = {
	{ "__tostring", lua_color_tostring },
	{ "__index", lua_color_index },
	{ "__newindex", lua_color_newindex },
	{ "__add", lua_color_add },
	{ "__sub", lua_color_subtract },
	{ "__mul", lua_color_multiply },
	{ "__div", lua_color_divide },
	{ "__eq", lua_color_equals },
	{ NULL, NULL }
};

void lua_register_color(lua_State *L) {
	luaL_newmetatable(L, "MT_COLOR");
	luaL_setfuncs(L, color_meta_methods, 0);
	lua_pop(L, 1);

	lua_register(L, "color", lua_new_color);
}

void lua_push_color(lua_State *L, Color c) {
	Color *color = lua_newuserdata(L, sizeof(Color));
	*color = c;
	luaL_getmetatable(L, "MT_COLOR");
	lua_setmetatable(L, -2);
}

bool lua_is_color(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_COLOR");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

Color lua_check_color(lua_State *L, int index) {
	return *lua_check_color_ptr(L, index);
}

Color *lua_check_color_ptr(lua_State *L, int index) {
	return luaL_checkudata(L, index, "MT_COLOR");
}

Color lua_to_color(lua_State *L, int index) {
	return *lua_to_color_ptr(L, index);
}

Color *lua_to_color_ptr(lua_State *L, int index) {
	return lua_touserdata(L, index);
}
