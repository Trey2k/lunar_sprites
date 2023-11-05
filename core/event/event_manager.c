#include "core/event/event_manager.h"

#include "core/core.h"

struct EventManager {
	size_t handler_count;
	EventHandler *handlers;
	void **user_data;
};

EventManager *event_manager_create() {
	EventManager *event_manager = core_malloc(sizeof(struct EventManager));
	event_manager->handler_count = 0;
	event_manager->handlers = core_malloc(sizeof(EventHandler) * 1);
	event_manager->user_data = core_malloc(sizeof(void *) * 1);

	return event_manager;
}

void event_manager_destroy(EventManager *event_manager) {
	core_free(event_manager->handlers);

	core_free(event_manager);
}

void event_manager_emit(const EventManager *event_manager, Event *e) {
	for (size_t i = event_manager->handler_count; i > 0; i--) {
		event_manager->handlers[i - 1](e, event_manager->user_data[i - 1]);
		if (e->handled) {
			break;
		}
	}
}

void event_manager_add_handler(EventManager *event_manager, EventHandler handler, void *user_data) {
	const size_t index = event_manager->handler_count++;
	event_manager->handlers = core_realloc(event_manager->handlers, sizeof(EventHandler) * event_manager->handler_count);
	event_manager->user_data = core_realloc(event_manager->user_data, sizeof(void *) * event_manager->handler_count);
	event_manager->handlers[index] = handler;
	event_manager->user_data[index] = user_data;
}