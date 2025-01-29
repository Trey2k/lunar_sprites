
#include "core/input/input_manager.h"
#include "core/debug.h"
#include "core/input/keycodes.h"
#include "core/log.h"

typedef struct {
	bool pressed;
	uint32 repeat_count;
} KeyState;

typedef struct {
	bool pressed;
} ButtonState;

struct {
	// TODO: This wastes some memory, but it's not a big deal.
	KeyState key_state[LS_KEY_LAST + 1];
	KeyState previous_key_state[LS_KEY_LAST + 1];

	// This one on the other hand is sequential, so it's fine.
	ButtonState mouse_button_state[LS_MOUSE_BUTTON_LAST + 1];
	ButtonState previous_mouse_button_state[LS_MOUSE_BUTTON_LAST + 1];

	Vector2u mouse_position;

	const LSWindow *active_window;
} input_manager = { 0 };

void input_poll() {
	for (size_t i = 0; i < LS_KEY_LAST; i++) {
		input_manager.previous_key_state[i] = input_manager.key_state[i];
	}

	for (size_t i = 0; i < LS_MOUSE_BUTTON_LAST; i++) {
		input_manager.previous_mouse_button_state[i] = input_manager.mouse_button_state[i];
	}
}

void input_set_active_window(const LSWindow *window) {
	input_manager.active_window = window;
}

void input_handle_press(LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return;
	}

	if (input_manager.key_state[keycode].pressed) {
		input_manager.key_state[keycode].repeat_count++;

		return;
	}

	input_manager.key_state[keycode].pressed = true;
	input_manager.key_state[keycode].repeat_count = 0;
}

void input_handle_release(LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return;
	}

	input_manager.key_state[keycode].pressed = false;
	input_manager.key_state[keycode].repeat_count = 0;
}

bool input_is_key_pressed(LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return input_manager.key_state[keycode].pressed;
}

bool input_is_key_just_pressed(LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return input_manager.key_state[keycode].pressed && input_manager.key_state[keycode].repeat_count == 0;
}

bool input_is_key_released(LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return !input_manager.key_state[keycode].pressed;
}

bool input_is_key_just_released(LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return !input_manager.key_state[keycode].pressed && input_manager.previous_key_state[keycode].pressed;
}

void input_handle_mouse_press(LSMouseButton button, Vector2u position) {
	LS_ASSERT(button >= LS_MOUSE_BUTTON_NONE && button <= LS_MOUSE_BUTTON_LAST);
	if (button == LS_MOUSE_BUTTON_NONE) {
		return;
	}

	if (!input_manager.mouse_button_state[button].pressed) {
		input_manager.mouse_button_state[button].pressed = true;
	} else {
		ls_log(LOG_LEVEL_WARNING, "Mouse button %d is already pressed\n", button);
	}

	input_manager.mouse_position = position;
}

void input_handle_mouse_release(LSMouseButton button, Vector2u position) {
	LS_ASSERT(button >= 0 && button <= LS_MOUSE_BUTTON_LAST);

	input_manager.mouse_button_state[button].pressed = false;
	input_manager.mouse_position = position;
}

void input_handle_mouse_move(Vector2u position) {
	input_manager.mouse_position = position;
}

void input_handle_mouse_enter(Vector2u position) {
	LS_ASSERT(input_manager.active_window);

	input_manager.mouse_position = position;
}

void input_handle_mouse_leave(Vector2u position) {
	input_manager.mouse_position = position;
}

bool input_is_mouse_pressed(LSMouseButton button) {
	LS_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return input_manager.mouse_button_state[button].pressed;
}

bool input_is_mouse_just_pressed(LSMouseButton button) {
	LS_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return input_manager.mouse_button_state[button].pressed && !input_manager.previous_mouse_button_state[button].pressed;
}

bool input_is_mouse_released(LSMouseButton button) {
	LS_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return !input_manager.mouse_button_state[button].pressed;
}

bool input_is_mouse_just_released(LSMouseButton button) {
	LS_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return !input_manager.mouse_button_state[button].pressed && input_manager.previous_mouse_button_state[button].pressed;
}

Vector2u input_get_mouse_position() {
	Vector2u pos = input_manager.mouse_position;

	return pos;
}