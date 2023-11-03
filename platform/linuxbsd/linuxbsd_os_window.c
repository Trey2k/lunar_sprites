#include "platform/linuxbsd/linuxbsd_os_window.h"
#include "core/core.h"
#include "core/core_log.h"
#include "platform/linuxbsd/wayland/wayland_window.h"

Window *os_window_create(const OS *os, const char *title, int width, int height) {
	switch (os->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			WaylandWindow *win = wayland_window_create(os->wayland_server, title, width, height);
			if (!win) {
				core_log(LOG_LEVEL_ERROR, "Failed to create Wayland window\n");
				return NULL;
			}

			Window *window = core_malloc(sizeof(Window));
			window->display_server = DISPLAY_SERVER_WAYLAND;
			window->wayland_window = win;
			return window;
		} break;
#endif

		default:
			core_fatal("Unknown display server: %d\n", os->display_server);
			return NULL;
	};
}

void os_window_destroy(const OS *os, Window *window) {
	switch (window->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			wayland_window_destroy(window->wayland_window);
			break;
		} break;
#endif

		default:
			core_fatal("Unknown display server: %d\n", window->display_server);
	};

	core_free(window);
}