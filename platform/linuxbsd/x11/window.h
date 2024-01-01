#ifndef X11_WINDOW_H
#define X11_WINDOW_H

#include "core/types/slice.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

#include "platform/linuxbsd/x11/display_server.h"
#include "platform/window.h"
#include "renderer/window.h"

#include <X11/Xlib.h>

static const long EVENT_MASK = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask |
		ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | CWColormap | StructureNotifyMask;

typedef struct {
	Window window;

	int32 width;
	int32 height;
	String title;

	bool fullscreen;
	bool hidden;

	Display *display;

	InputManager *input_manager;
} X11Window;

X11Window *x11_window_create(const X11Server *server, WindowConfig config);
void x11_window_destroy(X11Window *window);

void x11_window_poll(X11Window *window);

void x11_window_set_title(X11Window *window, String title);
void x11_window_set_size(X11Window *window, int32 width, int32 height);
Vector2i x11_window_get_size(const X11Window *window);

void x11_window_set_fullscreen(X11Window *window, bool fullscreen);

void x11_window_show(X11Window *window);
void x11_window_hide(X11Window *window);

bool x11_window_is_visible(const X11Window *window);
bool x11_window_is_fullscreen(const X11Window *window);

#endif // X11_WINDOW_H