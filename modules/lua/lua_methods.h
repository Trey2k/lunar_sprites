#ifndef LUA_METHODS_H
#define LUA_METHODS_H

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

/*
 * Main methods
 */
int lua_set_application_interface(lua_State *L);
int lua_exit(lua_State *L);

/*
 * Renderer methods
 */
int lua_renderer_set_clear_color(lua_State *L);
int lua_renderer_clear(lua_State *L);
int lua_renderer_get_backend(lua_State *L);
int lua_renderer_get_viewport_size(lua_State *L);
int lua_renderer_backend_to_string(lua_State *L);

/*
 * Window methods
 */
int lua_renderer_create_window(lua_State *L);
int lua_window_destroy(lua_State *L);

int lua_window_set_title(lua_State *L);
int lua_window_set_size(lua_State *L);
int lua_window_get_size(lua_State *L);
int lua_window_make_current(lua_State *L);
int lua_window_swap_buffers(lua_State *L);
int lua_window_set_fullscreen(lua_State *L);
int lua_window_show(lua_State *L);
int lua_window_hide(lua_State *L);
int lua_window_is_visible(lua_State *L);
int lua_window_is_fullscreen(lua_State *L);

#endif // LUA_METHODS_H