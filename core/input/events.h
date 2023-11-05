#ifndef INPUT_EVENTS_H
#define INPUT_EVENTS_H

#include "core/input/keycodes.h"
#include "core/os/os_window.h"

typedef enum {
	EVENT_KEY_PRESSED,
	EVENT_KEY_RELEASED
} EventKeyType;

typedef struct {
	EventKeyType type;
	LSKeycode keycode;
	bool repeat;

	LSWindow *window;
} EventKey;

#endif // INPUT_EVENTS_H