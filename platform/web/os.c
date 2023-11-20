#include "platform/web/os.h"
#include "platform/web/keymap.h"

#include "core/core.h"

PlatformOS *platform_create_os(InputManager *input_manager) {
	PlatformOS *os = ls_malloc(sizeof(PlatformOS));
	os->input_manager = input_manager;
	keymap_init();

	return os;
}

void platform_destroy_os(PlatformOS *os) {
	keymap_deinit();
	ls_free(os);
}

LSNativeDisplayType platform_get_native_display(const PlatformOS *os) {
	return 0;
}