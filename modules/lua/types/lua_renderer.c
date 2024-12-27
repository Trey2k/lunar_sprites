#include "lua_renderer.h"

#include "lua_camera.h"
#include "lua_sprite.h"
#include "lua_vector.h"
#include "lua_window.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static int lua_renderer_new_window(lua_State *L) {
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

static int lua_renderer_new_sprite(lua_State *L) {
	Renderer *renderer = lua_check_renderer(L, 1);
	String path = luaL_checkstring(L, 2);
	Vector2 position = lua_check_vector2(L, 3);
	Vector2 scale = lua_check_vector2(L, 4);
	float32 rotation = luaL_checknumber(L, 5);

	Sprite *sprite = renderer_create_sprite(renderer, path, position, scale, rotation);

	lua_push_sprite(L, sprite);

	return 1;
}

static int lua_renderer_new_camera(lua_State *L) {
	Renderer *renderer = lua_check_renderer(L, 1);
	float32 fov = luaL_checknumber(L, 2);
	float32 aspect = luaL_checknumber(L, 3);
	float32 near = luaL_checknumber(L, 4);
	float32 far = luaL_checknumber(L, 5);

	Camera *camera = camera_create(fov, aspect, near, far);

	lua_push_camera(L, camera);

	return 1;
}

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

static const luaL_Reg renderer_meta_methods[] = {
	{ NULL, NULL }
};

extern void lua_renderer_init_constants(lua_State *L, int32 table_index);

static void lua_renderer_set_methods(lua_State *L, int32 table_index) {
	lua_pushcfunction(L, lua_renderer_set_clear_color);
	lua_setfield(L, table_index, "set_clear_color");

	lua_pushcfunction(L, lua_renderer_clear);
	lua_setfield(L, table_index, "clear");

	lua_pushcfunction(L, lua_renderer_new_window);
	lua_setfield(L, table_index, "new_window");

	lua_pushcfunction(L, lua_renderer_new_sprite);
	lua_setfield(L, table_index, "new_sprite");

	lua_pushcfunction(L, lua_renderer_new_camera);
	lua_setfield(L, table_index, "new_camera");

	lua_pushcfunction(L, lua_renderer_get_backend);
	lua_setfield(L, table_index, "get_backend");

	lua_pushcfunction(L, lua_renderer_get_viewport_size);
	lua_setfield(L, table_index, "get_viewport_size");
}

void lua_register_renderer(lua_State *L) {
	luaL_newmetatable(L, "MT_RENDERER");

	lua_newtable(L);
	lua_renderer_set_methods(L, -2);
	lua_renderer_init_constants(L, -2);
	lua_setfield(L, -2, "__index");

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