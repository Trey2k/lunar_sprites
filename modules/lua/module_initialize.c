#include "module_initialize.h"
#include "lua_application.h"

void initialize_lua_module(ModuleInitializationLevel p_level, void *p_arg) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_MAIN) {
		return;
	}

	lua_app_init();
}

void uninitialize_lua_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_MAIN) {
		return;
	}
}