#include "module_initialize.h"
#include "core/core.h"

#include "native_application.h"

void initialize_native_application_module(ModuleInitializationLevel p_level, void *p_arg) {
	switch (p_level) {
		case MODULE_INITIALIZATION_LEVEL_CORE: {
			native_application_init(p_arg);
		} break;
		case MODULE_INITIALIZATION_LEVEL_MAIN: {
			native_application_start();
		} break;
		default: {
		} break;
	};
}

void uninitialize_native_application_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}

	native_application_deinit();
}
