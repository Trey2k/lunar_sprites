#include "lua_event_manager.h"

#include "lua_vector.h"
#include "lua_window.h"

#include "core/core.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

typedef struct {
	lua_State *L;
	int32 function_ref;
} LuaEventHandlerData;

static const luaL_Reg event_manager_meta_methods[] = {
	{ NULL, NULL }
};

static void lua_push_event(lua_State *L, Event *event) {
	EventType type = event->type;
	lua_newtable(L);
	lua_pushinteger(L, type);
	lua_setfield(L, -2, "type");
	lua_pushboolean(L, event->handled);
	lua_setfield(L, -2, "handled");

	switch (type) {
		case EVENT_KEY:
			lua_newtable(L);
			lua_pushinteger(L, event->key.type);
			lua_setfield(L, -2, "type");
			lua_pushinteger(L, event->key.keycode);
			lua_setfield(L, -2, "keycode");
			lua_pushboolean(L, event->key.repeated);
			lua_setfield(L, -2, "repeated");
			lua_push_const_window(L, event->key.window);
			lua_setfield(L, -2, "window");

			lua_setfield(L, -2, "key");
			break;
		case EVENT_MOUSE:
			lua_newtable(L);
			lua_pushinteger(L, event->mouse.type);
			lua_setfield(L, -2, "type");
			lua_pushinteger(L, event->mouse.button);
			lua_setfield(L, -2, "button");
			lua_push_vector2i(L, event->mouse.position);
			lua_setfield(L, -2, "position");
			lua_push_const_window(L, event->mouse.window);
			lua_setfield(L, -2, "window");

			lua_setfield(L, -2, "mouse");
			break;
		case EVENT_WINDOW:
			lua_newtable(L);
			lua_pushinteger(L, event->window.type);
			lua_setfield(L, -2, "type");
			lua_push_const_window(L, event->window.window);
			lua_setfield(L, -2, "window");
			lua_push_vector2i(L, event->window.position);
			lua_setfield(L, -2, "position");
			lua_push_vector2i(L, event->window.size);
			lua_setfield(L, -2, "size");

			lua_setfield(L, -2, "window");
			break;
		case EVENT_NONE:
		default:
			break;
	}
}

static void lua_event_handler(Event *event, void *user_data) {
	LuaEventHandlerData *handler = user_data;
	int stack_size = lua_gettop(handler->L); // Check stack size before
	lua_rawgeti(handler->L, LUA_REGISTRYINDEX, handler->function_ref);
	lua_push_event(handler->L, event);
	lua_call(handler->L, 1, 1);
	bool handled = lua_toboolean(handler->L, -1);
	lua_pop(handler->L, 1);
	event->handled = handled;
	int new_stack_size = lua_gettop(handler->L); // Check stack size after
	LS_ASSERT(stack_size == new_stack_size); // Assert that the stack size hasn't changed
}

static int32 lua_event_manager_add_handler(lua_State *L) {
	EventManager *event_manager = lua_check_event_manager(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	LuaEventHandlerData *handler_data = ls_malloc(sizeof(LuaEventHandlerData));
	handler_data->L = L;
	lua_pushvalue(L, 2);
	handler_data->function_ref = luaL_ref(L, LUA_REGISTRYINDEX);

	event_manager_add_handler(event_manager, lua_event_handler, handler_data);
	return 0;
}

static void lua_event_manager_set_methods(lua_State *L, int32 table_index) {
	lua_pushcfunction(L, lua_event_manager_add_handler);
	lua_setfield(L, table_index, "add_handler");
}

void lua_register_event_manager(lua_State *L) {
	luaL_newmetatable(L, "MT_EVENT_MANAGER");

	lua_newtable(L);
	lua_event_manager_set_methods(L, -2);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, event_manager_meta_methods, 0);
	lua_pop(L, 1);
}

void lua_push_event_manager(lua_State *L, EventManager *event_manager) {
	EventManager **udata = (EventManager **)lua_newuserdata(L, sizeof(EventManager *));
	*udata = event_manager;

	luaL_getmetatable(L, "MT_EVENT_MANAGER");
	lua_setmetatable(L, -2);
}

bool lua_is_event_manager(lua_State *L, int index) {
	if (!lua_isuserdata(L, index)) {
		return 0;
	}

	if (lua_getmetatable(L, index)) {
		lua_getfield(L, LUA_REGISTRYINDEX, "MT_EVENT_MANAGER");
		if (lua_rawequal(L, -1, -2)) {
			lua_pop(L, 2);
			return 1;
		}
		lua_pop(L, 2);
	}

	return 0;
}

EventManager *lua_check_event_manager(lua_State *L, int index) {
	return *(EventManager **)luaL_checkudata(L, index, "MT_EVENT_MANAGER");
}

EventManager *lua_to_event_manager(lua_State *L, int index) {
	return *(EventManager **)lua_touserdata(L, index);
}