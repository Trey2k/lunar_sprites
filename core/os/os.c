#include "core/os/os.h"
#include "core/debug.h"
#include "core/memory/memory.h"

struct OS {
	PlatformOS *platform_os;
};

OS *ls_create_os() {
	OS *os = ls_malloc(sizeof(OS));
	os->platform_os = platform_create_os();
	return os;
}

void ls_destroy_os(OS *os) {
	LS_ASSERT(os);

	platform_destroy_os(os->platform_os);
	ls_free(os);
}

void *os_get_native_display(const OS *os) {
	LS_ASSERT(os);

	return platform_get_native_display(os->platform_os);
}

PlatformOS *os_get_platform_os(const OS *os) {
	LS_ASSERT(os);

	return os->platform_os;
}