#include "lua_renderer.h"

#include "lua_vector.h"
#include "lua_window.h"

static Hashtable *methods = NULL;

static int lua_renderer_set_clear_color(lua_State *L) {
	Renderer *renderer = lua_check_renderer(L, 1);

	float32 r = luaL_checknumber(L, 2);
	float32 g = luaL_checknumber(L, 3);
	float32 b = luaL_checknumber(L, 4);
	float32 a = luaL_checknumber(L, 5);

	renderer_set_clear_color(renderer, r, g, b, a);

	return 0;
}

static int lua_renderer_clear(lua_State *L) {
	Renderer *renderer = lua_check_renderer(L, 1);

	renderer_clear(renderer);

	return 0;
}

static int lua_renderer_get_backend(lua_State *L) {
	Renderer *renderer = lua_check_renderer(L, 1);

	lua_pushstring(L, renderer_backend_to_string(renderer_get_backend(renderer)));

	return 1;
}

static int lua_renderer_get_viewport_size(lua_State *L) {
	Renderer *renderer = lua_check_renderer(L, 1);

	Vector2i size = renderer_get_viewport_size(renderer);

	lua_push_vector2i(L, size);

	return 1;
}

static int lua_renderer_index(lua_State *L) {
	String key = luaL_checkstring(L, 2);

	if (hashtable_contains(methods, HASH_KEY(str, key))) {
		lua_pushcfunction(L, hashtable_get(methods, HASH_KEY(str, key)).ptr);
		return 1;
	}

	return 0;
}

static const luaL_Reg renderer_meta_methods[] = {
	{ "__index", lua_renderer_index },
	{ NULL, NULL }
};

void lua_register_renderer(lua_State *L) {
	LS_ASSERT(methods == NULL);

	methods = hashtable_create(HASHTABLE_KEY_STRING, 5, false);

	hashtable_set(methods, HASH_KEY(str, "set_clear_color"), HASH_VAL(ptr, lua_renderer_set_clear_color));
	hashtable_set(methods, HASH_KEY(str, "clear"), HASH_VAL(ptr, lua_renderer_clear));
	hashtable_set(methods, HASH_KEY(str, "get_backend"), HASH_VAL(ptr, lua_renderer_get_backend));
	hashtable_set(methods, HASH_KEY(str, "get_viewport_size"), HASH_VAL(ptr, lua_renderer_get_viewport_size));

	hashtable_set(methods, HASH_KEY(str, "new_window"), HASH_VAL(ptr, lua_new_window));

	luaL_newmetatable(L, "MT_RENDERER");
	luaL_setfuncs(L, renderer_meta_methods, 0);
	lua_pop(L, 1);
}

void lua_push_renderer(lua_State *L, Renderer *renderer) {
	Renderer **udata = (Renderer **)lua_newuserdata(L, sizeof(Renderer *));
	*udata = renderer;

	luaL_getmetatable(L, "MT_RENDERER");
	lua_setmetatable(L, -2);
}

bool lua_is_renderer(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_RENDERER");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

Renderer *lua_check_renderer(lua_State *L, int index) {
	return *(Renderer **)luaL_checkudata(L, index, "MT_RENDERER");
}

Renderer *lua_to_renderer(lua_State *L, int index) {
	return *(Renderer **)lua_touserdata(L, index);
}