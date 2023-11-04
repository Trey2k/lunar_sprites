#include "main/main.h"

#include "core/core.h"
#include "core/core_log.h"
#include "core/event/event_manager.h"
#include "core/os/os.h"
#include "core/os/os_window.h"
#include "modules/register_module_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static bool running = true;

void main_event_handler(Event *event);

void main_start() {
	core_init();
	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	const OS *os = core_get_os();

	core_add_event_handler(&main_event_handler);

	LunarWindow *window = os_window_create(os, "Test", 800, 600);
	if (!window) {
		core_log(LOG_LEVEL_FATAL, "Failed to create window\n");
		goto after_window_destroy;
	}
	while (running) {
		window_poll(window);
	}

	window_destroy(window);
after_window_destroy:
	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	core_deinit();
}

void main_event_handler(Event *event) {
	switch (event->type) {
		case EVENT_KEYPRESS: {
			const char keycode = event->keypress.keycode;
			core_log(LOG_LEVEL_INFO, "Key pressed: %d\n", keycode);

			if (keycode == 9) {
				running = false;
			}
		} break;
		default:
			core_log(LOG_LEVEL_INFO, "Unknown event type: %d\n", event->type);
	}
}