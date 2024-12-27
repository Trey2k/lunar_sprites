#ifndef LUA_SPRITE_H
#define LUA_SPRITE_H

#include "core/core.h"

#include "renderer/sprite.h"

#include "lua_state.h"

void lua_register_sprite(lua_State *L);

LS_EXPORT void lua_push_sprite(lua_State *L, Sprite *sprite);
LS_EXPORT bool lua_is_sprite(lua_State *L, int index);
LS_EXPORT Sprite *lua_check_sprite(lua_State *L, int index);
LS_EXPORT Sprite *lua_to_sprite(lua_State *L, int index);

#endif // LUA_SPRITE_H