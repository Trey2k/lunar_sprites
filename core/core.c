#include "core/core.h"
#include "core/core_log.h"
#include "core/os/os.h"
#include "renderer/renderer.h"

#include <stdarg.h>
#include <stdlib.h>

static OS *os;
static Renderer *renderer;

void core_init() {
	os = os_create();
	if (!os) {
		core_fatal("Failed to create OS\n");
	}

	renderer = renderer_create(RENDERER_BACKEND_OPENGL3);
	if (!renderer) {
		os_destroy(os);
		core_fatal("Failed to create renderer\n");
	}
}

void core_deinit() {
	os_destroy(os);
	os = NULL;
	renderer_destroy(renderer);
	renderer = NULL;
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

void core_fatal(String message, ...) {
	va_list args;
	va_start(args, message);
	core_log(LOG_LEVEL_FATAL, message, args);
	va_end(args);
	exit(1);
}

OS *core_get_os() {
	return os;
}

Renderer *core_get_renderer() {
	return renderer;
}