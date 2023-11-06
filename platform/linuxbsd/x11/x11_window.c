#include "platform/linuxbsd/x11/x11_window.h"

#include "core/events/events.h"
#include "core/input/input.h"
#include "core/os/os_window.h"
#include "platform/linuxbsd/x11/x11_keymap.h"

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

	XStoreName(display, window->window, title);

	XSelectInput(display, window->window,
			ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask |
					ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask);
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
				input_handle_press(parent, x11_map_key(event.xkey.keycode));
			} break;

			case KeyRelease: {
				XEvent nev;
				if (XPending(display)) {
					XPeekEvent(display, &nev);
					if (nev.type == KeyPress && nev.xkey.time == event.xkey.time && nev.xkey.keycode == event.xkey.keycode) {
						break;
					}
				}

				input_handle_release(parent, x11_map_key(event.xkey.keycode));
			} break;

			case ButtonPress: {
				input_handle_mouse_press(parent, x11_map_mbutton(event.xbutton.button), (Vector2i){ event.xbutton.x, event.xbutton.y });
			} break;

			case ButtonRelease: {
				input_handle_mouse_release(parent, x11_map_mbutton(event.xbutton.button), (Vector2i){ event.xbutton.x, event.xbutton.y });
			} break;

			case MotionNotify: {
				input_handle_mouse_move(parent, (Vector2i){ event.xmotion.x, event.xmotion.y });
			} break;

			case EnterNotify: {
				input_handle_mouse_enter(parent, (Vector2i){ event.xcrossing.x, event.xcrossing.y });
			} break;

			case LeaveNotify: {
				input_handle_mouse_leave(parent, (Vector2i){ event.xcrossing.x, event.xcrossing.y });
			} break;
		}
	}
}