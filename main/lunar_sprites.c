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
	Renderer *renderer;
	LSWindow *root_window;

	const InputManager *input_manager;
};

static struct Main main;

static void init(int32 argc, char *argv[]);
static void register_flags();
static void deinit();
static RendererBackend get_render_backend();

void lunar_spriates_main(int32 argc, char *argv[]) {
	init(argc, argv);

	while (!input_is_key_just_released(main.input_manager, LS_KEY_ESCAPE) &&
			!input_is_key_just_released(main.input_manager, LS_KEY_Q)) {
		// Core should always be polled first.
		core_poll();

		if (main.root_window) {
			window_poll(main.root_window);
		}
	}

	deinit();

	ls_flags_deinit();
}

static void init(int32 argc, char *argv[]) {
	core_register_flags();
	register_flags();
	core_init(argc, argv);
	main.input_manager = core_get_input_manager();

	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);

	RendererBackend renderer_backend = get_render_backend();
	main.renderer = renderer_create(renderer_backend, ls_get_os());
	if (!main.renderer) {
		ls_log_fatal("Failed to create renderer!\n");
	}

	if (renderer_backend != RENDERER_BACKEND_NONE) {
		main.root_window = ls_create_window(main.renderer, "Lunar Sprites", 1280, 720);
		if (!main.root_window) {
			ls_log_fatal("Failed to create window!\n");
		}
	}
}

static void register_flags() {
	ls_register_flag("renderer", FLAG_TYPE_STRING, (FlagValue){ .string = "GLES2" }, "The renderer to use. Valid values are `NONE` and `GLES2`");
}

static void deinit() {
	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	core_deinit();

	window_destroy(main.root_window);

	renderer_destroy(main.renderer);
}

static RendererBackend get_render_backend() {
	RendererBackend renderer_backend = RENDERER_BACKEND_GLES2;

	const Flag *flag = ls_get_flag("renderer");

	char *renderer_string = ls_str_copy(flag->value.string);
	ls_str_to_upper(renderer_string);

	if (ls_str_equals(renderer_string, "NONE")) {
		renderer_backend = RENDERER_BACKEND_NONE;
	} else if (ls_str_equals(renderer_string, "GLES2")) {
		renderer_backend = RENDERER_BACKEND_GLES2;
	} else {
		ls_log_fatal("Invalid value for flag `renderer`. Exepected `NONE` or `OPENGL3`. Got `%s`\n", flag->value.string);
	}

	return renderer_backend;
}
