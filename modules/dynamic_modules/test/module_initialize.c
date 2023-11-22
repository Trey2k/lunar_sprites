#include "ls_api.gen.h"

#include "test_app.h"

void initailize_test_app_module(ModuleInitializationLevel p_level, void *p_arg) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}

	ls_set_application_interface(register_application());
}

void deinitialize_test_app_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}
}

LS_EXPORT DynamicModuleInterface register_test_app_module() {
	LS_ASSERT(_impl_ls_calloc);

	DynamicModuleInterface module_interface = {
		.init = initailize_test_app_module,
		.deinit = deinitialize_test_app_module
	};

	return module_interface;
}