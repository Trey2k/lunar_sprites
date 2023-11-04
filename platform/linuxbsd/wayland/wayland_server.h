#ifndef WAYLAND_SERVER_H
#define WAYLAND_SERVER_H

#include "core/event/event_manager.h"
#include "platform/linuxbsd/wayland/xdg-shell.gen.h"
#include <wayland-client.h>

typedef struct {
	struct wl_display *display;
	struct wl_compositor *compositor;
	struct wl_subcompositor *subcompositor;
	struct xdg_wm_base *xdg_wm_base;
	struct wl_seat *seat;
	struct wl_shm *shm;
	struct wl_cursor_theme *cursor_theme;
	struct wl_surface *cursor_surface;

	const EventManager *event_manager;
} WaylandServer;

WaylandServer *wayland_server_create();
void wayland_server_destroy(WaylandServer *server);

#endif // WAYLAND_SERVER_H