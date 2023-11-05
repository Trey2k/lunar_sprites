#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "core/input/events.h"
#include "core/typedefs.h"

typedef enum {
	EVENT_NONE,
	EVENT_KEY,
} EventType;

typedef struct {
	EventType type;
	union {
		EventKey key;
	};

	bool handled;
} Event;

typedef struct EventManager EventManager;

typedef void (*EventHandler)(Event *event, void *user_data);

EventManager *event_manager_create();
void event_manager_destroy(EventManager *event_manager);

void event_manager_emit(const EventManager *event_manager, Event *e);
void event_manager_add_handler(EventManager *event_manager, EventHandler handler, void *user_data);

#endif // EVENT_H