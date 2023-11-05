#include "platform/linuxbsd/x11/x11_window.h"
#include "core/input/events.h"
#include "core/input/input_methods.h"
#include <X11/Xlib.h>

#include "core/os/os_window.h"
#include "platform/linuxbsd/x11/x11_keymap.h"

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

	XSelectInput(display, window->window, ExposureMask | KeyPressMask | KeyReleaseMask);
	XMapWindow(display, window->window);

	return window;
}

void x11_window_destroy(X11Window *window) {
	XDestroyWindow(window->server->display, window->window);
	core_free(window);
}

void x11_window_poll(const X11Window *window, const LSWindow *parent) {
	Display *display = window->server->display;
	XEvent event;

	while (XPending(display)) {
		XNextEvent(display, &event);
		switch (event.type) {
			case Expose:
				break;
			case KeyPress: {
				input_handle_press(window->server->input, parent, x11_map_key(event.xkey.keycode));
			} break;

			case KeyRelease: {
				XEvent nev;
				if (XPending(display)) {
					XPeekEvent(display, &nev);
					if (nev.type == KeyPress && nev.xkey.time == event.xkey.time && nev.xkey.keycode == event.xkey.keycode) {
						break;
					}
				}

				input_handle_release(window->server->input, parent, x11_map_key(event.xkey.keycode));
			} break;
		}
	}
}