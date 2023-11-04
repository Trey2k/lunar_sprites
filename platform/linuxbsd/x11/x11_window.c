#include "platform/linuxbsd/x11/x11_window.h"
#include <X11/Xlib.h>

X11Window *x11_window_create(X11Server *server, String title, int32 width, int32 height) {
	CORE_ASSERT(server);

	X11Window *window = core_malloc(sizeof(X11Window));
	window->server = server;
	window->width = width;
	window->height = height;

	Display *display = server->display;
	const int32 screen = server->default_screen;

	window->window = XCreateSimpleWindow(
			display,
			RootWindow(display, screen),
			10, 10, width, height, 1,
			BlackPixel(display, screen),
			WhitePixel(display, screen));
	if (!window->window) {
		core_log(LOG_LEVEL_ERROR, "Failed to create X11 window\n");
		return NULL;
	}

	XSelectInput(display, window->window, ExposureMask | KeyPressMask);
	XMapWindow(display, window->window);

	return window;
}

void x11_window_destroy(X11Window *window) {
	XDestroyWindow(window->server->display, window->window);
	core_free(window);
}

void x11_window_poll(X11Window *window) {
	Display *display = window->server->display;
	XEvent event;
	while (XPending(display)) {
		XNextEvent(display, &event);
		switch (event.type) {
			case Expose:
				break;
			case KeyPress: {
				Event e;
				e.type = EVENT_KEYPRESS;
				e.keypress.keycode = event.xkey.keycode;
				event_manager_emit(window->server->event_manager, &e);
			} break;
		}
	}
}