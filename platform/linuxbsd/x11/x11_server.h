#ifndef X11_SERVER_H
#define X11_SERVER_H

#include "core/input/input.h"
#include "renderer/renderer.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
	Display *display;

	int32 default_screen;
	void *best_fbc;
} X11Server;

X11Server *x11_server_create();
void x11_server_destroy(X11Server *server);

#endif // X11_SERVER_H