#include "module_initialize.h"

#include "ui.h"

static LSCore *core = NULL;

void initialize_ui_module(ModuleInitializationLevel p_level, void *p_arg) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
		core = (LSCore *)p_arg;
		LS_ASSERT(core);
	} else if (p_level == MODULE_INITIALIZATION_LEVEL_APPLICATION) {
		LSWindow *window = (LSWindow *)p_arg;
		LS_ASSERT(window);

		ls_printf("Initializing UI module.\n");
		ui_init(core, window);
	}
}

void uninitialize_ui_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_APPLICATION) {
		return;
	}

	ls_printf("Uninitializing UI module.\n");
	ui_deinit();
}