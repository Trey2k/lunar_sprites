#include "platform/linuxbsd/x11/window.h"
#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"
#include "platform/linuxbsd/x11/keymap.h"
#include "platform/window.h"

#include <X11/Xlib.h>

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

PlatformInput *x11_window_poll(const X11Window *window) {
	XEvent event;

	PlatformInput *input = NULL;
	size_t i = 0;

	while (XPending(window->display)) {
		XNextEvent(window->display, &event);
		switch (event.type) {
			case Expose:
				break;
			case KeyPress: {
				input = ls_realloc(input, sizeof(PlatformInput) * (i + 1));
				input[i].type = PLATFORM_INPUT_KEYPRESS;
				input[i].keycode = x11_map_key(event.xkey.keycode);
				i++;
			} break;

			case KeyRelease: {
				XEvent nev;
				if (XPending(window->display)) {
					XPeekEvent(window->display, &nev);
					if (nev.type == KeyPress && nev.xkey.time == event.xkey.time && nev.xkey.keycode == event.xkey.keycode) {
						break;
					}
				}

				input = ls_realloc(input, sizeof(PlatformInput) * (i + 1));
				input[i].type = PLATFORM_INPUT_KEYRELEASE;
				input[i].keycode = x11_map_key(event.xkey.keycode);
				i++;
			} break;

			case ButtonPress: {
				input = ls_realloc(input, sizeof(PlatformInput) * (i + 1));
				input[i].type = PLATFORM_INPUT_MOUSEPRESS;
				input[i].button = x11_map_mbutton(event.xbutton.button);
				input[i].position.x = event.xbutton.x;
				input[i].position.y = event.xbutton.y;
				i++;

			} break;

			case ButtonRelease: {
				input = ls_realloc(input, sizeof(PlatformInput) * (i + 1));
				input[i].type = PLATFORM_INPUT_MOUSERELEASE;
				input[i].button = x11_map_mbutton(event.xbutton.button);
				input[i].position.x = event.xbutton.x;
				input[i].position.y = event.xbutton.y;
				i++;
			} break;

			case MotionNotify: {
				input = ls_realloc(input, sizeof(PlatformInput) * (i + 1));
				input[i].type = PLATFORM_INPUT_MOUSEMOVE;
				input[i].position.x = event.xmotion.x;
				input[i].position.y = event.xmotion.y;
				i++;
			} break;

			case EnterNotify: {
				input = ls_realloc(input, sizeof(PlatformInput) * (i + 1));
				input[i].type = PLATFORM_INPUT_MOUSEENTER;
				input[i].position.x = event.xcrossing.x;
				input[i].position.y = event.xcrossing.y;
				i++;
			} break;

			case LeaveNotify: {
				input = ls_realloc(input, sizeof(PlatformInput) * (i + 1));
				input[i].type = PLATFORM_INPUT_MOUSELEAVE;
				input[i].position.x = event.xcrossing.x;
				input[i].position.y = event.xcrossing.y;
				i++;
			} break;
		}
	}

	input = ls_realloc(input, sizeof(PlatformInput) * (i + 1));
	input[i].type = PLATFORM_INPUT_NONE;

	return input;
}

void x11_window_set_title(const X11Window *window, String title) {
	XStoreName(window->display, window->window, title);
}

void x11_window_set_size(const X11Window *window, int32 width, int32 height) {
	XResizeWindow(window->display, window->window, width, height);
}