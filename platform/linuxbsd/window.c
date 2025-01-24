#include "platform/linuxbsd/window.h"
#include "core/log.h"
#include "core/memory.h"
#include "platform/window.h"
#include "renderer/window.h"

#if defined(WAYLAND_ENABLED)
#include "platform/linuxbsd/wayland/window.h"
#endif

#if defined(X11_ENABLED)
#include "platform/linuxbsd/x11/window.h"
#endif

PlatformWindow *platform_create_window(const PlatformOS *os, WindowConfig config, LSWindow *parent) {
	PlatformWindow *window = ls_malloc(sizeof(PlatformWindow));
	window->display_server = os->display_server;

	switch (window->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			WaylandWindow *win = wayland_window_create(os->wayland_server, config);
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
			X11Window *win = x11_window_create(os->x11_server, config);
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

LSNativeWindow platform_window_get_native_window(const PlatformWindow *window) {
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

void platform_window_poll(PlatformWindow *window) {
	switch (window->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND: {
			wayland_window_poll(window->wayland_window, window);
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
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};
}

void platform_window_set_title(PlatformWindow *window, String title) {
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

void platform_window_set_min_size(PlatformWindow *window, Vector2u size) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_set_min_size(window->x11_window, size);
			break;
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};
}

Vector2u platform_window_get_min_size(const PlatformWindow *window) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			return x11_window_get_min_size(window->x11_window);
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
			return vec2u(0, 0);
	};
}

void platform_window_set_max_size(PlatformWindow *window, Vector2u size) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_set_max_size(window->x11_window, size);
			break;
		} break;

#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};
}

Vector2u platform_window_get_max_size(const PlatformWindow *window) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			return x11_window_get_max_size(window->x11_window);
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
			return vec2u(0, 0);
	};
}

void platform_window_set_size(PlatformWindow *window, Vector2u size) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_set_size(window->x11_window, size);
			break;
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};
}

Vector2u platform_window_get_size(const PlatformWindow *window) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			return x11_window_get_size(window->x11_window);
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
			return vec2u(0, 0);
	};
}

void platform_window_set_fullscreen(PlatformWindow *window, bool fullscreen) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_set_fullscreen(window->x11_window, fullscreen);
			break;
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};
}

void platform_window_show(PlatformWindow *window) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_show(window->x11_window);
			break;
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};
}

void platform_window_hide(PlatformWindow *window) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			x11_window_hide(window->x11_window);
			break;
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
	};
}

bool platform_window_is_visible(const PlatformWindow *window) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			return x11_window_is_visible(window->x11_window);
		} break;

#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
			return false;
	};
}

bool platform_window_is_fullscreen(const PlatformWindow *window) {
	switch (window->display_server) {
#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11: {
			return x11_window_is_fullscreen(window->x11_window);
		} break;
#endif

		default:
			ls_log_fatal("Unknown display server: %d\n", window->display_server);
			return false;
	};
}