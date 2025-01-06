#ifndef EVENTS_H
#define EVENTS_H

#include "core/input/keycodes.h"
#include "core/math/vector.h"
#include "core/types/typedefs.h"
#include "core/window.h"

typedef enum {
	EVENT_KEY_PRESSED,
	EVENT_KEY_RELEASED
} EventKeyType;

typedef struct {
	EventKeyType type;
	LSKeycode keycode;
	bool repeated;

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
	Vector2u position;

	const LSWindow *window;
} EventMouse;

typedef enum {
	EVENT_WINDOW_OPEN,
	EVENT_WINDOW_CLOSE,
	EVENT_WINDOW_RESIZE,
	EVENT_WINDOW_MOVE,
	EVENT_WINDOW_FOCUS,
	EVENT_WINDOW_LOST_FOCUS,
	EVENT_WINDOW_MINIMIZE,
	EVENT_WINDOW_MAXIMIZE
} EventWindowType;

typedef struct {
	EventWindowType type;
	Vector2u position;
	Vector2u size;
	const LSWindow *window;
} EventWindow;

typedef enum {
	EVENT_NONE,
	EVENT_KEY,
	EVENT_MOUSE,
	EVENT_WINDOW
} EventType;

typedef struct {
	EventType type;
	union {
		EventKey key;
		EventMouse mouse;
		EventWindow window;
	};

	bool handled;
} Event;

#endif // EVENTS_H