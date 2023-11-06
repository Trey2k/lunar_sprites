#ifndef LINUXBSD_OS_H
#define LINUXBSD_OS_H

#include "renderer/renderer.h"
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

DisplayServer os_linuxbsd_get_display_server();

#if defined(WAYLAND_ENABLED)
WaylandServer *os_linuxbsd_get_wayland_server();
#endif

#if defined(X11_ENABLED)
X11Server *os_linuxbsd_get_x11_server();
#endif

#endif // LINUXBSD_OS_H