#include "main/main.h"

#include "core/core.h"
#include "core/core_log.h"
#include "core/event/event_manager.h"
#include "core/input/input_methods.h"
#include "core/input/keycodes.h"
#include "core/os/os_window.h"
#include "modules/register_module_types.h"

void main_event_handler(Event *event, void *user_data);

void main_start() {
	core_init();
	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);

	const OS *os = core_get_os();
	const Input *input = core_get_input();

	bool running = true;
	core_add_event_handler(&main_event_handler, &running);

	LSWindow *window = os_window_create(os, "Test", 800, 600);
	if (!window) {
		core_log(LOG_LEVEL_FATAL, "Failed to create window\n");
		goto after_window_destroy;
	}
	while (running) {
		core_poll();
		window_poll(window);

		if (input_is_key_just_released(input, LS_KEY_SPACE)) {
			core_log(LOG_LEVEL_INFO, "Space released!\n");
		}
	}

	window_destroy(window);
after_window_destroy:
	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	core_deinit();
}

void main_event_handler(Event *event, void *user_data) {
	bool *running = user_data;
	switch (event->type) {
		case EVENT_KEY: {
			const EventKey *key_event = &event->key;
			switch (key_event->type) {
				case EVENT_KEY_PRESSED: {
					if (key_event->repeat) {
						core_log(LOG_LEVEL_INFO, "Key %s repeated\n", keycode_to_string(key_event->keycode));
					} else {
						core_log(LOG_LEVEL_INFO, "Key %s pressed\n", keycode_to_string(key_event->keycode));
					}

					if (key_event->keycode == LS_KEY_ESCAPE) {
						*running = false;
					}
				} break;
				default: {
					core_log(LOG_LEVEL_INFO, "Key %s released\n", keycode_to_string(key_event->keycode));
				} break;
			}
		} break;
		default:
			core_log(LOG_LEVEL_INFO, "Unknown event type: %d\n", event->type);
	}
}