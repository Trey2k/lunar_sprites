#include "main/main.h"

#include "core/core.h"
#include "core/core_log.h"
#include "core/events/events.h"
#include "core/events/input_events.h"
#include "core/input/input.h"
#include "core/input/keycodes.h"
#include "core/os/os_window.h"
#include "core/typedefs.h"
#include "modules/register_module_types.h"

void main_event_handler(Event *event, void *user_data);

void main_start() {
	core_init();
	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);

	const OS *os = core_get_os();

	bool running = true;
	events_add_handler(&main_event_handler, &running);

	LSWindow *window = os_window_create(os, "Test", 800, 600);
	if (!window) {
		core_log(LOG_LEVEL_FATAL, "Failed to create window\n");
		goto after_window_destroy;
	}
	while (running) {
		core_poll();
		window_poll(window);

		if (input_is_key_just_released(LS_KEY_SPACE)) {
			core_log(LOG_LEVEL_INFO, "Space released!\n");
		}
	}

	window_destroy(window);
after_window_destroy:
	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	core_deinit();
}

void key_event_handler(const EventKey *e, bool *running) {
	switch (e->type) {
		case EVENT_KEY_PRESSED: {
			if (e->repeat) {
				core_log(LOG_LEVEL_INFO, "Key %s repeated\n", keycode_to_string(e->keycode));
			} else {
				core_log(LOG_LEVEL_INFO, "Key %s just pressed\n", keycode_to_string(e->keycode));
			}
		} break;

		case EVENT_KEY_RELEASED: {
			core_log(LOG_LEVEL_INFO, "Key %s released\n", keycode_to_string(e->keycode));
		} break;

		default: {
			core_log(LOG_LEVEL_INFO, "Unknown key event type: %d\n", e->type);
		} break;
	}
}

void mouse_event_handler(const EventMouse *e) {
	switch (e->type) {
		case EVENT_MOUSE_PRESSED: {
			core_log(LOG_LEVEL_INFO, "Mouse button %s pressed\n", mouse_button_to_string(e->button));
		} break;

		case EVENT_MOUSE_RELEASED: {
			core_log(LOG_LEVEL_INFO, "Mouse button %s released\n", mouse_button_to_string(e->button));
		} break;

		case EVENT_MOUSE_MOVED: {
			core_log(LOG_LEVEL_INFO, "Mouse moved to %d, %d\n", e->position.x, e->position.y);
		} break;

		case EVENT_MOUSE_ENTERED: {
			core_log(LOG_LEVEL_INFO, "Mouse entered window\n");
		} break;

		case EVENT_MOUSE_LEFT: {
			core_log(LOG_LEVEL_INFO, "Mouse left window\n");
		} break;

		default: {
			core_log(LOG_LEVEL_INFO, "Unknown mouse event type: %d\n", e->type);
		} break;
	}
}

void main_event_handler(Event *event, void *user_data) {
	bool *running = user_data;
	switch (event->type) {
		case EVENT_KEY: {
			key_event_handler(&event->key, running);
		} break;

		case EVENT_MOUSE: {
			mouse_event_handler(&event->mouse);
		} break;

		default:
			core_log(LOG_LEVEL_INFO, "Unknown event type: %d\n", event->type);
	}
}