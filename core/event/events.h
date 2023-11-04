#ifndef EVENTS_H
#define EVENTS_H

#include "core/typedefs.h"

typedef struct {
	unsigned char keycode;
} EventKeyPress;

typedef enum {
	EVENT_NONE,
	EVENT_KEYPRESS,
} EventType;

typedef struct {
	EventType type;
	union {
		EventKeyPress keypress;
	};

	bool handled;
} Event;

#endif // EVENTS_H