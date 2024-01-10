#include "lua_vector.h"

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

void lua_register_vector2(lua_State *L) {
	luaL_newmetatable(L, "MT_VECTOR2");

	luaL_setfuncs(L, vector2_meta_methods, 0);
	lua_pop(L, 1);

	lua_pushcfunction(L, lua_new_vector2);
	lua_setglobal(L, "vec2");
}

void lua_push_vector2(lua_State *L, Vector2 v) {
	Vector2 *vec = lua_newuserdata(L, sizeof(Vector2));
	*vec = v;
	luaL_setmetatable(L, "MT_VECTOR2");
}

bool lua_is_vector2(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_VECTOR2");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

Vector2 lua_check_vector2(lua_State *L, int index) {
	return *(Vector2 *)luaL_checkudata(L, index, "MT_VECTOR2");
}

Vector2 lua_to_vector2(lua_State *L, int index) {
	return *(Vector2 *)lua_touserdata(L, index);
}

static int lua_vector2i_eq(lua_State *L) {
	Vector2i *v1 = luaL_checkudata(L, 1, "MT_VECTOR2I");
	Vector2i *v2 = luaL_checkudata(L, 2, "MT_VECTOR2I");
	lua_pushboolean(L, vec2i_equals(*v1, *v2));
	return 1;
}

static int lua_vector2i_add(lua_State *L) {
	Vector2i *v1 = luaL_checkudata(L, 1, "MT_VECTOR2I");
	Vector2i *v2 = luaL_checkudata(L, 2, "MT_VECTOR2I");
	Vector2i *v3 = lua_newuserdata(L, sizeof(Vector2i));
	*v3 = vec2i_add(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR2I");
	return 1;
}

static int lua_vector2i_sub(lua_State *L) {
	Vector2i *v1 = luaL_checkudata(L, 1, "MT_VECTOR2I");
	Vector2i *v2 = luaL_checkudata(L, 2, "MT_VECTOR2I");
	Vector2i *v3 = lua_newuserdata(L, sizeof(Vector2i));
	*v3 = vec2i_sub(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR2I");
	return 1;
}

static int lua_vector2i_mul(lua_State *L) {
	Vector2i *v1 = luaL_checkudata(L, 1, "MT_VECTOR2I");
	Vector2i *v2 = luaL_checkudata(L, 2, "MT_VECTOR2I");
	Vector2i *v3 = lua_newuserdata(L, sizeof(Vector2i));
	*v3 = vec2i_mul(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR2I");
	return 1;
}

static int lua_vector2i_div(lua_State *L) {
	Vector2i *v1 = luaL_checkudata(L, 1, "MT_VECTOR2I");
	Vector2i *v2 = luaL_checkudata(L, 2, "MT_VECTOR2I");
	Vector2i *v3 = lua_newuserdata(L, sizeof(Vector2i));
	*v3 = vec2i_div(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR2I");
	return 1;
}

static int lua_vector2i_tostring(lua_State *L) {
	Vector2i *v = luaL_checkudata(L, 1, "MT_VECTOR2I");
	lua_pushfstring(L, "Vector2i(%d, %d)", v->x, v->y);
	return 1;
}

static int lua_vecto2i_index(lua_State *L) {
	Vector2i *v = luaL_checkudata(L, 1, "MT_VECTOR2I");
	const char *key = luaL_checkstring(L, 2);
	if (ls_str_equals(key, "x") == 0) {
		lua_pushinteger(L, v->x);
	} else if (ls_str_equals(key, "y") == 0) {
		lua_pushinteger(L, v->y);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int lua_vector2i_newindex(lua_State *L) {
	Vector2i *v = luaL_checkudata(L, 1, "MT_VECTOR2I");
	const char *key = luaL_checkstring(L, 2);
	if (ls_str_equals(key, "x") == 0) {
		v->x = luaL_checkinteger(L, 3);
	} else if (ls_str_equals(key, "y") == 0) {
		v->y = luaL_checkinteger(L, 3);
	}
	return 0;
}

static const luaL_Reg vector2i_meta_methods[] = {
	{ "__eq", lua_vector2i_eq },
	{ "__add", lua_vector2i_add },
	{ "__sub", lua_vector2i_sub },
	{ "__mul", lua_vector2i_mul },
	{ "__div", lua_vector2i_div },
	{ "__tostring", lua_vector2i_tostring },
	{ "__index", lua_vecto2i_index },
	{ "__newindex", lua_vector2i_newindex },
	{ NULL, NULL }
};

static int lua_new_vector2i(lua_State *L) {
	Vector2i *v = lua_newuserdata(L, sizeof(Vector2i));
	v->x = luaL_checkinteger(L, 1);
	v->y = luaL_checkinteger(L, 2);
	luaL_setmetatable(L, "MT_VECTOR2I");
	return 1;
}

void lua_register_vector2i(lua_State *L) {
	luaL_newmetatable(L, "MT_VECTOR2I");

	luaL_setfuncs(L, vector2i_meta_methods, 0);
	lua_pop(L, 1);

	lua_pushcfunction(L, lua_new_vector2i);
	lua_setglobal(L, "vec2i");
}

void lua_push_vector2i(lua_State *L, Vector2i v) {
	Vector2i *vec = lua_newuserdata(L, sizeof(Vector2i));
	*vec = v;
	luaL_setmetatable(L, "MT_VECTOR2I");
}

bool lua_is_vector2i(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_VECTOR2I");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

Vector2i lua_check_vector2i(lua_State *L, int index) {
	return *(Vector2i *)luaL_checkudata(L, index, "MT_VECTOR2I");
}

Vector2i lua_to_vector2i(lua_State *L, int index) {
	return *(Vector2i *)lua_touserdata(L, index);
}

static int lua_vector3_eq(lua_State *L) {
	Vector3 *v1 = luaL_checkudata(L, 1, "MT_VECTOR3");
	Vector3 *v2 = luaL_checkudata(L, 2, "MT_VECTOR3");
	lua_pushboolean(L, vec3_equals(*v1, *v2));
	return 1;
}

static int lua_vector3_add(lua_State *L) {
	Vector3 *v1 = luaL_checkudata(L, 1, "MT_VECTOR3");
	Vector3 *v2 = luaL_checkudata(L, 2, "MT_VECTOR3");
	Vector3 *v3 = lua_newuserdata(L, sizeof(Vector3));
	*v3 = vec3_add(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR3");
	return 1;
}

static int lua_vector3_sub(lua_State *L) {
	Vector3 *v1 = luaL_checkudata(L, 1, "MT_VECTOR3");
	Vector3 *v2 = luaL_checkudata(L, 2, "MT_VECTOR3");
	Vector3 *v3 = lua_newuserdata(L, sizeof(Vector3));
	*v3 = vec3_sub(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR3");
	return 1;
}

static int lua_vector3_mul(lua_State *L) {
	Vector3 *v1 = luaL_checkudata(L, 1, "MT_VECTOR3");
	Vector3 *v2 = luaL_checkudata(L, 2, "MT_VECTOR3");
	Vector3 *v3 = lua_newuserdata(L, sizeof(Vector3));
	*v3 = vec3_mul(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR3");
	return 1;
}

static int lua_vector3_div(lua_State *L) {
	Vector3 *v1 = luaL_checkudata(L, 1, "MT_VECTOR3");
	Vector3 *v2 = luaL_checkudata(L, 2, "MT_VECTOR3");
	Vector3 *v3 = lua_newuserdata(L, sizeof(Vector3));
	*v3 = vec3_div(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR3");
	return 1;
}

static int lua_vector3_tostring(lua_State *L) {
	Vector3 *v = luaL_checkudata(L, 1, "MT_VECTOR3");
	lua_pushfstring(L, "Vector3(%f, %f, %f)", v->x, v->y, v->z);
	return 1;
}

static int lua_vecto3_index(lua_State *L) {
	Vector3 *v = luaL_checkudata(L, 1, "MT_VECTOR3");
	const char *key = luaL_checkstring(L, 2);
	if (ls_str_equals(key, "x") == 0) {
		lua_pushnumber(L, v->x);
	} else if (ls_str_equals(key, "y") == 0) {
		lua_pushnumber(L, v->y);
	} else if (ls_str_equals(key, "z") == 0) {
		lua_pushnumber(L, v->z);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int lua_vector3_newindex(lua_State *L) {
	Vector3 *v = luaL_checkudata(L, 1, "MT_VECTOR3");
	const char *key = luaL_checkstring(L, 2);
	if (ls_str_equals(key, "x") == 0) {
		v->x = luaL_checknumber(L, 3);
	} else if (ls_str_equals(key, "y") == 0) {
		v->y = luaL_checknumber(L, 3);
	} else if (ls_str_equals(key, "z") == 0) {
		v->z = luaL_checknumber(L, 3);
	}
	return 0;
}

static const luaL_Reg vector3_meta_methods[] = {
	{ "__eq", lua_vector3_eq },
	{ "__add", lua_vector3_add },
	{ "__sub", lua_vector3_sub },
	{ "__mul", lua_vector3_mul },
	{ "__div", lua_vector3_div },
	{ "__tostring", lua_vector3_tostring },
	{ "__index", lua_vecto3_index },
	{ "__newindex", lua_vector3_newindex },
	{ NULL, NULL }
};

static int lua_new_vector3(lua_State *L) {
	Vector3 *v = lua_newuserdata(L, sizeof(Vector3));
	v->x = luaL_checknumber(L, 1);
	v->y = luaL_checknumber(L, 2);
	v->z = luaL_checknumber(L, 3);
	luaL_setmetatable(L, "MT_VECTOR3");
	return 1;
}

void lua_register_vector3(lua_State *L) {
	luaL_newmetatable(L, "MT_VECTOR3");

	luaL_setfuncs(L, vector3_meta_methods, 0);
	lua_pop(L, 1);

	lua_pushcfunction(L, lua_new_vector3);
	lua_setglobal(L, "vec3");
}

void lua_push_vector3(lua_State *L, Vector3 v) {
	Vector3 *vec = lua_newuserdata(L, sizeof(Vector3));
	*vec = v;
	luaL_setmetatable(L, "MT_VECTOR3");
}

bool lua_is_vector3(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_VECTOR3");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

Vector3 lua_check_vector3(lua_State *L, int index) {
	return *(Vector3 *)luaL_checkudata(L, index, "MT_VECTOR3");
}

Vector3 lua_to_vector3(lua_State *L, int index) {
	return *(Vector3 *)lua_touserdata(L, index);
}

static int lua_vector3i_eq(lua_State *L) {
	Vector3i *v1 = luaL_checkudata(L, 1, "MT_VECTOR3I");
	Vector3i *v2 = luaL_checkudata(L, 2, "MT_VECTOR3I");
	lua_pushboolean(L, vec3i_equals(*v1, *v2));
	return 1;
}

static int lua_vector3i_add(lua_State *L) {
	Vector3i *v1 = luaL_checkudata(L, 1, "MT_VECTOR3I");
	Vector3i *v2 = luaL_checkudata(L, 2, "MT_VECTOR3I");
	Vector3i *v3 = lua_newuserdata(L, sizeof(Vector3i));
	*v3 = vec3i_add(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR3I");
	return 1;
}

static int lua_vector3i_sub(lua_State *L) {
	Vector3i *v1 = luaL_checkudata(L, 1, "MT_VECTOR3I");
	Vector3i *v2 = luaL_checkudata(L, 2, "MT_VECTOR3I");
	Vector3i *v3 = lua_newuserdata(L, sizeof(Vector3i));
	*v3 = vec3i_sub(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR3I");
	return 1;
}

static int lua_vector3i_mul(lua_State *L) {
	Vector3i *v1 = luaL_checkudata(L, 1, "MT_VECTOR3I");
	Vector3i *v2 = luaL_checkudata(L, 2, "MT_VECTOR3I");
	Vector3i *v3 = lua_newuserdata(L, sizeof(Vector3i));
	*v3 = vec3i_mul(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR3I");
	return 1;
}

static int lua_vector3i_div(lua_State *L) {
	Vector3i *v1 = luaL_checkudata(L, 1, "MT_VECTOR3I");
	Vector3i *v2 = luaL_checkudata(L, 2, "MT_VECTOR3I");
	Vector3i *v3 = lua_newuserdata(L, sizeof(Vector3i));
	*v3 = vec3i_div(*v1, *v2);
	luaL_setmetatable(L, "MT_VECTOR3I");
	return 1;
}

static int lua_vector3i_tostring(lua_State *L) {
	Vector3i *v = luaL_checkudata(L, 1, "MT_VECTOR3I");
	lua_pushfstring(L, "Vector3i(%d, %d, %d)", v->x, v->y, v->z);
	return 1;
}

static int lua_vecto3i_index(lua_State *L) {
	Vector3i *v = luaL_checkudata(L, 1, "MT_VECTOR3I");
	const char *key = luaL_checkstring(L, 2);
	if (ls_str_equals(key, "x") == 0) {
		lua_pushinteger(L, v->x);
	} else if (ls_str_equals(key, "y") == 0) {
		lua_pushinteger(L, v->y);
	} else if (ls_str_equals(key, "z") == 0) {
		lua_pushinteger(L, v->z);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int lua_vector3i_newindex(lua_State *L) {
	Vector3i *v = luaL_checkudata(L, 1, "MT_VECTOR3I");
	const char *key = luaL_checkstring(L, 2);
	if (ls_str_equals(key, "x") == 0) {
		v->x = luaL_checkinteger(L, 3);
	} else if (ls_str_equals(key, "y") == 0) {
		v->y = luaL_checkinteger(L, 3);
	} else if (ls_str_equals(key, "z") == 0) {
		v->z = luaL_checkinteger(L, 3);
	}
	return 0;
}

static const luaL_Reg vector3i_meta_methods[] = {
	{ "__eq", lua_vector3i_eq },
	{ "__add", lua_vector3i_add },
	{ "__sub", lua_vector3i_sub },
	{ "__mul", lua_vector3i_mul },
	{ "__div", lua_vector3i_div },
	{ "__tostring", lua_vector3i_tostring },
	{ "__index", lua_vecto3i_index },
	{ "__newindex", lua_vector3i_newindex },
	{ NULL, NULL }
};

static int lua_new_vector3i(lua_State *L) {
	Vector3i *v = lua_newuserdata(L, sizeof(Vector3i));
	v->x = luaL_checkinteger(L, 1);
	v->y = luaL_checkinteger(L, 2);
	v->z = luaL_checkinteger(L, 3);
	luaL_setmetatable(L, "MT_VECTOR3I");
	return 1;
}

void lua_register_vector3i(lua_State *L) {
	luaL_newmetatable(L, "MT_VECTOR3I");

	luaL_setfuncs(L, vector3i_meta_methods, 0);
	lua_pop(L, 1);

	lua_pushcfunction(L, lua_new_vector3i);
	lua_setglobal(L, "vec3i");
}

void lua_push_vector3i(lua_State *L, Vector3i v) {
	Vector3i *vec = lua_newuserdata(L, sizeof(Vector3i));
	*vec = v;
	luaL_setmetatable(L, "MT_VECTOR3I");
}

bool lua_is_vector3i(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_VECTOR3I");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

Vector3i lua_check_vector3i(lua_State *L, int index) {
	return *(Vector3i *)luaL_checkudata(L, index, "MT_VECTOR3I");
}

Vector3i lua_to_vector3i(lua_State *L, int index) {
	return *(Vector3i *)lua_touserdata(L, index);
}