#ifndef LUA_INPUT_MANAGER_H
#define LUA_INPUT_MANAGER_H

#include "core/input/input_manager.h"

#include "lua_state.h"

void lua_register_input_manager(lua_State *L);

LS_EXPORT void lua_push_input_manager(lua_State *L, InputManager *input_manager);
LS_EXPORT bool lua_is_input_manager(lua_State *L, int index);
LS_EXPORT InputManager *lua_check_input_manager(lua_State *L, int index);
LS_EXPORT InputManager *lua_to_input_manager(lua_State *L, int index);

#endif // LUA_INPUT_MANAGER_H