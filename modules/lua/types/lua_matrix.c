#include "lua_matrix.h"

static int lua_new_matrix4(lua_State *L) {
	int argc = lua_gettop(L);
	switch (argc) {
		case 0: {
			Matrix4 m = { { 0 } };
			lua_push_matrix4(L, m);
			break;
			return 1;
		} break;

		case 1: {
			float32 v = luaL_checknumber(L, 1);
			Matrix4 m = { { 0 } };
			for (int i = 0; i < 16; i++) {
				m.mat[i] = v;
			}
			lua_push_matrix4(L, m);
			return 1;
		} break;

		case 16: {
			Matrix4 m = { { 0 } };
			for (int i = 0; i < 16; i++) {
				m.mat[i] = luaL_checknumber(L, i + 1);
			}
			lua_push_matrix4(L, m);
			return 1;
		} break;
	}

	return luaL_error(L, "invalid arguments to matrix4 constructor");
}

static int lua_matrix4_tostring(lua_State *L) {
	Matrix4 m = lua_check_matrix4(L, 1);
	char *str = ls_str_format("Matrix4[\n\t%f, %f, %f, %f,\n\t%f, %f, %f, %f,\n\t%f, %f, "
							  "%f, %f,\n\t%f, %f, %f, %f\n]\n",
			m.x0, m.x1, m.x2, m.x3, m.y0, m.y1, m.y2, m.y3,
			m.z0, m.z1, m.z2, m.z3, m.w0, m.w1, m.w2, m.w3);

	lua_pushstring(L, str);
	return 1;
}

static int lua_matrix4_index(lua_State *L) {
	Matrix4 m = lua_check_matrix4(L, 1);
	if (lua_isinteger(L, 2)) {
		int32 index = lua_tointeger(L, 2);
		if (index < 0 || index > 15) {
			return luaL_error(L, "invalid index '%d' for matrix4", index);
		}
		lua_pushnumber(L, m.mat[index]);
		return 1;
	}

	const char *key = luaL_checkstring(L, 2);

	if (ls_str_equals(key, "x0")) {
		lua_pushnumber(L, m.x0);
		return 1;
	} else if (ls_str_equals(key, "x1")) {
		lua_pushnumber(L, m.x1);
		return 1;
	} else if (ls_str_equals(key, "x2")) {
		lua_pushnumber(L, m.x2);
		return 1;
	} else if (ls_str_equals(key, "x3")) {
		lua_pushnumber(L, m.x3);
		return 1;
	} else if (ls_str_equals(key, "y0")) {
		lua_pushnumber(L, m.y0);
		return 1;
	} else if (ls_str_equals(key, "y1")) {
		lua_pushnumber(L, m.y1);
		return 1;
	} else if (ls_str_equals(key, "y2")) {
		lua_pushnumber(L, m.y2);
		return 1;
	} else if (ls_str_equals(key, "y3")) {
		lua_pushnumber(L, m.y3);
		return 1;
	} else if (ls_str_equals(key, "z0")) {
		lua_pushnumber(L, m.z0);
		return 1;
	} else if (ls_str_equals(key, "z1")) {
		lua_pushnumber(L, m.z1);
		return 1;
	} else if (ls_str_equals(key, "z2")) {
		lua_pushnumber(L, m.z2);
		return 1;
	} else if (ls_str_equals(key, "z3")) {
		lua_pushnumber(L, m.z3);
		return 1;
	} else if (ls_str_equals(key, "w0")) {
		lua_pushnumber(L, m.w0);
		return 1;
	} else if (ls_str_equals(key, "w1")) {
		lua_pushnumber(L, m.w1);
		return 1;
	} else if (ls_str_equals(key, "w2")) {
		lua_pushnumber(L, m.w2);
		return 1;
	} else if (ls_str_equals(key, "w3")) {
		lua_pushnumber(L, m.w3);
		return 1;
	}

	return luaL_error(L, "invalid index '%s' for matrix4", key);
}

static int lua_matrix4_newindex(lua_State *L) {
	Matrix4 *m = lua_check_matrix4_ptr(L, 1);
	if (lua_isinteger(L, 2)) {
		int32 index = lua_tointeger(L, 2);
		if (index < 0 || index > 15) {
			return luaL_error(L, "invalid index '%d' for matrix4", index);
		}
		m->mat[index] = luaL_checknumber(L, 3);
		return 0;
	}

	const char *key = luaL_checkstring(L, 2);

	if (ls_str_equals(key, "x0")) {
		m->x0 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "x1")) {
		m->x1 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "x2")) {
		m->x2 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "x3")) {
		m->x3 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "y0")) {
		m->y0 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "y1")) {
		m->y1 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "y2")) {
		m->y2 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "y3")) {
		m->y3 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "z0")) {
		m->z0 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "z1")) {
		m->z1 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "z2")) {
		m->z2 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "z3")) {
		m->z3 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "w0")) {
		m->w0 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "w1")) {
		m->w1 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "w2")) {
		m->w2 = luaL_checknumber(L, 3);
		return 0;
	} else if (ls_str_equals(key, "w3")) {
		m->w3 = luaL_checknumber(L, 3);
		return 0;
	}

	return luaL_error(L, "invalid index '%s' for matrix4", key);
}

static int lua_matrix4_multiply(lua_State *L) {
	Matrix4 a = lua_check_matrix4(L, 1);
	Matrix4 b = lua_check_matrix4(L, 2);
	lua_push_matrix4(L, mat4_multiply(a, b));
	return 1;
}

static int lua_matrix4_divide(lua_State *L) {
	Matrix4 m = lua_check_matrix4(L, 1);
	float32 v = luaL_checknumber(L, 2);
	lua_push_matrix4(L, mat4_divide(m, v));
	return 1;
}

static int lua_matrix4_add(lua_State *L) {
	Matrix4 a = lua_check_matrix4(L, 1);
	Matrix4 b = lua_check_matrix4(L, 2);
	lua_push_matrix4(L, mat4_add(a, b));
	return 1;
}

static int lua_matrix4_subtract(lua_State *L) {
	Matrix4 a = lua_check_matrix4(L, 1);
	Matrix4 b = lua_check_matrix4(L, 2);
	lua_push_matrix4(L, mat4_subtract(a, b));
	return 1;
}

static int lua_matrix4_equals(lua_State *L) {
	Matrix4 a = lua_check_matrix4(L, 1);
	Matrix4 b = lua_check_matrix4(L, 2);
	lua_pushboolean(L, mat4_equals(a, b));
	return 1;
}

static const luaL_Reg matrix4_meta_methods[] = {
	{ "__tostring", lua_matrix4_tostring },
	{ "__index", lua_matrix4_index },
	{ "__newindex", lua_matrix4_newindex },
	{ "__mul", lua_matrix4_multiply },
	{ "__div", lua_matrix4_divide },
	{ "__add", lua_matrix4_add },
	{ "__sub", lua_matrix4_subtract },
	{ "__eq", lua_matrix4_equals },
	{ NULL, NULL }
};

void lua_register_matrix4(lua_State *L) {
	luaL_newmetatable(L, "Matrix4");
	luaL_setfuncs(L, matrix4_meta_methods, 0);
	lua_pop(L, 1);

	lua_register(L, "mat4", lua_new_matrix4);
}

void lua_push_matrix4(lua_State *L, Matrix4 m) {
	Matrix4 *matrix = lua_newuserdata(L, sizeof(Matrix4));
	*matrix = m;

	luaL_getmetatable(L, "Matrix4");
	lua_setmetatable(L, -2);
}

bool lua_is_matrix4(lua_State *L, int index) {
	return lua_isuserdata(L, index) && lua_getmetatable(L, index) && lua_rawequal(L, -1, -2);
}

Matrix4 lua_check_matrix4(lua_State *L, int index) {
	return *lua_check_matrix4_ptr(L, index);
}

Matrix4 *lua_check_matrix4_ptr(lua_State *L, int index) {
	return (Matrix4 *)luaL_checkudata(L, index, "Matrix4");
}

Matrix4 lua_to_matrix4(lua_State *L, int index) {
	return *lua_to_matrix4_ptr(L, index);
}

Matrix4 *lua_to_matrix4_ptr(lua_State *L, int index) {
	return (Matrix4 *)lua_touserdata(L, index);
}