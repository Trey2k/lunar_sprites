#include "module_initialize.h"
#include "lua_application.h"

#include "renderer/renderer.h"

static LSCore *core = NULL;
static Renderer *renderer = NULL;

void initialize_lua_module(ModuleInitializationLevel p_level, void *p_arg) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
		core = (LSCore *)p_arg;
		return;
	} else if (p_level == MODULE_INITIALIZATION_LEVEL_RENDER) {
		renderer = (Renderer *)p_arg;
		return;
		ls_printf("Initializing Lua module.\n");
	} else if (p_level != MODULE_INITIALIZATION_LEVEL_MAIN) {
		return;
	}

	LS_ASSERT(core != NULL);
	LS_ASSERT(renderer != NULL);

	lua_project_init(core, renderer);
}

void uninitialize_lua_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_MAIN) {
		return;
	}
}