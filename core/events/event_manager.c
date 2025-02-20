#include "core/events/event_manager.h"
#include "core/debug.h"
#include "core/events/events.h"
#include "core/memory.h"

struct EventManager {
	size_t handler_count;
	EventHandler *handlers;
	void **user_data;
};

EventManager *event_manager_create() {
	EventManager *event_manager = ls_malloc(sizeof(EventManager));
	event_manager->handler_count = 0;
	event_manager->handlers = NULL;
	event_manager->user_data = NULL;

	return event_manager;
}

void event_manager_destroy(EventManager *event_manager) {
	LS_ASSERT(event_manager);

	ls_free(event_manager->handlers);
	ls_free(event_manager->user_data);
	ls_free(event_manager);
}

void event_manager_emit(const EventManager *event_manager, Event *event) {
	event->handled = false;
	for (size_t i = event_manager->handler_count; i > 0; i--) {
		event_manager->handlers[i - 1](event, event_manager->user_data[i - 1]);
		if (event->handled) {
			break;
		}
	}
}

void event_manager_add_handler(EventManager *event_manager, EventHandler handler, void *user_data) {
	LS_ASSERT(event_manager);
	LS_ASSERT(handler);

	event_manager->handler_count++;
	event_manager->handlers = ls_realloc(event_manager->handlers, sizeof(EventHandler) * event_manager->handler_count);
	event_manager->user_data = ls_realloc(event_manager->user_data, sizeof(void *) * event_manager->handler_count);

	event_manager->handlers[event_manager->handler_count - 1] = handler;
	event_manager->user_data[event_manager->handler_count - 1] = user_data;
}