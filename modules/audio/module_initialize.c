#include "module_initialize.h"

#include "audio_server.h"

void initialize_audio_module(ModuleInitializationLevel p_level, void *p_arg) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}

	ls_log(LOG_LEVEL_INFO, "Initializing Audio module\n");

	audio_server_init();
}

void uninitialize_audio_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}

	ls_log(LOG_LEVEL_INFO, "Uninitializing Audio module\n");

	audio_server_deinit();
}