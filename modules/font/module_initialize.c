#include "module_initialize.h"

#include "font.h"

static LSCore *core = NULL;
static Renderer *renderer = NULL;

void initialize_font_module(ModuleInitializationLevel p_level, void *p_arg) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
		core = (LSCore *)p_arg;
		LS_ASSERT(core);
	} else if (p_level == MODULE_INITIALIZATION_LEVEL_RENDER) {
		renderer = (Renderer *)p_arg;
		LS_ASSERT(renderer);
	} else if (p_level == MODULE_INITIALIZATION_LEVEL_APPLICATION) {
		LSWindow *window = (LSWindow *)p_arg;
		LS_ASSERT(window);

		ls_printf("Initializing Font module.\n");
		font_renderer_init(renderer, core, window);
	}
}

void uninitialize_font_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_APPLICATION) {
		return;
	}

	ls_printf("Uninitializing Font module.\n");
	font_renderer_deinit();
}