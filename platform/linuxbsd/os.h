#ifndef LINUXBSD_OS_H
#define LINUXBSD_OS_H

#include "core/os/os.h"
#include "platform/os.h"

#if defined(WAYLAND_ENABLED)
#include "platform/linuxbsd/wayland/wayland_server.h"
#endif

#if defined(X11_ENABLED)
#include "platform/linuxbsd/x11/display_server.h"
#endif

typedef enum {
	DISPLAY_SERVER_NONE,
	DISPLAY_SERVER_WAYLAND,
	DISPLAY_SERVER_X11
} DisplayServer;

DisplayServer os_linuxbsd_get_display_server();

#if defined(WAYLAND_ENABLED)
WaylandServer *os_linuxbsd_get_wayland_server(const PlatformOS *os);
#endif

#if defined(X11_ENABLED)
X11Server *os_linuxbsd_get_x11_server();
#endif

#endif // LINUXBSD_OS_H