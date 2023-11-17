#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "core/api.h"
#include "core/events/events.h"

typedef struct EventManager EventManager;
typedef void (*EventHandler)(Event *event, void *user_data);

EventManager *ls_create_event_manager();
void ls_destroy_event_manager(EventManager *event_manager);

void events_emit(const EventManager *event_manager, Event *e);
void events_add_handler(EventManager *event_manager, EventHandler handler, void *user_data);

// Defined in core/lunar_sprites.c
const EventManager *ls_get_event_manager();

#endif // EVENT_MANAGER_H