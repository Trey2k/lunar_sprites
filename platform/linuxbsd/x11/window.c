#include "platform/linuxbsd/x11/window.h"

#include "core/core.h"

#include "platform/linuxbsd/x11/keymap.h"
#include "platform/window.h"

#include <X11/Xlib.h>

static void handle_event(const X11Window *window, XEvent *event);

X11Window *x11_window_create(const X11Server *server, String title, int32 width, int32 height) {
	LS_ASSERT(server);

	Display *display = server->display;
	const int32 screen = server->default_screen;
	Window root = RootWindow(display, screen);
	Visual *visual = DefaultVisual(display, screen);

	X11Window *window = ls_malloc(sizeof(X11Window));
	window->width = width;
	window->height = height;
	window->display = display;

	Colormap colormap = XCreateColormap(display, root, visual, AllocNone);

	XSetWindowAttributes attributes;
	attributes.colormap = colormap;
	attributes.event_mask = EVENT_MASK;

	window->window = XCreateWindow(display, root, 0, 0, width, height, 0,
			DefaultDepth(display, screen), InputOutput, visual,
			CWColormap | CWEventMask, &attributes);
	if (!window->window) {
		ls_log(LOG_LEVEL_ERROR, "Failed to create X11 window\n");
		return NULL;
	}

	XFreeColormap(display, colormap);
	XMapWindow(display, window->window);
	x11_window_set_title(window, title);

	return window;
}

void x11_window_destroy(X11Window *window) {
	XDestroyWindow(window->display, window->window);
	ls_free(window);
}

void x11_window_poll(const X11Window *window) {
	XEvent event;

	while (XPending(window->display)) {
		XNextEvent(window->display, &event);
		handle_event(window, &event);
	}
}

void x11_window_set_title(const X11Window *window, String title) {
	XStoreName(window->display, window->window, title);
}

void x11_window_set_size(const X11Window *window, int32 width, int32 height) {
	XResizeWindow(window->display, window->window, width, height);
}

static void handle_event(const X11Window *window, XEvent *event) {
	switch (event->type) {
		case Expose:
			break;
		case KeyPress: {
			core_handle_press(x11_map_key(event->xkey.keycode));
		} break;

		case KeyRelease: {
			XEvent nev;
			if (XPending(window->display)) {
				XPeekEvent(window->display, &nev);
				if (nev.type == KeyPress && nev.xkey.time == event->xkey.time && nev.xkey.keycode == event->xkey.keycode) {
					break;
				}
			}

			core_handle_release(x11_map_key(event->xkey.keycode));
		} break;

		case ButtonPress: {
			core_handle_mouse_press(x11_map_mbutton(event->xbutton.button),
					(Vector2i){ event->xbutton.x, event->xbutton.y });

		} break;

		case ButtonRelease: {
			core_handle_mouse_release(x11_map_mbutton(event->xbutton.button),
					(Vector2i){ event->xbutton.x, event->xbutton.y });
		} break;

		case MotionNotify: {
			core_handle_mouse_move((Vector2i){ event->xmotion.x, event->xmotion.y });
		} break;

		case EnterNotify: {
			core_handle_mouse_enter((Vector2i){ event->xcrossing.x, event->xcrossing.y });
		} break;

		case LeaveNotify: {
			core_handle_mouse_leave((Vector2i){ event->xcrossing.x, event->xcrossing.y });
		} break;
	}
}