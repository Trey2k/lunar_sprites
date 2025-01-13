#include "core/core.h"
#include "core/events/event_manager.h"
#include "core/events/events.h"
#include "core/flags.h"
#include "core/input/input_manager.h"
#include "core/log.h"
#include "core/types/string.h"

struct LSCore {
	FlagManager *flag_manager;
	EventManager *event_manager;
	InputManager *input_manager;
	OS *os;

	const LSWindow *active_window;

	FlagValue *log_level;
};

static void core_check_flags(const LSCore *core);

LSCore *core_create(FlagManager *flag_manager) {
	LSCore *core = ls_malloc(sizeof(LSCore));

	core->flag_manager = flag_manager;

	core->log_level = flag_manager_register(core->flag_manager, "log-level", FLAG_TYPE_STRING, (FlagValue){ .str = "INFO" },
			"The log level to use. Valid values are `INFO`, `DEBUG`, `WARNING` and `ERROR`");

	core->event_manager = event_manager_create();
	LS_ASSERT(core->event_manager);

	core->input_manager = ls_create_input_manager(core->event_manager);
	LS_ASSERT(core->input_manager);

	core->os = ls_create_os(core->input_manager);
	LS_ASSERT(core->os);

	return core;
}

void core_start(const LSCore *core) {
	core_check_flags(core);
}

void core_poll(const LSCore *core) {
	input_poll(core->input_manager);
}

void core_destroy(LSCore *core) {
	ls_destroy_os(core->os);
	core->os = NULL;

	input_manager_destroy(core->input_manager);
	core->input_manager = NULL;

	event_manager_destroy(core->event_manager);
	core->event_manager = NULL;

// TODO: Fix seg fault when destroying flag manager on Windows
#if !defined(WINDOWS_ENABLED)
	flag_manager_destroy(core->flag_manager);
#endif // !LS_WINDOWS
	core->flag_manager = NULL;
}

FlagManager *core_get_flag_manager(const LSCore *core) {
	return core->flag_manager;
}

InputManager *core_get_input_manager(const LSCore *core) {
	return core->input_manager;
}

EventManager *core_get_event_manager(const LSCore *core) {
	return core->event_manager;
}

const OS *core_get_os(const LSCore *core) {
	return core->os;
}

static void core_check_flags(const LSCore *core) {
	ls_str_to_upper(core->log_level->str);

	ls_str_to_upper(core->log_level->str);
	if (ls_str_equals(core->log_level->str, "INFO")) {
		ls_set_log_level(LOG_LEVEL_INFO);
	} else if (ls_str_equals(core->log_level->str, "DEBUG")) {
		ls_set_log_level(LOG_LEVEL_DEBUG);
	} else if (ls_str_equals(core->log_level->str, "WARNING")) {
		ls_set_log_level(LOG_LEVEL_WARNING);
	} else if (ls_str_equals(core->log_level->str, "ERROR")) {
		ls_set_log_level(LOG_LEVEL_ERROR);
	} else {
		ls_log_fatal("Invalid log level `%s`.\n", core->log_level->str);
	}
	ls_free(core->log_level->str);
}