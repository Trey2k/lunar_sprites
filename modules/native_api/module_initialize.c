#include "module_initialize.h"
#include "core/core.h"

#include "native_api.h"

void initialize_native_api_module(ModuleInitializationLevel p_level) {
	switch (p_level) {
		case MODULE_INITIALIZATION_LEVEL_CORE: {
			native_api_init();
			native_api_init_clients(API_CLIENT_INITIALIZATION_LEVEL_CORE);
		} break;
		case MODULE_INITIALIZATION_LEVEL_RENDER: {
			native_api_init_clients(API_CLIENT_INITIALIZATION_LEVEL_RENDER);
		} break;
		case MODULE_INITIALIZATION_LEVEL_MAIN: {
			native_api_start();
			native_api_init_clients(API_CLIENT_INITIALIZATION_LEVEL_MAIN);
		} break;
	};
}

void uninitialize_native_api_module(ModuleInitializationLevel p_level) {
	// These two should always match
	native_api_uninit_clients((APIClientInitializationLevel)p_level);

	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}

	native_api_deinit();
}
