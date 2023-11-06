#include "core/core.h"
#include "core/core_log.h"
#include "core/os/window.h"
#include "platform/linuxbsd/linuxbsd_os.h"
#include "renderer/context.h"

#if defined(WAYLAND_ENABLED)
#include "platform/linuxbsd/wayland/wayland_window.h"
#endif

#if defined(X11_ENABLED)
#include "platform/linuxbsd/linuxbsd_os_window.h"
#endif

LSWindow *window_create(String title, int32 width, int32 height) {
	const DisplayServer display_server = os_linuxbsd_get_display_server();
	LSWindow *window = core_malloc(sizeof(LSWindow));
	window->display_server = display_server;

	switch (display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			WaylandWindow *win = wayland_window_create(title, width, height);
			if (!win) {
				core_log(LOG_LEVEL_ERROR, "Failed to create Wayland window\n");
				return NULL;
			}
			window->wayland_window = win;
		} break;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			X11Window *win = x11_window_create(title, width, height);
			if (!win) {
				core_log(LOG_LEVEL_ERROR, "Failed to create X11 window\n");
				return NULL;
			}
			window->x11_window = win;
		} break;
#endif

		default:
			core_fatal("Unknown display server: %d\n", display_server);
			return NULL;
	};

	window->context = context_create(window);
	context_make_current(window->context);

	return window;
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

unsigned long window_get_native_window(const LSWindow *window) {
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
			core_fatal("Unknown display server: %d\n", window->display_server);
			return 0;
	};
}

void window_poll(const LSWindow *window) {
	switch (window->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			wayland_window_poll(window->wayland_window, window);
			break;
		} break;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_poll(window->x11_window, window);
			break;
		} break;
#endif

		default:
			core_fatal("Unknown display server: %d\n", window->display_server);
	};
}

void window_set_title(const LSWindow *window, String title) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_set_title(window->x11_window, title);
			break;
		} break;
#endif

		default:
			core_fatal("Unknown display server: %d\n", window->display_server);
	};
}

void window_set_size(const LSWindow *window, int32 width, int32 height) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_set_size(window->x11_window, width, height);
			break;
		} break;
#endif

		default:
			core_fatal("Unknown display server: %d\n", window->display_server);
	};
}