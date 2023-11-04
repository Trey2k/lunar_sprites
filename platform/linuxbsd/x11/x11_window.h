#ifndef X11_WINDOW_H
#define X11_WINDOW_H

#include "core/core.h"
#include "platform/linuxbsd/x11/x11_server.h"

#include <X11/Xlib.h>

typedef struct {
	Window window;

	X11Server *server;

	int32 width;
	int32 height;
	String title;

	bool maximized;
} X11Window;

X11Window *x11_window_create(X11Server *server, const String title, int32 width, int32 height);
void x11_window_destroy(X11Window *window);

void x11_window_poll(X11Window *window);

#endif // X11_WINDOW_H