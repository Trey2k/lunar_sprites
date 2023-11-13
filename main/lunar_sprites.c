#include "main/lunar_sprites.h"

#include "core/core.h"

#include "core/events/events.h"
#include "renderer/context.h"
#include "renderer/renderer.h"
#include "renderer/window.h"

#include "modules/register_module_types.h"

struct Main {
	LSWindow *root_window;

	bool running;

	const InputManager *input_manager;
};

static struct Main main;

static void init();
static void start(int32 argc, char *argv[]);
static void deinit();

static void input_handler(Event *event, void *user_data);
static void check_input();

static const WindowConfig ROOT_WINDOW_CONFIG = {
	.position = { 0, 0 },
	.size = { 1280, 720 },
	.title = "Lunar Sprites",
	.fullscreen = false,
	.hidden = false,
};

void lunar_spriates_main(int32 argc, char *argv[]) {
	init();
	start(argc, argv);

	core_add_event_handler(input_handler, NULL);
	renderer_set_clear_color(0.0f, 1.0f, 0.0f, 0.5f);

	while (main.running) {
		core_poll();

		if (main.root_window) {
			window_poll(main.root_window);
			check_input();
		}

		renderer_clear();

		if (main.root_window) {
			window_make_current(main.root_window);
			window_swap_buffers(main.root_window);
		}
	}

	deinit();

	ls_flags_deinit();
}

static void init() {
	core_init();
	renderer_init();

	main.input_manager = core_get_input_manager();

	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
}

static void start(int32 argc, char *argv[]) {
	core_start(argc, argv);
	renderer_start(ls_get_os());

	RendererBackend renderer_backend = renderer_get_backend();

	if (renderer_backend != RENDERER_BACKEND_NONE) {
		main.root_window = renderer_create_window(ROOT_WINDOW_CONFIG);
		if (!main.root_window) {
			ls_log_fatal("Failed to create window!\n");
		}
	}

	main.running = true;
}

static void deinit() {
	window_destroy(main.root_window);
	renderer_deinit();

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	core_deinit();
}

static void input_handler(Event *e, void *user_data) {
	LS_ASSERT(e);

	switch (e->type) {
		case EVENT_WINDOW_CLOSE: {
			main.running = false;
		} break;

		default: {
		} break;
	}
}

static void check_input() {
	if (input_is_key_just_released(main.input_manager, LS_KEY_ESCAPE) ||
			input_is_key_just_released(main.input_manager, LS_KEY_Q)) {
		main.running = false;
	}
}