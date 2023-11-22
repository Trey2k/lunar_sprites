#include "module_initialize.h"

#include "dynamic_modules.h"

void initialize_dynamic_modules_module(ModuleInitializationLevel p_level, void *p_arg) {
	switch (p_level) {
		case MODULE_INITIALIZATION_LEVEL_CORE: {
			// Dynamic modules currently can not have a flag manager registration level.
			dynamic_modules_init(p_arg);
		} break;
		case MODULE_INITIALIZATION_LEVEL_FLAGS: {
			dynamic_modules_start();
		} break;
		default: {
		} break;
	};

	initialize_dynamic_modules(p_level, p_arg);
}

void uninitialize_dynamic_modules_module(ModuleInitializationLevel p_level) {
	uninitialize_dynamic_modules(p_level);

	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
		dynamic_modules_deinit();
	}
}