#include "lua_sprite.h"

#include "lua_vector.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static Hashtable *methods = NULL;

static int lua_sprite_gc(lua_State *L) {
	Sprite *sprite = lua_check_sprite(L, 1);
	sprite_destroy(sprite);

	return 0;
}

static int lua_sprite_index(lua_State *L) {
	Sprite *sprite = lua_check_sprite(L, 1);
	String key = luaL_checkstring(L, 2);

	if (hashtable_contains(methods, HASH_KEY(str, key))) {
		lua_pushcfunction(L, hashtable_get(methods, HASH_KEY(str, key)).ptr);
		return 1;
	}

	if (ls_str_equals(key, "position")) {
		lua_push_vector2i(L, sprite_get_position(sprite));
		return 1;
	} else if (ls_str_equals(key, "rotation")) {
		lua_pushnumber(L, sprite_get_rotation(sprite));
		return 1;
	}

	return 0;
}

static int lua_sprite_newindex(lua_State *L) {
	Sprite *sprite = lua_check_sprite(L, 1);
	String key = luaL_checkstring(L, 2);

	if (ls_str_equals(key, "position")) {
		Vector2i position = lua_check_vector2i(L, 3);
		sprite_set_position(sprite, position);
	} else if (ls_str_equals(key, "rotation")) {
		float32 rotation = luaL_checknumber(L, 3);
		sprite_set_rotation(sprite, rotation);
	} else if (ls_str_equals(key, "scale")) {
		Vector2 scale = lua_check_vector2(L, 3);
		sprite_scale(sprite, scale);
	}

	return 0;
}

static int lua_sprite_draw(lua_State *L) {
	Sprite *sprite = lua_check_sprite(L, 1);

	sprite_draw(sprite);

	return 0;
}

static const luaL_Reg sprite_meta_methods[] = {
	{ "__index", lua_sprite_index },
	{ "__newindex", lua_sprite_newindex },
	{ "__gc", lua_sprite_gc },
	{ NULL, NULL }
};

void lua_register_sprite(lua_State *L) {
	if (methods == NULL) {
		methods = hashtable_create(HASHTABLE_KEY_STRING, 1, false);

		hashtable_set(methods, HASH_KEY(str, "draw"), HASH_VAL(ptr, lua_sprite_draw));
	}

	luaL_newmetatable(L, "MT_SPRITE");
	luaL_setfuncs(L, sprite_meta_methods, 0);
	lua_pop(L, 1);
}

void lua_push_sprite(lua_State *L, Sprite *sprite) {
	Sprite **udata = (Sprite **)lua_newuserdata(L, sizeof(Sprite *));
	*udata = sprite;

	luaL_getmetatable(L, "MT_SPRITE");
	lua_setmetatable(L, -2);
}

bool lua_is_sprite(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_SPRITE");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

Sprite *lua_check_sprite(lua_State *L, int index) {
	return *(Sprite **)luaL_checkudata(L, index, "MT_SPRITE");
}

Sprite *lua_to_sprite(lua_State *L, int index) {
	return *(Sprite **)lua_touserdata(L, index);
}