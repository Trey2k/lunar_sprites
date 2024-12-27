#include "lua_types.h"

void lua_register_types(LSCore *core, lua_State *L) {
	lua_register_vector2(L);
	lua_register_vector2i(L);
	lua_register_vector3(L);
	lua_register_vector3i(L);
	lua_register_matrix4(L);
	lua_register_color(L);

	lua_register_input_manager(L);
	lua_register_event_manager(L);
	lua_register_core(core, L);

	lua_register_window(L);
	lua_register_sprite(L);
	lua_register_camera(L);

	lua_register_renderer(L);
}