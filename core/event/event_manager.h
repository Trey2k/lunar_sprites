#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "core/event/events.h"

typedef struct EventManager EventManager;

typedef void (*EventHandler)(Event *event);

EventManager *event_manager_create();
void event_manager_destroy(EventManager *event_manager);

void event_manager_emit(const EventManager *event_manager, Event *e);

void event_manager_add_handler(EventManager *event_manager, EventHandler handler);

#endif // EVENT_H