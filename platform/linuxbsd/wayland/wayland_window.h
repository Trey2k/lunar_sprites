#ifndef WAYLAND_WINDOW_H
#define WAYLAND_WINDOW_H

#include "core/core.h"
#include "platform/linuxbsd/wayland/wayland_server.h"

#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-egl.h>

typedef struct {
	struct wl_surface *surface;
	struct xdg_surface *xdg_surface;
	struct xdg_toplevel *xdg_toplevel;
	struct wl_egl_window *egl_window;

	int32 width;
	int32 height;
	uint32 border_size;
	uint32 title_size;

	bool maximized;
} WaylandWindow;

WaylandWindow *wayland_window_create(const WaylandServer *server, String title, int32 width, int32 height);
void wayland_window_destroy(WaylandWindow *window);

void wayland_window_poll(const WaylandWindow *window, const LSWindow *parent);

void wayland_window_resize(WaylandWindow *window, int32 width, int32 height, bool full_screen);
void wayland_window_set_title(WaylandWindow *window, String title);

#endif // WAYLAND_WINDOW_H