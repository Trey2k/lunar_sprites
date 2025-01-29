#include "core/core.h"
#include "core/flags.h"
#include "core/input/input_manager.h"
#include "core/log.h"
#include "core/object/object_db.h"
#include "core/resource/resource_db.h"
#include "core/types/bstring.h"
#include "core/types/string.h"

struct {
	const LSWindow *active_window;

	FlagValue *log_level;
} core = { 0 };

void core_init() {
	core.log_level = flag_manager_register("log-level", FLAG_TYPE_STRING, (FlagValue){ .str = "INFO" },
			"The log level to use. Valid values are `INFO`, `DEBUG`, `WARNING` and `ERROR`");

	os_init();

	object_db_init();
	resource_db_init();
}

void core_deinit() {
	object_db_deinit();
	resource_db_deinit();

	os_deinit();
}

void core_check_flags() {
	ls_str_to_upper(core.log_level->str);

	ls_str_to_upper(core.log_level->str);
	if (ls_str_equals(core.log_level->str, "INFO")) {
		ls_set_log_level(LOG_LEVEL_INFO);
	} else if (ls_str_equals(core.log_level->str, "DEBUG")) {
		ls_set_log_level(LOG_LEVEL_DEBUG);
	} else if (ls_str_equals(core.log_level->str, "WARNING")) {
		ls_set_log_level(LOG_LEVEL_WARNING);
	} else if (ls_str_equals(core.log_level->str, "ERROR")) {
		ls_set_log_level(LOG_LEVEL_ERROR);
	} else {
		ls_log_fatal("Invalid log level `%s`.\n", core.log_level->str);
	}
}