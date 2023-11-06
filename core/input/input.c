#include "core/input/input.h"
#include "core/core.h"
#include "core/core_log.h"

typedef struct {
	bool pressed;
	uint32 repeat_count;
} KeyState;

typedef struct {
	bool pressed;
} ButtonState;

struct Input {
	// TODO: This wastes some memory, but it's not a big deal.
	KeyState key_state[LS_KEY_LAST + 1];
	KeyState previous_key_state[LS_KEY_LAST + 1];

	// This one on the other hand is sequential, so it's fine.
	ButtonState mouse_button_state[LS_MOUSE_BUTTON_LAST + 1];
	ButtonState previous_mouse_button_state[LS_MOUSE_BUTTON_LAST + 1];

	Vector2i mouse_position;
};

static struct Input input;

void input_poll() {
	for (size_t i = 0; i < LS_KEY_LAST; i++) {
		input.previous_key_state[i] = input.key_state[i];
	}

	for (size_t i = 0; i < LS_MOUSE_BUTTON_LAST; i++) {
		input.previous_mouse_button_state[i] = input.mouse_button_state[i];
	}
}

void input_handle_press(const LSWindow *window, LSKeycode keycode) {
	CORE_ASSERT(window);

	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return;
	}

	if (input.key_state[keycode].pressed) {
		input.key_state[keycode].repeat_count++;
	} else {
		input.key_state[keycode].pressed = true;
		input.key_state[keycode].repeat_count = 0;
	}

	Event e;
	e.type = EVENT_KEY;
	e.key.type = EVENT_KEY_PRESSED;
	e.key.keycode = keycode;
	e.key.repeat = input.key_state[keycode].repeat_count > 0;
	e.handled = false;
	events_emit(&e);
}

void input_handle_release(const LSWindow *window, LSKeycode keycode) {
	CORE_ASSERT(window);

	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return;
	}

	input.key_state[keycode].pressed = false;
	input.key_state[keycode].repeat_count = 0;

	Event e;
	e.type = EVENT_KEY;
	e.key.type = EVENT_KEY_RELEASED;
	e.key.keycode = keycode;
	e.key.repeat = false; // Release events don't have repeats.
	e.handled = false;
	events_emit(&e);
}

bool input_is_key_pressed(LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return input.key_state[keycode].pressed;
}

bool input_is_key_just_pressed(LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return input.key_state[keycode].pressed && input.key_state[keycode].repeat_count == 0;
}

bool input_is_key_released(LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return !input.key_state[keycode].pressed;
}

bool input_is_key_just_released(LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		core_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return !input.key_state[keycode].pressed && input.previous_key_state[keycode].pressed;
}

void input_handle_mouse_press(const LSWindow *window, LSMouseButton button, Vector2i position) {
	CORE_ASSERT(window);
	CORE_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	if (!input.mouse_button_state[button].pressed) {
		input.mouse_button_state[button].pressed = true;
	} else {
		core_log(LOG_LEVEL_WARNING, "Mouse button %d is already pressed\n", button);
	}

	input.mouse_position = position;

	Event e;
	e.type = EVENT_MOUSE;
	e.mouse.type = EVENT_MOUSE_PRESSED;
	e.mouse.button = button;
	e.mouse.position = position;
	e.handled = false;
	events_emit(&e);
}

void input_handle_mouse_release(const LSWindow *window, LSMouseButton button, Vector2i position) {
	CORE_ASSERT(window);
	CORE_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	input.mouse_button_state[button].pressed = false;
	input.mouse_position = position;

	Event e;
	e.type = EVENT_MOUSE;
	e.mouse.type = EVENT_MOUSE_RELEASED;
	e.mouse.button = button;
	e.mouse.position = position;
	e.handled = false;
	events_emit(&e);
}

void input_handle_mouse_move(const LSWindow *window, Vector2i position) {
	CORE_ASSERT(window);

	input.mouse_position = position;

	Event e;
	e.type = EVENT_MOUSE;
	e.mouse.type = EVENT_MOUSE_MOVED;
	e.mouse.button = LS_MOUSE_BUTTON_NONE;
	e.mouse.position = position;
	e.handled = false;
	events_emit(&e);
}

void input_handle_mouse_enter(const LSWindow *window, Vector2i position) {
	CORE_ASSERT(window);

	input.mouse_position = position;

	Event e;
	e.type = EVENT_MOUSE;
	e.mouse.type = EVENT_MOUSE_ENTERED;
	e.mouse.button = LS_MOUSE_BUTTON_NONE;
	e.mouse.position = position;
	e.handled = false;
	events_emit(&e);
}

void input_handle_mouse_leave(const LSWindow *window, Vector2i position) {
	CORE_ASSERT(window);

	input.mouse_position = position;

	Event e;
	e.type = EVENT_MOUSE;
	e.mouse.type = EVENT_MOUSE_LEFT;
	e.mouse.button = LS_MOUSE_BUTTON_NONE;
	e.mouse.position = position;
	e.handled = false;
	events_emit(&e);
}

bool input_is_mouse_button_pressed(LSMouseButton button) {
	CORE_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return input.mouse_button_state[button].pressed;
}

bool input_is_mouse_button_just_pressed(LSMouseButton button) {
	CORE_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return input.mouse_button_state[button].pressed && !input.previous_mouse_button_state[button].pressed;
}

bool input_is_mouse_button_released(LSMouseButton button) {
	CORE_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return !input.mouse_button_state[button].pressed;
}

bool input_is_mouse_button_just_released(LSMouseButton button) {
	CORE_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return !input.mouse_button_state[button].pressed && input.previous_mouse_button_state[button].pressed;
}

Vector2i input_get_mouse_position() {
	return input.mouse_position;
}