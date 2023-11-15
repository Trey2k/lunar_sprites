#include "platform/web/os.h"
#include "platform/web/keymap.h"

#include "core/core.h"

PlatformOS *platform_create_os() {
	PlatformOS *os = ls_malloc(sizeof(PlatformOS));
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