#include "platform/linuxbsd/wayland/wayland_server.h"

#include "core/core.h"
#include "core/core_log.h"
#include "core/typedefs.h"

#include <wayland-cursor.h>

static void global_registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version) {
	WaylandServer *server = (WaylandServer *)data;
	CORE_ASSERT_MSG(server, "Wayland server is NULL");

	if (string_equals(interface, "wl_compositor")) {
		server->compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
		core_log(LOG_LEVEL_INFO, "Found compositor\n");
	} else if (string_equals(interface, "wl_subcompositor")) {
		server->subcompositor = wl_registry_bind(registry, id, &wl_subcompositor_interface, 1);
		core_log(LOG_LEVEL_INFO, "Found subcompositor\n");
	} else if (string_equals(interface, "wl_seat")) {
		server->seat = wl_registry_bind(registry, id, &wl_seat_interface, 1);
		core_log(LOG_LEVEL_INFO, "Found seat\n");
	} else if (string_equals(interface, "wl_shm")) {
		server->shm = wl_registry_bind(registry, id, &wl_shm_interface, 1);
		server->cursor_theme = wl_cursor_theme_load(NULL, 32, server->shm);
		core_log(LOG_LEVEL_INFO, "Found shared memory\n");
	} else if (string_equals(interface, xdg_wm_base_interface.name)) {
		server->xdg_wm_base = wl_registry_bind(registry, id, &xdg_wm_base_interface, 1);
		core_log(LOG_LEVEL_INFO, "Found xdg_wm_base\n");
	}
}

static void global_registry_remover(void *data, struct wl_registry *registry, uint32_t id) {
	core_log(LOG_LEVEL_INFO, "Got a registry losing event for %d\n", id);
}

static const struct wl_registry_listener registry_listener = {
	global_registry_handler,
	global_registry_remover
};

WaylandServer *wayland_server_create() {
	struct wl_display *display = wl_display_connect(NULL);
	if (!display) {
		return NULL;
	}

	struct wl_registry *registry = wl_display_get_registry(display);
	CORE_ASSERT_MSG(registry, "Failed to get Wayland registry\n");

	WaylandServer *server = core_malloc(sizeof(WaylandServer));
	server->event_manager = core_get_event_manager();
	CORE_ASSERT(server->event_manager);
	server->display = display;

	wl_registry_add_listener(registry, &registry_listener, server);

	wl_display_dispatch(display);
	wl_display_roundtrip(display);

	if (!server->compositor) {
		wayland_server_destroy(server);
		core_log(LOG_LEVEL_WARNING, "Failed to find Wayland compositor.\n");
		return NULL;
	}

	if (!server->subcompositor) {
		wayland_server_destroy(server);
		core_log(LOG_LEVEL_WARNING, "Failed to find Wayland subcompositor.\n");
		return NULL;
	}

	if (!server->seat) {
		wayland_server_destroy(server);
		core_log(LOG_LEVEL_WARNING, "Failed to find Wayland seat.\n");
		return NULL;
	}

	if (!server->shm) {
		wayland_server_destroy(server);
		core_log(LOG_LEVEL_WARNING, "Failed to find Wayland shared memory.\n");
		return NULL;
	}

	return server;
}

void wayland_server_destroy(WaylandServer *server) {
	if (!server) {
		return;
	}

	wl_display_disconnect(server->display);
	core_free(server);
}