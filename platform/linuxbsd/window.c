#include "platform/linuxbsd/window.h"
#include "core/log.h"
#include "core/memory.h"

#if defined(WAYLAND_ENABLED)
#include "platform/linuxbsd/wayland/window.h"
#endif

#if defined(X11_ENABLED)
#include "platform/linuxbsd/x11/window.h"
#endif

PlatformWindow *platform_create_window(const PlatformOS *os, String title, int32 width, int32 height) {
	PlatformWindow *window = ls_malloc(sizeof(PlatformWindow));
	window->display_server = os->display_server;

	switch (window->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			WaylandWindow *win = wayland_window_create(os->wayland_server, title, width, height);
			if (!win) {
				ls_log(LOG_LEVEL_ERROR, "Failed to create Wayland window\n");
				return NULL;
			}
			window->wayland_window = win;
		} break;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			// TODO: Pass in X11Server
			X11Window *win = x11_window_create(os->x11_server, title, width, height);
			if (!win) {
				ls_log(LOG_LEVEL_ERROR, "Failed to create X11 window\n");
				return NULL;
			}
			window->x11_window = win;
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
			return NULL;
	};

	return window;
}

void platform_destroy_window(PlatformWindow *window) {
	switch (window->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			wayland_window_destroy(window->wayland_window);
			break;
		} break;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_destroy(window->x11_window);
			break;
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};

	ls_free(window);
}

unsigned long platform_window_get_native_window(const PlatformWindow *window) {
	switch (window->display_server) {
#if defined(WAYLAND_ENABLED)

		case DISPLAY_SERVER_WAYLAND: {
			return window->wayland_window->surface;
		} break;

#endif

#if defined(X11_ENABLED)

		case DISPLAY_SERVER_X11: {
			return window->x11_window->window;
		} break;

#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
			return 0;
	};
}

PlatformInput *platform_window_poll(const PlatformWindow *window) {
	switch (window->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			return wayland_window_poll(window->wayland_window, window);
			break;
		} break;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			return x11_window_poll(window->x11_window);
			break;
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};

	return NULL;
}

void platform_window_set_title(const PlatformWindow *window, String title) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_set_title(window->x11_window, title);
			break;
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};
}

void platform_window_set_size(const PlatformWindow *window, int32 width, int32 height) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_set_size(window->x11_window, width, height);
			break;
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};
}