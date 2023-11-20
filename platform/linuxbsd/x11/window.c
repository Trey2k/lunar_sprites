#include "platform/linuxbsd/x11/window.h"

#include "core/core.h"

#include "platform/linuxbsd/x11/keymap.h"
#include "platform/window.h"

#include <X11/Xlib.h>

static void handle_event(const X11Window *window, XEvent *event);

X11Window *x11_window_create(const X11Server *server, WindowConfig config) {
	LS_ASSERT(server);

	Display *display = server->display;
	const int32 screen = server->default_screen;
	Window root = RootWindow(display, screen);

	Visual *visual = DefaultVisual(display, screen);

	X11Window *window = ls_malloc(sizeof(X11Window));
	window->width = config.size.x;
	window->height = config.size.y;
	window->display = display;
	window->hidden = config.hidden;
	window->fullscreen = false;
	window->title = config.title;
	window->input_manager = server->input_manager;

	Colormap colormap = XCreateColormap(display, root, visual, AllocNone);

	XSetWindowAttributes attributes;
	attributes.colormap = colormap;
	attributes.event_mask = EVENT_MASK;

	window->window = XCreateWindow(display, root, config.position.x, config.position.y, window->width, window->height, 0,
			DefaultDepth(display, screen), InputOutput, visual,
			CWColormap | CWEventMask, &attributes);
	XFreeColormap(display, colormap);
	if (!window->window) {
		ls_log(LOG_LEVEL_ERROR, "Failed to create X11 window\n");
		return NULL;
	}

	if (!config.hidden) {
		x11_window_show(window);
	}

	x11_window_set_fullscreen(window, config.fullscreen);

	x11_window_set_title(window, window->title);

	return window;
}

void x11_window_destroy(X11Window *window) {
	XDestroyWindow(window->display, window->window);
	ls_free(window);
}

void x11_window_poll(X11Window *window) {
	XEvent event;

	while (XPending(window->display)) {
		XNextEvent(window->display, &event);
		handle_event(window, &event);
	}
}

void x11_window_set_title(X11Window *window, String title) {
	XStoreName(window->display, window->window, title);
}

void x11_window_set_size(X11Window *window, int32 width, int32 height) {
	XResizeWindow(window->display, window->window, width, height);
}

static void handle_event(const X11Window *window, XEvent *event) {
	switch (event->type) {
		case Expose:
			break;
		case KeyPress: {
			input_handle_press(window->input_manager, x11_map_key(event->xkey.keycode));
		} break;

		case KeyRelease: {
			XEvent nev;
			if (XPending(window->display)) {
				XPeekEvent(window->display, &nev);
				if (nev.type == KeyPress && nev.xkey.time == event->xkey.time && nev.xkey.keycode == event->xkey.keycode) {
					break;
				}
			}

			input_handle_release(window->input_manager, x11_map_key(event->xkey.keycode));
		} break;

		case ButtonPress: {
			input_handle_mouse_press(window->input_manager, x11_map_mbutton(event->xbutton.button),
					(Vector2i){ event->xbutton.x, event->xbutton.y });

		} break;

		case ButtonRelease: {
			input_handle_mouse_release(window->input_manager, x11_map_mbutton(event->xbutton.button),
					(Vector2i){ event->xbutton.x, event->xbutton.y });
		} break;

		case MotionNotify: {
			input_handle_mouse_move(window->input_manager, (Vector2i){ event->xmotion.x, event->xmotion.y });
		} break;

		case EnterNotify: {
			input_handle_mouse_enter(window->input_manager, (Vector2i){ event->xcrossing.x, event->xcrossing.y });
		} break;

		case LeaveNotify: {
			input_handle_mouse_leave(window->input_manager, (Vector2i){ event->xcrossing.x, event->xcrossing.y });
		} break;
	}
}

void x11_window_set_fullscreen(X11Window *window, bool fullscreen) {
	LS_ASSERT(window);
	if (window->fullscreen == fullscreen) {
		return;
	}

	window->fullscreen = fullscreen;

	XEvent xev = { 0 };
	xev.xclient.type = ClientMessage;
	xev.xclient.window = window->window;
	xev.xclient.message_type = XInternAtom(window->display, "_NET_WM_STATE", false);
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = window->fullscreen ? 1 : 0; // 1 for adding the hint, 0 for removing
	xev.xclient.data.l[1] = XInternAtom(window->display, "_NET_WM_STATE_FULLSCREEN", false);
	xev.xclient.data.l[2] = 0; // no second property to change

	XSendEvent(window->display, DefaultRootWindow(window->display), false, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}

void x11_window_show(X11Window *window) {
	XMapWindow(window->display, window->window);
	window->hidden = false;
}

void x11_window_hide(X11Window *window) {
	XUnmapWindow(window->display, window->window);
	window->hidden = true;
}

bool x11_window_is_visible(const X11Window *window) {
	LS_ASSERT(window);

	return !window->hidden;
}

bool x11_window_is_fullscreen(const X11Window *window) {
	LS_ASSERT(window);

	return window->fullscreen;
}