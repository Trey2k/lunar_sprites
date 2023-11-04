#ifndef LINUXBSD_OS_WINDOW_H
#define LINUXBSD_OS_WINDOW_H

#include "core/os/os_window.h"

#include "platform/linuxbsd/linuxbsd_os.h"

#if defined(WAYLAND_ENABLED)
#include "platform/linuxbsd/wayland/wayland_window.h"
#endif

#if defined(X11_ENABLED)
#include "platform/linuxbsd/x11/x11_window.h"
#endif

struct LSWindow {
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

#endif // LINUXBSD_OS_WINDOW_H