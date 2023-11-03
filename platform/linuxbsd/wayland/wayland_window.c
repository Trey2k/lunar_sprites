#include "platform/linuxbsd/wayland/wayland_window.h"

#include "core/core.h"
#include "core/core_log.h"
#include "core/math/math.h"
#include "core/os/os_window.h"
#include "platform/linuxbsd/wayland/xdg-shell.gen.h"

WaylandWindow *wayland_window_create(WaylandServer *server, String title, int32 width, int32 height) {
	struct wl_surface *surface = wl_compositor_create_surface(server->compositor);
	if (!surface) {
		core_log(LOG_LEVEL_ERROR, "Failed to create Wayland surface\n");
		return NULL;
	}

	WaylandWindow *window = core_malloc(sizeof(WaylandWindow));
	window->width = width;
	window->surface = surface;

	if (server->xdg_wm_base) {
		window->xdg_surface = xdg_wm_base_get_xdg_surface(server->xdg_wm_base, surface);
		window->xdg_toplevel = xdg_surface_get_toplevel(window->xdg_surface);
		// xdg_surface_add_listener(window->xdg_surface, &xdg_surface_listener, window);
		// xdg_toplevel_add_listener(window->xdg_toplevel, &xdg_toplevel_listener, window);
		// xdg_wm_base_add_listener(server->xdg_wm_base, &xdg_wm_base_listener, window);
		xdg_toplevel_set_title(window->xdg_toplevel, title);
		xdg_toplevel_set_app_id(window->xdg_toplevel, "LunarSprites"); //TODO: Make this configurable
	} else {
		core_log(LOG_LEVEL_WARNING, "No xdg_wm_base.\n");
	}

	return window;
}

void wayland_window_destroy(WaylandWindow *window) {
	wl_surface_destroy(window->surface);
	core_free(window);
}

void wayland_window_resize(WaylandWindow *window, int32 width, int32 height, bool full_screen) {
	int32 main_w, main_h;
	if (full_screen) {
		main_w = width - 2 * window->border_size;
		main_h = height - 2 * window->border_size - window->title_size;
	} else {
		main_w = width;
		main_h = height;
	}

	main_w = math_max(main_w, MIN_WINDOW_WIDTH);
	main_h = math_max(main_h, MIN_WINDOW_HEIGHT);
}