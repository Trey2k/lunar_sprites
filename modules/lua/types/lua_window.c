#include "lua_window.h"

#include "lua_renderer.h"
#include "lua_vector.h"

static Hashtable *methods = NULL;

static int lua_window_poll(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);

	window_poll(window);

	return 0;
}

static int lua_window_set_title(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);
	String title = luaL_checkstring(L, 2);

	window_set_title(window, title);

	return 0;
}

static int lua_window_set_size(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);
	int32 width = luaL_checkinteger(L, 2);
	int32 height = luaL_checkinteger(L, 3);

	window_set_size(window, width, height);

	return 0;
}

static int lua_window_get_size(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);

	Vector2i size = window_get_size(window);

	lua_push_vector2i(L, size);

	return 1;
}

static int lua_window_make_current(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);

	window_make_current(window);

	return 0;
}

static int lua_window_swap_buffers(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);

	window_swap_buffers(window);

	return 0;
}

static int lua_window_set_fullscreen(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);
	bool fullscreen = lua_toboolean(L, 2);

	window_set_fullscreen(window, fullscreen);

	return 0;
}

static int lua_window_show(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);

	window_show(window);

	return 0;
}

static int lua_window_hide(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);

	window_hide(window);

	return 0;
}

static int lua_window_is_visible(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);

	lua_pushboolean(L, window_is_visible(window));

	return 1;
}

static int lua_window_is_fullscreen(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);

	lua_pushboolean(L, window_is_fullscreen(window));

	return 1;
}

static int lua_window_index(lua_State *L) {
	String key = luaL_checkstring(L, 2);

	if (hashtable_contains(methods, HASH_KEY(str, key))) {
		lua_pushcfunction(L, hashtable_get(methods, HASH_KEY(str, key)).ptr);
		return 1;
	}

	return 0;
}

static int lua_window_gc(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);

	window_destroy(window);

	return 0;
}

static const luaL_Reg window_meta_methods[] = {
	{ "__index", lua_window_index },
	{ "__gc", lua_window_gc },
	{ NULL, NULL }
};

void lua_register_window(lua_State *L) {
	if (methods == NULL) {
		methods = hashtable_create(HASHTABLE_KEY_STRING, 11, false);

		hashtable_set(methods, HASH_KEY(str, "poll"), HASH_VAL(ptr, lua_window_poll));
		hashtable_set(methods, HASH_KEY(str, "set_title"), HASH_VAL(ptr, lua_window_set_title));
		hashtable_set(methods, HASH_KEY(str, "set_size"), HASH_VAL(ptr, lua_window_set_size));
		hashtable_set(methods, HASH_KEY(str, "get_size"), HASH_VAL(ptr, lua_window_get_size));
		hashtable_set(methods, HASH_KEY(str, "make_current"), HASH_VAL(ptr, lua_window_make_current));
		hashtable_set(methods, HASH_KEY(str, "swap_buffers"), HASH_VAL(ptr, lua_window_swap_buffers));
		hashtable_set(methods, HASH_KEY(str, "set_fullscreen"), HASH_VAL(ptr, lua_window_set_fullscreen));
		hashtable_set(methods, HASH_KEY(str, "show"), HASH_VAL(ptr, lua_window_show));
		hashtable_set(methods, HASH_KEY(str, "hide"), HASH_VAL(ptr, lua_window_hide));
		hashtable_set(methods, HASH_KEY(str, "is_visible"), HASH_VAL(ptr, lua_window_is_visible));
		hashtable_set(methods, HASH_KEY(str, "is_fullscreen"), HASH_VAL(ptr, lua_window_is_fullscreen));
	}

	luaL_newmetatable(L, "MT_WINDOW");
	luaL_setfuncs(L, window_meta_methods, 0);
	lua_pop(L, 1);
}

void lua_push_window(lua_State *L, LSWindow *window) {
	LSWindow **udata = (LSWindow **)lua_newuserdata(L, sizeof(LSWindow *));
	*udata = window;

	luaL_getmetatable(L, "MT_WINDOW");
	lua_setmetatable(L, -2);
}

bool lua_is_window(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_WINDOW");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

LSWindow *lua_check_window(lua_State *L, int index) {
	return *(LSWindow **)luaL_checkudata(L, index, "MT_WINDOW");
}

LSWindow *lua_to_window(lua_State *L, int index) {
	return *(LSWindow **)lua_touserdata(L, index);
}
