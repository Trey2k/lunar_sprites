#include "platform/linuxbsd/x11/display_server.h"
#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"

X11Server *x11_server_create() {
	X11Server *server = ls_malloc(sizeof(X11Server));

	server->display = XOpenDisplay(NULL);
	if (!server->display) {
		ls_log(LOG_LEVEL_ERROR, "Failed to open X11 display\n");
		return NULL;
	}

	server->default_screen = DefaultScreen(server->display);

	return server;
}

void x11_server_destroy(X11Server *server) {
	LS_ASSERT(server);

	XCloseDisplay(server->display);
	ls_free(server);
}