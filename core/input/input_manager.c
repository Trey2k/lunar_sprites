
#include "core/input/input_manager.h"
#include "core/debug.h"
#include "core/events/event_manager.h"
#include "core/events/events.h"
#include "core/input/keycodes.h"
#include "core/log.h"
#include "core/memory.h"

typedef struct {
	bool pressed;
	uint32 repeat_count;
} KeyState;

typedef struct {
	bool pressed;
} ButtonState;

struct InputManager {
	// TODO: This wastes some memory, but it's not a big deal.
	KeyState key_state[LS_KEY_LAST + 1];
	KeyState previous_key_state[LS_KEY_LAST + 1];

	// This one on the other hand is sequential, so it's fine.
	ButtonState mouse_button_state[LS_MOUSE_BUTTON_LAST + 1];
	ButtonState previous_mouse_button_state[LS_MOUSE_BUTTON_LAST + 1];

	Vector2u mouse_position;

	const EventManager *event_manager;

	const LSWindow *active_window;
};

InputManager *ls_create_input_manager(const EventManager *event_manager) {
	LS_ASSERT(event_manager);

	InputManager *input_manager = ls_calloc(1, sizeof(InputManager));
	input_manager->event_manager = event_manager;

	return input_manager;
}

void input_manager_destroy(InputManager *input_manager) {
	LS_ASSERT(input_manager);

	ls_free(input_manager);
}

void input_poll(InputManager *input_manager) {
	for (size_t i = 0; i < LS_KEY_LAST; i++) {
		input_manager->previous_key_state[i] = input_manager->key_state[i];
	}

	for (size_t i = 0; i < LS_MOUSE_BUTTON_LAST; i++) {
		input_manager->previous_mouse_button_state[i] = input_manager->mouse_button_state[i];
	}
}

void input_set_active_window(InputManager *input_manager, const LSWindow *window) {
	input_manager->active_window = window;
}

void input_handle_window_open(InputManager *input_manager, const LSWindow *window) {
	LS_ASSERT(window);

	Event e;
	e.type = EVENT_WINDOW;
	e.window.type = EVENT_WINDOW_OPEN;
	e.window.window = window;
	event_manager_emit(input_manager->event_manager, &e);
}

void input_handle_window_close(InputManager *input_manager) {
	LS_ASSERT(input_manager->active_window);

	Event e;
	e.type = EVENT_WINDOW;
	e.window.type = EVENT_WINDOW_CLOSE;
	e.window.window = input_manager->active_window;
	event_manager_emit(input_manager->event_manager, &e);
}

void input_handle_press(InputManager *input_manager, LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return;
	}

	if (input_manager->key_state[keycode].pressed) {
		input_manager->key_state[keycode].repeat_count++;
	} else {
		input_manager->key_state[keycode].pressed = true;
		input_manager->key_state[keycode].repeat_count = 0;
	}

	Event e;
	e.type = EVENT_KEY;
	e.key.type = EVENT_KEY_PRESSED;
	e.key.keycode = keycode;
	e.key.repeated = input_manager->key_state[keycode].repeat_count > 0;
	e.key.window = input_manager->active_window;
	e.handled = false;
	event_manager_emit(input_manager->event_manager, &e);
}

void input_handle_release(InputManager *input_manager, LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return;
	}

	input_manager->key_state[keycode].pressed = false;
	input_manager->key_state[keycode].repeat_count = 0;

	Event e;
	e.type = EVENT_KEY;
	e.key.type = EVENT_KEY_RELEASED;
	e.key.keycode = keycode;
	e.key.window = input_manager->active_window;
	e.handled = false;
	event_manager_emit(input_manager->event_manager, &e);
}

void input_handle_resize(InputManager *input_manager, Vector2u size) {
	LS_ASSERT(input_manager->active_window);

	Event e;
	e.type = EVENT_WINDOW;
	e.window.type = EVENT_WINDOW_RESIZE;
	e.window.size = size;
	e.window.window = input_manager->active_window;
	e.handled = false;
	event_manager_emit(input_manager->event_manager, &e);
}

bool input_is_key_pressed(const InputManager *input_manager, LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return input_manager->key_state[keycode].pressed;
}

bool input_is_key_just_pressed(const InputManager *input_manager, LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return input_manager->key_state[keycode].pressed && input_manager->key_state[keycode].repeat_count == 0;
}

bool input_is_key_released(const InputManager *input_manager, LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return !input_manager->key_state[keycode].pressed;
}

bool input_is_key_just_released(const InputManager *input_manager, LSKeycode keycode) {
	if (keycode < 0 || keycode > LS_KEY_LAST) {
		ls_log(LOG_LEVEL_WARNING, "Invalid keycode %d\n", keycode);

		return false;
	}

	return !input_manager->key_state[keycode].pressed && input_manager->previous_key_state[keycode].pressed;
}

void input_handle_mouse_press(InputManager *input_manager, LSMouseButton button, Vector2u position) {
	LS_ASSERT(button >= LS_MOUSE_BUTTON_NONE && button <= LS_MOUSE_BUTTON_LAST);
	if (button == LS_MOUSE_BUTTON_NONE) {
		return;
	}

	if (!input_manager->mouse_button_state[button].pressed) {
		input_manager->mouse_button_state[button].pressed = true;
	} else {
		ls_log(LOG_LEVEL_WARNING, "Mouse button %d is already pressed\n", button);
	}

	input_manager->mouse_position = position;

	Event e;
	e.type = EVENT_MOUSE;
	e.mouse.type = EVENT_MOUSE_PRESSED;
	e.mouse.button = button;
	e.mouse.position = position;
	e.mouse.window = input_manager->active_window;
	e.handled = false;
	event_manager_emit(input_manager->event_manager, &e);
}

void input_handle_mouse_release(InputManager *input_manager, LSMouseButton button, Vector2u position) {
	LS_ASSERT(button >= 0 && button <= LS_MOUSE_BUTTON_LAST);

	input_manager->mouse_button_state[button].pressed = false;
	input_manager->mouse_position = position;

	Event e;
	e.type = EVENT_MOUSE;
	e.mouse.type = EVENT_MOUSE_RELEASED;
	e.mouse.button = button;
	e.mouse.position = position;
	e.mouse.window = input_manager->active_window;
	e.handled = false;
	event_manager_emit(input_manager->event_manager, &e);
}

void input_handle_mouse_move(InputManager *input_manager, Vector2u position) {
	input_manager->mouse_position = position;

	Event e;
	e.type = EVENT_MOUSE;
	e.mouse.type = EVENT_MOUSE_MOVED;
	e.mouse.button = LS_MOUSE_BUTTON_NONE;
	e.mouse.position = position;
	e.mouse.window = input_manager->active_window;
	e.handled = false;
	event_manager_emit(input_manager->event_manager, &e);
}

void input_handle_mouse_enter(InputManager *input_manager, Vector2u position) {
	LS_ASSERT(input_manager->active_window);

	input_manager->mouse_position = position;

	Event e;
	e.type = EVENT_MOUSE;
	e.mouse.type = EVENT_MOUSE_ENTERED;
	e.mouse.button = LS_MOUSE_BUTTON_NONE;
	e.mouse.position = position;
	e.mouse.window = input_manager->active_window;
	e.handled = false;
	event_manager_emit(input_manager->event_manager, &e);
}

void input_handle_mouse_leave(InputManager *input_manager, Vector2u position) {
	input_manager->mouse_position = position;

	Event e;
	e.type = EVENT_MOUSE;
	e.mouse.type = EVENT_MOUSE_LEFT;
	e.mouse.button = LS_MOUSE_BUTTON_NONE;
	e.mouse.position = position;
	e.mouse.window = input_manager->active_window;
	e.handled = false;
	event_manager_emit(input_manager->event_manager, &e);
}

bool input_is_mouse_pressed(const InputManager *input_manager, LSMouseButton button) {
	LS_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return input_manager->mouse_button_state[button].pressed;
}

bool input_is_mouse_just_pressed(const InputManager *input_manager, LSMouseButton button) {
	LS_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return input_manager->mouse_button_state[button].pressed && !input_manager->previous_mouse_button_state[button].pressed;
}

bool input_is_mouse_released(const InputManager *input_manager, LSMouseButton button) {
	LS_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return !input_manager->mouse_button_state[button].pressed;
}

bool input_is_mouse_just_released(const InputManager *input_manager, LSMouseButton button) {
	LS_ASSERT(button > 0 && button < LS_MOUSE_BUTTON_LAST);

	return !input_manager->mouse_button_state[button].pressed && input_manager->previous_mouse_button_state[button].pressed;
}

Vector2u input_get_mouse_position(const InputManager *input_manager) {
	return input_manager->mouse_position;
}