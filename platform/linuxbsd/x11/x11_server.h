#ifndef X11_SERVER_H
#define X11_SERVER_H

#include "core/event/event_manager.h"
#include <X11/Xlib.h>

typedef struct {
	Display *display;

	const EventManager *event_manager;

	int default_screen;
} X11Server;

X11Server *x11_server_create();
void x11_server_destroy(X11Server *server);

#endif // X11_SERVER_H