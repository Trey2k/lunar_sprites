#include "lua_application.h"
#include "lua_state.h"
#include "types/lua_types.h"

#include "main/lunar_sprites.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#define PROJECT_FILE_NAME "project.lua"

static LSWindow *lua_app_init(LSCore *core, Renderer *renderer, void *user_data) {
	lua_State *L = (lua_State *)user_data;

	lua_pushcfunction(L, ls_lua_error_handler);
	lua_getglobal(L, "init");
	if (lua_isfunction(L, -1)) {
		if (!ls_lua_call(L, 0, 1, -2)) {
			ls_log_fatal("Error calling init function\n");
			return NULL;
		}

		if (lua_is_window(L, -1)) {
			LSWindow *window = lua_to_window(L, -1);
			lua_pop(L, 2);
			return window;
		}

		lua_pop(L, 1);

	} else {
		ls_log_fatal("No init function found\n");
		return NULL;
	}

	ls_log_fatal("Init function must return a window\n");
	return NULL;
}

static void lua_app_start(void *user_data) {
	lua_State *L = (lua_State *)user_data;
	lua_pushcfunction(L, ls_lua_error_handler);
	lua_getglobal(L, "start");
	if (lua_isfunction(L, -1)) {
		if (!ls_lua_call(L, 0, 0, -2)) {
			ls_log_fatal("Error calling start function\n");
		}
	}
	lua_pop(L, 1);
}

static void lua_app_update(float64 delta_time, void *user_data) {
	lua_State *L = (lua_State *)user_data;
	lua_pushcfunction(L, ls_lua_error_handler);
	lua_getglobal(L, "update");
	if (lua_isfunction(L, -1)) {
		lua_pushnumber(L, delta_time);

		if (!ls_lua_call(L, 1, 0, -3)) {
			ls_log_fatal("Error calling update function\n");
		}
	}
	lua_pop(L, 1);
}

static void lua_app_deinit(void *user_data) {
	lua_State *L = (lua_State *)user_data;
	lua_pushcfunction(L, ls_lua_error_handler);
	lua_getglobal(L, "deinit");
	if (lua_isfunction(L, -1)) {
		if (!ls_lua_call(L, 0, 0, -2)) {
			ls_log_fatal("Error calling deinit function\n");
		}
	}
	lua_pop(L, 1);
	lua_close(L);
}

static bool lua_app_should_stop(void *user_data) {
	lua_State *L = (lua_State *)user_data;
	lua_pushcfunction(L, ls_lua_error_handler);
	lua_getglobal(L, "should_stop");
	if (lua_isfunction(L, -1)) {
		if (!ls_lua_call(L, 0, 1, -2)) {
			ls_log_fatal("Error calling should_stop function\n");
			return false;
		}

		if (lua_isboolean(L, -1)) {
			bool should_stop = lua_toboolean(L, -1);
			lua_pop(L, 2);
			return should_stop;
		}

		lua_pop(L, 1);
	}
	ls_log_fatal("should_stop function must return a boolean\n");

	return false;
}

void lua_project_init(LSCore *core) {
	if (!os_path_exists(PROJECT_FILE_NAME)) {
		ls_log(LOG_LEVEL_DEBUG, "No lua project file found, skipping lua initialization\n");
		return;
	}

	lua_State *settings_state = ls_lua_new_settings_state();

	bool success = ls_lua_dofile(settings_state, PROJECT_FILE_NAME);
	if (!success) {
		ls_log(LOG_LEVEL_ERROR, "Error loading lua project config\n");
		lua_close(settings_state);
		return;
	}

	int32 type = lua_getglobal(settings_state, "source_files");
	if (type != LUA_TTABLE) {
		ls_log(LOG_LEVEL_ERROR, "No source_files table found in project settings file\n");
		lua_close(settings_state);
		return;
	}

	lua_State *application_state = ls_lua_new_application_state(core);

	int32 len = lua_rawlen(settings_state, -1);
	for (int32 i = 1; i <= len; i++) {
		lua_rawgeti(settings_state, -1, i);
		String filename = luaL_checkstring(settings_state, -1);
		success = ls_lua_dofile(application_state, filename);
		lua_pop(settings_state, 1);

		if (!success) {
			break;
		}
	}

	lua_close(settings_state);
	if (!success) {
		ls_log(LOG_LEVEL_ERROR, "Error loading lua project\n");
		lua_close(application_state);
		return;
	}

	ApplicationInterface application_interface;
	application_interface.init = lua_app_init;
	application_interface.start = lua_app_start;
	application_interface.deinit = lua_app_deinit;
	application_interface.update = lua_app_update;
	application_interface.should_stop = lua_app_should_stop;
	application_interface.user_data = application_state;

	ls_set_application_interface(application_interface);

	ls_log(LOG_LEVEL_DEBUG, "Lua project initialized\n");
}