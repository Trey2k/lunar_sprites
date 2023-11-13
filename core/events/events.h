#ifndef EVENTS_H
#define EVENTS_H

#include "core/events/event_manager.h"
#include "core/input/keycodes.h"
#include "core/types/typedefs.h"
#include "core/types/vector/vector2.h"
#include "core/window.h"

typedef enum {
	EVENT_KEY_PRESSED,
	EVENT_KEY_RELEASED
} EventKeyType;

typedef struct {
	EventKeyType type;
	LSKeycode keycode;
	bool repeat;

	const LSWindow *window;
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

	const LSWindow *window;
} EventMouse;

typedef enum {
	EVENT_NONE,
	EVENT_KEY,
	EVENT_MOUSE,
	EVENT_WINDOW_CLOSE,
} EventType;

struct Event {
	EventType type;
	union {
		EventKey key;
		EventMouse mouse;
		const LSWindow *window;
	};

	bool handled;
};

#endif // EVENTS_H