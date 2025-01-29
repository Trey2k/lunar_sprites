#include "module_initialize.h"

#include "ui.h"

void initialize_ui_module(ModuleInitializationLevel p_level, void *p_arg) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_APPLICATION) {
		ls_printf("Initializing UI module.\n");
		ui_init(p_arg);
	}
}

void uninitialize_ui_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_APPLICATION) {
		return;
	}

	ls_printf("Uninitializing UI module.\n");
	ui_deinit();
}