#include "core/core.h"
#include "core/core_log.h"
#include "core/events/events.h"
#include "core/input/input.h"
#include "core/os/os.h"
#include "renderer/renderer.h"

#include <stdarg.h>
#include <stdlib.h>

static OS *os;
static Renderer *renderer;

void core_init() {
	events_init();

	os = os_create();
	if (!os) {
		events_deinit();
		core_fatal("Failed to create OS\n");
	}

	renderer = renderer_create(RENDERER_BACKEND_OPENGL3);
	if (!renderer) {
		os_destroy(os);
		events_deinit();
		core_fatal("Failed to create renderer\n");
	}
}

void core_poll() {
	input_poll();
}

void core_deinit() {
	renderer_destroy(renderer);
	os_destroy(os);
	events_deinit();
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

const OS *core_get_os() {
	return os;
}

const Renderer *core_get_renderer() {
	return renderer;
}