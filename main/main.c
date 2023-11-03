#include "main/main.h"

#include "core/core.h"
#include "core/core_log.h"
#include "core/os/os.h"
#include "core/os/os_window.h"
#include "modules/register_module_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void main_start() {
	core_init();
	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	OS *os = core_get_os();
	Window *window = os_window_create(os, "Test", 800, 600);
	if (!window) {
		core_log(LOG_LEVEL_FATAL, "Failed to create window\n");
		goto after_window_destroy;
	}

	// delay for 2 seconds
	time_t start_time = time(NULL);
	while (time(NULL) - start_time < 2) {
	}

	os_window_destroy(os, window);
after_window_destroy:
	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	core_deinit();
}
