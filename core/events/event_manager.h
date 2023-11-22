#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "core/api.h"
#include "core/events/events.h"

typedef struct EventManager EventManager;
typedef void (*EventHandler)(Event *event, void *user_data);

EventManager *event_manager_create();
void event_manager_destroy(EventManager *event_manager);

void event_manager_emit(const EventManager *event_manager, Event *e);
void event_manager_add_handler(EventManager *event_manager, EventHandler handler, void *user_data);

#endif // EVENT_MANAGER_H