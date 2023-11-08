#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

typedef struct Event Event;

typedef struct EventManager EventManager;
typedef void (*EventHandler)(Event *event, void *user_data);

EventManager *ls_create_event_manager();
void ls_destroy_event_manager(EventManager *event_manager);

void ls_emit_event(const EventManager *event_manager, Event *e);
void ls_add_event_handler(EventHandler handler, void *user_data);

// Defined in core/lunar_sprites.c
const EventManager *ls_get_event_manager();

#endif // EVENT_MANAGER_H