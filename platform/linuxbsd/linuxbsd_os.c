#include "platform/linuxbsd/linuxbsd_os.h"

#include "core/core.h"

OS *os_create() {
#if defined(WAYLAND_ENABLED)
	WaylandServer *wayland_server = NULL;
	wayland_server = wayland_server_create();
	if (wayland_server) {
		OS *os = core_malloc(sizeof(OS));
		os->display_server = DISPLAY_SERVER_WAYLAND;
		os->wayland_server = wayland_server;
		return os;
	}
#endif
	return NULL;
}

void os_destroy(OS *os) {
	switch (os->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND:
			wayland_server_destroy(os->wayland_server);
			break;
#endif
		default:
			core_fatal("Unknown display type: %d\n", os->display_server);
	};

	core_free(os);
}