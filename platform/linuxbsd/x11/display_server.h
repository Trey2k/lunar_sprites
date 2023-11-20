#ifndef X11_SERVER_H
#define X11_SERVER_H

#include "core/input/input_manager.h"
#include "core/types/typedefs.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
	Display *display;

	int32 default_screen;
	void *best_fbc;

	InputManager *input_manager;
} X11Server;

X11Server *x11_server_create(InputManager *input_manager);
void x11_server_destroy(X11Server *server);

#endif // X11_SERVER_H