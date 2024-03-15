#ifndef LUA_TYPES_H
#define LUA_TYPES_H

#include "lua_camera.h"
#include "lua_core.h"
#include "lua_event_manager.h"
#include "lua_input_manager.h"
#include "lua_matrix.h"
#include "lua_renderer.h"
#include "lua_sprite.h"
#include "lua_vector.h"
#include "lua_window.h"

#include "core/core.h"

void lua_register_types(LSCore *core, lua_State *L);

#endif // LUA_TYPES_H