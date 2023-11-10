#include "platform/windows/os.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"

#include <windows.h>

PlatformOS *platform_create_os() {
	PlatformOS *os = ls_malloc(sizeof(PlatformOS));

	return os;
}

void platform_destroy_os(PlatformOS *os) {
	ls_free(os);
}

void *platform_get_native_display(const PlatformOS *os) {
	return CreateCompatibleDC(NULL);
}