#ifndef LINUXBSD_WINDOW_H
#define LINUXBSD_WINDOW_H

#include "platform/window.h"

#include "platform/linuxbsd/os.h"

#if defined(WAYLAND_ENABLED)
#include "platform/linuxbsd/wayland/wayland_window.h"
#endif

#if defined(X11_ENABLED)
#include "platform/linuxbsd/x11/window.h"
#endif

struct PlatformWindow {
	DisplayServer display_server;
	union {
#if defined(X11_ENABLED)
		X11Window *x11_window;
#endif

#if defined(WAYLAND_ENABLED)
		WaylandWindow *wayland_window;
#endif
	};
};

#endif // LINUXBSD_WINDOW_H