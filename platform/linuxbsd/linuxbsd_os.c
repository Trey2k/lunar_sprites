#include "platform/linuxbsd/linuxbsd_os.h"

#include "core/core.h"

OS *os_create(Input *input) {
#if defined(WAYLAND_ENABLED)
	WaylandServer *wayland_server = NULL;
	wayland_server = wayland_server_create(input);
	if (wayland_server) {
		OS *os = core_malloc(sizeof(OS));
		os->display_server = DISPLAY_SERVER_WAYLAND;
		os->wayland_server = wayland_server;
		return os;
	}
#endif

#if defined(X11_ENABLED)
	X11Server *x11_server = NULL;
	x11_server = x11_server_create(input);
	if (x11_server) {
		OS *os = core_malloc(sizeof(OS));
		os->display_server = DISPLAY_SERVER_X11;
		os->x11_server = x11_server;
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

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11:
			x11_server_destroy(os->x11_server);
			break;
#endif
		default:
			core_fatal("Unknown display type: %d\n", os->display_server);
	};

	core_free(os);
}