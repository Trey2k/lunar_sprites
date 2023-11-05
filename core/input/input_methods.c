#include "core/input/input_methods.h"
#include "core/core.h"
#include "core/core_log.h"
#include "core/input/input.h"
#include "core/input/keycodes.h"
#include "core/typedefs.h"

typedef struct {
	bool pressed;
	uint32 repeat_count;
} KeyState;

struct Input {
	const EventManager *event_manager;

	// TODO: This wastes some memory, but it's not a big deal.
	KeyState key_state[LS_KEY_LAST + 1];
	KeyState previous_key_state[LS_KEY_LAST + 1];
};

Input *input_create(const EventManager *event_manager) {
	CORE_ASSERT(event_manager);

	Input *input = core_malloc(sizeof(Input));
	input->event_manager = event_manager;

	for (size_t i = 0; i < LS_KEY_LAST; i++) {
		input->key_state[i].pressed = false;
		input->key_state[i].repeat_count = 0;
		input->previous_key_state[i].pressed = false;
		input->previous_key_state[i].repeat_count = 0;
	}

	return input;
}

void input_destroy(Input *input) {
	core_free(input);
}

void input_poll(Input *input) {
	CORE_ASSERT(input);

	for (size_t i = 0; i < LS_KEY_LAST; i++) {
		input->previous_key_state[i] = input->key_state[i];
	}
}

void input_handle_press(Input *input, const LSWindow *window, LSKeycode keycode) {
	CORE_ASSERT(input);
	CORE_ASSERT(window);

	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return;
	}

	if (input->key_state[keycode].pressed) {
		input->key_state[keycode].repeat_count++;
	} else {
		input->key_state[keycode].pressed = true;
		input->key_state[keycode].repeat_count = 0;
	}

	Event e;
	e.type = EVENT_KEY;
	e.key.type = EVENT_KEY_PRESSED;
	e.key.keycode = keycode;
	e.key.repeat = input->key_state[keycode].repeat_count > 0;
	e.handled = false;
	event_manager_emit(input->event_manager, &e);
}

void input_handle_release(Input *input, const LSWindow *window, LSKeycode keycode) {
	CORE_ASSERT(input);
	CORE_ASSERT(window);

	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return;
	}

	Event e;
	e.type = EVENT_KEY;
	e.key.type = EVENT_KEY_RELEASED;
	e.key.keycode = keycode;
	e.key.repeat = false; // Release events don't have repeats.
	e.handled = false;
	event_manager_emit(input->event_manager, &e);

	input->key_state[keycode].pressed = false;
	input->key_state[keycode].repeat_count = 0;
}

bool input_is_key_pressed(const Input *input, LSKeycode keycode) {
	CORE_ASSERT(input);

	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return input->key_state[keycode].pressed;
}

bool input_is_key_just_pressed(const Input *input, LSKeycode keycode) {
	CORE_ASSERT(input);

	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return input->key_state[keycode].pressed && input->key_state[keycode].repeat_count == 0;
}

bool input_is_key_released(const Input *input, LSKeycode keycode) {
	CORE_ASSERT(input);

	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return !input->key_state[keycode].pressed;
}

bool input_is_key_just_released(const Input *input, LSKeycode keycode) {
	CORE_ASSERT(input);

	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return !input->key_state[keycode].pressed && input->previous_key_state[keycode].pressed;
}