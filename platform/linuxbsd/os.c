#include "platform/linuxbsd/os.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"

PlatformOS *platform_create_os() {
	PlatformOS *os = ls_malloc(sizeof(PlatformOS));

#if defined(WAYLAND_ENABLED)
	WaylandServer *wayland_server = NULL;
	wayland_server = wayland_server_create();
	if (wayland_server) {
		os->display_server = DISPLAY_SERVER_WAYLAND;
		os->wayland_server = wayland_server;
	}
#endif

#if defined(X11_ENABLED)
	X11Server *x11_server = NULL;
	x11_server = x11_server_create();
	if (x11_server) {
		os->display_server = DISPLAY_SERVER_X11;
		os->x11_server = x11_server;
	}
#endif

	if (os->display_server == DISPLAY_SERVER_NONE) {
		ls_log_fatal("Failed to create display server\n");
		return NULL;
	}

	return os;
}

void platform_destroy_os(PlatformOS *os) {
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
			ls_log_fatal("Unknown display type: %d\n", os->display_server);
	};
}

void *platform_get_native_display(const PlatformOS *os) {
	switch (os->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND:
			return os->wayland_server->display;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11:
			return os->x11_server->display;
#endif

		default:
			ls_log_fatal("Unknown display type: %d\n", os->display_server);
			return NULL;
	};
}

DisplayServer os_linuxbsd_get_display_server(const PlatformOS *os) {
	return os->display_server;
}

#if defined(WAYLAND_ENABLED)
WaylandServer *os_linuxbsd_get_wayland_server(const PlatformOS *os) {
	return os->wayland_server;
}
#endif

#if defined(X11_ENABLED)
X11Server *os_linuxbsd_get_x11_server(const PlatformOS *os) {
	return os->x11_server;
}
#endif