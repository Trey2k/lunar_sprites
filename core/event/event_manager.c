#include "core/event/event_manager.h"

#include "core/core.h"

struct EventManager {
	size_t handler_count;
	EventHandler *handlers;
};

EventManager *event_manager_create() {
	EventManager *event_manager = core_malloc(sizeof(struct EventManager));
	event_manager->handler_count = 0;
	event_manager->handlers = core_malloc(sizeof(EventHandler) * 1);

	return event_manager;
}

void event_manager_destroy(EventManager *event_manager) {
	core_free(event_manager->handlers);

	core_free(event_manager);
}

void event_manager_emit(const EventManager *event_manager, Event *e) {
	for (size_t i = event_manager->handler_count; i > 0; i--) {
		event_manager->handlers[i - 1](e);
		if (e->handled) {
			break;
		}
	}
}

void event_manager_add_handler(EventManager *event_manager, EventHandler handler) {
	const size_t index = event_manager->handler_count++;
	event_manager->handlers = core_realloc(event_manager->handlers,
			sizeof(EventHandler) * event_manager->handler_count);
	event_manager->handlers[index] = handler;
}