#ifndef INPUT_EVENTS_H
#define INPUT_EVENTS_H

#include "core/input/keycodes.h"
#include "core/os/window.h"

#include "core/types/vector2.h"

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

typedef enum {
	EVENT_MOUSE_PRESSED,
	EVENT_MOUSE_RELEASED,
	EVENT_MOUSE_MOVED,
	EVENT_MOUSE_ENTERED,
	EVENT_MOUSE_LEFT
} EventMouseType;

typedef struct {
	EventMouseType type;
	LSMouseButton button;
	Vector2i position;

	LSWindow *window;
} EventMouse;

#endif // INPUT_EVENTS_H