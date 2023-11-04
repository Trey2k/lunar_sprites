#ifndef INPUT_EVENTS_H
#define INPUT_EVENTS_H

#include "core/input/keycodes.h"
#include "core/os/os_window.h"

typedef struct {
	LSKeyCode key_code;
	bool repeat;

	const LSWindow *window;
} EventKeyPress;

#endif // INPUT_EVENTS_H