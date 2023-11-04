#include "platform/linuxbsd/x11/x11_server.h"
#include "core/core.h"

X11Server *x11_server_create() {
	X11Server *server = core_malloc(sizeof(X11Server));
	server->event_manager = core_get_event_manager();
	CORE_ASSERT(server->event_manager);

	server->display = XOpenDisplay(NULL);
	if (!server->display) {
		core_log(LOG_LEVEL_ERROR, "Failed to open X11 display\n");
		return NULL;
	}

	server->default_screen = DefaultScreen(server->display);
	return server;
}

void x11_server_destroy(X11Server *server) {
	XCloseDisplay(server->display);
	core_free(server);
}