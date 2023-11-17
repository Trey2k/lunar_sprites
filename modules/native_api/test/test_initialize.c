#include "ls_api.gen.h"

void initialize_native_api_client(APIClientInitializationLevel p_level) {
	if (p_level != API_CLIENT_INITIALIZATION_LEVEL_MAIN) {
		return;
	}
	// set clear color to yellow
	renderer_set_clear_color(1.0f, 1.0f, 0.0f, 1.0f);
}

void uninitialize_native_api_client(APIClientInitializationLevel p_level) {
	if (p_level != API_CLIENT_INITIALIZATION_LEVEL_MAIN) {
		return;
	}
}

LS_EXPORT APIClientInterface register_api_client() {
	APIClientInterface interface;

	interface.initialize = initialize_native_api_client;
	interface.uninitialize = uninitialize_native_api_client;

	return interface;
}