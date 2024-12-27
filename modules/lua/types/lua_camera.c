#include "lua_camera.h"

#include "lua_matrix.h"
#include "lua_vector.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static Hashtable *methods = NULL;

static int lua_camera_gc(lua_State *L) {
	Camera *camera = lua_check_camera(L, 1);
	camera_destroy(camera);

	return 0;
}

static int lua_camera_index(lua_State *L) {
	Camera *camera = lua_check_camera(L, 1);
	String key = luaL_checkstring(L, 2);

	if (hashtable_contains(methods, HASH_KEY(str, key))) {
		lua_pushcfunction(L, hashtable_get(methods, HASH_KEY(str, key)).ptr);
		return 1;
	}

	if (ls_str_equals(key, "position")) {
		lua_push_vector3(L, camera_get_position(camera));
		return 1;
	} else if (ls_str_equals(key, "rotation")) {
		lua_push_vector3(L, camera_get_rotation(camera));
		return 1;
	} else if (ls_str_equals(key, "view_matrix")) {
		lua_push_matrix4(L, camera_get_view_matrix(camera));
		return 1;
	} else if (ls_str_equals(key, "projection_matrix")) {
		lua_push_matrix4(L, camera_get_projection_matrix(camera));
		return 1;
	}

	return 0;
}

static int lua_camera_newindex(lua_State *L) {
	Camera *camera = lua_check_camera(L, 1);
	String key = luaL_checkstring(L, 2);

	if (ls_str_equals(key, "position")) {
		Vector3 position = lua_check_vector3(L, 3);
		camera_set_position(camera, position);
	} else if (ls_str_equals(key, "rotation")) {
		Vector3 rotation = lua_check_vector3(L, 3);
		camera_set_rotation(camera, rotation);
	}

	return 0;
}

static int lua_camera_move(lua_State *L) {
	Camera *camera = lua_check_camera(L, 1);
	Vector3 offset = lua_check_vector3(L, 2);

	camera_move(camera, offset);

	return 0;
}

static int lua_camera_rotate(lua_State *L) {
	Camera *camera = lua_check_camera(L, 1);
	Vector3 offset = lua_check_vector3(L, 2);

	camera_rotate(camera, offset);

	return 0;
}

static int lua_camera_set_active(lua_State *L) {
	Camera *camera = lua_check_camera(L, 1);

	camera_set_active(camera);

	return 0;
}

static int lua_camera_set_projection(lua_State *L) {
	Camera *camera = lua_check_camera(L, 1);
	float32 fov = luaL_checknumber(L, 2);
	float32 aspect = luaL_checknumber(L, 3);
	float32 near = luaL_checknumber(L, 4);
	float32 far = luaL_checknumber(L, 5);

	camera_set_projection(camera, fov, aspect, near, far);

	return 0;
}

static const luaL_Reg camera_meta_methods[] = {
	{ "__index", lua_camera_index },
	{ "__newindex", lua_camera_newindex },
	{ "__gc", lua_camera_gc },
	{ NULL, NULL }
};

void lua_register_camera(lua_State *L) {
	if (methods == NULL) {
		methods = hashtable_create(HASHTABLE_KEY_STRING, 6, false);

		hashtable_set(methods, HASH_KEY(str, "move"), HASH_VAL(ptr, lua_camera_move));
		hashtable_set(methods, HASH_KEY(str, "rotate"), HASH_VAL(ptr, lua_camera_rotate));
		hashtable_set(methods, HASH_KEY(str, "set_active"), HASH_VAL(ptr, lua_camera_set_active));
		hashtable_set(methods, HASH_KEY(str, "set_projection"), HASH_VAL(ptr, lua_camera_set_projection));
	}

	luaL_newmetatable(L, "Camera");
	luaL_setfuncs(L, camera_meta_methods, 0);
	lua_pop(L, 1);
}

void lua_push_camera(lua_State *L, Camera *camera) {
	Camera **udata = (Camera **)lua_newuserdata(L, sizeof(Camera *));
	*udata = camera;

	luaL_getmetatable(L, "Camera");
	lua_setmetatable(L, -2);
}

bool lua_is_camera(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "Camera");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

Camera *lua_check_camera(lua_State *L, int index) {
	return *(Camera **)luaL_checkudata(L, index, "Camera");
}

Camera *lua_to_camera(lua_State *L, int index) {
	return *(Camera **)lua_touserdata(L, index);
}