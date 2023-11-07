#include "platform/linuxbsd/x11/x11_window.h"

#include "core/events/events.h"
#include "core/input/input.h"
#include "core/os/window.h"
#include "platform/linuxbsd/linuxbsd_os.h"
#include "platform/linuxbsd/x11/x11_keymap.h"

#include <X11/Xlib.h>

X11Window *x11_window_create(String title, int32 width, int32 height) {
	X11Window *window = core_malloc(sizeof(X11Window));
	window->width = width;
	window->height = height;

	const X11Server *server = os_linuxbsd_get_x11_server();

	Display *display = server->display;
	const int32 screen = server->default_screen;
	Window root = RootWindow(display, screen);
	Visual *visual = DefaultVisual(display, screen);

	Colormap colormap = XCreateColormap(display, root, visual, AllocNone);

	XSetWindowAttributes attributes;
	attributes.colormap = colormap;
	attributes.event_mask = EVENT_MASK;

	window->window = XCreateWindow(display, root, 0, 0, width, height, 0,
			DefaultDepth(display, screen), InputOutput, visual,
			CWColormap | CWEventMask, &attributes);
	if (!window->window) {
		core_log(LOG_LEVEL_ERROR, "Failed to create X11 window\n");
		return NULL;
	}

	XFreeColormap(display, colormap);
	XMapWindow(display, window->window);
	x11_window_set_title(window, title);

	return window;
}

void x11_window_destroy(X11Window *window) {
	const X11Server *server = os_linuxbsd_get_x11_server();
	XDestroyWindow(server->display, window->window);
	core_free(window);
}

void x11_window_poll(const X11Window *window, const LSWindow *parent) {
	const X11Server *server = os_linuxbsd_get_x11_server();
	Display *display = server->display;
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

void x11_window_set_title(const X11Window *window, String title) {
	const X11Server *server = os_linuxbsd_get_x11_server();
	XStoreName(server->display, window->window, title);
}

void x11_window_set_size(const X11Window *window, int32 width, int32 height) {
	const X11Server *server = os_linuxbsd_get_x11_server();
	XResizeWindow(server->display, window->window, width, height);
}