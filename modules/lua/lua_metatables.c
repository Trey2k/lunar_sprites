#include "lua_metatables.h"

#include "core/core.h"

static int lua_vector2_eq(lua_State *L) {
	Vector2 *v1 = luaL_checkudata(L, 1, "MT_VECTOR2");
	Vector2 *v2 = luaL_checkudata(L, 2, "MT_VECTOR2");
	lua_pushboolean(L, vec2_equals(*v1, *v2));
	return 1;
}

static int lua_vector2_add(lua_State *L) {
	Vector2 *v1 = luaL_checkudata(L, 1, "MT_VECTOR2");
	Vector2 *v2 = luaL_checkudata(L, 2, "MT_VECTOR2");
	Vector2 *v3 = lua_newuserdata(L, sizeof(Vector2));
	*v3 = vec2_add(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR2");
	return 1;
}

static int lua_vector2_sub(lua_State *L) {
	Vector2 *v1 = luaL_checkudata(L, 1, "MT_VECTOR2");
	Vector2 *v2 = luaL_checkudata(L, 2, "MT_VECTOR2");
	Vector2 *v3 = lua_newuserdata(L, sizeof(Vector2));
	*v3 = vec2_sub(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR2");
	return 1;
}

static int lua_vector2_mul(lua_State *L) {
	Vector2 *v1 = luaL_checkudata(L, 1, "MT_VECTOR2");
	Vector2 *v2 = luaL_checkudata(L, 2, "MT_VECTOR2");
	Vector2 *v3 = lua_newuserdata(L, sizeof(Vector2));
	*v3 = vec2_mul(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR2");
	return 1;
}

static int lua_vector2_div(lua_State *L) {
	Vector2 *v1 = luaL_checkudata(L, 1, "MT_VECTOR2");
	Vector2 *v2 = luaL_checkudata(L, 2, "MT_VECTOR2");
	Vector2 *v3 = lua_newuserdata(L, sizeof(Vector2));
	*v3 = vec2_div(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR2");
	return 1;
}

static int lua_vector2_tostring(lua_State *L) {
	Vector2 *v = luaL_checkudata(L, 1, "MT_VECTOR2");
	lua_pushfstring(L, "Vector2(%f, %f)", v->x, v->y);
	return 1;
}

static int lua_vecto2_index(lua_State *L) {
	Vector2 *v = luaL_checkudata(L, 1, "MT_VECTOR2");
	const char *key = luaL_checkstring(L, 2);
	if (ls_str_equals(key, "x") == 0) {
		lua_pushnumber(L, v->x);
	} else if (ls_str_equals(key, "y") == 0) {
		lua_pushnumber(L, v->y);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int lua_vector2_newindex(lua_State *L) {
	Vector2 *v = luaL_checkudata(L, 1, "MT_VECTOR2");
	const char *key = luaL_checkstring(L, 2);
	if (ls_str_equals(key, "x") == 0) {
		v->x = luaL_checknumber(L, 3);
	} else if (ls_str_equals(key, "y") == 0) {
		v->y = luaL_checknumber(L, 3);
	}
	return 0;
}

static const luaL_Reg vector2_meta_methods[] = {
	{ "__eq", lua_vector2_eq },
	{ "__add", lua_vector2_add },
	{ "__sub", lua_vector2_sub },
	{ "__mul", lua_vector2_mul },
	{ "__div", lua_vector2_div },
	{ "__tostring", lua_vector2_tostring },
	{ "__index", lua_vecto2_index },
	{ "__newindex", lua_vector2_newindex },
	{ NULL, NULL }
};

static int lua_new_vector2(lua_State *L) {
	Vector2 *v = lua_newuserdata(L, sizeof(Vector2));
	v->x = luaL_checknumber(L, 1);
	v->y = luaL_checknumber(L, 2);
	luaL_setmetatable(L, "MT_VECTOR2");
	return 1;
}

void lua_register_metatables(lua_State *L) {
	luaL_newmetatable(L, "MT_VECTOR2");
	luaL_setfuncs(L, vector2_meta_methods, 0);
	lua_pop(L, 1);

	lua_pushcfunction(L, lua_new_vector2);
	lua_setglobal(L, "vec2");
}