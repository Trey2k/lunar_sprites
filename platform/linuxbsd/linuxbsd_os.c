#include "platform/linuxbsd/linuxbsd_os.h"

#include "core/core.h"

struct OS {
	DisplayServer display_server;
	union {
#if defined(WAYLAND_ENABLED)
		WaylandServer *wayland_server;
#endif

#if defined(X11_ENABLED)
		X11Server *x11_server;
#endif
	};
};

static struct OS os;

bool os_init() {
#if defined(WAYLAND_ENABLED)
	WaylandServer *wayland_server = NULL;
	wayland_server = wayland_server_create();
	if (wayland_server) {
		os.display_server = DISPLAY_SERVER_WAYLAND;
		os.wayland_server = wayland_server;
		return true;
	}
#endif

#if defined(X11_ENABLED)
	X11Server *x11_server = NULL;
	x11_server = x11_server_create();
	if (x11_server) {
		os.display_server = DISPLAY_SERVER_X11;
		os.x11_server = x11_server;
		return true;
	}
#endif

	return false;
}

void os_deinit() {
	switch (os.display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND:
			wayland_server_destroy(os.wayland_server);
			break;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11:
			x11_server_destroy(os.x11_server);
			break;
#endif
		default:
			core_fatal("Unknown display type: %d\n", os.display_server);
	};
}

void *os_get_native_display() {
	switch (os.display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND:
			return os.wayland_server->display;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11:
			return os.x11_server->display;
#endif

		default:
			core_fatal("Unknown display type: %d\n", os.display_server);
			return NULL;
	};
}

DisplayServer os_linuxbsd_get_display_server() {
	return os.display_server;
}

#if defined(WAYLAND_ENABLED)
WaylandServer *os_linuxbsd_get_wayland_server() {
	return os.wayland_server;
}
#endif

#if defined(X11_ENABLED)
X11Server *os_linuxbsd_get_x11_server() {
	return os.x11_server;
}
#endif