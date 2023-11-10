#include "main/lunar_sprites.h"

#include "core/core.h"
#include "core/flags.h"
#include "core/log.h"
#include "core/types/string.h"
#include "core/window.h"

#include "renderer/context.h"
#include "renderer/renderer.h"

#include "modules/register_module_types.h"

struct Main {
	LSWindow *root_window;

	const InputManager *input_manager;
};

static struct Main main;

static void init();
static void start(int32 argc, char *argv[]);
static void deinit();

void lunar_spriates_main(int32 argc, char *argv[]) {
	init();
	start(argc, argv);

	while (!input_is_key_just_released(main.input_manager, LS_KEY_ESCAPE) &&
			!input_is_key_just_released(main.input_manager, LS_KEY_Q)) {
		core_poll();

		if (main.root_window) {
			window_poll(main.root_window);
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
		main.root_window = ls_create_window("Lunar Sprites", 1280, 720);
		if (!main.root_window) {
			ls_log_fatal("Failed to create window!\n");
		}
	}
}

static void deinit() {
	window_destroy(main.root_window);
	renderer_deinit();

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	core_deinit();
}
