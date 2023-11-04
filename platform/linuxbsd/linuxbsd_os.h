#ifndef LINUXBSD_OS_H
#define LINUXBSD_OS_H

#if defined(WAYLAND_ENABLED)
#include "platform/linuxbsd/wayland/wayland_server.h"
#endif

#if defined(X11_ENABLED)
#include "platform/linuxbsd/x11/x11_server.h"
#endif

#include "core/os/os.h"

typedef enum {
	DISPLAY_SERVER_WAYLAND,
	DISPLAY_SERVER_X11
} DisplayServer;

struct OS {
	DisplayServer display_server;
	union {
#if defined(WAYLAND_ENABLED)
		WaylandServer *wayland_server;
#endif

#if defined(X11_ENABLED)
		X11Server *x11_server;
#endif
	};
};

#endif // LINUXBSD_OS_H