#include "lua_window.h"

#include "lua_renderer.h"
#include "lua_vector.h"

static Hashtable *methods = NULL;

int lua_new_window(lua_State *L) {
	Renderer *renderer = lua_check_renderer(L, 1);
	WindowConfig config = { 0 };

	// sane defaults
	config.position = vec2i(0, 0);
	config.size = vec2i(640, 480);
	config.title = "Lunar Sprits";
	config.fullscreen = false;
	config.hidden = false;
	config.root_window = true;

	if (lua_istable(L, 2)) {
		lua_getfield(L, 2, "position");
		if (lua_is_vector2i(L, -1)) {
			config.position = lua_to_vector2i(L, -1);
		}

		lua_getfield(L, 2, "size");
		if (lua_is_vector2i(L, -1)) {
			config.size = lua_to_vector2i(L, -1);
		}

		lua_getfield(L, 2, "title");
		if (lua_isstring(L, -1)) {
			config.title = luaL_checkstring(L, -1);
		}
		lua_pop(L, 1);

		lua_getfield(L, 2, "fullscreen");
		if (lua_isboolean(L, -1)) {
			config.fullscreen = lua_toboolean(L, -1);
		}

		lua_getfield(L, 2, "hidden");
		if (lua_isboolean(L, -1)) {
			config.hidden = lua_toboolean(L, -1);
		}

		lua_getfield(L, 2, "root_window");
		if (lua_isboolean(L, -1)) {
			config.root_window = lua_toboolean(L, -1);
		}
	}

	LSWindow *window = renderer_create_window(renderer, config);

	lua_push_window(L, window);
	return 1;
}

static int lua_window_destroy(lua_State *L) {
	LSWindow *window = lua_check_window(L, 1);

	window_destroy(window);

	return 0;
}

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

static const luaL_Reg window_meta_methods[] = {
	{ "__index", lua_window_index },
	{ NULL, NULL }
};

void lua_register_window(lua_State *L) {
	LS_ASSERT(methods == NULL);

	methods = hashtable_create(HASHTABLE_KEY_STRING, 12, false);

	hashtable_set(methods, HASH_KEY(str, "destroy"), HASH_VAL(ptr, lua_window_destroy));
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

	luaL_newmetatable(L, "MT_WINDOW");
	luaL_setfuncs(L, window_meta_methods, 0);
	lua_pop(L, 1);

	lua_register(L, "new_window", lua_new_window);
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
