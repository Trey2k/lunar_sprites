#include "platform/linuxbsd/linuxbsd_os_window.h"
#include "core/core.h"
#include "core/core_log.h"
#include "platform/linuxbsd/wayland/wayland_window.h"

LSWindow *os_window_create(const OS *os, const String title, int32 width, int32 height) {
	switch (os->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			WaylandWindow *win = wayland_window_create(os->wayland_server, title, width, height);
			if (!win) {
				core_log(LOG_LEVEL_ERROR, "Failed to create Wayland window\n");
				return NULL;
			}

			LSWindow *window = core_malloc(sizeof(LSWindow));
			window->display_server = DISPLAY_SERVER_WAYLAND;
			window->wayland_window = win;
			return window;
		} break;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			X11Window *win = x11_window_create(os->x11_server, title, width, height);
			if (!win) {
				core_log(LOG_LEVEL_ERROR, "Failed to create X11 window\n");
				return NULL;
			}

			LSWindow *window = core_malloc(sizeof(LSWindow));
			window->display_server = DISPLAY_SERVER_X11;
			window->x11_window = win;
			return window;
		} break;
#endif
		default:
			core_fatal("Unknown display server: %d\n", os->display_server);
			return NULL;
	};
}

void window_destroy(LSWindow *window) {
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
			core_fatal("Unknown display server: %d\n", window->display_server);
	};

	core_free(window);
}

void window_poll(const LSWindow *window) {
	switch (window->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			wayland_window_poll(window->wayland_window);
			break;
		} break;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_poll(window->x11_window);
			break;
		} break;
#endif
		default:
			core_fatal("Unknown display server: %d\n", window->display_server);
	};
}