#include "core/core.h"
#include "core/core_log.h"
#include "core/os/os.h"
#include "renderer/renderer.h"

#include <stdarg.h>
#include <stdlib.h>

static EventManager *event_manager;
static OS *os;
static Renderer *renderer;

void core_init() {
	event_manager = event_manager_create();
	if (!event_manager) {
		core_fatal("Failed to create event manager\n");
	}

	os = os_create();
	if (!os) {
		event_manager_destroy(event_manager);
		core_fatal("Failed to create OS\n");
	}

	renderer = renderer_create(RENDERER_BACKEND_OPENGL3);
	if (!renderer) {
		event_manager_destroy(event_manager);
		os_destroy(os);
		core_fatal("Failed to create renderer\n");
	}
}

void core_deinit() {
	os_destroy(os);
	renderer_destroy(renderer);
	event_manager_destroy(event_manager);
}

void *core_malloc(size_t size) {
	return malloc(size);
}

void *core_realloc(void *ptr, size_t size) {
	return realloc(ptr, size);
}

void core_free(void *ptr) {
	free(ptr);
}

void core_fatal(const String message, ...) {
	va_list args;
	va_start(args, message);
	core_log(LOG_LEVEL_FATAL, message, args);
	va_end(args);
	exit(1);
}

// This is needed becuase only core has a none const ref to the event manager
void core_add_event_handler(const EventHandler handler) {
	event_manager_add_handler(event_manager, handler);
}

const OS *core_get_os() {
	return os;
}

const Renderer *core_get_renderer() {
	return renderer;
}

const EventManager *core_get_event_manager() {
	return event_manager;
}