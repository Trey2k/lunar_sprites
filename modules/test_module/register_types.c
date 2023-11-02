#include "register_types.h"

#include <stdio.h>

void initialize_test_module_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}

	printf("Initializing test_module module.\n");
}

void uninitialize_test_module_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}

	printf("Uninitializing test_module module.\n");
}
