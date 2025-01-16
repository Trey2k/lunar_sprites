#include "module_initialize.h"

#include "font.h"
#include "modules/initialize_modules.h"

const Renderer *renderer = NULL;

void initialize_font_module(ModuleInitializationLevel p_level, void *p_arg) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_RENDER) {
		renderer = (const Renderer *)p_arg;
	}

	if (p_level != MODULE_INITIALIZATION_LEVEL_APPLICATION) {
		return;
	}

	ls_printf("Initializing Font module.\n");
	font_renderer_init(renderer);
}

void uninitialize_font_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_APPLICATION) {
		return;
	}

	ls_printf("Uninitializing Font module.\n");
	font_renderer_deinit();
}