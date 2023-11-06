#include "core/core.h"
#include "core/core_log.h"
#include "core/events/events.h"
#include "core/input/input.h"
#include "core/os/os.h"

#include <stdarg.h>
#include <stdlib.h>

void core_init(RendererBackend renderer_backend) {
	events_init();

	if (!os_init()) {
		events_deinit();
		core_fatal("Failed to create OS\n");
	}

	if (!renderer_init(renderer_backend)) {
		os_deinit();
		events_deinit();
		core_fatal("Failed to create renderer\n");
	}
}

void core_poll() {
	input_poll();
}

void core_deinit() {
	renderer_deinit();
	os_deinit();
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

void core_fatal(String message, ...) {
	va_list args;
	va_start(args, message);
	core_log(LOG_LEVEL_FATAL, message, args);
	va_end(args);
	exit(1);
}