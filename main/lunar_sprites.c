#include "main/lunar_sprites.h"

#include "core/core.h"

#include "core/events/events.h"
#include "renderer/context.h"
#include "renderer/renderer.h"
#include "renderer/window.h"

#include "modules/initialize_modules.h"

struct Main {
	LSCore *core;
	Renderer *renderer;
	const LSWindow *root_window;

	ApplicationInterface *application_interface;

	const InputManager *input_manager;

	int64 last_frame_time;
	float64 delta_time;
};

static struct Main main;

void ls_main_init(int32 argc, char *argv[]) {
	main.application_interface = NULL;
	main.delta_time = 0.0;

	main.core = core_create();
	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE, main.core);
	main.renderer = renderer_create(main.core);
	initialize_modules(MODULE_INITIALIZATION_LEVEL_RENDER, main.renderer);

	main.input_manager = core_get_input_manager(main.core);

	core_start(main.core, argc, argv);
	renderer_start(main.renderer);

	if (!main.application_interface) {
		ls_log_fatal("No application interface set.\n");
	}

	main.root_window = main.application_interface->init(main.core, main.renderer, main.application_interface->user_data);
	initialize_modules(MODULE_INITIALIZATION_LEVEL_MAIN, NULL);
	main.application_interface->start(main.application_interface->user_data);
}

void ls_main_loop() {
	int64 current_time = ls_get_time();
	main.delta_time = (current_time - main.last_frame_time) / 1000000.0;
	main.last_frame_time = current_time;

	core_poll(main.core);

	if (main.root_window) {
		window_poll(main.root_window);
	}

	renderer_clear(main.renderer);

	main.application_interface->update(main.delta_time, main.application_interface->user_data);

	if (main.root_window) {
		window_make_current(main.root_window);
		window_swap_buffers(main.root_window);
	}
}

void ls_main_deinit() {
	main.application_interface->deinit(main.application_interface->user_data);

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_MAIN);

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_RENDER);
	renderer_destroy(main.renderer);

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	core_destroy(main.core);
}

bool ls_should_stop() {
	return main.application_interface->should_stop(main.application_interface->user_data);
}

void ls_set_application_interface(ApplicationInterface *application_interface) {
	if (main.application_interface) {
		ls_log(LOG_LEVEL_WARNING, "Application interface already set. Overwriting.\n");
		ls_free(main.application_interface);
	}

	main.application_interface = application_interface;
}