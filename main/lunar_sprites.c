#include "main/lunar_sprites.h"

#include "core/core.h"

#include "core/types/slice.h"
#include "renderer/batch_renderer.h"
#include "renderer/renderer.h"

#include "modules/initialize_modules.h"

#include "main_loop.h"

struct Main {
	FlagManager *flag_manager;
	LSCore *core;

	LSWindow *root_window;

	ApplicationInterface application_interface;

	Slice64 *update_callbacks;

	FlagValue *path;

	bool should_stop;
	int32 exit_code;
};

static struct Main main;

static void check_path();

void ls_main_init(int32 argc, char *argv[]) {
	main.should_stop = false;
	main.exit_code = 0;

	main.flag_manager = flag_manager_create();
	main.path = flag_manager_register(main.flag_manager, "path", FLAG_TYPE_STRING, FLAG_VAL(str, "./"), "Path to the game directory.");

	main.update_callbacks = slice64_create(16, false);

	// Lazy parse early flags.
	flag_manager_parse(main.flag_manager, argc, argv, true);

	main.core = core_create(main.flag_manager);
	check_path();
	ls_log(LOG_LEVEL_INFO, "Initializing Lunar Sprites.\n");
	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE, main.core);
	ls_log(LOG_LEVEL_INFO, "Initialization level core done.\n");

	renderer_init(main.core);
	initialize_modules(MODULE_INITIALIZATION_LEVEL_RENDER, NULL);
	ls_log(LOG_LEVEL_INFO, "Initialization level render done.\n");

	// Parse all flags.
	flag_manager_parse(main.flag_manager, argc, argv, false);

	initialize_modules(MODULE_INITIALIZATION_LEVEL_FLAGS, NULL);
	ls_log(LOG_LEVEL_INFO, "Initialization level flags done.\n");

	core_start(main.core);

	renderer_start();

	initialize_modules(MODULE_INITIALIZATION_LEVEL_MAIN, NULL);
	ls_log(LOG_LEVEL_INFO, "Initialization level main done.\n");

	if (!main.application_interface.init) {
		ls_log_fatal("No application interface set.\n");
	}

	main.root_window = main.application_interface.init(main.core, main.application_interface.user_data);

	batch_renderer_init();

	initialize_modules(MODULE_INITIALIZATION_LEVEL_APPLICATION, (void *)main.root_window);
	ls_log(LOG_LEVEL_INFO, "Initialization level application done.\n");

	main.application_interface.start(main.application_interface.user_data);

	ls_main_loop_init(main.core, main.root_window);
}

void ls_update(float64 delta_time) {
	main.application_interface.update(delta_time, main.application_interface.user_data);

	size_t n_callbacks = slice64_get_size(main.update_callbacks);
	for (size_t i = 0; i < n_callbacks; i++) {
		OnUpdateCallback callback = slice64_get(main.update_callbacks, i).ptr;
		LS_ASSERT(callback);
		callback(delta_time);
	}
}

int32 ls_main_deinit() {
	ls_main_loop_deinit();

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_APPLICATION);
	main.application_interface.deinit(main.application_interface.user_data);
	batch_renderer_deinit();

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_MAIN);

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_RENDER);
	renderer_deinit();

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);

	core_destroy(main.core);
	slice64_destroy(main.update_callbacks);

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

void ls_register_update_callback(OnUpdateCallback callback) {
	slice64_append(main.update_callbacks, SLICE_VAL64(ptr, callback));
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