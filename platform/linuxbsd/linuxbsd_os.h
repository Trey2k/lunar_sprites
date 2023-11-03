#ifndef LINUXBSD_OS_H
#define LINUXBSD_OS_H

#include "platform/linuxbsd/wayland/wayland_server.h"

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
	};
};

#endif // LINUXBSD_OS_H