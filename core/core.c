#include "core/core.h"
#include "core/flags.h"
#include "core/input/input_manager.h"
#include "core/log.h"
#include "core/types/string.h"

struct LunarSprites {
	EventManager *event_manager;
	InputManager *input_manager;
	OS *os;
};

static struct LunarSprites ls;

static void core_check_flags();

// Must be called before core_init.
void core_register_flags() {
	ls_flags_init();

	ls_register_flag("log-level", FLAG_TYPE_STRING, (FlagValue) "INFO",
			"The log level to use. Valid values are `INFO`, `DEBUG`, `WARNING` and `ERROR`");
}

void core_init(int32 argc, char *argv[]) {
	ls_parse_flags(argc, argv);

	core_check_flags();

	ls.event_manager = ls_create_event_manager();
	LS_ASSERT(ls.event_manager);

	ls.os = ls_create_os();
	LS_ASSERT(ls.os);

	ls.input_manager = ls_create_input_manager(ls.event_manager);
	LS_ASSERT(ls.input_manager);
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

void core_handle_press(const LSWindow *window, LSKeycode keycode) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_press(ls.input_manager, window, keycode);
}

void core_handle_release(const LSWindow *window, LSKeycode keycode) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_release(ls.input_manager, window, keycode);
}

void core_handle_mouse_press(const LSWindow *window, LSMouseButton button, Vector2i position) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_press(ls.input_manager, window, button, position);
}

void core_handle_mouse_release(const LSWindow *window, LSMouseButton button, Vector2i position) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_release(ls.input_manager, window, button, position);
}

void core_handle_mouse_move(const LSWindow *window, Vector2i position) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_move(ls.input_manager, window, position);
}

void core_handle_mouse_enter(const LSWindow *window, Vector2i position) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_enter(ls.input_manager, window, position);
}

void core_handle_mouse_leave(const LSWindow *window, Vector2i position) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_leave(ls.input_manager, window, position);
}

static void core_check_flags() {
	const Flag *log_level = ls_get_flag("log-level");
	char *log_level_string = ls_str_copy(log_level->value.string);
	ls_str_to_upper(log_level_string);

	ls_str_to_upper(log_level_string);
	if (ls_str_equals(log_level_string, "INFO")) {
		ls_set_log_level(LOG_LEVEL_INFO);
	} else if (ls_str_equals(log_level_string, "DEBUG")) {
		ls_set_log_level(LOG_LEVEL_DEBUG);
	} else if (ls_str_equals(log_level_string, "WARNING")) {
		ls_set_log_level(LOG_LEVEL_WARNING);
	} else if (ls_str_equals(log_level_string, "ERROR")) {
		ls_set_log_level(LOG_LEVEL_ERROR);
	} else {
		ls_log_fatal("Invalid log level `%s`.\n", log_level->value.string);
	}
	ls_free(log_level_string);
}