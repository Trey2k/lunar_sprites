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

static void input_handler(Event *event, void *user_data);
static void check_input();

static const WindowConfig ROOT_WINDOW_CONFIG = {
	.position = { 0, 0 },
	.size = { 1280, 720 },
	.title = "Lunar Sprites",
	.fullscreen = false,
	.hidden = false,
	.root_window = true,
};

void ls_main_init(int32 argc, char *argv[]) {
	core_init();
	renderer_init();

	main.input_manager = core_get_input_manager();

	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);

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

	core_add_event_handler(input_handler, NULL);
	renderer_set_clear_color(0.0f, 1.0f, 0.0f, 0.5f);
}

void ls_main_loop() {
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

void ls_main_deinit() {
	window_destroy(main.root_window);
	renderer_deinit();

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	core_deinit();
}

bool ls_should_quit() {
	return !main.running;
}

static void input_handler(Event *e, void *user_data) {
	LS_ASSERT(e);

	switch (e->type) {
		case EVENT_WINDOW_CLOSE: {
			main.running = false;
		} break;

		case EVENT_KEY: {
			switch (e->key.type) {
				case EVENT_KEY_PRESSED: {
					if (!e->key.repeat) {
						ls_printf("Key pressed: %s\n", keycode_to_string(e->key.keycode));
					} else {
						ls_printf("Key repeated: %s\n", keycode_to_string(e->key.keycode));
					}
				} break;

				case EVENT_KEY_RELEASED: {
					ls_printf("Key released: %s\n", keycode_to_string(e->key.keycode));
				} break;
			}
		} break;

		case EVENT_MOUSE: {
			switch (e->mouse.type) {
				case EVENT_MOUSE_PRESSED: {
					ls_printf("Mouse pressed: %s\n", mouse_button_to_string(e->mouse.button));
				} break;

				case EVENT_MOUSE_RELEASED: {
					ls_printf("Mouse released: %s\n", mouse_button_to_string(e->mouse.button));
				} break;

				case EVENT_MOUSE_MOVED: {
					ls_printf("Mouse moved: %d, %d\n", e->mouse.position.x, e->mouse.position.y);
				} break;

				case EVENT_MOUSE_ENTERED: {
					ls_printf("Mouse entered: %d, %d\n", e->mouse.position.x, e->mouse.position.y);
				} break;

				case EVENT_MOUSE_LEFT: {
					ls_printf("Mouse left: %d, %d\n", e->mouse.position.x, e->mouse.position.y);
				} break;
			}
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