#ifndef LUA_EVENT_MANAGER_H
#define LUA_EVENT_MANAGER_H

#include "core/events/event_manager.h"

#include "lua_state.h"

void lua_register_event_manager(lua_State *L);

LS_EXPORT void lua_push_event_manager(lua_State *L, EventManager *event_manager);
LS_EXPORT bool lua_is_event_manager(lua_State *L, int index);
LS_EXPORT EventManager *lua_check_event_manager(lua_State *L, int index);
LS_EXPORT EventManager *lua_to_event_manager(lua_State *L, int index);

#endif // LUA_EVENT_MANAGER_H