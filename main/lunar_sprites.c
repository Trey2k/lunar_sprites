#include "main/lunar_sprites.h"

#include "core/core.h"

#include "core/os/thread.h"
#include "core/types/slice.h"
#include "renderer/batch_renderer.h"
#include "renderer/renderer.h"
#include "renderer/window.h"

#include "modules/initialize_modules.h"

struct Main {
	FlagManager *flag_manager;
	LSCore *core;
	Renderer *renderer;

	LSMutex *render_mutex;
	LSThread *render_thread;

	const LSWindow *root_window;

	ApplicationInterface application_interface;

	Slice *update_callbacks;
	Slice *eof_update_callbacks;

	const InputManager *input_manager;

	uint64 last_frame_time;
	float64 delta_time;

	FlagValue *path;

	bool should_stop;
	int32 exit_code;
};

static struct Main main;

static void check_path();
static void ls_render_loop(void *data);

void ls_main_init(int32 argc, char *argv[]) {
	main.should_stop = false;
	main.exit_code = 0;
	main.delta_time = 0.0;

	main.last_frame_time = os_get_time();
	main.flag_manager = flag_manager_create();
	main.path = flag_manager_register(main.flag_manager, "path", FLAG_TYPE_STRING, FLAG_VAL(str, "./"), "Path to the game directory.");

	main.update_callbacks = slice_create(16, false);
	main.eof_update_callbacks = slice_create(16, false);

	// Lazy parse early flags.
	flag_manager_parse(main.flag_manager, argc, argv, true);

	main.core = core_create(main.flag_manager);
	check_path();
	ls_log(LOG_LEVEL_INFO, "Initializing Lunar Sprites.\n");
	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE, main.core);
	ls_log(LOG_LEVEL_INFO, "Initialization level core done.\n");

	main.renderer = renderer_create(main.core);
	initialize_modules(MODULE_INITIALIZATION_LEVEL_RENDER, main.renderer);
	ls_log(LOG_LEVEL_INFO, "Initialization level render done.\n");

	main.input_manager = core_get_input_manager(main.core);

	// Parse all flags.
	flag_manager_parse(main.flag_manager, argc, argv, false);

	initialize_modules(MODULE_INITIALIZATION_LEVEL_FLAGS, NULL);
	ls_log(LOG_LEVEL_INFO, "Initialization level flags done.\n");

	core_start(main.core);

	renderer_start(main.renderer);

	initialize_modules(MODULE_INITIALIZATION_LEVEL_MAIN, NULL);
	ls_log(LOG_LEVEL_INFO, "Initialization level main done.\n");

	if (!main.application_interface.init) {
		ls_log_fatal("No application interface set.\n");
	}

	main.root_window = main.application_interface.init(main.core, main.renderer, main.application_interface.user_data);

	batch_renderer_init(main.renderer);

	initialize_modules(MODULE_INITIALIZATION_LEVEL_APPLICATION, (void *)main.root_window);
	ls_log(LOG_LEVEL_INFO, "Initialization level application done.\n");

	main.application_interface.start(main.application_interface.user_data);

	// Make sure the render context is detached from the main thread.
	window_detach(main.root_window);

	// Start render thread, at this point all rendering should be done in the render loop.
	main.render_mutex = os_mutex_create();
	main.render_thread = os_thread_create(ls_render_loop, NULL);
}

static void ls_render_loop(void *data) {
	ls_log(LOG_LEVEL_INFO, "Starting render loop.\n");
	window_make_current(main.root_window);

	while (!main.should_stop) {
		os_mutex_lock(main.render_mutex);

		renderer_clear(main.renderer);
		size_t n_callbacks = slice_get_size(main.update_callbacks);
		for (size_t i = 0; i < n_callbacks; i++) {
			OnUpdateCallback callback = slice_get(main.update_callbacks, i).ptr;
			LS_ASSERT(callback);
			callback(main.delta_time);
		}

		main.application_interface.update(main.delta_time, main.application_interface.user_data);

		n_callbacks = slice_get_size(main.eof_update_callbacks);
		for (size_t i = 0; i < n_callbacks; i++) {
			OnUpdateCallback callback = slice_get(main.eof_update_callbacks, i).ptr;
			LS_ASSERT(callback);
			callback(main.delta_time);
		}

		batch_renderer_end_frame();

		if (main.root_window) {
			window_make_current(main.root_window);
			window_swap_buffers(main.root_window);
		}

		os_mutex_unlock(main.render_mutex);
	}

	// Make sure to hand the render context back to the main thread.
	window_detach(main.root_window);
}

void ls_main_loop() {
	uint64 current_time = os_get_time();
	main.delta_time = (current_time - main.last_frame_time) / 1000000.0;
	main.last_frame_time = current_time;

	core_poll(main.core);

	if (main.root_window) {
		window_poll(main.root_window);
	}
}

int32 ls_main_deinit() {
	main.should_stop = true;
	// Wait for render thread to finish first.
	os_thread_join(main.render_thread);
	// Make the current context the main thread.
	window_make_current(main.root_window);

	os_mutex_destroy(main.render_mutex);
	os_thread_destroy(main.render_thread);

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_APPLICATION);
	main.application_interface.deinit(main.application_interface.user_data);
	batch_renderer_deinit();

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_MAIN);

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_RENDER);
	renderer_destroy(main.renderer);

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);

	core_destroy(main.core);
	slice_destroy(main.update_callbacks);

	return main.exit_code;
}

bool ls_should_stop() {
	if (main.should_stop) {
		return true;
	}

	return main.application_interface.should_stop(main.application_interface.user_data);
}

void ls_set_application_interface(ApplicationInterface application_interface) {
	if (main.application_interface.init) {
		ls_log(LOG_LEVEL_WARNING, "Application interface already set. Overwriting.\n");
	}

	main.application_interface = application_interface;
}

void ls_register_update_callback(OnUpdateCallback callback, bool end_of_frame) {
	if (end_of_frame) {
		slice_append(main.eof_update_callbacks, SLICE_VAL(ptr, callback));
		return;
	}

	slice_append(main.update_callbacks, SLICE_VAL(ptr, callback));
}

void ls_exit(int32 exit_code) {
	main.should_stop = true;
	main.exit_code = exit_code;
}

static void check_path() {
	if (!main.path->str) {
		ls_log_fatal("No path specified.\n");
	}

	if (!os_path_exists(main.path->str)) {
		ls_log_fatal("Path %s does not exist.\n", main.path->str);
	}

	if (!os_path_is_directory(main.path->str)) {
		ls_log_fatal("Path %s is not a directory.\n", main.path->str);
	}

	if (!ls_str_equals(main.path->str, "./")) {
		ls_log(LOG_LEVEL_INFO, "Setting working directory to %s.\n", main.path->str);
		os_set_working_directory(main.path->str);
	}
}