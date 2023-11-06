#ifndef EVENTS_H
#define EVENTS_H

#include "core/events/input_events.h"
#include "core/typedefs.h"

typedef enum {
	EVENT_NONE,
	EVENT_KEY,
	EVENT_MOUSE
} EventType;

typedef struct {
	EventType type;
	union {
		EventKey key;
		EventMouse mouse;
	};

	bool handled;
} Event;

typedef void (*EventHandler)(Event *event, void *user_data);

void events_init();
void events_deinit();

void events_emit(Event *e);
void events_add_handler(EventHandler handler, void *user_data);

#endif // EVENTS_H