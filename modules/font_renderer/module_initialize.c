#include "module_initialize.h"

#include "font_renderer.h"

static LSCore *core = NULL;

void initialize_font_renderer_module(ModuleInitializationLevel p_level, void *p_arg) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
		core = (LSCore *)p_arg;
		LS_ASSERT(core != NULL);
	} else if (p_level == MODULE_INITIALIZATION_LEVEL_RENDER) {
		Renderer *renderer = (Renderer *)p_arg;
		LS_ASSERT(renderer != NULL);

		ls_printf("Initializing font rendering module.\n");
		font_renderer_init(renderer, core);
	}
}

void uninitialize_font_renderer_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_RENDER) {
		return;
	}

	ls_printf("Uninitializing font rendering module.\n");
	font_renderer_deinit();
}