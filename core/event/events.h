#ifndef EVENTS_H
#define EVENTS_H

#include "core/input/events.h"
#include "core/typedefs.h"

typedef enum {
	EVENT_NONE,
	EVENT_KEYPRESS,
} EventType;

typedef struct {
	EventType type;
	union {
		EventKeyPress key_press;
	};

	bool handled;
} Event;

#endif // EVENTS_H