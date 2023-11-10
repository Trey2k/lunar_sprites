#include "core/core.h"
#include "core/flags.h"
#include "core/input/input_manager.h"
#include "core/log.h"
#include "core/types/string.h"

struct LunarSprites {
	EventManager *event_manager;
	InputManager *input_manager;
	OS *os;

	const LSWindow *active_window;

	FlagValue *log_level;
};

static struct LunarSprites ls;

static void core_check_flags();

void core_init() {
	ls_flags_init();

	ls.log_level = ls_register_flag("log-level", FLAG_TYPE_STRING, (FlagValue){ .string = "INFO" },
			"The log level to use. Valid values are `INFO`, `DEBUG`, `WARNING` and `ERROR`");

	ls.event_manager = ls_create_event_manager();
	LS_ASSERT(ls.event_manager);

	ls.os = ls_create_os();
	LS_ASSERT(ls.os);

	ls.input_manager = ls_create_input_manager(ls.event_manager);
	LS_ASSERT(ls.input_manager);
}

void core_start(int32 argc, char *argv[]) {
	ls_parse_flags(argc, argv);
	core_check_flags();
}

void core_poll() {
	input_poll(ls.input_manager);
}

void core_deinit() {
	ls_destroy_os(ls.os);
	ls.os = NULL;

	input_manager_destroy(ls.input_manager);
	ls.input_manager = NULL;

	ls_destroy_event_manager(ls.event_manager);
	ls.event_manager = NULL;
}

const EventManager *ls_get_event_manager() {
	return ls.event_manager;
}

const InputManager *core_get_input_manager() {
	return ls.input_manager;
}

const OS *ls_get_os() {
	return ls.os;
}

void core_set_active_window(const LSWindow *window) {
	ls.active_window = window;
}

void core_handle_press(LSKeycode keycode) {
	LS_ASSERT(ls.active_window);
	LS_ASSERT(ls.input_manager);

	input_handle_press(ls.input_manager, ls.active_window, keycode);
}

void core_handle_release(LSKeycode keycode) {
	LS_ASSERT(ls.active_window);
	LS_ASSERT(ls.input_manager);

	input_handle_release(ls.input_manager, ls.active_window, keycode);
}

void core_handle_mouse_press(LSMouseButton button, Vector2i position) {
	LS_ASSERT(ls.active_window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_press(ls.input_manager, ls.active_window, button, position);
}

void core_handle_mouse_release(LSMouseButton button, Vector2i position) {
	LS_ASSERT(ls.active_window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_release(ls.input_manager, ls.active_window, button, position);
}

void core_handle_mouse_move(Vector2i position) {
	LS_ASSERT(ls.active_window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_move(ls.input_manager, ls.active_window, position);
}

void core_handle_mouse_enter(Vector2i position) {
	LS_ASSERT(ls.active_window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_enter(ls.input_manager, ls.active_window, position);
}

void core_handle_mouse_leave(Vector2i position) {
	LS_ASSERT(ls.active_window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_leave(ls.input_manager, ls.active_window, position);
}

static void core_check_flags() {
	ls_str_to_upper(ls.log_level->string);

	ls_str_to_upper(ls.log_level->string);
	if (ls_str_equals(ls.log_level->string, "INFO")) {
		ls_set_log_level(LOG_LEVEL_INFO);
	} else if (ls_str_equals(ls.log_level->string, "DEBUG")) {
		ls_set_log_level(LOG_LEVEL_DEBUG);
	} else if (ls_str_equals(ls.log_level->string, "WARNING")) {
		ls_set_log_level(LOG_LEVEL_WARNING);
	} else if (ls_str_equals(ls.log_level->string, "ERROR")) {
		ls_set_log_level(LOG_LEVEL_ERROR);
	} else {
		ls_log_fatal("Invalid log level `%s`.\n", ls.log_level->string);
	}
	ls_free(ls.log_level->string);
}