#include "platform/web/os.h"
#include "platform/web/keymap.h"

#include "core/core.h"

#include <sys/time.h>

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

static uint64 web_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64)tv.tv_sec * 1000000 + (uint64)tv.tv_usec;
}

PlatformOSInterface platform_get_os_interface(const PlatformOS *os) {
	PlatformOSInterface interface;
	interface.get_time = web_get_time;
	return interface;
}